#include "devolay.h"

#include <cstdio>

#include "ndi_stream_NDISource.h"

JNIEXPORT void JNICALL Java_ndi_stream_NDISource_deallocSource(JNIEnv *env, jclass jClazz, jlong pSource) {
    //delete reinterpret_cast<NDIlib_source_t *>(pSource);
}

JNIEXPORT jstring JNICALL Java_ndi_stream_NDISource_getSourceName(JNIEnv *env, jclass jClazz, jlong pSource) {
    return env->NewStringUTF(reinterpret_cast<NDIlib_source_t *>(pSource)->p_ndi_name);
}
