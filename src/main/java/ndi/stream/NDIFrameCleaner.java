package ndi.stream;

/**
 * An internal interface to track where frames have been allocated.
 * This is implemented by {@link NDISender}, {@link NDIReceiver}, and {@link NDIFrameSync}, as they need to
 * free their own frame buffers.
 *
 * When a frame is returned by a receiver, sender, or framesync, it will eventually need to be freed by that same
 * receiver, so each frame stores a {@link NDIFrameCleaner} which it will call to destroy its buffer when needed.
 */
abstract class NDIFrameCleaner {
    abstract void freeVideo(NDIVideoFrame videoFrame);
    abstract void freeAudio(NDIAudioFrame audioFrame);
    abstract void freeMetadata(NDIMetadataFrame metadataFrame);
}
