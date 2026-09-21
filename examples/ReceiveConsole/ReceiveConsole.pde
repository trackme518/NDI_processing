// ReceiveConsole: Connects to the first NDI source found on the network and prints
// information about each received frame to the console.
// Adapted from NDIlib_Recv_Video.cpp / the NDIStream RecvExample.

import ndi.stream.*;

NDIReceiver receiver;
NDIVideoFrame videoFrame;
NDIAudioFrame audioFrame;
NDIMetadataFrame metadataFrame;

long startTime;

void setup() {
  size(400, 300);

  receiver = new NDIReceiver();
  videoFrame = new NDIVideoFrame();
  audioFrame = new NDIAudioFrame();
  metadataFrame = new NDIMetadataFrame();

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

  startTime = millis();
}

void draw() {
  background(0);
  fill(255);
  text("Receiving. See console output.", 20, 30);

  // Capture with a timeout of 5 milliseconds.
  // This call clears data previously allocated in the frames.
  switch (receiver.receiveCapture(videoFrame, audioFrame, metadataFrame, 5)) {
    case NONE:
      break;
    case VIDEO:
      println("Video data received (" + videoFrame.getXResolution() + "x" + videoFrame.getYResolution() + ", " +
              videoFrame.getFrameRateN() + "/" + videoFrame.getFrameRateD() + ").");
      break;
    case AUDIO:
      println("Audio data received (" + audioFrame.getSamples() + ", " + audioFrame.getChannelStride() + ").");
      break;
    case METADATA:
      println("Metadata received (" + metadataFrame.getData() + ").");
      break;
  }

  if (receiver.getConnectionCount() < 1) {
    println("Lost connection.");
  }
}

void stop() {
  videoFrame.close();
  audioFrame.close();
  metadataFrame.close();
  receiver.close();
}
