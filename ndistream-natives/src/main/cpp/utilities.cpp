#include "devolay.h"

#include "ndi_stream_NDIUtilities.h"

JNIEXPORT void JNICALL Java_ndi_stream_NDIUtilities_convertToInterleaved16s(JNIEnv *env, jclass jClazz, jlong pSourceFrame, jlong pTargetFrame) {
    getNDILib()->util_audio_to_interleaved_16s_v2(reinterpret_cast<NDIlib_audio_frame_v2_t *>(pSourceFrame),
                                                         reinterpret_cast<NDIlib_audio_frame_interleaved_16s_t *>(pTargetFrame));
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIUtilities_convertFromInterleaved16s(JNIEnv *env, jclass jClazz, jlong pSourceFrame, jlong pTargetFrame) {
    getNDILib()->util_audio_from_interleaved_16s_v2(reinterpret_cast<NDIlib_audio_frame_interleaved_16s_t *>(pSourceFrame),
                                                        reinterpret_cast<NDIlib_audio_frame_v2_t *>(pTargetFrame));
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIUtilities_convertToInterleaved32s(JNIEnv *env, jclass jClazz, jlong pSourceFrame, jlong pTargetFrame) {
    getNDILib()->util_audio_to_interleaved_32s_v2(reinterpret_cast<NDIlib_audio_frame_v2_t *>(pSourceFrame),
                                                         reinterpret_cast<NDIlib_audio_frame_interleaved_32s_t *>(pTargetFrame));
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIUtilities_convertFromInterleaved32s(JNIEnv *env, jclass jClazz, jlong pSourceFrame, jlong pTargetFrame) {
    getNDILib()->util_audio_from_interleaved_32s_v2(reinterpret_cast<NDIlib_audio_frame_interleaved_32s_t *>(pSourceFrame),
                                                           reinterpret_cast<NDIlib_audio_frame_v2_t *>(pTargetFrame));
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIUtilities_convertToInterleaved32f(JNIEnv *env, jclass jClazz, jlong pSourceFrame, jlong pTargetFrame) {
    getNDILib()->util_audio_to_interleaved_32f_v2(reinterpret_cast<NDIlib_audio_frame_v2_t *>(pSourceFrame),
                                                         reinterpret_cast<NDIlib_audio_frame_interleaved_32f_t *>(pTargetFrame));
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIUtilities_convertFromInterleaved32f(JNIEnv *env, jclass jClazz, jlong pSourceFrame, jlong pTargetFrame) {
    getNDILib()->util_audio_from_interleaved_32f_v2(reinterpret_cast<NDIlib_audio_frame_interleaved_32f_t *>(pSourceFrame),
                                                           reinterpret_cast<NDIlib_audio_frame_v2_t *>(pTargetFrame));
}
