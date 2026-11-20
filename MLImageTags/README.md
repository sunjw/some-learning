# MLImageTags

A macOS command-line tool that analyzes an image and prints a JSON object describing it:

- **`tags`** — semantic labels produced by Apple's Vision framework, e.g. `outdoor`, `tree_plant`, `clownfish`
- **`colors`** — dominant color families with their share of the image, e.g. `green` 62%, `blue` 27%
- **`elapsedMs`** — wall-clock time spent analyzing the image

Everything runs locally. No network access, no API keys, no extra model downloads — only the frameworks that ship with macOS.

---

## Requirements

- macOS 26.3 or later
- A universal binary (arm64 + x86_64) is provided, so it runs natively on both Apple Silicon and Intel Macs

No network access, no API keys, no Python, no Homebrew packages, no third-party dependencies.

Download the `MLImageTags` binary and put it anywhere on your `PATH`:

```bash
chmod +x MLImageTags
sudo mv MLImageTags /usr/local/bin/
```

Verify it works:

```bash
MLImageTags --help
```

To use it without installing, just call it by path:

```bash
./MLImageTags -i ~/Pictures/forest.jpg
```

---

## Usage

```
MLImageTags -i <image_path> [-o <output_path>]
MLImageTags -d
```

| Option | Description |
| --- | --- |
| `-i`, `--input <path>` | Path of the image to analyze. Required unless `-d` is given. `~` is expanded. |
| `-o`, `--output <path>` | Write the JSON result to this file instead of printing it to stdout. `~` is expanded. |
| `-d`, `--daemon` | Run as a daemon: read image paths from stdin, write JSON results to stdout. Cannot be combined with `-i` or `-o`. |
| `-h`, `--help` | Print usage information and exit. |

### Examples

Print the result to stdout:

```bash
MLImageTags -i ~/Pictures/forest.jpg
```

```json
{"colors":[{"color":"black","ratio":0.4238682},{"color":"gold","ratio":0.4088389},{"color":"yellow","ratio":0.090588346},{"color":"gray","ratio":0.051119182}],"elapsedMs":265,"path":"/Users/me/Pictures/forest.jpg","tags":[{"confidence":0.6058059,"tag":"outdoor"},{"confidence":0.5756326,"tag":"land"},{"confidence":0.5649465,"tag":"road"}]}
```

Write the result to a file:

```bash
MLImageTags -i ~/Pictures/forest.jpg -o ./result.json
```

Use it in a shell pipeline:

```bash
MLImageTags -i ~/Pictures/forest.jpg | jq -r '.tags[] | "\(.tag)\t\(.confidence)"'
```

---

## Daemon mode

With `-d`, the tool becomes a long-running service that reads image paths from
stdin and writes one JSON object per line to stdout.

```bash
MLImageTags -d
```

```
stdin                                      stdout
────────────────────────────────────────────────────────────────────────
/Users/me/Pictures/forest.jpg        →     {"colors":[...],"elapsedMs":...,"path":"/Users/me/Pictures/forest.jpg","tags":[...]}
/Users/me/Pictures/My Photo.jpg       →     {"colors":[...],"elapsedMs":...,"path":"/Users/me/Pictures/My Photo.jpg","tags":[...]}
```

### Protocol

- **Input** — one image path per line, terminated by `\n`. A path may be
  written bare, or wrapped in matching single or double quotes (useful for
  paths containing spaces). Blank lines are ignored. A leading `~` is expanded
  to the home directory.
- **Output** — exactly one JSON object per input line, terminated by `\n`.
  Every line of input produces exactly one line of output, so a client can
  read and process replies synchronously.
- **Errors** — a failed image is reported as a regular JSON object with an
  extra `error` field and empty `tags` / `colors`, so the client never has to
  special-case the reply shape:

  ```json
  {"colors":[],"elapsedMs":0,"error":"failed to load image: /nope.jpg","path":"/nope.jpg","tags":[]}
  ```

- **Termination** — the daemon exits when stdin reaches end-of-file.

### Example client

```python
import json
import subprocess

proc = subprocess.Popen(
    ["MLImageTags", "-d"],
    stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True, bufsize=1,
)

for path in ["~/Pictures/forest.jpg", "~/Pictures/My Photo.jpg"]:
    proc.stdin.write(f'"{path}"\n')      # quotes are optional
    proc.stdin.flush()
    reply = json.loads(proc.stdout.readline())
    if "error" in reply:
        print("failed:", reply["error"])
    else:
        print(reply["path"], [t["tag"] for t in reply["tags"]])

proc.stdin.close()
proc.wait()
```

### Why it is faster

The daemon keeps the process — and therefore the Vision classification model —
alive between requests, so the model is loaded once instead of on every image.
The first request still pays the model-loading cost; subsequent ones are
noticeably faster than launching a fresh process per image.

---

## Output format

The output is a single JSON object. Keys are sorted alphabetically, so the output is byte-for-byte reproducible for the same image and the same binary.

```json
{
  "path": "/absolute/path/to/image.jpg",
  "colors": [
    { "color": "black", "ratio": 0.4238682 },
    { "color": "gold", "ratio": 0.4088389 }
  ],
  "tags": [
    { "tag": "outdoor", "confidence": 0.6058059 }
  ],
  "elapsedMs": 265
}
```

### `path`

The absolute path of the analyzed image.

### `error` (daemon mode only)

Present only when the image could not be analyzed. The value is a
human-readable description of what went wrong. When this field is present,
`tags` and `colors` are empty arrays.

### `tags`

An array of semantic labels, sorted by `confidence` (descending). Each entry contains:

| Field | Type | Description |
| --- | --- | --- |
| `tag` | string | The label, e.g. `outdoor`, `tree_plant`, `clownfish` |
| `confidence` | number | Confidence between `0.0` and `1.0` |

Only labels with a confidence of **0.2 or higher** are reported. The vocabulary is Apple's built-in image classification taxonomy, so it covers common subjects and scenes (`outdoor`, `sky`, `water`, `animal`, `vehicle`, `painting`, …) but not abstract or highly stylized images — for those the `tags` array may be empty.

### `colors`

An array of the dominant color families, sorted by `ratio` (descending). Each entry contains:

| Field | Type | Description |
| --- | --- | --- |
| `color` | string | One of `red`, `gold`, `yellow`, `green`, `blue`, `purple`, `black`, `gray`, `white` |
| `ratio` | number | Share of the (non-transparent) pixels belonging to that family, between `0.0` and `1.0` |

Colors are grouped into families by hue, with special handling for achromatic (gray) pixels:

- `red` — hue below 20° and above 345° (also covers magenta)
- `gold` — hue 20°–48° (amber, ochre, warm gold)
- `yellow` — hue 48°–70° (pure yellow)
- `green` — hue 70°–170°
- `blue` — hue 170°–250° (includes cyan and indigo)
- `purple` — hue 250°–320°
- `black` — low-saturation pixels with luminance below 0.15 (roughly `#262626` and darker)
- `white` — bright **and** desaturated pixels (luminance ≥ 0.70 and saturation < 0.15)
- `gray` — remaining low-saturation pixels

A color is reported when it covers at least **5%** of the image. If fewer than three colors qualify, the most prominent remaining ones are added (down to a 1% floor) so that small but visually salient subjects — for example an orange fish in green water — are not lost.

### `elapsedMs`

Total processing time in milliseconds, including image decoding, Vision classification, and color analysis.

---

## Exit codes

| Code | Meaning |
| --- | --- |
| `0` | Success |
| `1` | Runtime error (file not found, unsupported image, failed analysis) |
| `2` | Usage error (missing or unknown option, `-d` combined with `-i` / `-o`) |

Error messages are written to stderr:

```
error: failed to load image: /Users/me/does-not-exist.jpg
```

In daemon mode the process keeps running after a per-image failure — the error
is reported in the JSON reply instead, and the exit code only reflects whether
the daemon itself ran correctly.

---

## Supported image formats

Any format macOS can decode, including JPEG, PNG, HEIC/HEIF, TIFF, WebP, BMP and GIF (first frame). EXIF orientation is honoured, so photos taken in portrait mode are analysed upright.

---

## Performance

On an Apple Silicon Mac, a typical 4K image takes roughly **80–200 ms**. Very large images with wide-gamut (Display P3) colour profiles take longer, because the colour-space conversion dominates:

| Image | Typical time |
| --- | --- |
| Small JPEG | ~80 ms |
| 4K JPEG | ~150–200 ms |
| 6016 × 6016 Display P3 PNG | ~600–900 ms |

The Vision classification and the color analysis run concurrently, so the total time is roughly that of the slower of the two.

In daemon mode the per-image time drops further, because the Vision model stays
loaded between requests. The very first image in a daemon session still pays the
model-loading cost.

---

## How it works

- **Tags** — the full-resolution image is analyzed by Apple's Vision framework, which runs on the Neural Engine / GPU. No model download is needed; the classifier ships with macOS.
- **Colors** — the image is decoded once, and a 512 × 512 thumbnail is derived from it for the color pass. Every opaque pixel is converted to HSV and mapped to a color family; the counts are aggregated into ratios.
- Both passes run in parallel, so the total time is roughly that of the slower of the two.

---

## Notes and limitations

- The tag vocabulary is Apple's built-in image classification taxonomy. It is strong on natural scenes, objects and paintings, but weak on some categories — for example it may not identify a specific car model, and it returns nothing for abstract wallpapers.
- The color analysis deliberately groups hues into nine broad families rather than reporting exact RGB values. If you need precise colors, this tool is not the right fit.
- Only the first frame of an animated image is analyzed.
- The result is deterministic: the same image and the same binary always produce the same JSON, so the output can be diffed or checksummed.
