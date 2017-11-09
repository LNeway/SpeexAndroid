package com.devtom.speexandroid;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

import java.io.File;
import java.io.IOException;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

    }

    public void test(View view) {
        new Thread() {
            @Override
            public void run() {
                String input = Environment.getExternalStorageDirectory() + "/test.pcm";
                String out = Environment.getExternalStorageDirectory() + "/test32.pcm";
                File file = new File(out);
                if (file.exists()) {
                    file.delete();
                }
                try {
                    file.createNewFile();
                } catch (IOException e) {

                }

                SpeexAndroid.resample(input, out, 2, 44100, 32000, 10);
            }
        }.start();
    }
}
