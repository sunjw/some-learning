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

        var threshold: Float = 0.2
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
            case "-t", "--threshold":
                guard i + 1 < args.count else {
                    printError("\(a) requires a value in [0, 1]")
                    exit(2)
                }
                guard let v = Float(args[i + 1]), (0...1).contains(v) else {
                    printError("invalid threshold: \(args[i + 1]) (expected a number in [0, 1])")
                    exit(2)
                }
                threshold = v
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

        let tags: [Tag]
        do {
            tags = try classify(imageAt: inputURL, threshold: threshold)
        } catch {
            printError("\(error)")
            exit(1)
        }

        do {
            try emitJSON(path: absolutePath(of: inputURL), tags: tags, to: outputURL)
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
        guard let imageSource = CGImageSourceCreateWithURL(url as CFURL, nil),
              let cgImage = CGImageSourceCreateImageAtIndex(imageSource, 0, nil) else {
            throw TaggerError.imageLoadFailed(url.path)
        }

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

    private static func emitJSON(path: String, tags: [Tag], to outputURL: URL?) throws {
        let payload = Result(path: path, tags: tags)
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
        usage: MLImageTags -i <image_path> [-o <output_path>] [-t <0..1>]

        Recognizes tags in an image using Apple's Vision framework
        (VNClassifyImageRequest) and prints (or writes) a JSON object:

          {"path":"...","tags":[{"tag":"some","confidence":0.95},...]}

        Options:
          -i, --input <path>      image to analyze (required)
          -o, --output <path>     write JSON to file instead of stdout
          -t, --threshold <0..1>  minimum confidence (default: 0.2)
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
