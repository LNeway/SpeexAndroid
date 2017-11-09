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


    public static native final void resample(String input, String output, int channelCount, int inSampleRate, int outSampleRate, int quality);

    private static native final long createInstance(int channelCount, int inSampleRate, int outSampleRate, int quality);

    public static native final void init();
}
