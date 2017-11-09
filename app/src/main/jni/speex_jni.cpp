//
// Created by Tom Liu on 2017/11/8.
//

#include <jni.h>
#include "speex/speex.h"
#include "speex/speex_resampler.h"
#include <stdint.h>
#include "cmath"
#include <android/log.h>
#include <string.h>
#include <fstream>

#define LOGV(...)   __android_log_print((int)ANDROID_LOG_INFO, "SOUNDTOUCH", __VA_ARGS__)
#define LOGE(...)   __android_log_print((int)ANDROID_LOG_ERROR, "SOUNDTOUCH", __VA_ARGS__)

#define BUFFERSAMPLES 256
#define MIN(x, y) ((x)<(y)?(x):(y))

using namespace std;

static std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

int speexresample(const char *infile, const char *outfile, const int outrate, const int quality)
{
    LOGE("infile: %s outfile: %s", infile, outfile);

    int resampler_err = 0;
    SpeexResamplerState* resampler_state = speex_resampler_init(2, 44100, outrate, quality, &resampler_err);


    int inputSamples = filesize(infile) / 4;  //one sample = 16bit * 2 channel = 4byte.
    LOGV("file length is  %d", inputSamples);
    const int expected_samples_written = (double) inputSamples * ((double)outrate/44100);
    int total_samples_written = 0;
    ifstream inStream;
    inStream.open(infile, ios::binary);
    if (!inStream.is_open()) {
        return 0;
    }

    ofstream outStream(outfile, ios::out);

    char buffer[BUFFERSAMPLES * 4];
    float outBuffer [BUFFERSAMPLES];

    while(!inStream.eof()) {

        inStream.read(buffer, BUFFERSAMPLES * sizeof(float));
        unsigned int readSize = inStream.gcount() / 4;
        LOGE("read %d samples", readSize);
        unsigned int out_processed = BUFFERSAMPLES;
        speex_resampler_process_interleaved_float(resampler_state,
                                                  (float *) buffer,
                                                  &readSize,
                                                  outBuffer,
                                                  &out_processed);
        LOGE("write %d samples", out_processed);
        outStream.write((char *) outBuffer, out_processed * 4);
    }

    speex_resampler_destroy(resampler_state);

    LOGE("read %d samples, wrote %d samples\n", inputSamples, total_samples_written);

    return 0;
}


extern  "C" JNIEXPORT void JNICALL Java_com_devtom_speexandroid_SpeexAndroid_init(JNIEnv *env, jclass type) {

}


extern "C"
JNIEXPORT jlong JNICALL
Java_com_devtom_speexandroid_SpeexAndroid_createInstance(JNIEnv *env, jclass type, jint channelCount,
                                                        jint inSampleRate, jint outSampleRate,
                                                        jint quality) {

    SpeexResamplerState* instance = speex_resampler_init(channelCount, inSampleRate, outSampleRate,
                                                         quality, 0);


   return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_devtom_speexandroid_SpeexAndroid_resample(JNIEnv *env, jclass type, jstring input_,
                                                   jstring output_, jint channelCount,
                                                   jint inSampleRate, jint outSampleRate,
                                                   jint quality) {
    const char *input = env->GetStringUTFChars(input_, 0);
    const char *output = env->GetStringUTFChars(output_, 0);

    speexresample(input, output, 32000, 10);

    env->ReleaseStringUTFChars(input_, input);
    env->ReleaseStringUTFChars(output_, output);
}