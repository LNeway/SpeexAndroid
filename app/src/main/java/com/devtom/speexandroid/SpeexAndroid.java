package com.devtom.speexandroid;

/**
 * Created by tomliu on 2017/11/8.
 */

public class SpeexAndroid {

    static {
        System.loadLibrary("speex");
    }

    private long ptr;

    public SpeexAndroid(int channelCount, int inSampleRate, int outSampleRate, int quality) {
        ptr  = createInstance(channelCount, inSampleRate, outSampleRate, quality);
    }

    public void release(){
        if (ptr != 0) {
            release(ptr);
        }
    }

    public static void resampleFile(String input, String output,
                                              int channelCount, int inSampleRate, int outSampleRate, int quality) {
        resample(input, output, channelCount, inSampleRate, outSampleRate, quality);
    }

    public void resampleSample(short [] pcm,  int size, byte [] out) {
        resampleSample(ptr,  pcm, size, out);
    }

    private static native final void resample(String input, String output,
                                              int channelCount, int inSampleRate, int outSampleRate, int quality);

    private static native final void resampleSample(long ptr, short [] pcm, int size, byte [] out);

    private static native final long createInstance(int channelCount, int inSampleRate, int outSampleRate, int quality);

    private static native final void release(long ptr);

}
