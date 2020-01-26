package org.sunjw.learnand.ch2.socket;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.support.annotation.Nullable;
import android.util.Log;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Random;

/**
 * Created by sunjw on 16/6/13.
 */
public class TCPServerSerivce extends Service {

    private static String TAG = TCPServerSerivce.class.getSimpleName();

    public static final int PORT = 8688;

    private boolean isServiceDestroyed = false;
    private String[] definedMessages = new String[]{
            "Hello!", "What's your name?", "Today is nice weather.",
            "I'm chatting with many people.", "Let me tell you a joke, hahaha!"
    };

    @Override
    public void onCreate() {
        super.onCreate();

        Log.i(TAG, "onCreate()");

        new Thread(new TcpServer()).start();
    }

    @Override
    public void onDestroy() {
        Log.i(TAG, "onDestroy()");

        isServiceDestroyed = true;

        super.onDestroy();
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private class TcpServer implements Runnable {
        @Override
        public void run() {
            ServerSocket serverSocket = null;
            try {
                serverSocket = new ServerSocket(PORT);
            } catch (IOException ioe) {
                ioe.printStackTrace();
                return;
            }

            while (!isServiceDestroyed) {
                try {
                    final Socket client = serverSocket.accept();
                    System.out.println("accept");
                    new Thread() {
                        @Override
                        public void run() {
                            try {
                                responseClient(client);
                            } catch (IOException ioe) {
                                ioe.printStackTrace();
                            }
                        }
                    }.start();

                } catch (IOException ioe) {
                    ioe.printStackTrace();
                }
            }
        }

        private void responseClient(Socket client) throws IOException {
            BufferedReader br = new BufferedReader(new InputStreamReader(client.getInputStream()));
            PrintWriter pw = new PrintWriter(new BufferedWriter(
                    new OutputStreamWriter(client.getOutputStream())), true);

            pw.println("Welcome!");

            while (!isServiceDestroyed) {
                String str = br.readLine();
                System.out.println("Client: " + str);
                if (str == null) {
                    break;
                }

                int i = new Random().nextInt(definedMessages.length);
                String msg = definedMessages[i];
                pw.println(msg);

                System.out.println("Server: " + msg);
            }

            System.out.println("Client quit.");

            pw.close();
            br.close();

            client.close();
        }
    }
}
