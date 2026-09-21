package p5.ndi;

/**
 * An internal interface to track where frames have been allocated.
 * This is implemented by {@link NDIP5Sender}, {@link NDIP5Receiver}, and {@link NDIP5FrameSync}, as they need to
 * free their own frame buffers.
 *
 * When a frame is returned by a receiver, sender, or framesync, it will eventually need to be freed by that same
 * receiver, so each frame stores a {@link NDIP5FrameCleaner} which it will call to destroy its buffer when needed.
 */
abstract class NDIP5FrameCleaner {
    abstract void freeVideo(NDIP5VideoFrame videoFrame);
    abstract void freeAudio(NDIP5AudioFrame audioFrame);
    abstract void freeMetadata(NDIP5MetadataFrame metadataFrame);
}
