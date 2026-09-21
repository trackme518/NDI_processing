#include "devolay.h"

#include <cstring>

#include "ndi_stream_NDIP5AudioFrame.h"
#include "ndi_stream_NDIP5AudioFrameInterleaved16s.h"
#include "ndi_stream_NDIP5AudioFrameInterleaved32s.h"
#include "ndi_stream_NDIP5AudioFrameInterleaved32f.h"
#include "ndi_stream_NDIP5MetadataFrame.h"
#include "ndi_stream_NDIP5VideoFrame.h"

/** Audio Frame **/

JNIEXPORT jlong JNICALL Java_ndi_stream_NDIP5AudioFrame_createNewAudioFrameDefaultSettings(JNIEnv * env, jclass jClazz) {
    NDIlib_audio_frame_v2_t *NDI_audio_frame = new NDIlib_audio_frame_v2_t();
    return (jlong) NDI_audio_frame;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrame_destroyAudioFrame(JNIEnv *env, jclass jClazz, jlong pFrame) {
    delete reinterpret_cast<NDIlib_audio_frame_v2_t *>(pFrame);
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrame_setSampleRate(JNIEnv *env, jclass jClazz, jlong pFrame, jint jSampleRate) {
    reinterpret_cast<NDIlib_audio_frame_v2_t *>(pFrame)->sample_rate = jSampleRate;
}

JNIEXPORT jint JNICALL Java_ndi_stream_NDIP5AudioFrame_getSampleRate(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_audio_frame_v2_t *>(pFrame)->sample_rate;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrame_setNoChannels(JNIEnv *env, jclass jClazz, jlong pFrame, jint jNoChannels) {
    reinterpret_cast<NDIlib_audio_frame_v2_t *>(pFrame)->no_channels = jNoChannels;
}

JNIEXPORT jint JNICALL Java_ndi_stream_NDIP5AudioFrame_getNoChannels(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_audio_frame_v2_t *>(pFrame)->no_channels;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrame_setNoSamples(JNIEnv *env , jclass jClazz, jlong pFrame, jint jNoSamples) {
    reinterpret_cast<NDIlib_audio_frame_v2_t *>(pFrame)->no_samples = jNoSamples;
}

JNIEXPORT jint JNICALL Java_ndi_stream_NDIP5AudioFrame_getNoSamples(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_audio_frame_v2_t *>(pFrame)->no_samples;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrame_setTimecode(JNIEnv *env, jclass jClazz, jlong pFrame, jlong jTimecode) {
    reinterpret_cast<NDIlib_audio_frame_v2_t *>(pFrame)->timecode = jTimecode;
}

JNIEXPORT jlong JNICALL Java_ndi_stream_NDIP5AudioFrame_getTimecode(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_audio_frame_v2_t *>(pFrame)->timecode;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrame_setChannelStride(JNIEnv *env, jclass jClazz, jlong pFrame, jint jChannelStride) {
    reinterpret_cast<NDIlib_audio_frame_v2_t *>(pFrame)->channel_stride_in_bytes = jChannelStride;
}

JNIEXPORT jint JNICALL Java_ndi_stream_NDIP5AudioFrame_getChannelStride(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_audio_frame_v2_t *>(pFrame)->channel_stride_in_bytes;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrame_setMetadata(JNIEnv *env, jclass jClazz, jlong pFrame, jstring jMetadata) {
    auto *isCopy = new jboolean();
    *isCopy = JNI_TRUE;
    const char *metadata = env->GetStringUTFChars(jMetadata, isCopy);
    delete isCopy;

    reinterpret_cast<NDIlib_audio_frame_v2_t *>(pFrame)->p_metadata = metadata;
}

JNIEXPORT jstring JNICALL Java_ndi_stream_NDIP5AudioFrame_getMetadata(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return env->NewStringUTF(reinterpret_cast<NDIlib_audio_frame_v2_t *>(pFrame)->p_metadata);
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrame_setTimestamp(JNIEnv *env, jclass jClazz, jlong pFrame, jlong jTimestamp) {
    reinterpret_cast<NDIlib_audio_frame_v2_t *>(pFrame)->timestamp = jTimestamp;
}

JNIEXPORT jlong JNICALL Java_ndi_stream_NDIP5AudioFrame_getTimestamp(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_audio_frame_v2_t *>(pFrame)->timestamp;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrame_setData(JNIEnv *env, jclass jClazz, jlong pFrame, jobject jData) {
    auto frame = reinterpret_cast<NDIlib_audio_frame_v2_t *>(pFrame);

    if(env->GetDirectBufferCapacity(jData) < frame->no_samples * frame->no_channels * sizeof(float)) {
        env->ThrowNew(env->FindClass("java/lang/IllegalArgumentException"), "Audio buffer not large enough.");
        return;
    }

    frame->p_data = static_cast<float *>(env->GetDirectBufferAddress(jData));
}

JNIEXPORT jobject JNICALL Java_ndi_stream_NDIP5AudioFrame_getData(JNIEnv *env, jclass jClazz, jlong pFrame) {
    auto frame = reinterpret_cast<NDIlib_audio_frame_v2_t *>(pFrame);

    // TODO: Resolve this sizing issue. If the no samples, no channels are increased, will read unknown memory. Maybe force deallocate data on changing one of those?
    return env->NewDirectByteBuffer(frame->p_data, frame->no_samples * frame->no_channels * sizeof(float));
}

/** Audio Frame 16s Interleaved **/
JNIEXPORT jlong JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved16s_createNewAudioFrameInterleaved16sDefaultSettings(JNIEnv *env, jclass jClazz) {
    auto *NDI_audio_frame = new NDIlib_audio_frame_interleaved_16s_t();
    return (jlong) NDI_audio_frame;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved16s_destroyAudioFrameInterleaved16s(JNIEnv *env, jclass jClazz, jlong pFrame) {
    delete reinterpret_cast<NDIlib_audio_frame_interleaved_16s_t *>(pFrame);
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved16s_setSampleRate(JNIEnv *env, jclass jClazz, jlong pFrame, jint jSampleRate) {
    reinterpret_cast<NDIlib_audio_frame_interleaved_16s_t *>(pFrame)->sample_rate = jSampleRate;
}

JNIEXPORT jint JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved16s_getSampleRate(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_audio_frame_interleaved_16s_t *>(pFrame)->sample_rate;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved16s_setNoChannels(JNIEnv *env, jclass jClazz, jlong pFrame, jint jNoChannels) {
    reinterpret_cast<NDIlib_audio_frame_interleaved_16s_t *>(pFrame)->no_channels = jNoChannels;
}

JNIEXPORT jint JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved16s_getNoChannels(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_audio_frame_interleaved_16s_t *>(pFrame)->no_channels;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved16s_setNoSamples(JNIEnv *env, jclass jClazz, jlong pFrame, jint jNoSamples) {
    reinterpret_cast<NDIlib_audio_frame_interleaved_16s_t *>(pFrame)->no_samples = jNoSamples;
}

JNIEXPORT jint JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved16s_getNoSamples(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_audio_frame_interleaved_16s_t *>(pFrame)->no_samples;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved16s_setTimecode(JNIEnv *env, jclass jClazz, jlong pFrame, jlong jTimecode) {
    reinterpret_cast<NDIlib_audio_frame_interleaved_16s_t *>(pFrame)->timecode = jTimecode;
}

JNIEXPORT jlong JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved16s_getTimecode(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_audio_frame_interleaved_16s_t *>(pFrame)->timecode;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved16s_setReferenceLevel(JNIEnv *env, jclass jClazz, jlong pFrame, jint jReferenceLevel) {
    reinterpret_cast<NDIlib_audio_frame_interleaved_16s_t *>(pFrame)->reference_level = jReferenceLevel;
}

JNIEXPORT jint JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved16s_getReferenceLevel(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_audio_frame_interleaved_16s_t *>(pFrame)->reference_level;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved16s_setData(JNIEnv *env, jclass jClazz, jlong pFrame, jobject jData) {
    reinterpret_cast<NDIlib_audio_frame_interleaved_16s_t *>(pFrame)->p_data = static_cast<int16_t *>(env->GetDirectBufferAddress(jData));
}

JNIEXPORT jobject JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved16s_getData(JNIEnv *env, jclass jClazz, jlong pFrame) {
    auto frame = reinterpret_cast<NDIlib_audio_frame_interleaved_16s_t *>(pFrame);

    // TODO: Resolve this sizing issue. If the no samples, no channels are increased, will read unknown memory. Maybe force deallocate data on changing one of those?
    return env->NewDirectByteBuffer(frame->p_data, frame->no_samples * frame->no_channels * sizeof(int16_t));
}

/** Audio Frame 32s Interleaved **/
JNIEXPORT jlong JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32s_createNewAudioFrameInterleaved32sDefaultSettings(JNIEnv *env, jclass jClazz) {
    auto *NDI_audio_frame = new NDIlib_audio_frame_interleaved_32s_t();
    return (jlong) NDI_audio_frame;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32s_destroyAudioFrameInterleaved32s(JNIEnv *env, jclass jClazz, jlong pFrame) {
    delete reinterpret_cast<NDIlib_audio_frame_interleaved_32s_t *>(pFrame);
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32s_setSampleRate(JNIEnv *env, jclass jClazz, jlong pFrame, jint jSampleRate) {
    reinterpret_cast<NDIlib_audio_frame_interleaved_32s_t *>(pFrame)->sample_rate = jSampleRate;
}

JNIEXPORT jint JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32s_getSampleRate(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_audio_frame_interleaved_32s_t *>(pFrame)->sample_rate;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32s_setNoChannels(JNIEnv *env, jclass jClazz, jlong pFrame, jint jNoChannels) {
    reinterpret_cast<NDIlib_audio_frame_interleaved_32s_t *>(pFrame)->no_channels = jNoChannels;
}

JNIEXPORT jint JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32s_getNoChannels(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_audio_frame_interleaved_32s_t *>(pFrame)->no_channels;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32s_setNoSamples(JNIEnv *env, jclass jClazz, jlong pFrame, jint jNoSamples) {
    reinterpret_cast<NDIlib_audio_frame_interleaved_32s_t *>(pFrame)->no_samples = jNoSamples;
}

JNIEXPORT jint JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32s_getNoSamples(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_audio_frame_interleaved_32s_t *>(pFrame)->no_samples;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32s_setTimecode(JNIEnv *env, jclass jClazz, jlong pFrame, jlong jTimecode) {
    reinterpret_cast<NDIlib_audio_frame_interleaved_32s_t *>(pFrame)->timecode = jTimecode;
}

JNIEXPORT jlong JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32s_getTimecode(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_audio_frame_interleaved_32s_t *>(pFrame)->timecode;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32s_setReferenceLevel(JNIEnv *env, jclass jClazz, jlong pFrame, jint jReferenceLevel) {
    reinterpret_cast<NDIlib_audio_frame_interleaved_32s_t *>(pFrame)->reference_level = jReferenceLevel;
}

JNIEXPORT jint JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32s_getReferenceLevel(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_audio_frame_interleaved_32s_t *>(pFrame)->reference_level;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32s_setData(JNIEnv *env, jclass jClazz, jlong pFrame, jobject jData) {
    reinterpret_cast<NDIlib_audio_frame_interleaved_32s_t *>(pFrame)->p_data = static_cast<int32_t *>(env->GetDirectBufferAddress(jData));
}

JNIEXPORT jobject JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32s_getData(JNIEnv *env, jclass jClazz, jlong pFrame) {
    auto frame = reinterpret_cast<NDIlib_audio_frame_interleaved_32s_t *>(pFrame);

    // TODO: Resolve this sizing issue. If the no samples, no channels are increased, will read unknown memory. Maybe force deallocate data on changing one of those?
    return env->NewDirectByteBuffer(frame->p_data, frame->no_samples * frame->no_channels * sizeof(int32_t));
}

/** Audio frame 32f Interleaved **/
JNIEXPORT jlong JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32f_createNewAudioFrameInterleaved32fDefaultSettings(JNIEnv *env, jclass jClazz) {
    auto *NDI_audio_frame = new NDIlib_audio_frame_interleaved_32f_t();
    return (jlong) NDI_audio_frame;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32f_destroyAudioFrameInterleaved32f(JNIEnv *env, jclass jClazz, jlong pFrame) {
    delete reinterpret_cast<NDIlib_audio_frame_interleaved_32f_t *>(pFrame);
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32f_setSampleRate(JNIEnv *env, jclass jClazz, jlong pFrame, jint jSampleRate) {
    reinterpret_cast<NDIlib_audio_frame_interleaved_32f_t *>(pFrame)->sample_rate = jSampleRate;
}

JNIEXPORT jint JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32f_getSampleRate(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_audio_frame_interleaved_32f_t *>(pFrame)->sample_rate;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32f_setNoChannels(JNIEnv *env, jclass jClazz, jlong pFrame, jint jNoChannels) {
    reinterpret_cast<NDIlib_audio_frame_interleaved_32f_t *>(pFrame)->no_channels = jNoChannels;
}

JNIEXPORT jint JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32f_getNoChannels(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_audio_frame_interleaved_32f_t *>(pFrame)->no_channels;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32f_setNoSamples(JNIEnv *env, jclass jClazz, jlong pFrame, jint jNoSamples) {
    reinterpret_cast<NDIlib_audio_frame_interleaved_32f_t *>(pFrame)->no_samples = jNoSamples;
}

JNIEXPORT jint JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32f_getNoSamples(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_audio_frame_interleaved_32f_t *>(pFrame)->no_samples;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32f_setTimecode(JNIEnv *env, jclass jClazz, jlong pFrame, jlong jTimecode) {
    reinterpret_cast<NDIlib_audio_frame_interleaved_32f_t *>(pFrame)->timecode = jTimecode;
}

JNIEXPORT jlong JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32f_getTimecode(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_audio_frame_interleaved_32f_t *>(pFrame)->timecode;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32f_setData(JNIEnv *env, jclass jClazz, jlong pFrame, jobject jData) {
    reinterpret_cast<NDIlib_audio_frame_interleaved_32f_t *>(pFrame)->p_data = static_cast<float *>(env->GetDirectBufferAddress(jData));
}

JNIEXPORT jobject JNICALL Java_ndi_stream_NDIP5AudioFrameInterleaved32f_getData(JNIEnv *env, jclass jClazz, jlong pFrame) {
    auto frame = reinterpret_cast<NDIlib_audio_frame_interleaved_32f_t *>(pFrame);

    // TODO: Resolve this sizing issue. If the no samples, no channels are increased, will read unknown memory. Maybe force deallocate data on changing one of those?
    return env->NewDirectByteBuffer(frame->p_data, frame->no_samples * frame->no_channels * sizeof(float));
}

/** Metadata Frame **/
JNIEXPORT jlong JNICALL Java_ndi_stream_NDIP5MetadataFrame_createNewMetadataFrameDefaultSettings(JNIEnv *env, jclass jClazz) {
    auto *NDI_metadata_frame = new NDIlib_metadata_frame_t();
    return (jlong) NDI_metadata_frame;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5MetadataFrame_destroyMetadataFrame(JNIEnv *env, jclass jClazz, jlong pFrame) {
    delete reinterpret_cast<NDIlib_metadata_frame_t *>(pFrame);
}

JNIEXPORT jstring JNICALL Java_ndi_stream_NDIP5MetadataFrame_getData(JNIEnv *env, jclass jClazz, jlong pFrame) {
    auto *NDI_metadata_frame = reinterpret_cast<NDIlib_metadata_frame_t *>(pFrame);
    return env->NewStringUTF(NDI_metadata_frame->p_data);
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5MetadataFrame_setData(JNIEnv *env, jclass jClazz, jlong pFrame, jstring jData) {
    auto *isCopy = new jboolean();
    *isCopy = JNI_FALSE;
    const char *data = env->GetStringUTFChars(jData, isCopy);
    delete isCopy;

    char *mutable_data = new char[env->GetStringUTFLength(jData)];

    strcpy(mutable_data, data);

    env->ReleaseStringUTFChars(jData, data);

    reinterpret_cast<NDIlib_metadata_frame_t *>(pFrame)->p_data = mutable_data;
}

JNIEXPORT jlong JNICALL Java_ndi_stream_NDIP5MetadataFrame_getTimecode(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_metadata_frame_t *>(pFrame)->timecode;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5MetadataFrame_setTimecode(JNIEnv *env, jclass jClazz, jlong pFrame, jlong jTimecode) {
    reinterpret_cast<NDIlib_metadata_frame_t *>(pFrame)->timecode = jTimecode;
}

/** Video Frame **/
JNIEXPORT jlong JNICALL Java_ndi_stream_NDIP5VideoFrame_createNewVideoFrameDefaultSettings(JNIEnv *env, jclass jClazz) {
    auto *NDI_video_frame = new NDIlib_video_frame_v2_t();
    return (jlong) NDI_video_frame;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5VideoFrame_destroyVideoFrame(JNIEnv *env, jclass jClazz, jlong pFrame) {
    delete reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame);
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5VideoFrame_setXRes(JNIEnv *env, jclass jClazz, jlong pFrame, jint jXRes) {
    reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame)->xres = jXRes;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5VideoFrame_setYRes(JNIEnv *env, jclass jClazz, jlong pFrame, jint jYRes) {
    reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame)->yres = jYRes;
}

JNIEXPORT jint JNICALL Java_ndi_stream_NDIP5VideoFrame_getXRes(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame)->xres;
}

JNIEXPORT jint JNICALL Java_ndi_stream_NDIP5VideoFrame_getYRes(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame)->yres;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5VideoFrame_setFourCCType(JNIEnv *env, jclass jClazz, jlong pFrame, jint j4CCType) {
    reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame)->FourCC = static_cast<NDIlib_FourCC_video_type_e>(j4CCType);
}

JNIEXPORT jint JNICALL Java_ndi_stream_NDIP5VideoFrame_getFourCCType(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame)->FourCC;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5VideoFrame_setFrameRateN(JNIEnv *env, jclass jClazz, jlong pFrame, jint jFrameRateN) {
    reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame)->frame_rate_N = jFrameRateN;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5VideoFrame_setFrameRateD(JNIEnv *env, jclass jClazz, jlong pFrame, jint jFrameRateD) {
    reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame)->frame_rate_D = jFrameRateD;
}

JNIEXPORT jint JNICALL Java_ndi_stream_NDIP5VideoFrame_getFrameRateN(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame)->frame_rate_N;
}

JNIEXPORT jint JNICALL Java_ndi_stream_NDIP5VideoFrame_getFrameRateD(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame)->frame_rate_D;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5VideoFrame_setPictureAspectRatio(JNIEnv *env, jclass jClazz, jlong pFrame, jfloat jAspectRatio) {
    reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame)->picture_aspect_ratio = jAspectRatio;
}

JNIEXPORT jfloat JNICALL Java_ndi_stream_NDIP5VideoFrame_getPictureAspectRatio(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame)->picture_aspect_ratio;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5VideoFrame_setFrameFormatType(JNIEnv *env, jclass jClazz, jlong pFrame, jint jFrameFormatType) {
    reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame)->frame_format_type = static_cast<NDIlib_frame_format_type_e>(jFrameFormatType);
}

JNIEXPORT jint JNICALL Java_ndi_stream_NDIP5VideoFrame_getFrameFormatType(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame)->frame_format_type;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5VideoFrame_setTimecode(JNIEnv *env, jclass jClazz, jlong pFrame, jlong jTimecode) {
    reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame)->timecode = jTimecode;
}

JNIEXPORT jlong JNICALL Java_ndi_stream_NDIP5VideoFrame_getTimecode(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame)->timecode;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5VideoFrame_setLineStride(JNIEnv *env, jclass jClazz, jlong pFrame, jint jLineStride) {
    reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame)->line_stride_in_bytes = jLineStride;
}

JNIEXPORT jint JNICALL Java_ndi_stream_NDIP5VideoFrame_getLineStride(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame)->line_stride_in_bytes;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5VideoFrame_setMetadata(JNIEnv *env, jclass jClazz, jlong pFrame, jstring jMetadata) {
    auto *isCopy = new jboolean();
    *isCopy = JNI_TRUE;
    const char *metadata = env->GetStringUTFChars(jMetadata, isCopy);
    delete isCopy;

    reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame)->p_metadata = metadata;
}

JNIEXPORT jstring JNICALL Java_ndi_stream_NDIP5VideoFrame_getMetadata(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return env->NewStringUTF(reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame)->p_metadata);
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5VideoFrame_setTimestamp(JNIEnv *env, jclass jClazz, jlong pFrame, jint jTimestamp) {
    reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame)->timestamp = jTimestamp;
}

JNIEXPORT jint JNICALL Java_ndi_stream_NDIP5VideoFrame_getTimestamp(JNIEnv *env, jclass jClazz, jlong pFrame) {
    return reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame)->timestamp;
}

JNIEXPORT void JNICALL Java_ndi_stream_NDIP5VideoFrame_setData(JNIEnv *env, jclass jClazz, jlong pFrame, jobject jData) {
    reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame)->p_data = static_cast<uint8_t *>(env->GetDirectBufferAddress(jData));
}

JNIEXPORT jobject JNICALL Java_ndi_stream_NDIP5VideoFrame_getData(JNIEnv *env, jclass jClazz, jlong pFrame) {
    auto *frame = reinterpret_cast<NDIlib_video_frame_v2_t *>(pFrame);
    if(frame->p_data) {
        // TODO: Resolve this sizing issue. If the no samples, no channels are increased, will read unknown memory. Maybe force deallocate data on changing one of those?
        return env->NewDirectByteBuffer(frame->p_data, frame->line_stride_in_bytes * frame->yres);
    } else {
        return nullptr;
    }
}
