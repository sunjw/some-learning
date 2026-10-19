import Foundation
import CoreGraphics
import Vision
import ImageIO

@main
struct App {
    static func main() async {
        do {
            try await run()
            exit(0)
        } catch let err as CLIUsageError {
            printError(err.message)
            exit(2)
        } catch {
            printError("\(error)")
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

        var i = 1
        while i < args.count {
            let a = args[i]
            switch a {
            case "-h", "--help":
                printUsage()
                return
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

        guard let inputArg else {
            throw CLIUsageError(message: "missing required -i / --input <path>")
        }

        let inputURL = resolveURL(inputArg)
        guard FileManager.default.fileExists(atPath: inputURL.path) else {
            throw TaggerError.imageLoadFailed(inputURL.path)
        }

        let outputURL: URL? = outputArg.map { resolveURL($0) }

        let startTime = Date()
        let images = try loadCGImage(at: inputURL)

        async let tagsResult = try classify(cgImage: images.full, orientation: images.orientation)
        async let colorsResult = try analyzeColors(cgImage: images.thumb)

        let tags = try await tagsResult
        let colors = try await colorsResult

        let elapsedMs = Int(Date().timeIntervalSince(startTime) * 1000)
        try emitJSON(path: inputURL.path, tags: tags, colors: colors, elapsedMs: elapsedMs, to: outputURL)
    }

    // MARK: - Steps

    private static func resolveURL(_ raw: String) -> URL {
        if raw.hasPrefix("~") {
            return URL(fileURLWithPath: (raw as NSString).expandingTildeInPath)
        }
        return URL(fileURLWithPath: raw)
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

        let bytesPerPixel = 4
        let bytesPerRow = width * bytesPerPixel
        let totalPixels = width * height

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

        guard let raw = context.data else {
            throw TaggerError.colorAnalysisFailed
        }
        let pixels = raw.bindMemory(to: UInt8.self, capacity: totalPixels * bytesPerPixel)

        var counts: [Int] = Array(repeating: 0, count: palette.count)
        var opaqueCount = 0
        var i = 0
        let stride = totalPixels * bytesPerPixel
        while i < stride {
            if Int(pixels[i + 3]) < 128 {
                i += bytesPerPixel
                continue
            }
            let r = Float(pixels[i]) / 255.0
            let g = Float(pixels[i + 1]) / 255.0
            let b = Float(pixels[i + 2]) / 255.0
            counts[nearestPaletteIndex(r: r, g: g, b: b)] += 1
            opaqueCount += 1
            i += bytesPerPixel
        }

        guard opaqueCount > 0 else { return [] }

        let total = Float(opaqueCount)
        var groupCounts: [String: Int] = [:]
        for (idx, count) in counts.enumerated() where count > 0 {
            groupCounts[palette[idx].group, default: 0] += count
        }
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
        return result
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

    private static func nearestPaletteIndex(r: Float, g: Float, b: Float) -> Int {
        var bestIndex = 0
        var bestDistance: Float = .greatestFiniteMagnitude
        for (i, color) in palette.enumerated() {
            let dr = color.r - r
            let dg = color.g - g
            let db = color.b - b
            let d = dr * dr + dg * dg + db * db
            if d < bestDistance {
                bestDistance = d
                bestIndex = i
            }
        }
        return bestIndex
    }

    private static func emitJSON(path: String, tags: [Tag], colors: [ColorResult], elapsedMs: Int, to outputURL: URL?) throws {
        let payload = TaggingOutput(path: path, colors: colors, tags: tags, elapsedMs: elapsedMs)
        let encoder = JSONEncoder()
        encoder.outputFormatting = [.withoutEscapingSlashes, .sortedKeys]
        let data = try encoder.encode(payload)

        if let outputURL {
            try data.write(to: outputURL, options: .atomic)
        } else {
            FileHandle.standardOutput.write(data)
            FileHandle.standardOutput.write(Data("\n".utf8))
        }
    }

    // MARK: - I/O helpers

    private static func printUsage() {
        let usage = """
        usage: MLImageTags -i <image_path> [-o <output_path>]

        Recognizes tags and dominant colors in an image using Apple's
        Vision framework (VNClassifyImageRequest) and a Core Graphics
        color histogram, then prints (or writes) a JSON object:

          {"path":"...","tags":[{"tag":"some","confidence":0.95},...],"colors":[{"color":"green","ratio":0.45},...],"elapsedMs":1234}

        Options:
          -i, --input <path>      image to analyze (required)
          -o, --output <path>     write JSON to file instead of stdout
          -h, --help              show this message
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
}

private struct Tag: Codable {
    let tag: String
    let confidence: Float
}

private struct ColorResult: Codable {
    let color: String
    let ratio: Float
}

private struct PaletteColor {
    let r: Float
    let g: Float
    let b: Float
    let group: String
}

private let colorMinRatio: Float = 0.05
private let visionThreshold: Float = 0.2
private let thumbnailMaxPixelSize: Int = 512

private let palette: [PaletteColor] = [
    // Apple system colors
    PaletteColor(r: 1.00, g: 0.23, b: 0.19, group: "red"),
    PaletteColor(r: 1.00, g: 0.58, b: 0.00, group: "red"),
    PaletteColor(r: 1.00, g: 0.80, b: 0.00, group: "yellow"),
    PaletteColor(r: 0.20, g: 0.78, b: 0.35, group: "green"),
    PaletteColor(r: 0.00, g: 0.78, b: 0.75, group: "green"),
    PaletteColor(r: 0.35, g: 0.78, b: 0.91, group: "blue"),
    PaletteColor(r: 0.39, g: 0.82, b: 0.93, group: "blue"),
    PaletteColor(r: 0.00, g: 0.48, b: 1.00, group: "blue"),
    PaletteColor(r: 0.35, g: 0.34, b: 0.84, group: "blue"),
    PaletteColor(r: 0.75, g: 0.35, b: 0.95, group: "purple"),
    PaletteColor(r: 1.00, g: 0.17, b: 0.55, group: "red"),
    PaletteColor(r: 0.64, g: 0.52, b: 0.37, group: "yellow"),
    PaletteColor(r: 0.56, g: 0.56, b: 0.58, group: "gray"),
    // Custom descriptive
    PaletteColor(r: 0.53, g: 0.81, b: 0.92, group: "blue"),
    PaletteColor(r: 0.13, g: 0.55, b: 0.27, group: "green"),
    PaletteColor(r: 0.00, g: 0.39, b: 0.00, group: "green"),
    PaletteColor(r: 0.68, g: 0.85, b: 0.90, group: "blue"),
    PaletteColor(r: 0.00, g: 0.00, b: 0.55, group: "blue"),
    PaletteColor(r: 0.50, g: 0.50, b: 0.00, group: "green"),
    PaletteColor(r: 0.00, g: 0.00, b: 0.00, group: "gray"),
    PaletteColor(r: 0.33, g: 0.33, b: 0.33, group: "gray"),
    PaletteColor(r: 0.83, g: 0.83, b: 0.83, group: "gray"),
    PaletteColor(r: 1.00, g: 1.00, b: 1.00, group: "gray"),
]

private enum TaggerError: LocalizedError {
    case imageLoadFailed(String)
    case thumbnailFailed(String)
    case colorAnalysisFailed

    var errorDescription: String? {
        switch self {
        case .imageLoadFailed(let path): return "failed to load image: \(path)"
        case .thumbnailFailed(let path): return "failed to create thumbnail for: \(path)"
        case .colorAnalysisFailed: return "failed to create bitmap context for color analysis"
        }
    }
}

private struct CLIUsageError: Error {
    let message: String
}
