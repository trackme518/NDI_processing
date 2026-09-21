// SendAudio16bpp: Sends generated audio as interleaved 16-bit signed NDI audio.
// Adapted from the NDIP5 SendAudio16bppExample.

import ndi.stream.*;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

NDIP5Sender sender;
NDIP5AudioFrameInterleaved16s audioFrame;

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

  sender = new NDIP5Sender("NDI_p5 Processing Audio 16bpp Example", null, false, true);

  // Interleaved 16-bit signed data: LRLRLR...
  data = ByteBuffer.allocateDirect((SAMPLE_COUNT * CHANNEL_COUNT * Short.SIZE) / Byte.SIZE)
          .order(ByteOrder.LITTLE_ENDIAN);

  audioFrame = new NDIP5AudioFrameInterleaved16s();
  audioFrame.setSampleRate(SAMPLE_RATE);
  audioFrame.setChannels(CHANNEL_COUNT);
  audioFrame.setSamples(SAMPLE_COUNT);
  audioFrame.setData(data);
}

void draw() {
  background(0);
  fill(255);
  text("Sending 16bpp audio as \"NDI_p5 Processing Audio 16bpp Example\"\nSee console output.", 20, 30);

  // Fill each buffer and send it
  data.position(0);
  for (int sample = 0; sample < SAMPLE_COUNT; sample++) {
    for (int ch = 0; ch < CHANNEL_COUNT; ch++) {
      short val = (short) (Math.sin(totalSamplesPerChannel[ch] * Math.PI * frequencyPerChannel[ch] * (1f / SAMPLE_RATE)) * Short.MAX_VALUE);
      data.putShort(val);
      totalSamplesPerChannel[ch]++;
    }
  }
  data.flip();

  println("Sending " + SAMPLE_COUNT + " samples of audio data.");

  sender.sendAudioFrameInterleaved16s(audioFrame);
}

void stop() {
  audioFrame.close();
  sender.close();
}
