#include "devolay.h"

#include <cstdio>

#include "p5_ndi_NDIP5Source.h"

JNIEXPORT void JNICALL Java_p5_ndi_NDIP5Source_deallocSource(JNIEnv *env, jclass jClazz, jlong pSource) {
    //delete reinterpret_cast<NDIlib_source_t *>(pSource);
}

JNIEXPORT jstring JNICALL Java_p5_ndi_NDIP5Source_getSourceName(JNIEnv *env, jclass jClazz, jlong pSource) {
    return env->NewStringUTF(reinterpret_cast<NDIlib_source_t *>(pSource)->p_ndi_name);
}
