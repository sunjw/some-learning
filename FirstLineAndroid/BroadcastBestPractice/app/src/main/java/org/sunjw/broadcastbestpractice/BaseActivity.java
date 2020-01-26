package org.sunjw.broadcastbestpractice;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;

/**
 * Created by sunjw on 2017/3/25.
 */

public class BaseActivity extends AppCompatActivity {

    public final static String BROADCAST_FORCE_OFFLINE = "org.sunjw.broadcastbestpractice.FORCE_OFFLINE";

    class ForceOfflineReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(final Context context, Intent intent) {
            AlertDialog.Builder adBuilder = new AlertDialog.Builder(context);
            adBuilder.setTitle("Warning");
            adBuilder.setMessage("You are forced to be offline. Please try to login again");
            adBuilder.setCancelable(false);
            adBuilder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    ActivityCollector.finishAll();
                    Intent intentInner = new Intent(context, LoginActivity.class);
                    context.startActivity(intentInner);
                }
            });
            adBuilder.show();
        }
    }

    private ForceOfflineReceiver forceOfflineReceiver = null;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        ActivityCollector.addActivity(this);
    }

    @Override
    protected void onResume() {
        super.onResume();
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BROADCAST_FORCE_OFFLINE);
        forceOfflineReceiver = new ForceOfflineReceiver();
        registerReceiver(forceOfflineReceiver, intentFilter);
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (forceOfflineReceiver != null) {
            unregisterReceiver(forceOfflineReceiver);
            forceOfflineReceiver = null;
        }
    }

    @Override
    protected void onDestroy() {
        ActivityCollector.removeActivity(this);
        super.onDestroy();
    }

}
