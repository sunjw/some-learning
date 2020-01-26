package org.sunjw.broadcasttest;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Bundle;
import android.support.v4.content.LocalBroadcastManager;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {

    class NetworkChangeReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            ConnectivityManager connectivityManager = (ConnectivityManager)
                    getSystemService(Context.CONNECTIVITY_SERVICE);
            NetworkInfo networkInfo = connectivityManager.getActiveNetworkInfo();
            if (networkInfo == null || !networkInfo.isAvailable()) {
                Toast.makeText(context, "Network is unavailable", Toast.LENGTH_SHORT).show();
            } else if (networkInfo != null) {
                if (networkInfo.getType() == ConnectivityManager.TYPE_MOBILE) {
                    Toast.makeText(context, "Mobile data is " +
                                    (networkInfo.isAvailable() ? "available" : "unavailable"),
                            Toast.LENGTH_SHORT).show();
                } else if (networkInfo.getType() == ConnectivityManager.TYPE_WIFI) {
                    Toast.makeText(context, "WIFI is " +
                                    (networkInfo.isAvailable() ? "available" : "unavailable"),
                            Toast.LENGTH_SHORT).show();
                } else {
                    Toast.makeText(context, "Network is " +
                                    (networkInfo.isAvailable() ? "available" : "unavailable"),
                            Toast.LENGTH_SHORT).show();
                }
            }
        }
    }

    class LocalReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            Toast.makeText(context, "Received in LocalReceiver", Toast.LENGTH_SHORT).show();
        }
    }

    private IntentFilter intentFilter = null;

    private NetworkChangeReceiver networkChangeReceiver = null;

    private final static String LOCAL_BROADCAST_NAME = "org.sunjw.broadcasttest.LOCAL_BROADCAST";

    private LocalReceiver localReceiver = null;

    private LocalBroadcastManager localBroadcastManager = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        localBroadcastManager = LocalBroadcastManager.getInstance(this);

        Button button = (Button) findViewById(R.id.button);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(LOCAL_BROADCAST_NAME);
                localBroadcastManager.sendBroadcast(intent);
            }
        });

        intentFilter = new IntentFilter();
        intentFilter.addAction(LOCAL_BROADCAST_NAME);
        localReceiver = new LocalReceiver();
        localBroadcastManager.registerReceiver(localReceiver, intentFilter);
    }

    @Override
    protected void onDestroy() {
        localBroadcastManager.unregisterReceiver(localReceiver);
        super.onDestroy();
    }
}
