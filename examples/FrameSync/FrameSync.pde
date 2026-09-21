// FrameSync: Receives from the first NDI source found using a frame-synchronizer,
// which resamples audio and paces video so both arrive at a fixed rate.
// Adapted from the NDIP5 FrameSyncExample / NDIlib_Recv_FrameSync.cpp.

import p5.ndi.*;

NDIP5Receiver receiver;
NDIP5FrameSync frameSync;
NDIP5VideoFrame videoFrame;
NDIP5AudioFrame audioFrame;

// Run at 30Hz
final float CLOCK_SPEED = 30;

void setup() {
  size(400, 300);

  receiver = new NDIP5Receiver();

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

  videoFrame = new NDIP5VideoFrame();
  audioFrame = new NDIP5AudioFrame();

  // Attach the frame-synchronizer to ensure audio is dynamically resampled based on request frequency
  frameSync = new NDIP5FrameSync(receiver);

  frameRate(CLOCK_SPEED);
}

void draw() {
  background(0);
  fill(255);
  text("Frame-syncing receive. See console output.", 20, 30);

  // Capture a video frame. Only returns true if a video frame was returned.
  if (frameSync.captureVideo(videoFrame)) {
    println("Received video data: " + videoFrame.getFourCCType().name());
  }

  // Capture audio samples
  frameSync.captureAudio(audioFrame, 48000, 2, (int) (48000 / CLOCK_SPEED));
  println("Received audio data: " + audioFrame.getSamples());
}

void stop() {
  videoFrame.close();
  audioFrame.close();
  // Make sure to close the framesync before the receiver
  frameSync.close();
  receiver.close();
}
