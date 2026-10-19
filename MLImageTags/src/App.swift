import Foundation
import CoreGraphics
import Vision
import ImageIO

@main
struct App {
    static func main() {
        let args = CommandLine.arguments
        guard args.count >= 2 else {
            printUsage()
            exit(2)
        }

        var inputArg: String?
        var outputArg: String?

        var i = 1
        while i < args.count {
            let a = args[i]
            switch a {
            case "-h", "--help":
                printUsage()
                exit(0)
            case "-i", "--input":
                guard i + 1 < args.count else {
                    printError("\(a) requires a path")
                    exit(2)
                }
                inputArg = args[i + 1]
                i += 2
            case "-o", "--output":
                guard i + 1 < args.count else {
                    printError("\(a) requires a path")
                    exit(2)
                }
                outputArg = args[i + 1]
                i += 2
            default:
                if a.hasPrefix("-") {
                    printError("unknown option: \(a)")
                    exit(2)
                }
                printError("unexpected positional argument: \(a)")
                exit(2)
            }
        }

        guard let inputArg else {
            printError("missing required -i / --input <path>")
            printUsage()
            exit(2)
        }

        let inputURL = resolveURL(inputArg)
        guard FileManager.default.fileExists(atPath: inputURL.path) else {
            printError("file not found: \(inputURL.path)")
            exit(1)
        }

        let outputURL: URL? = outputArg.map { resolveURL($0) }

        let startTime = Date()

        let tags: [Tag]
        do {
            tags = try classify(imageAt: inputURL, threshold: 0.2)
        } catch {
            printError("\(error)")
            exit(1)
        }

        let colors: [ColorResult]
        do {
            colors = try analyzeColors(imageAt: inputURL)
        } catch {
            printError("\(error)")
            exit(1)
        }

        let elapsedMs = Int(Date().timeIntervalSince(startTime) * 1000)

        do {
            try emitJSON(path: absolutePath(of: inputURL), tags: tags, colors: colors, elapsedMs: elapsedMs, to: outputURL)
        } catch {
            printError("failed to write output: \(error)")
            exit(1)
        }
    }

    // MARK: - Steps

    private static func resolveURL(_ raw: String) -> URL {
        if raw.hasPrefix("~") {
            return URL(fileURLWithPath: (raw as NSString).expandingTildeInPath)
        }
        return URL(fileURLWithPath: raw)
    }

    private static func absolutePath(of url: URL) -> String {
        if url.path.hasPrefix("/") { return url.path }
        return FileManager.default.currentDirectoryPath + "/" + url.path
    }

    private static func classify(imageAt url: URL, threshold: Float) throws -> [Tag] {
        let cgImage = try loadCGImage(at: url)

        let request = VNClassifyImageRequest()
        let handler = VNImageRequestHandler(cgImage: cgImage, options: [:])
        try handler.perform([request])

        let observations = (request.results ?? [])
            .filter { $0.confidence >= threshold }
            .sorted { $0.confidence > $1.confidence }

        var seen = Set<String>()
        var tags: [Tag] = []
        for obs in observations {
            let identifier = obs.identifier.trimmingCharacters(in: .whitespacesAndNewlines)
            guard !identifier.isEmpty, !seen.contains(identifier) else { continue }
            seen.insert(identifier)
            tags.append(Tag(tag: identifier, confidence: obs.confidence))
        }
        return tags
    }

    private static func analyzeColors(imageAt url: URL) throws -> [ColorResult] {
        let cgImage = try loadCGImage(at: url, maxPixelSize: 512)

        let width = cgImage.width
        let height = cgImage.height
        guard width > 0, height > 0 else { return [] }

        let bytesPerPixel = 4
        let bytesPerRow = width * bytesPerPixel
        let totalPixels = width * height

        var pixels = [UInt8](repeating: 0, count: totalPixels * bytesPerPixel)
        let colorSpace = CGColorSpace(name: CGColorSpace.sRGB) ?? CGColorSpaceCreateDeviceRGB()
        let bitmapInfo = CGImageAlphaInfo.premultipliedLast.rawValue
            | CGBitmapInfo.byteOrder32Big.rawValue

        guard let context = CGContext(
            data: &pixels,
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

        var counts: [Int] = Array(repeating: 0, count: palette.count)
        var i = 0
        while i < pixels.count {
            let r = Float(pixels[i]) / 255.0
            let g = Float(pixels[i + 1]) / 255.0
            let b = Float(pixels[i + 2]) / 255.0
            counts[nearestPaletteIndex(r: r, g: g, b: b)] += 1
            i += bytesPerPixel
        }

        let total = Float(totalPixels)
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
        result.sort { $0.ratio > $1.ratio }
        return result
    }

    private static func loadCGImage(at url: URL, maxPixelSize: Int = 0) throws -> CGImage {
        guard let imageSource = CGImageSourceCreateWithURL(url as CFURL, nil) else {
            throw TaggerError.imageLoadFailed(url.path)
        }

        if maxPixelSize > 0 {
            let options: [CFString: Any] = [
                kCGImageSourceCreateThumbnailFromImageAlways: true,
                kCGImageSourceCreateThumbnailWithTransform: true,
                kCGImageSourceShouldCacheImmediately: true,
                kCGImageSourceThumbnailMaxPixelSize: maxPixelSize,
            ]
            if let thumbnail = CGImageSourceCreateThumbnailAtIndex(
                imageSource, 0, options as CFDictionary
            ) {
                return thumbnail
            }
        }

        guard let cgImage = CGImageSourceCreateImageAtIndex(imageSource, 0, nil) else {
            throw TaggerError.imageLoadFailed(url.path)
        }
        return cgImage
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
        let payload = Result(path: path, tags: tags, colors: colors, elapsedMs: elapsedMs)
        let encoder = JSONEncoder()
        encoder.outputFormatting = []
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

private struct Result: Codable {
    let path: String
    let tags: [Tag]
    let colors: [ColorResult]
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
    let name: String
    let r: Float
    let g: Float
    let b: Float
    let group: String
}

private let colorMinRatio: Float = 0.05

private let palette: [PaletteColor] = [
    // Apple system colors (13)
    PaletteColor(name: "red",    r: 1.00, g: 0.23, b: 0.19, group: "red"),
    PaletteColor(name: "orange", r: 1.00, g: 0.58, b: 0.00, group: "red"),
    PaletteColor(name: "yellow", r: 1.00, g: 0.80, b: 0.00, group: "yellow"),
    PaletteColor(name: "green",  r: 0.20, g: 0.78, b: 0.35, group: "green"),
    PaletteColor(name: "mint",   r: 0.00, g: 0.78, b: 0.75, group: "green"),
    PaletteColor(name: "teal",   r: 0.35, g: 0.78, b: 0.91, group: "blue"),
    PaletteColor(name: "cyan",   r: 0.39, g: 0.82, b: 0.93, group: "blue"),
    PaletteColor(name: "blue",   r: 0.00, g: 0.48, b: 1.00, group: "blue"),
    PaletteColor(name: "indigo", r: 0.35, g: 0.34, b: 0.84, group: "blue"),
    PaletteColor(name: "purple", r: 0.75, g: 0.35, b: 0.95, group: "purple"),
    PaletteColor(name: "pink",   r: 1.00, g: 0.17, b: 0.55, group: "red"),
    PaletteColor(name: "brown",  r: 0.64, g: 0.52, b: 0.37, group: "yellow"),
    PaletteColor(name: "gray",   r: 0.56, g: 0.56, b: 0.58, group: "gray"),
    // Custom descriptive (10)
    PaletteColor(name: "sky_blue",     r: 0.53, g: 0.81, b: 0.92, group: "blue"),
    PaletteColor(name: "forest_green", r: 0.13, g: 0.55, b: 0.27, group: "green"),
    PaletteColor(name: "dark_green",   r: 0.00, g: 0.39, b: 0.00, group: "green"),
    PaletteColor(name: "light_blue",   r: 0.68, g: 0.85, b: 0.90, group: "blue"),
    PaletteColor(name: "dark_blue",    r: 0.00, g: 0.00, b: 0.55, group: "blue"),
    PaletteColor(name: "olive",        r: 0.50, g: 0.50, b: 0.00, group: "green"),
    PaletteColor(name: "black",        r: 0.00, g: 0.00, b: 0.00, group: "gray"),
    PaletteColor(name: "dark_gray",    r: 0.33, g: 0.33, b: 0.33, group: "gray"),
    PaletteColor(name: "light_gray",   r: 0.83, g: 0.83, b: 0.83, group: "gray"),
    PaletteColor(name: "white",        r: 1.00, g: 1.00, b: 1.00, group: "gray"),
]

private enum TaggerError: LocalizedError {
    case imageLoadFailed(String)
    case colorAnalysisFailed

    var errorDescription: String? {
        switch self {
        case .imageLoadFailed(let path): return "failed to load image: \(path)"
        case .colorAnalysisFailed: return "failed to create bitmap context for color analysis"
        }
    }
}
