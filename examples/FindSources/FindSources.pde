// FindSources: Lists all NDI sources found on the local network.
// NDI sources are any application on the network sending video with the NDI protocol
// (e.g. NDI Screen Capture, OBS with the NDI plugin, vMix, another Processing sketch...).

import ndi.stream.*;

NDIFinder finder;

String[] sourceNames = new String[0];

void setup() {
  size(640, 480);
  textFont(createFont("SansSerif", 16));

  finder = new NDIFinder();
}

void draw() {
  background(32);
  fill(240);

  text("NDI sources found on the network:", 20, 30);
  if (sourceNames.length == 0) {
    text("(searching...)", 20, 60);
  } else {
    for (int i = 0; i < sourceNames.length; i++) {
      text(sourceNames[i], 20, 60 + i * 22);
    }
  }

  // Update the list of sources whenever it changes (blocking for up to 100ms)
  if (finder.waitForSources(100)) {
    NDISource[] sources = finder.getCurrentSources();
    sourceNames = new String[sources.length];
    for (int i = 0; i < sources.length; i++) {
      sourceNames[i] = sources[i].getSourceName();
      println("Found source: " + sourceNames[i]);
    }
  }
}

void stop() {
  // Frees native memory faster than the garbage collector would
  finder.close();
}
