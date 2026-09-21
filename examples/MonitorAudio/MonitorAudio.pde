// MonitorAudio: Receives the first NDI source found on the network and plays its
// audio through the computer's speakers using javax.sound, with a frame-synchronizer
// keeping audio and video aligned.
// Adapted from the NDIStream MonitorExample.

import ndi.stream.*;

import javax.sound.sampled.*;
import java.nio.ByteBuffer;

NDIReceiver receiver;
NDIFrameSync frameSync;
NDIVideoFrame videoFrame;
NDIAudioFrame audioFrame;
NDIAudioFrameInterleaved16s interleaved16s;

SourceDataLine soundLine;

final int SAMPLE_RATE = 48000;
final int CHANNEL_COUNT = 2;
// Run at 30Hz
final float CLOCK_SPEED = 30;

void setup() {
  size(400, 300);

  // We will reformat the planar float data to 16-bit signed data for javax.sound
  AudioFormat audioFormat = new AudioFormat(SAMPLE_RATE, 16, CHANNEL_COUNT, true, false);
  DataLine.Info info = new DataLine.Info(SourceDataLine.class, audioFormat);
  try {
      soundLine = (SourceDataLine) AudioSystem.getLine(info);
      soundLine.open();
      soundLine.start();
  } catch (LineUnavailableException e) {
      throw new RuntimeException(e);
  }

  receiver = new NDIReceiver();

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

  videoFrame = new NDIVideoFrame();
  audioFrame = new NDIAudioFrame();

  // Setup frame to convert floating-point data to 16-bit signed data
  interleaved16s = new NDIAudioFrameInterleaved16s();
  interleaved16s.setReferenceLevel(20); // Recommended receiving level per NDI docs
  interleaved16s.setData(ByteBuffer.allocateDirect((int) (SAMPLE_RATE / CLOCK_SPEED) * CHANNEL_COUNT * Short.BYTES));

  // Attach the frame-synchronizer to ensure audio is dynamically resampled based on request frequency
  frameSync = new NDIFrameSync(receiver);
}

void draw() {
  background(0);
  fill(255);
  text("Monitoring audio. See console output.", 20, 30);

  // Capture a video frame. Only returns true if a video frame was returned.
  if (frameSync.captureVideo(videoFrame)) {
    println("Received video data: " + videoFrame.getFourCCType().name());
  }

  // Capture audio samples
  frameSync.captureAudio(audioFrame, SAMPLE_RATE, CHANNEL_COUNT, (int) (SAMPLE_RATE / CLOCK_SPEED));

  // Convert the given float data to interleaved 16-bit signed data
  NDIUtilities.planarFloatToInterleaved16s(audioFrame, interleaved16s);

  println("Received audio data: " + audioFrame.getSamples());

  // Get the audio data in a byte array, needed to write to a SourceDataLine
  byte[] audioData = new byte[audioFrame.getSamples() * Short.BYTES * audioFrame.getChannels()];
  interleaved16s.getData().get(audioData);

  // Write the audio data to the javax.sound api
  soundLine.write(audioData, 0, audioData.length);

  // Here is the clock. The frame-sync adapts the video and audio to match 30Hz with this.
  try {
    Thread.sleep((long) (1000 / CLOCK_SPEED));
  } catch (InterruptedException e) {
    // Ignore
  }

  frameRate(CLOCK_SPEED);
}

void stop() {
  videoFrame.close();
  audioFrame.close();
  // Make sure to close the framesync before the receiver
  frameSync.close();
  receiver.close();
  soundLine.close();
}
