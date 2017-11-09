//
// Created by Tom Liu on 2017/11/8.
//

#include <jni.h>
#include "speex/speex.h"
#include "speex/speex_resampler.h"
#include <stdint.h>
#include "cmath"
#include <android/log.h>
#include <fstream>

#define LOGV(...)   __android_log_print((int)ANDROID_LOG_INFO, "SOUNDTOUCH", __VA_ARGS__)
#define LOGE(...)   __android_log_print((int)ANDROID_LOG_ERROR, "SOUNDTOUCH", __VA_ARGS__)

#define BUFFERSAMPLES 1024
#define MIN(x, y) ((x)<(y)?(x):(y))

#define NULLPTR 0

using namespace std;

static std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

int speexresample(const char *infile, const char *outfile, const int channelCount, const int inputRate, const int outrate, const int quality)
{
    LOGE("infile: %s outfile: %s", infile, outfile);

    int resampler_err = 0;
    SpeexResamplerState* resampler_state = speex_resampler_init(channelCount, inputRate, outrate, quality, &resampler_err);

    FILE* file;
    FILE* outFile;
    file = fopen(infile, "r");
    outFile = fopen(outfile, "w");

    char buf[BUFFERSAMPLES * 2];
    short out[BUFFERSAMPLES];
    spx_uint32_t readlen = 0;
    do {
        readlen = fread(buf, sizeof(short), BUFFERSAMPLES, file);
        if (readlen > 0) {
            spx_uint32_t inlen = readlen;
            spx_uint32_t outlen = BUFFERSAMPLES;
            int ret = speex_resampler_process_int(resampler_state, 0, (const spx_int16_t *) buf, &inlen, out, &outlen);
            if (ret == RESAMPLER_ERR_SUCCESS) {
                fwrite(out, sizeof(short), outlen, outFile);
            }
        }
    } while (readlen == BUFFERSAMPLES);

    speex_resampler_destroy(resampler_state);
    fclose(outFile);
    LOGE("convert finish....");
    return 0;
}


extern  "C" JNIEXPORT void JNICALL Java_com_devtom_speexandroid_SpeexAndroid_init(JNIEnv *env, jclass type) {

}


extern "C"
JNIEXPORT jlong JNICALL
Java_com_devtom_speexandroid_SpeexAndroid_createInstance(JNIEnv *env, jclass type, jint channelCount,
                                                        jint inSampleRate, jint outSampleRate,
                                                        jint quality) {

    int err = 0;
    static SpeexResamplerState* instance = speex_resampler_init(channelCount, inSampleRate, outSampleRate,
                                                         quality, &err);

    if (instance == NULLPTR || err == RESAMPLER_ERR_ALLOC_FAILED) {
        return -1;
    }
    return (jlong) instance;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_devtom_speexandroid_SpeexAndroid_resample(JNIEnv *env, jclass type, jstring input_,
                                                   jstring output_, jint channelCount,
                                                   jint inSampleRate, jint outSampleRate,
                                                   jint quality) {
    const char *input = env->GetStringUTFChars(input_, 0);
    const char *output = env->GetStringUTFChars(output_, 0);

    speexresample(input, output, channelCount , inSampleRate , outSampleRate, quality);

    env->ReleaseStringUTFChars(input_, input);
    env->ReleaseStringUTFChars(output_, output);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_devtom_speexandroid_SpeexAndroid_resampleSample(JNIEnv *env, jclass type, jlong ptr,
                                                   jshortArray pcm, jint size, jbyteArray out) {

    SpeexResamplerState* st = (SpeexResamplerState*) ptr;
    short outputBuf[BUFFERSAMPLES];
    const char * fileName = "/sdcard/testpcm.pcm";
    if (size == BUFFERSAMPLES) {
        spx_uint32_t inlen = size;
        spx_uint32_t outlen = BUFFERSAMPLES;
        int ret = speex_resampler_process_int(st, 0, (const spx_int16_t *) pcm, &inlen, outputBuf, &outlen);
        if (ret == RESAMPLER_ERR_SUCCESS) {
            FILE* outFile;
            outFile = fopen(fileName, "a+");
            fwrite(outputBuf, sizeof(short), outlen, outFile);
            fclose(outFile);
            LOGE("write %d bytes to file", outlen);
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_devtom_speexandroid_SpeexAndroid_release(JNIEnv *env, jclass type, jlong ptr) {
    speex_resampler_destroy((SpeexResamplerState*) ptr);
}