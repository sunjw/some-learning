import Foundation
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
        if args[1] == "-h" || args[1] == "--help" {
            printUsage()
            exit(0)
        }

        let url = resolveURL(args[1])
        guard FileManager.default.fileExists(atPath: url.path) else {
            printError("file not found: \(url.path)")
            exit(1)
        }

        let tags: [Tag]
        do {
            tags = try classify(imageAt: url)
        } catch {
            printError("\(error)")
            exit(1)
        }

        emitJSON(path: absolutePath(of: url), tags: tags)
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

    private static func classify(imageAt url: URL) throws -> [Tag] {
        guard let imageSource = CGImageSourceCreateWithURL(url as CFURL, nil),
              let cgImage = CGImageSourceCreateImageAtIndex(imageSource, 0, nil) else {
            throw TaggerError.imageLoadFailed(url.path)
        }

        let request = VNClassifyImageRequest()
        let handler = VNImageRequestHandler(cgImage: cgImage, options: [:])
        try handler.perform([request])

        let observations = (request.results ?? []).sorted { $0.confidence > $1.confidence }

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

    private static func emitJSON(path: String, tags: [Tag]) {
        let payload = Result(path: path, tags: tags)
        let encoder = JSONEncoder()
        encoder.outputFormatting = []
        do {
            let data = try encoder.encode(payload)
            FileHandle.standardOutput.write(data)
            FileHandle.standardOutput.write(Data("\n".utf8))
        } catch {
            printError("json encode failed: \(error)")
            exit(1)
        }
    }

    // MARK: - I/O helpers

    private static func printUsage() {
        let usage = """
        usage: MLImageTags <image_path>

        Recognizes tags in an image using Apple's Vision framework
        (VNClassifyImageRequest) and prints a JSON object:

          {"path":"...","tags":[{"tag":"a","confidence":0.95},...]}
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
}

private struct Tag: Codable {
    let tag: String
    let confidence: Float
}

private enum TaggerError: LocalizedError {
    case imageLoadFailed(String)

    var errorDescription: String? {
        switch self {
        case .imageLoadFailed(let path): return "failed to load image: \(path)"
        }
    }
}