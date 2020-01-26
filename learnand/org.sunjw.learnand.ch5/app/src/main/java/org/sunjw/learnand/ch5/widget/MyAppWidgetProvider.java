package org.sunjw.learnand.ch5.widget;

import android.app.PendingIntent;
import android.appwidget.AppWidgetManager;
import android.appwidget.AppWidgetProvider;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.os.SystemClock;
import android.util.Log;
import android.widget.RemoteViews;
import android.widget.Toast;

import org.sunjw.learnand.ch5.R;

/**
 * Created by sunjw on 16/7/24.
 */
public class MyAppWidgetProvider extends AppWidgetProvider {
    public static final String TAG = MyAppWidgetProvider.class.getCanonicalName();

    public static final String CLICK_ACTION = "org.sunjw.learnand.ch5.action.CLICK";

    public MyAppWidgetProvider() {
        super();
    }

    private Bitmap rotateBitmap(Context context, Bitmap srcBitmap, float degree) {
        Matrix matrix = new Matrix();
        matrix.reset();
        matrix.setRotate(degree);
        Bitmap retBitmap = Bitmap.createBitmap(srcBitmap, 0, 0,
                srcBitmap.getWidth(), srcBitmap.getHeight(), matrix, true);
        return retBitmap;
    }

    @Override
    public void onReceive(final Context context, Intent intent) {
        super.onReceive(context, intent);
        Log.i(TAG, "onReceive: action = " + intent.getAction());

        if (intent.getAction().equals(CLICK_ACTION)) {
            Toast.makeText(context, "Clicked it", Toast.LENGTH_SHORT).show();

            new Thread(new Runnable() {
                @Override
                public void run() {
                    Bitmap srcBitmap = BitmapFactory.decodeResource(context.getResources(),
                            R.mipmap.ic_launcher);
                    AppWidgetManager appWidgetManager = AppWidgetManager.getInstance(context);

                    for (int i = 0; i < 37; ++i) {
                        float degree = (i*10)%360;
                        RemoteViews remoteViews = new RemoteViews(context.getPackageName(),
                                R.layout.widget);
                        remoteViews.setImageViewBitmap(R.id.imageView1,
                                rotateBitmap(context, srcBitmap, degree));

                        Intent intentClick = new Intent();
                        intentClick.setAction(CLICK_ACTION);
                        PendingIntent pendingIntent = PendingIntent.getBroadcast(context,
                                0, intentClick, 0);
                        remoteViews.setOnClickPendingIntent(R.id.imageView1, pendingIntent);

                        appWidgetManager.updateAppWidget(
                                new ComponentName(context, MyAppWidgetProvider.class), remoteViews);

                        SystemClock.sleep(30);
                    }
                }
            }).start();
        }
    }

    @Override
    public void onUpdate(Context context, AppWidgetManager appWidgetManager, int[] appWidgetIds) {
        super.onUpdate(context, appWidgetManager, appWidgetIds);
        Log.i(TAG, "onUpdate");

        final int counter = appWidgetIds.length;
        Log.i(TAG, "counter = " + counter);
        for (int i = 0; i < counter; ++i) {
            int appWidgetId = appWidgetIds[i];
            onWidgetUpdate(context, appWidgetManager, appWidgetId);
        }
    }

    private void onWidgetUpdate(Context context, AppWidgetManager appWidgetManager, int appWidgetId) {
        Log.i(TAG, "onWidgetUpdate");

        RemoteViews remoteViews = new RemoteViews(context.getPackageName(), R.layout.widget);

        Intent intentClick = new Intent();
        intentClick.setAction(CLICK_ACTION);
        PendingIntent pendingIntent = PendingIntent.getBroadcast(context, 0, intentClick, 0);
        remoteViews.setOnClickPendingIntent(R.id.imageView1, pendingIntent);

        appWidgetManager.updateAppWidget(appWidgetId, remoteViews);
    }
}
