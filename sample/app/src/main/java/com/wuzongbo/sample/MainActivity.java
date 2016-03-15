package com.wuzongbo.sample;

import android.app.Activity;
import android.os.Handler;
import android.os.Message;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

public class MainActivity extends Activity implements Handler.Callback {

    TextView tvSample;

    public static final String TAG = "MainActivity";
    MyThread myThread;

    Handler mHandler = new Handler(this);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        tvSample = (TextView) findViewById(R.id.tvSample);
        myThread = new MyThread();
        myThread.start();
    }

    @Override
    public boolean handleMessage(Message msg) {
        tvSample.append((String) msg.obj);
        return false;
    }


    void writelog(String message) {
        mHandler.sendMessage(mHandler.obtainMessage(0, message));
    }

    class MyThread extends Thread {

        boolean isExists;

        @Override
        public void run() {
            writelog("createSocket...\n");
            if (!createSocket()) {
                writelog("createSocket failed！\n");
                return;
            }

            boolean isConnected = false;
            int rlen;
            byte[] buffer = new byte[4096];
            while (!isExists) {
                try {
                    if (!isConnected) {
                        writelog("connectSocket...\n");
                        if (connectSocket("192.168.0.150", 6000)) {
                            isConnected = true;
                            Log.d(TAG, "");
                            writelog("connect succeed!\n");
                        } else {
                            sleep(1000);
                            continue;
                        }
                    }
                    rlen = recvSocket(buffer, 0, buffer.length);
                    writelog("receive length:" + rlen + "\n");
                    if (rlen > 0) {
                        mHandler.sendMessage(mHandler.obtainMessage(0, new String(buffer, 0, rlen, "GB2312")));
                        sendSocket("OK\n");
                    } else {
                        writelog("receive failed!\n");
                        closeSocket();
                        isConnected = false;
                        writelog("disconnected!\n");
                        createSocket();
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }

            closeSocket();
        }
    }

    public native boolean createSocket();

    public native boolean connectSocket(String ip, int port);

    public native int recvSocket(byte[] buffer, int offset, int count);

    public native boolean sendSocket(String sendData);

    public native boolean closeSocket();

    static {
        System.loadLibrary("tcomm");
    }

}
