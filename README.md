# NDIStream for Processing

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

**Library info**

- **Download:** latest release - [`NDIStream.zip`](https://github.com/trackme518/NDI_processing/releases/latest/download/NDIStream.zip)
- **Last updated:** September 2026 (v4.0.0)
- **Tested with:** latest Processing version 4.4 on macOS (Apple Silicon and Intel);
  Windows x86-64 and Linux x86-64 are built and packaged with every release via GitHub Actions
- **Dependencies:** none at runtime for the default all-in-one build (NDI runtime bundled);
  the `system-runtime` variant requires the free
  [NDI Runtime](https://ndi.video/tools/) installed on the system
- **Keywords:** NDI, video, audio, network, IP video, streaming, sender, receiver, discovery, routing
- **License:** GPL-3.0 (see [License](#license))

## Installation

NDIStream is published in two release variants:

- **`NDIStream-allinone.zip`** - the **default**. The NDI runtime (`libndi`) and native bindings are
  bundled inside the library jar, so it works out of the box with no separate NDI install. For
  macOS this covers Apple Silicon and Intel from a single jar.
- **`NDIStream-system-runtime.zip`** - a fully GPL-3 distribution that ships **only** this library's
  own code (no proprietary NDI binaries). It requires the free NDI Runtime to be installed on the
  machine. Choose this when you need a build free of the proprietary NDI runtime (see
  [License](#license)).

Both install the same `NDIStream` library - **do not install both** into the same sketchbook
(duplicate classes on the classpath).

### Option 1: Install from this repository (Contribution Manager)

1. Open Processing and go to `Sketch` > `Import Library...` > `Add Library...`
2. In the Contribution Manager, click the `Settings` tab and add this repository's
   `library.properties` URL (or the raw URL of this repo) as a custom library source.
   The Contribution Manager entry points at the default `allinone` build.

### Option 2: Manual install

Run the gradle task `deployToProcessingSketchbook` (see [Building](#building)), or unzip
`NDIStream-allinone.zip` (or `NDIStream-system-runtime.zip`) from the latest release into your
Processing sketchbook's `libraries` folder:

```
Documents/Processing/libraries/NDIStream/
├── library.properties
├── library/NDIStream.jar
├── examples/...
├── reference/...
└── src/...
```

Restart Processing; the library then appears under `Sketch` > `Import Library...`.

## Usage

Add the import to your sketch:

```java
import ndi.stream.*;
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
import ndi.stream.*;

NDIReceiver receiver = new NDIReceiver();
NDIVideoFrame videoFrame = new NDIVideoFrame();

void settings() { size(1280, 720); }

void setup() {
  try (NDIFinder finder = new NDIFinder()) {
    finder.waitForSources(5000);
    receiver.connect(finder.getCurrentSources()[0]);
  }
}

void draw() {
  if (receiver.receiveCapture(videoFrame, null, null, 0) == NDIFrameType.VIDEO) {
    // display frames (see the ReceiveVideo example for the pixel copy)
  }
}
```

NDIStream aims to be close to the original NDI SDK while still following Java standards and
conventions. The vast majority of applications can be simply translated from NDI SDK calls to
NDIStream calls. Javadocs are in `reference/` (or run `./gradlew javadoc`).

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

### Release variants

`buildReleaseArtifacts` selects the distribution with `-Pvariant`:

```
./gradlew buildReleaseArtifacts                        # allinone (default): bundles the NDI runtime
./gradlew buildReleaseArtifacts -Pvariant=system-runtime  # GPL-3 only: relies on an installed NDI runtime
```

- **allinone** - bundles the NDI runtime (`libndi`) into the jar for out-of-the-box use. The jar is
  an aggregate of GPL-3 code and proprietary NDI binaries (see [License](#license)).
- **system-runtime** - bundles only this library's own code and JNI bindings, so the jar is fully
  GPL-3. It requires the free NDI Runtime to be installed on the user's system. The NDI SDK is
  still needed at build time to compile the JNI bindings against its headers.

macOS builds on Apple Silicon produce both `arm64` and `x86-64` slices of the JNI library, and the
bundled `libndi.dylib` from the NDI SDK is universal, so the jar works on both Apple Silicon and
Intel Macs (including the native arm64 build of Processing 4).

Linux and Windows `allinone` builds bundle the NDI runtime the same way when the SDK is found at
build time; otherwise a system NDI runtime install is required at run time. `system-runtime` builds
never bundle the runtime on any platform.

## License

The NDIStream source code and everything in this repository are licensed under the
**GNU General Public License v3.0** (see [LICENSE](LICENSE)). This covers the Java library, the
JNI bindings, the build scripts, and the examples.

## ⚠️Licensing Considerations⚠️

The GPL-3 license covers **our code only**. Of the two release variants (see [Installation](#installation)):

- The **`system-runtime`** jar contains **only** our GPL-3 code (Java + our JNI bindings) and
  bundles **no** proprietary NDI binaries - it is fully GPL-3 and relies on an NDI Runtime
  installed separately on the user's system.
- The **`allinone`** jar (the default) additionally bundles the **proprietary NDI runtime**
  (`libndi.dylib` / `libndi.so` / `ndi.dll`) from the NDI SDK so it works out of the box. Those
  runtime binaries are **not** GPL-licensed; they remain under the separate, proprietary NDI SDK /
  NDI Runtime License Agreement you accept when you install the SDK or runtime. The two parts are
  distributed as an aggregate with license terms separated per file.

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
