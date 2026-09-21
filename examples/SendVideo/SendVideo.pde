// SendVideo: Sends this sketch's own visuals out as an NDI video source.
// Anything you can draw in Processing can be sent as NDI.

import ndi.stream.*;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;

NDISender sender;
NDIVideoFrame videoFrame;

PGraphics canvas;

final int VIDEO_WIDTH = 1280;
final int VIDEO_HEIGHT = 720;
final int VIDEO_FPS = 30;

// RGBA/BGRA has a pixel depth of 4
final int PIXEL_DEPTH = 4;

ByteBuffer data;
IntBuffer dataInts;

int frameCounter = 0;

void setup() {
  size(640, 360);

  // Create the sender with a name. Other applications will see it on the network under this name.
  sender = new NDISender("NDI_p5 Processing Example");

  videoFrame = new NDIVideoFrame();
  videoFrame.setResolution(VIDEO_WIDTH, VIDEO_HEIGHT);
  videoFrame.setFourCCType(NDIFrameFourCCType.BGRA);
  videoFrame.setFrameRate(VIDEO_FPS, 1);
  videoFrame.setLineStride(VIDEO_WIDTH * PIXEL_DEPTH);

  data = ByteBuffer.allocateDirect(VIDEO_WIDTH * VIDEO_HEIGHT * PIXEL_DEPTH)
          .order(ByteOrder.LITTLE_ENDIAN);
  dataInts = data.asIntBuffer();

  canvas = createGraphics(VIDEO_WIDTH, VIDEO_HEIGHT);
}

void draw() {
  // Render the outgoing video into the offscreen canvas
  canvas.beginDraw();
  canvas.background(16);
  canvas.noStroke();
  canvas.fill(255, 128 + 127 * sin(frameCounter * 0.05f), 64);
  canvas.ellipse(canvas.width / 2 + canvas.width / 3 * sin(frameCounter * 0.02f),
                 canvas.height / 2 + canvas.height / 4 * cos(frameCounter * 0.03f),
                 200, 200);
  canvas.fill(255);
  canvas.textSize(64);
  canvas.text("NDI_p5: frame " + frameCounter, 60, 100);
  canvas.endDraw();

  // Bulk-copy the rendered pixels into the frame buffer.
  // PImage pixels are 0xAARRGGBB ints, which in little-endian memory are the
  // B,G,R,A byte order that the BGRA FourCC expects, so no per-pixel
  // conversion is needed: loadPixels() + one array copy is enough.
  canvas.loadPixels();
  dataInts.position(0);
  dataInts.put(canvas.pixels);
  videoFrame.setData(data);

  // Submit the frame. The sender is clocked by default,
  // so this will submit at <= VIDEO_FPS fps.
  sender.sendVideoFrame(videoFrame);

  // Preview the outgoing video in the sketch window
  image(canvas, 0, 0, width, height);

  frameCounter++;
}

void stop() {
  videoFrame.close();
  sender.close();
}
