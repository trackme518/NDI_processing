// ReceiveVideo: Connects to the first NDI source found on the network and displays
// its video, scaled to fit the sketch window.

import p5.ndi.*;

NDIP5Receiver receiver;
NDIP5VideoFrame videoFrame;

PImage videoImage;

void setup() {
  size(1280, 720);

  // Receive the video in the RGBA color format, which maps directly to PImage pixels
  receiver = new NDIP5Receiver(NDIP5Receiver.ColorFormat.RGBX_RGBA, 100, false, "NDI_p5 Processing Example");
  videoFrame = new NDIP5VideoFrame();

  // Find a source to connect to
  NDIP5Source[] sources = null;
  try (NDIP5Finder finder = new NDIP5Finder()) {
    while ((sources = finder.getCurrentSources()).length == 0) {
      println("Waiting for sources...");
      finder.waitForSources(5000);
    }
    println("Connecting to source: " + sources[0].getSourceName());
    receiver.connect(sources[0]);
  }
}

void draw() {
  background(0);

  // Non-blocking capture of one frame element
  NDIP5FrameType frameType = receiver.receiveCapture(videoFrame, null, null, 0);

  if (frameType == NDIP5FrameType.VIDEO) {
    int w = videoFrame.getXResolution();
    int h = videoFrame.getYResolution();

    if (videoImage == null || videoImage.width != w || videoImage.height != h) {
      videoImage = createImage(w, h, ARGB);
    }

    // Copy the RGBA frame data into the PImage
    java.nio.ByteBuffer data = videoFrame.getData();
    data.rewind();
    videoImage.loadPixels();
    for (int i = 0; i < w * h; i++) {
      int r = data.get() & 0xFF;
      int g = data.get() & 0xFF;
      int b = data.get() & 0xFF;
      int a = data.get() & 0xFF;
      videoImage.pixels[i] = (a << 24) | (r << 16) | (g << 8) | b;
    }
    videoImage.updatePixels();
  }

  if (videoImage != null) {
    // Display, scaled to fit the window while preserving the aspect ratio
    float scale = min((float) width / videoImage.width, (float) height / videoImage.height);
    image(videoImage, 0, 0, (int) (videoImage.width * scale), (int) (videoImage.height * scale));
  }

  if (receiver.getConnectionCount() < 1) {
    println("Lost connection.");
  }
}

void stop() {
  videoFrame.close();
  receiver.close();
}
