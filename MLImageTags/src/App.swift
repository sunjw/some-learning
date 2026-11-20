import Foundation
import CoreGraphics
import Vision
import ImageIO

@main
struct App {
    static func main() async {
        // A client that goes away closes the pipe; ignore SIGPIPE so that
        // writing to it fails with EPIPE instead of killing the process.
        signal(SIGPIPE, SIG_IGN)

        do {
            try await run()
            exit(0)
        } catch let err as CLIUsageError {
            printError(err.message)
            exit(2)
        } catch {
            printError(error.localizedDescription)
            exit(1)
        }
    }

    private static func run() async throws {
        let args = CommandLine.arguments
        guard args.count >= 2 else {
            printUsage()
            throw CLIUsageError(message: "missing arguments")
        }

        var inputArg: String?
        var outputArg: String?
        var daemon = false

        var i = 1
        while i < args.count {
            let a = args[i]
            switch a {
            case "-h", "--help":
                printUsage()
                return
            case "-d", "--daemon":
                daemon = true
                i += 1
            case "-i", "--input":
                guard i + 1 < args.count else { throw CLIUsageError(message: "\(a) requires a path") }
                inputArg = args[i + 1]
                i += 2
            case "-o", "--output":
                guard i + 1 < args.count else { throw CLIUsageError(message: "\(a) requires a path") }
                outputArg = args[i + 1]
                i += 2
            default:
                if a.hasPrefix("-") {
                    throw CLIUsageError(message: "unknown option: \(a)")
                }
                throw CLIUsageError(message: "unexpected positional argument: \(a)")
            }
        }

        if daemon {
            guard inputArg == nil else {
                throw CLIUsageError(message: "-d / --daemon cannot be combined with -i / --input")
            }
            guard outputArg == nil else {
                throw CLIUsageError(message: "-d / --daemon cannot be combined with -o / --output")
            }
            await runDaemon()
            return
        }

        guard let inputArg else {
            throw CLIUsageError(message: "missing required -i / --input <path>")
        }

        let inputURL = resolveURL(inputArg)
        guard FileManager.default.fileExists(atPath: inputURL.path) else {
            throw TaggerError.imageLoadFailed(inputURL.path)
        }

        let outputURL: URL? = outputArg.map { resolveURL($0) }
        let payload = try await analyze(path: inputURL.path)
        try emitJSON(payload, to: outputURL)
    }

    // MARK: - Daemon mode

    /// Reads one image path per line from stdin and writes one JSON object per
    /// line to stdout. Blank lines are ignored. The line "EXIT" terminates the
    /// daemon without producing output. Every other input line produces
    /// exactly one output line, so a client can read them back synchronously.
    private static func runDaemon() async {
        while let rawLine = readLine(strippingNewline: true) {
            if rawLine == daemonExitCommand { break }

            let path = stripQuotes(rawLine)
            if path.isEmpty { continue }

            let payload: TaggingOutput
            do {
                payload = try await analyze(path: path)
            } catch {
                payload = TaggingOutput(path: path, colors: [], tags: [],
                                        elapsedMs: 0, error: error.localizedDescription)
            }

            // Always emit exactly one line per request, even if encoding fails.
            let line: Data
            if let data = encodeJSON(payload) {
                line = data
            } else {
                line = Data(fallbackJSON(path: path).utf8)
            }

            do {
                try FileHandle.standardOutput.write(contentsOf: line)
                try FileHandle.standardOutput.write(contentsOf: Data("\n".utf8))
            } catch {
                // The client closed the pipe; stop serving.
                break
            }
        }
    }

    /// A minimal JSON object used when the normal payload cannot be encoded.
    /// The path is escaped by hand because the fallback cannot rely on the
    /// encoder that just failed.
    private static func fallbackJSON(path: String) -> String {
        var escaped = ""
        for character in path.unicodeScalars {
            switch character {
            case "\"": escaped += "\\\""
            case "\\": escaped += "\\\\"
            case "\n": escaped += "\\n"
            case "\r": escaped += "\\r"
            case "\t": escaped += "\\t"
            default:
                if character.value < 0x20 {
                    escaped += String(format: "\\u%04x", character.value)
                } else {
                    escaped.unicodeScalars.append(character)
                }
            }
        }
        return "{\"colors\":[],\"elapsedMs\":0,\"error\":\"failed to encode JSON result\",\"path\":\"\(escaped)\",\"tags\":[]}"
    }

    /// Removes a matching pair of surrounding single or double quotes.
    private static func stripQuotes(_ line: String) -> String {
        var s = line.trimmingCharacters(in: .whitespaces)
        if s.count >= 2 {
            let first = s.first!
            let last = s.last!
            if (first == "\"" && last == "\"") || (first == "'" && last == "'") {
                s = String(s.dropFirst().dropLast())
            }
        }
        return s
    }

    // MARK: - Steps

    private static func resolveURL(_ raw: String) -> URL {
        if raw.hasPrefix("~") {
            return URL(fileURLWithPath: (raw as NSString).expandingTildeInPath)
        }
        return URL(fileURLWithPath: raw)
    }

    /// Analyzes one image and returns the full result payload.
    /// Shared by one-shot and daemon mode.
    private static func analyze(path: String) async throws -> TaggingOutput {
        let inputURL = resolveURL(path)
        guard FileManager.default.fileExists(atPath: inputURL.path) else {
            throw TaggerError.imageLoadFailed(inputURL.path)
        }

        let startTime = Date()
        let images = try loadCGImage(at: inputURL)

        async let tagsResult = try classify(cgImage: images.full, orientation: images.orientation)
        async let colorsResult = try analyzeColors(cgImage: images.thumb)

        let tags = try await tagsResult
        let colors = try await colorsResult

        let elapsedMs = Int(Date().timeIntervalSince(startTime) * 1000)
        return TaggingOutput(path: inputURL.path, colors: colors, tags: tags, elapsedMs: elapsedMs)
    }

    private static func classify(cgImage: CGImage, orientation: CGImagePropertyOrientation) throws -> [Tag] {
        let request = VNClassifyImageRequest()
        let handler = VNImageRequestHandler(cgImage: cgImage, orientation: orientation, options: [:])
        try handler.perform([request])

        let observations = (request.results ?? [])
            .filter { $0.confidence >= visionThreshold }
            .sorted {
                if $0.confidence != $1.confidence { return $0.confidence > $1.confidence }
                return $0.identifier < $1.identifier
            }

        var seen = Set<String>()
        var tags: [Tag] = []
        for obs in observations {
            let identifier = obs.identifier
            guard !seen.contains(identifier) else { continue }
            seen.insert(identifier)
            tags.append(Tag(tag: identifier, confidence: obs.confidence))
        }
        return tags
    }

    private static func analyzeColors(cgImage: CGImage) throws -> [ColorResult] {
        let width = cgImage.width
        let height = cgImage.height
        guard width > 0, height > 0 else { return [] }

        let bytesPerRow = width * 4
        let colorSpace = CGColorSpace(name: CGColorSpace.sRGB) ?? CGColorSpaceCreateDeviceRGB()
        let bitmapInfo = CGImageAlphaInfo.premultipliedLast.rawValue
            | CGBitmapInfo.byteOrder32Big.rawValue

        guard let context = CGContext(
            data: nil,
            width: width,
            height: height,
            bitsPerComponent: 8,
            bytesPerRow: bytesPerRow,
            space: colorSpace,
            bitmapInfo: bitmapInfo
        ) else {
            throw TaggerError.colorAnalysisFailed
        }
        context.draw(cgImage, in: CGRect(x: 0, y: 0, width: width, height: height))

        let (groupCounts, opaqueCount) = countPixels(context: context, width: width, height: height)

        guard opaqueCount > 0 else { return [] }

        let total = Float(opaqueCount)
        var result: [ColorResult] = []
        for (group, count) in groupCounts {
            let ratio = Float(count) / total
            if ratio >= colorMinRatio {
                result.append(ColorResult(color: group, ratio: ratio))
            }
        }
        result.sort {
            if $0.ratio != $1.ratio { return $0.ratio > $1.ratio }
            return $0.color < $1.color
        }
        // Keep the most prominent colors even when they fall below the ratio threshold,
        // so small-but-salient subjects (e.g. an orange fish in green water) still show up.
        if result.count < minColorCount, result.count < groupCounts.count {
            let missing = groupCounts
                .map { (color: $0.key, ratio: Float($0.value) / total) }
                .filter { entry in
                    entry.ratio >= colorAbsoluteMinRatio
                        && !result.contains { $0.color == entry.color }
                }
                .sorted { $0.ratio > $1.ratio }
            for entry in missing {
                result.append(ColorResult(color: entry.color, ratio: entry.ratio))
                if result.count >= minColorCount { break }
            }
            result.sort {
                if $0.ratio != $1.ratio { return $0.ratio > $1.ratio }
                return $0.color < $1.color
            }
        }
        return result
    }

    private static func countPixels(context: CGContext, width: Int, height: Int) -> (counts: [String: Int], opaqueCount: Int) {
        let totalPixels = width * height
        let bytesPerPixel = 4
        let stride = totalPixels * bytesPerPixel

        guard let raw = context.data else {
            return ([:], 0)
        }
        let pixels = raw.bindMemory(to: UInt8.self, capacity: stride)

        var groupCounts: [String: Int] = [:]
        var opaqueCount = 0
        var i = 0
        while i < stride {
            if Int(pixels[i + 3]) < 128 {
                i += bytesPerPixel
                continue
            }
            let r = Float(pixels[i]) / 255.0
            let g = Float(pixels[i + 1]) / 255.0
            let b = Float(pixels[i + 2]) / 255.0
            groupCounts[classifyColor(r: r, g: g, b: b), default: 0] += 1
            opaqueCount += 1
            i += bytesPerPixel
        }
        return (groupCounts, opaqueCount)
    }

    // MARK: - Color classification (HSL-based)

    private static func classifyColor(r: Float, g: Float, b: Float) -> String {
        let maxC = max(r, max(g, b))
        let minC = min(r, min(g, b))
        let delta = maxC - minC
        let saturation = maxC > 0 ? delta / maxC : 0
        let luminance = 0.2126 * r + 0.7152 * g + 0.0722 * b

        // White: bright AND desaturated (a bright saturated pixel is not white)
        if luminance >= whiteMinLuminance && saturation < whiteMaxSaturation {
            return "white"
        }

        // Very dark or near-gray (low saturation) → black / gray
        if maxC < 0.15 || saturation < 0.10 {
            return luminance < blackMaxLuminance ? "black" : "gray"
        }

        return colorFamily(hue: hue(r: r, g: g, b: b, max: maxC, delta: delta))
    }

    private static func hue(r: Float, g: Float, b: Float, max maxC: Float, delta: Float) -> Float {
        var h: Float
        if maxC == r {
            h = (g - b) / delta
        } else if maxC == g {
            h = 2 + (b - r) / delta
        } else {
            h = 4 + (r - g) / delta
        }
        h *= 60
        if h < 0 { h += 360 }
        return h
    }

    private static func colorFamily(hue h: Float) -> String {
        switch h {
        case ..<20, 345...:  return "red"     // red + red-orange
        case 20..<48:        return "gold"    // amber → gold
        case 48..<70:        return "yellow"  // pure yellow
        case 70..<170:       return "green"
        case 170..<200:      return "blue"    // cyan
        case 200..<250:      return "blue"    // blue + indigo
        case 250..<320:      return "purple"
        case 320..<345:      return "red"     // magenta
        default:             return "gray"
        }
    }

    private static func loadCGImage(at url: URL) throws -> (full: CGImage, thumb: CGImage, orientation: CGImagePropertyOrientation) {
        guard let imageSource = CGImageSourceCreateWithURL(url as CFURL, nil) else {
            throw TaggerError.imageLoadFailed(url.path)
        }

        let properties = CGImageSourceCopyPropertiesAtIndex(imageSource, 0, nil) as? [CFString: Any]
        let rawOrientation = (properties?[kCGImagePropertyOrientation] as? UInt32) ?? 1
        let orientation = CGImagePropertyOrientation(rawValue: rawOrientation) ?? .up

        guard let full = CGImageSourceCreateImageAtIndex(imageSource, 0, nil) else {
            throw TaggerError.imageLoadFailed(url.path)
        }

        let options: [CFString: Any] = [
            kCGImageSourceCreateThumbnailFromImageAlways: true,
            kCGImageSourceCreateThumbnailWithTransform: true,
            kCGImageSourceShouldCacheImmediately: true,
            kCGImageSourceThumbnailMaxPixelSize: thumbnailMaxPixelSize,
        ]
        guard let thumb = CGImageSourceCreateThumbnailAtIndex(imageSource, 0, options as CFDictionary) else {
            throw TaggerError.thumbnailFailed(url.path)
        }
        return (full, thumb, orientation)
    }

    private static func encodeJSON(_ payload: TaggingOutput) -> Data? {
        let encoder = JSONEncoder()
        encoder.outputFormatting = [.withoutEscapingSlashes, .sortedKeys]
        return try? encoder.encode(payload)
    }

    private static func emitJSON(_ payload: TaggingOutput, to outputURL: URL?) throws {
        guard let data = encodeJSON(payload) else {
            throw TaggerError.jsonEncodingFailed
        }

        if let outputURL {
            try data.write(to: outputURL, options: .atomic)
        } else {
            try FileHandle.standardOutput.write(contentsOf: data)
            try FileHandle.standardOutput.write(contentsOf: Data("\n".utf8))
        }
    }

    // MARK: - I/O helpers

    private static func printUsage() {
        let usage = """
        usage: MLImageTags -i <image_path> [-o <output_path>]
               MLImageTags -d

        Recognizes tags and dominant colors in an image using Apple's
        Vision framework (VNClassifyImageRequest) and a Core Graphics
        color histogram, then prints (or writes) a JSON object:

          {"path":"...","tags":[{"tag":"some","confidence":0.95},...],"colors":[{"color":"green","ratio":0.45},...],"elapsedMs":1234}

        Options:
          -i, --input <path>      image to analyze (required unless -d)
          -o, --output <path>     write JSON to file instead of stdout
          -d, --daemon            read image paths from stdin, one per line, and
                                  write one JSON object per line to stdout
          -h, --help              show this message

        Daemon mode:
          Each non-blank line of stdin is an image path, optionally wrapped in
          single or double quotes. Blank lines are ignored. Every input line
          produces exactly one output line; failures are reported as a JSON
          object with an "error" field. The line "EXIT" shuts the daemon down
          without producing output.
        """
        FileHandle.standardError.write(Data((usage + "\n").utf8))
    }

    private static func printError(_ message: String) {
        FileHandle.standardError.write(Data("error: \(message)\n".utf8))
    }
}

private struct TaggingOutput: Codable {
    let path: String
    let colors: [ColorResult]
    let tags: [Tag]
    let elapsedMs: Int
    var error: String?

    init(path: String, colors: [ColorResult], tags: [Tag], elapsedMs: Int, error: String? = nil) {
        self.path = path
        self.colors = colors
        self.tags = tags
        self.elapsedMs = elapsedMs
        self.error = error
    }
}

private struct Tag: Codable {
    let tag: String
    let confidence: Float
}

private struct ColorResult: Codable {
    let color: String
    let ratio: Float
}

private let colorMinRatio: Float = 0.05
private let colorAbsoluteMinRatio: Float = 0.01
private let minColorCount: Int = 3
private let visionThreshold: Float = 0.2
private let thumbnailMaxPixelSize: Int = 512
private let blackMaxLuminance: Float = 0.15
private let whiteMinLuminance: Float = 0.70
private let whiteMaxSaturation: Float = 0.15

/// The exact line that makes the daemon shut down.
private let daemonExitCommand = "EXIT"

private enum TaggerError: LocalizedError {
    case imageLoadFailed(String)
    case thumbnailFailed(String)
    case colorAnalysisFailed
    case jsonEncodingFailed

    var errorDescription: String? {
        switch self {
        case .imageLoadFailed(let path): return "failed to load image: \(path)"
        case .thumbnailFailed(let path): return "failed to create thumbnail for: \(path)"
        case .colorAnalysisFailed: return "failed to create bitmap context for color analysis"
        case .jsonEncodingFailed: return "failed to encode JSON result"
        }
    }
}

private struct CLIUsageError: Error {
    let message: String
}
