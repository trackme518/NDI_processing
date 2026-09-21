// SendVideoAsync: Sends this sketch's visuals out as an NDI source using the
// asynchronous frame submission, which returns immediately instead of waiting
// for the frame to be sent.
// Adapted from the NDIP5 SendVideoAsyncExample.

import ndi.stream.*;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;

NDIP5Sender sender;
NDIP5VideoFrame videoFrame;

PGraphics canvas;

final int VIDEO_WIDTH = 1280;
final int VIDEO_HEIGHT = 720;
final int VIDEO_FPS = 60;
final int PIXEL_DEPTH = 4;

// Use two frame buffers because one will typically be in flight (being used by NDI send)
// while the other is being filled.
ByteBuffer[] frameBuffers = new ByteBuffer[2];
IntBuffer[] frameIntViews = new IntBuffer[2];

int frameCounter = 0;
long fpsPeriod = 0;

void setup() {
  size(640, 360);

  sender = new NDIP5Sender("NDI_p5 Processing Async Example");

  videoFrame = new NDIP5VideoFrame();
  videoFrame.setResolution(VIDEO_WIDTH, VIDEO_HEIGHT);
  videoFrame.setFourCCType(NDIP5FrameFourCCType.BGRA);
  videoFrame.setLineStride(VIDEO_WIDTH * PIXEL_DEPTH);
  videoFrame.setFrameRate(VIDEO_FPS, 1);

  canvas = createGraphics(VIDEO_WIDTH, VIDEO_HEIGHT);

  for (int i = 0; i < 2; i++) {
    frameBuffers[i] = ByteBuffer.allocateDirect(VIDEO_WIDTH * VIDEO_HEIGHT * PIXEL_DEPTH)
            .order(ByteOrder.LITTLE_ENDIAN);
    frameIntViews[i] = frameBuffers[i].asIntBuffer();
  }

  fpsPeriod = millis();
}

void draw() {
  // Render the scene once into the offscreen canvas
  canvas.beginDraw();
  drawGradient(canvas, frameCounter);
  canvas.endDraw();
  canvas.loadPixels();

  // Bulk-copy the rendered pixels into the buffer that currently isn't in flight.
  // PImage pixels are 0xAARRGGBB ints, which in little-endian memory are the
  // B,G,R,A byte order that the BGRA FourCC expects, so no per-pixel
  // conversion is needed: one array copy is enough.
  int bufferIndex = frameCounter & 1;
  frameIntViews[bufferIndex].position(0);
  frameIntViews[bufferIndex].put(canvas.pixels);
  videoFrame.setData(frameBuffers[bufferIndex]);

  // Submit the frame asynchronously.
  // This call returns immediately and the API will "own" the buffer until a synchronizing event.
  // A synchronizing event is one of: sendVideoFrameAsync, sendVideoFrame, close.
  sender.sendVideoFrameAsync(videoFrame);

  // Give an FPS message every 30 frames submitted
  if (frameCounter % 30 == 29) {
    long timeSpent = millis() - fpsPeriod;
    println("Sent 30 frames. Average FPS: " + 30f / (timeSpent / 1000f));
    fpsPeriod = millis();
  }

  // Preview the outgoing video in the sketch window
  image(canvas, 0, 0, width, height);
  fill(255);
  text("Sending asynchronously as \"NDI_p5 Processing Async Example\"", 10, 30);

  frameCounter++;
}

// A smooth moving rainbow gradient: hue runs diagonally across the frame
// and sweeps over time.
float gradientHue(float x, float y, int w, int h, int frame) {
  float p = (x + y) / (float) (w + h) + frame * 0.005f;
  p = p % 1f;
  return p < 0 ? p + 1 : p;
}

// One period of a rainbow wave (classic cosine-free HSV hue->channel)
float rainbowWave(float p) {
  p = p % 1f;
  if (p < 0) p += 1f;
  float x = abs(p * 6f - 3f);
  return constrain(x - 1f, 0f, 1f);
}

void drawGradient(PGraphics g, int frame) {
  g.noStroke();
  int step = 16;
  for (int y = 0; y < g.height; y += step) {
    for (int x = 0; x < g.width; x += step) {
      float hue = gradientHue(x, y, g.width, g.height, frame);
      g.fill(255 * rainbowWave(hue),
             255 * rainbowWave(hue - 1f / 3f),
             255 * rainbowWave(hue - 2f / 3f));
      g.rect(x, y, step, step);
    }
  }
}

void stop() {
  videoFrame.close();
  sender.close();
}
