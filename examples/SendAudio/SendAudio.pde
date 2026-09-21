// SendAudio: Sends generated audio (a C6 chord arpeggio) as NDI audio, synchronized
// with the video from the SendVideo example.
// Adapted from the NDIP5 SendAudioExample.

import p5.ndi.*;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

NDIP5Sender sender;
NDIP5AudioFrame audioFrame;

final int SAMPLE_RATE = 48000;
final int CHANNEL_COUNT = 4;
// One thirtieth of a second of audio per submission
final int SAMPLE_COUNT = SAMPLE_RATE / 30;

ByteBuffer data;

// Four sine wave frequencies (C major chord)
final float[] frequencyPerChannel = { 1046.50f, 1318.51f, 1567.98f, 2093.00f };
int[] totalSamplesPerChannel = new int[CHANNEL_COUNT];

void setup() {
  size(400, 300);

  // Create the sender, clocking the audio to match the sample rate
  sender = new NDIP5Sender("NDI_p5 Processing Audio Example", null, false, true);

  // Audio is stored as planar 32-bit floating point data
  data = ByteBuffer.allocateDirect((SAMPLE_COUNT * CHANNEL_COUNT * Float.SIZE) / Byte.SIZE)
          .order(ByteOrder.LITTLE_ENDIAN);

  audioFrame = new NDIP5AudioFrame();
  audioFrame.setSampleRate(SAMPLE_RATE);
  audioFrame.setChannels(CHANNEL_COUNT);
  audioFrame.setSamples(SAMPLE_COUNT);
  audioFrame.setData(data);
  audioFrame.setChannelStride((SAMPLE_COUNT * Float.SIZE) / Byte.SIZE);
}

void draw() {
  background(0);
  fill(255);
  text("Sending audio as \"NDI_p5 Processing Audio Example\"\nSee console output.", 20, 30);

  // Fill the buffer with one chunk of sine waves, one frequency per channel (planar)
  data.position(0);
  for (int ch = 0; ch < CHANNEL_COUNT; ch++) {
    for (int sample = 0; sample < SAMPLE_COUNT; sample++) {
      float val = (float) Math.sin(totalSamplesPerChannel[ch] * Math.PI * frequencyPerChannel[ch] * (1f / SAMPLE_RATE));
      data.putFloat(val);
      totalSamplesPerChannel[ch]++;
    }
  }
  data.flip();

  println("Sending " + SAMPLE_COUNT + " samples of audio data.");

  // Submit the audio. The sender paces this to match the sample rate.
  sender.sendAudioFrame(audioFrame);
}

void stop() {
  audioFrame.close();
  sender.close();
}
