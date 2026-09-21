# NDI_p5 for Processing

A [Processing](https://processing.org) (4) library for sending and receiving video and audio over
the network using the NewTek NDI® SDK. For more information about NDI®, see:

http://NDI.NewTek.com/

NDI sources from other applications (NDI Screen Capture, OBS, vMix, TriCasters, ...) are received
as `PImage` video and audio buffers, and anything you draw in a sketch can be transmitted as an
NDI source.

**Author:** Vojtech Leischner.

This project is a fork of [Devolay](https://github.com/WalkerKnapp/devolay) by Walker Knapp
(originally Apache-2.0 licensed) and carries over its substantial original source; upstream
copyright is retained by its author. The fork's additions and modifications are licensed under
GPL-3.0 (see [LICENSE](LICENSE)), which is compatible with the original Apache-2.0 grant.

## Installation

The NDI runtime and native bindings are bundled inside the library jar for macOS
(Apple Silicon and Intel), so no separate NDI installation is needed there.

### Option 1: Install from this repository (Contribution Manager)

1. Open Processing and go to `Sketch` > `Import Library...` > `Add Library...`
2. In the Contribution Manager, click the `Settings` tab and add this repository's
   `library.properties` URL (or the raw URL of this repo) as a custom library source.

### Option 2: Manual install

Run the gradle task `deployToProcessingSketchbook` (see [Building](#building)), or unzip
`NDI_p5.zip` from the latest release into your Processing sketchbook's `libraries` folder:

```
Documents/Processing/libraries/NDI_p5/
├── library.properties
├── library/NDI_p5.jar
├── examples/...
├── reference/...
└── src/...
```

Restart Processing; the library then appears under `Sketch` > `Import Library...`.

## Usage

Add the import to your sketch:

```java
import p5.ndi.*;
```

The `examples` folder contains ready-to-run Processing sketches:

| Example | Description |
|---|---|
| `FindSources` | Lists all NDI sources found on the local network |
| `ReceiveVideo` | Displays the video of the first found NDI source |
| `ReceiveConsole` | Prints info about received video/audio/metadata frames |
| `MonitorAudio` | Plays the audio of the first found source through the speakers |
| `FrameSync` | Receives audio + video paced by a frame-synchronizer |
| `SendVideo` | Transmits the sketch's own visuals as an NDI source |
| `SendVideoAsync` | Asynchronous (non-blocking) video frame submission |
| `SendAudio` | Sends generated planar float audio |
| `SendAudio16bpp` | Sends generated interleaved 16-bit audio |
| `Routing` | A software switcher routing between network sources |

A minimal receiver:

```java
import p5.ndi.*;

NDIP5Receiver receiver = new NDIP5Receiver();
NDIP5VideoFrame videoFrame = new NDIP5VideoFrame();

void settings() { size(1280, 720); }

void setup() {
  try (NDIP5Finder finder = new NDIP5Finder()) {
    finder.waitForSources(5000);
    receiver.connect(finder.getCurrentSources()[0]);
  }
}

void draw() {
  if (receiver.receiveCapture(videoFrame, null, null, 0) == NDIP5FrameType.VIDEO) {
    // display frames (see the ReceiveVideo example for the pixel copy)
  }
}
```

NDI_p5 aims to be close to the original NDI SDK while still following Java standards and
conventions. The vast majority of applications can be simply translated from NDI SDK calls to
NDI_p5 calls. Javadocs are in `reference/` (or run `./gradlew javadoc`).

## Building

The build is self-contained: the Gradle wrapper, JDK 17 (the version Processing 4 uses), and all
Gradle artifacts are downloaded into this repository (`.toolchain/`, `.gradle-home/`) - nothing is
installed globally.

### Requirements
- A C++ compiler for your OS (`clang++` on macOS, `g++` on Linux, MSVC `cl` on Windows)
- The [NewTek NDI SDK](https://www.ndi.tv/sdk/) - either installed at the default location, set via
  the `NDI_SDK_DIR` environment variable / `-DndiSdk=<path>`, or placed in the repository root
  as `NDI SDK for Apple`, `NDI SDK for Linux`, etc. Its headers are used to compile the JNI
  bindings, and its runtime (`libndi`) is bundled into the jar.

### Tasks

```
./gradlew build                        # compile the library + JNI natives for the host platform
./gradlew buildReleaseArtifacts        # create release/ (library, examples, javadoc, zip, pdex)
./gradlew deployToProcessingSketchbook # build + install into the Processing sketchbook
./gradlew javadoc                      # generate reference docs
```

macOS builds on Apple Silicon produce both `arm64` and `x86-64` slices of the JNI library, and the
bundled `libndi.dylib` from the NDI SDK is universal, so the jar works on both Apple Silicon and
Intel Macs (including the native arm64 build of Processing 4).

Linux and Windows builds bundle the NDI runtime the same way when the SDK is found at build time;
otherwise a system NDI runtime install is required at run time.

## License

The NDI_p5 source code and everything in this repository are licensed under the
**GNU General Public License v3.0** (see [LICENSE](LICENSE)). This covers the Java library, the
JNI bindings, the build scripts, and the examples.

## ⚠️Licensing Considerations⚠️

The GPL-3 license covers **our code only**. The binaries published in the GitHub releases are an
"integrated" distribution: the jar also bundles the **proprietary NDI runtime** (`libndi.dylib` /
`libndi.so` / `ndi.dll`) from the NDI SDK. Those binaries are **not** GPL-licensed; they remain
under the separate, proprietary NDI SDK / NDI Runtime License Agreement you accept when you
install the SDK or runtime, and the two parts are distributed as an aggregate with license terms
separated per file.

If you **build from source**, the resulting artifacts contain no NDI binaries from us: our code
stays plain GPL-3. Note however that compiling the JNI bindings requires the NDI SDK headers
(downloading the SDK means accepting its license agreement), and running requires an NDI runtime
on the machine (its own free license). The NDI license governs those NDI components only - it
never applies to the GPL-3 code in this repository, and it imposes no restrictions on you unless
you redistribute the NDI binaries yourself.

If you distribute products that bundle the NDI runtime (including our release jars), you **must**
follow the guidelines in section 5.2 of the NDI SDK Documentation.

For full details, please install the NDI SDK and read the NDI SDK Documentation and NDI SDK
License Agreement. To summarize to the best of my knowledge (not legal advice, please get legal
consultation for any serious application), applications must:
- Provide a link to [http://ndi.tv/](http://ndi.tv/) where NDI is used in the application, on its website, and in its documentation.
- Refer to NDI, the product, with "NDI®", and contain the phrase "NDI® is a registered trademark of NewTek, Inc." on the same page where it is used (this only applies to the first use of "NDI" in a document).
- Include the phrase "NDI® is a registered trademark of NewTek, Inc." in any About Box and other locations where trademark attribution is provided.
