package org.sunjw.learnand.ch5.ui;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.RemoteViews;

import org.sunjw.learnand.ch5.R;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Intent intent = new Intent(this, MainActivity.class);
        PendingIntent pi = PendingIntent.getActivity(this, 0, intent, PendingIntent.FLAG_UPDATE_CURRENT);

        RemoteViews rvs = new RemoteViews(getPackageName(), R.layout.notification_test);
        rvs.setImageViewResource(R.id.imageView, R.mipmap.ic_launcher);
        rvs.setOnClickPendingIntent(R.id.textView2, pi);

        Notification.Builder notiBuilder = new Notification.Builder(this);
        notiBuilder.setSmallIcon(R.mipmap.ic_launcher)
                .setTicker("Hello notification")
                .setContentTitle(this.getClass().getCanonicalName())
                .setContentText("This is a really notification!")
                .setWhen(System.currentTimeMillis())
                .setAutoCancel(true)
                .setContent(rvs);
                //.setContentIntent(pi);

        Notification noti = notiBuilder.build();
        NotificationManager notiManager = (NotificationManager)getSystemService(Context.NOTIFICATION_SERVICE);
        notiManager.notify(1, noti);
    }
}
