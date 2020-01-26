package org.sunjw.learnand.ch2;

import android.app.Service;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.support.annotation.Nullable;
import android.util.Log;

import org.sunjw.learnand.ch2.util.Ch2Util;

/**
 * Created by sunjw on 16/5/10.
 */
public class MessengerService extends Service {

    private static final String TAG = MainActivity.class.getName();

    private static class MessengerHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case Ch2Util.MSG_FROM_CLIENT:
                    String clientString = msg.getData().getString("msg");
                    Log.i(TAG, "receive msg from client: " + clientString);
                    Messenger client = msg.replyTo;
                    Message replyMsg = Message.obtain(null, Ch2Util.MSG_FROM_SERVER);
                    Bundle bundle = new Bundle();
                    bundle.putString("reply", "Received message...");
                    replyMsg.setData(bundle);
                    try {
                        client.send(replyMsg);
                    } catch (RemoteException re) {
                        re.printStackTrace();
                    }
                    break;
                default:
                    super.handleMessage(msg);
            }
        }
    }

    private final Messenger mMessenger = new Messenger(new MessengerHandler());

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return mMessenger.getBinder();
    }
}
