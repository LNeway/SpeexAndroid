package com.devtom.speexandroid;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import java.io.File;
import java.io.IOException;

public class MainActivity extends AppCompatActivity {

    private SpeexAndroid speexAndroid;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.content);
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

                SpeexAndroid.resampleFile(input, out, 2, 44100, 32000, 10);
            }
        }.start();
    }

    private AudioRecord audioRecord;
    private short[] audioBuffer;

    private boolean hasStart = false;
    private AudioRecordThread recordThread;

    public void start(View view) {
        File file = new File(Environment.getExternalStorageDirectory() + "/testpcm.pcm");
        if (file.exists()) {
            file.delete();
        }

        try {
            file.createNewFile();
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (!hasStart) {
            prepareAudio();
            audioRecord.startRecording();
            recordThread = new AudioRecordThread();
            recordThread.start();
        } else {
            if (recordThread != null) {
                recordThread.quit();
            }
            recordThread = null;
        }
        hasStart = !hasStart;
    }


    private static final int[] AUDIO_SOURCES = new int[]{
            MediaRecorder.AudioSource.MIC,
            MediaRecorder.AudioSource.DEFAULT,
            MediaRecorder.AudioSource.CAMCORDER,
            MediaRecorder.AudioSource.VOICE_COMMUNICATION,
            MediaRecorder.AudioSource.VOICE_RECOGNITION,
    };

    private boolean prepareAudio() {
        audioBuffer = new short[1024];
        final int minBufferSize = AudioRecord.getMinBufferSize(
                44100, AudioFormat.CHANNEL_IN_STEREO,
                AudioFormat.ENCODING_PCM_16BIT);

        for (int source : AUDIO_SOURCES) {
            audioRecord = new AudioRecord(source, 44100,
                    AudioFormat.CHANNEL_IN_STEREO, AudioFormat.ENCODING_PCM_16BIT, minBufferSize * 5);
            if (AudioRecord.STATE_INITIALIZED == audioRecord.getState()) {
                break;
            }
        }
        speexAndroid = new SpeexAndroid(2, 44100, 32000,  10);
        return true;
    }

    class AudioRecordThread extends Thread {
        private volatile  boolean isRunning = true;

        AudioRecordThread() {
            isRunning = true;
        }

        public void quit() {
            isRunning = false;
        }

        @Override
        public void run() {
            Log.e("Speex", "recording now");
            while (isRunning) {
                int size = audioRecord.read(audioBuffer, 0, audioBuffer.length);
                if (isRunning && size > 0) {
                    speexAndroid.resampleSample(audioBuffer, size, null);
                }
            }
        }
    }
}
