package ndi.stream;

public class NDIUtilities {

    /**
     * Bulk-copy a received BGRA/BGRX video frame buffer into a Processing-style pixel array.
     * <p>
     * Processing {@code PImage} pixels are {@code 0xAARRGGBB} ints, which in little-endian
     * memory is exactly the BGRA byte order produced by
     * {@link NDIReceiver.ColorFormat#BGRX_BGRA}, so this is a single bulk int-array copy with
     * no per-pixel conversion (and no image format allocation).
     *
     * @param buffer The frame data from {@link NDIVideoFrame#getData()}, BGRA/BGRX bytes
     * @param pixels The destination pixel array, e.g. {@code PImage.pixels}
     */
    public static void copyBufferToPixels(java.nio.ByteBuffer buffer, int[] pixels) {
        java.nio.ByteBuffer src = buffer.duplicate().order(java.nio.ByteOrder.LITTLE_ENDIAN);
        int count = Math.min(pixels.length, src.capacity() / 4);
        src.limit(count * 4);
        src.asIntBuffer().get(pixels, 0, count);
    }

    /**
     * Bulk-copy a Processing-style pixel array into a BGRA/BGRX video frame buffer.
     * <p>
     * Inverse of {@link #copyBufferToPixels(java.nio.ByteBuffer, int[])}: {@code 0xAARRGGBB}
     * ints written little-endian yield the BGRA byte order expected by
     * {@link NDIFrameFourCCType#BGRA} frames.
     *
     * @param pixels The source pixel array, e.g. {@code PImage.pixels}
     * @param buffer The destination buffer for {@link NDIVideoFrame#setData(java.nio.ByteBuffer)}
     */
    public static void copyPixelsToBuffer(int[] pixels, java.nio.ByteBuffer buffer) {
        java.nio.ByteBuffer dst = buffer.duplicate().order(java.nio.ByteOrder.LITTLE_ENDIAN);
        int count = Math.min(pixels.length, dst.capacity() / 4);
        dst.limit(count * 4);
        dst.asIntBuffer().put(pixels, 0, count);
    }

    /**
     * Convert from the standard planar floating-point audio to interleaved 16s audio.
     *
     * @param srcFrame The frame to take input floating-point data from
     * @param targetFrame The frame to write output interleaved data to.
     */
    public static void planarFloatToInterleaved16s(NDIAudioFrame srcFrame, NDIAudioFrameInterleaved16s targetFrame) {
        convertToInterleaved16s(srcFrame.structPointer, targetFrame.structPointer);
    }

    /**
     * Convert from interleaved 16s audio to the standard floating-point audio.
     *
     * @param srcFrame The frame to take interleaved data from.
     * @param targetFrame The frame to write output floating-point data to.
     */
    public static void interleaved16sToPlanarFloat(NDIAudioFrameInterleaved16s srcFrame, NDIAudioFrame targetFrame) {
        targetFrame.freeBuffer();
        convertFromInterleaved16s(srcFrame.structPointer, targetFrame.structPointer);
    }

    /**
     * Convert from the standard planar floating-point audio to interleaved 32s audio.
     *
     * @param srcFrame The frame to take input floating-point data from.
     * @param targetFrame The frame to write output interleaved data to.
     */
    public static void planarFloatToInterleaved32s(NDIAudioFrame srcFrame, NDIAudioFrameInterleaved32s targetFrame) {
        convertToInterleaved32s(srcFrame.structPointer, targetFrame.structPointer);
    }

    /**
     * Convert from interleaved 32s audio data to the standard planar floating-point audio.
     *
     * @param srcFrame The frame to take input interleaved data from.
     * @param targetFrame The frame to write output floating-point data to.
     */
    public static void interleaved32sToPlanarFloat(NDIAudioFrameInterleaved32s srcFrame, NDIAudioFrame targetFrame) {
        targetFrame.freeBuffer();
        convertFromInterleaved32s(srcFrame.structPointer, targetFrame.structPointer);
    }

    /**
     * Convert from the standard floating-point audio data to interleaved floating-point audio data.
     *
     * @param srcFrame The frame to take input floating-point data from.
     * @param targetFrame The frame to write output interleaved floating-point data to.
     */
    public static void planarFloatToInterleavedFloat(NDIAudioFrame srcFrame, NDIAudioFrameInterleaved32f targetFrame) {
        convertToInterleaved32f(srcFrame.structPointer, targetFrame.structPointer);
    }

    /**
     * Convert from interleaved floating-point audio data to the standard floating-point audio data.
     *
     * @param srcFrame The frame to take input interleaved floating-point data from.
     * @param targetFrame The frame to write output floating-point data to.
     */
    public static void interleavedFloatToPlanarFloat(NDIAudioFrameInterleaved32f srcFrame, NDIAudioFrame targetFrame) {
        targetFrame.freeBuffer();
        convertFromInterleaved32f(srcFrame.structPointer, targetFrame.structPointer);
    }

    // Native methods

    private static native void convertToInterleaved16s(long pSrcFrame, long pDstFrame);
    private static native void convertFromInterleaved16s(long pSrcFrame, long pDstFrame);
    private static native void convertToInterleaved32s(long pSrcFrame, long pDstFrame);
    private static native void convertFromInterleaved32s(long pSrcFrame, long pDstFrame);
    private static native void convertToInterleaved32f(long pSrcFrame, long pDstFrame);
    private static native void convertFromInterleaved32f(long pSrcFrame, long pDstFrame);
}
