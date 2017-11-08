package com.devtom.speexandroid;

/**
 * Created by tomliu on 2017/11/8.
 */

public class SpeexAndroid {

    static {
        System.loadLibrary("speex");
    }

    public static native final void init();
}
