package org.sunjw.learnand.ch2;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

import org.sunjw.learnand.ch2.aidl.BookManagerActivity;
import org.sunjw.learnand.ch2.provider.ProviderActivity;
import org.sunjw.learnand.ch2.util.Ch2Util;
import org.sunjw.learnand.ch2.util.User;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

public class MainActivity extends ActionBarActivity implements OnClickListener {

    private static final String TAG = MainActivity.class.getName();

    private Map<Integer, Class<?>> buttonActivityMap = new HashMap<Integer, Class<?>>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        buttonActivityMap.put(R.id.button_open_secondary, SecondaryActivity.class);
        buttonActivityMap.put(R.id.button_open_third, ThirdActivity.class);
        buttonActivityMap.put(R.id.button_open_messenger, MessengerActivity.class);
        buttonActivityMap.put(R.id.button_open_bookmanager, BookManagerActivity.class);
        buttonActivityMap.put(R.id.button_open_provider, ProviderActivity.class);
        buttonActivityMap.put(R.id.button_open_chat, ChatActivity.class);

        initButtonToOpenActivity();

        persistToFile();
    }

    @Override
    public void onClick(View v) {
        int viewId = v.getId();
        Class<?> activityCls = buttonActivityMap.get(viewId);
        if (activityCls != null) {
            startActivityByClass(activityCls);
        }
    }

    private void initButtonToOpenActivity() {
        Set<Entry<Integer, Class<?>>> buttonSet = buttonActivityMap.entrySet();
        Iterator<Entry<Integer, Class<?>>> buttonIterator = buttonSet.iterator();
        while (buttonIterator.hasNext()) {
            Entry<Integer, Class<?>> buttonEntry = buttonIterator.next();
            addButtonListenerToSelf(buttonEntry.getKey());
        }
    }

    private void addButtonListenerToSelf(int viewId) {
        Button btn = null;
        btn = (Button) findViewById(viewId);
        btn.setOnClickListener(this);
    }

    private void startActivityByClass(Class<?> cls) {
        Intent intent = new Intent(this, cls);
        startActivity(intent);
    }

    private void persistToFile() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                User user = new User(1, "hello world", false);
                File dir = new File(Ch2Util.getDataStoreDir());
                if (!dir.exists()) {
                    dir.mkdirs();
                }

                File dataFile = new File(Ch2Util.getDataStoreFile());
                ObjectOutputStream oos = null;
                try {
                    oos = new ObjectOutputStream(new FileOutputStream(dataFile));
                    oos.writeObject(user);
                    Log.d(TAG, "persist user: " + user);
                } catch (IOException ioe) {
                    ioe.printStackTrace();
                }
            }
        }).start();
    }
}
