package org.sunjw.learnand.ch2;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.StrictMode;
import android.os.SystemClock;
import android.support.v7.app.ActionBarActivity;
import android.text.TextUtils;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;

import org.sunjw.learnand.ch2.socket.TCPServerSerivce;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.text.SimpleDateFormat;
import java.util.Date;

public class ChatActivity extends ActionBarActivity implements OnClickListener {

    private static final int MESSAGE_RECEIVE_NEW_MSG = 1;
    private static final int MESSAGE_SOCKET_CONNECTED = 2;

    private ScrollView svMessage;
    private TextView tvMessageView;

    private EditText etMessageSend;
    private Button btnSend;

    private PrintWriter pw;
    private Socket clientSocket;

    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MESSAGE_RECEIVE_NEW_MSG:
                    appendMsgAndScroll((String) msg.obj);
                    break;
                case MESSAGE_SOCKET_CONNECTED:
                    btnSend.setEnabled(true);
                    break;
                default:
                    break;
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_chat);

        if (android.os.Build.VERSION.SDK_INT > 9) {
            StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
            StrictMode.setThreadPolicy(policy);
        }

        svMessage = (ScrollView) findViewById(R.id.scrollViewMsg);

        tvMessageView = (TextView) findViewById(R.id.textViewChatMsg);
        tvMessageView.setText("");

        btnSend = (Button) findViewById(R.id.buttonSend);
        btnSend.setOnClickListener(this);
        btnSend.setEnabled(false);

        etMessageSend = (EditText) findViewById(R.id.editTextMessage);

        Intent service = new Intent(this, TCPServerSerivce.class);
        startService(service);
        new Thread() {
            @Override
            public void run() {
                connectTCPServer();
            }
        }.start();
    }

    @Override
    protected void onDestroy() {
        if (clientSocket != null) {
            try {
                clientSocket.shutdownInput();
                clientSocket.close();
            } catch (IOException ioe) {
                ioe.printStackTrace();
            }
        }

        super.onDestroy();
    }

    @Override
    public void onClick(View v) {
        if (v == btnSend) {
            final String msg = etMessageSend.getText().toString();
            if (!TextUtils.isEmpty(msg) && pw != null) {
                pw.println(msg);
                etMessageSend.setText("");
                String time = formatDateTime(System.currentTimeMillis());
                final String shownMsg = "self " + time + ": " + msg + "\n";
                appendMsgAndScroll(shownMsg);
            }
        }
    }

    private void appendMsgAndScroll(String msg) {
        tvMessageView.append(msg);
        svMessage.fullScroll(View.FOCUS_DOWN);
    }

    private String formatDateTime(long time) {
        return new SimpleDateFormat("(HH:mm:ss)").format(new Date(time));
    }

    private void connectTCPServer() {
        Socket socket = null;
        while (socket == null) {
            try {
                socket = new Socket("localhost", 8688);
                clientSocket = socket;
                pw = new PrintWriter(
                        new BufferedWriter(new OutputStreamWriter(clientSocket.getOutputStream())),
                        true);
                handler.sendEmptyMessage(MESSAGE_SOCKET_CONNECTED);
                System.out.println("Connect server success.");
            } catch (IOException ioe) {
                ioe.printStackTrace();
                SystemClock.sleep(1000);
                System.out.println("Connect TCP server failed, retry...");
            }

        }

        try {
            BufferedReader br = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            while (!this.isFinishing()) {
                String msg = br.readLine();
                System.out.println("Received: " + msg);
                if (msg != null) {
                    String time = formatDateTime(System.currentTimeMillis());
                    final String shownMsg = "server " + time + ": " + msg + "\n";
                    handler.obtainMessage(MESSAGE_RECEIVE_NEW_MSG, shownMsg).sendToTarget();
                }
            }

            System.out.println("Quit...");

            pw.close();
            br.close();

            clientSocket.close();

        } catch (IOException ioe) {
            ioe.printStackTrace();
        }

    }

}
