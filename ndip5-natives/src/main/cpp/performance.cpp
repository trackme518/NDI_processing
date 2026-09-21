#include "devolay.h"

#include "ndi_stream_NDIP5PerformanceData.h"

JNIEXPORT jlong JNICALL Java_ndi_stream_NDIP5PerformanceData_createPerformanceStruct(JNIEnv *env, jclass jClazz) {
    auto *ret = new NDIlib_recv_performance_t();
    return (jlong) ret;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5PerformanceData_destroyPerformanceStruct(JNIEnv *env, jclass jClazz, jlong pStruct) {
    delete reinterpret_cast<NDIlib_recv_performance_t *>(pStruct);
}

JNIEXPORT jlong JNICALL Java_ndi_stream_NDIP5PerformanceData_getPerformanceStructVideoFrames(JNIEnv *env, jclass jClazz, jlong pStruct) {
    return reinterpret_cast<NDIlib_recv_performance_t *>(pStruct)->video_frames;
}

JNIEXPORT jlong JNICALL Java_ndi_stream_NDIP5PerformanceData_getPerformanceStructAudioFrames(JNIEnv *env, jclass jClazz, jlong pStruct) {
    return reinterpret_cast<NDIlib_recv_performance_t *>(pStruct)->audio_frames;
}

JNIEXPORT jlong JNICALL Java_ndi_stream_NDIP5PerformanceData_getPerformanceStructMetadataFrames(JNIEnv *env, jclass jClazz, jlong pStruct) {
    return reinterpret_cast<NDIlib_recv_performance_t *>(pStruct)->metadata_frames;
}
