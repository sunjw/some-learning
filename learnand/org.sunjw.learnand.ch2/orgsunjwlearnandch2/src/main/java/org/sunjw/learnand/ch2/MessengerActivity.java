package org.sunjw.learnand.ch2;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;

import org.sunjw.learnand.ch2.util.Ch2Util;

public class MessengerActivity extends ActionBarActivity {

    private static final String TAG = MainActivity.class.getName();

    private Messenger mService;

    private ServiceConnection mConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            mService = new Messenger(service);
            Message msg = Message.obtain(null, Ch2Util.MSG_FROM_CLIENT);
            Bundle data = new Bundle();
            data.putString("msg", "Hello, this is client");
            msg.setData(data);
            msg.replyTo = mReplyMessenger;
            try {
                mService.send(msg);
            } catch (RemoteException re) {
                re.printStackTrace();
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {

        }
    };

    private Messenger mReplyMessenger = new Messenger(new MessengerHandler());

    private static class MessengerHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case Ch2Util.MSG_FROM_SERVER:
                    String msgString = msg.getData().getString("reply");
                    Log.i(TAG, "received msg from server: " + msgString);
                    break;
                default:
                    super.handleMessage(msg);
            }
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_messenger);

        Intent intent = new Intent(this, MessengerService.class);
        bindService(intent, mConnection, Context.BIND_AUTO_CREATE);
    }

    @Override
    protected void onDestroy() {
        unbindService(mConnection);
        super.onDestroy();
    }
}
