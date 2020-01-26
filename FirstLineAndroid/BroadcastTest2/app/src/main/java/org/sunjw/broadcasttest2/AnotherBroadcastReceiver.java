package org.sunjw.broadcasttest2;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.widget.Toast;

public class AnotherBroadcastReceiver extends BroadcastReceiver {

    @Override
    public void onReceive(Context context, Intent intent) {
        Toast.makeText(context, "Received in AnotherBroadcastReceiver", Toast.LENGTH_SHORT).show();
        abortBroadcast();
    }
}
