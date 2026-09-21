// ReceiveVideo: Connects to the first NDI source found on the network and displays
// its video, scaled to fit the sketch window.

import ndi.stream.*;

NDIReceiver receiver;
NDIVideoFrame videoFrame;

PImage videoImage;

void setup() {
  size(1280, 720);

  // Receive the video in the BGRA/BGRX color format: in little-endian memory these bytes
  // are exactly the 0xAARRGGBB ints of PImage.pixels, so the copy below is a single bulk copy
  receiver = new NDIReceiver(NDIReceiver.ColorFormat.BGRX_BGRA, 100, false, "NDI Stream Processing Example");
  videoFrame = new NDIVideoFrame();

  // Find a source to connect to
  NDISource[] sources = null;
  try (NDIFinder finder = new NDIFinder()) {
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
  NDIFrameType frameType = receiver.receiveCapture(videoFrame, null, null, 0);

  if (frameType == NDIFrameType.VIDEO) {
    int w = videoFrame.getXResolution();
    int h = videoFrame.getYResolution();

    if (videoImage == null || videoImage.width != w || videoImage.height != h) {
      videoImage = createImage(w, h, ARGB);
    }

    // Bulk-copy the BGRA frame data into the PImage pixels (no per-pixel conversion)
    NDIUtilities.copyBufferToPixels(videoFrame.getData(), videoImage.pixels);
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
