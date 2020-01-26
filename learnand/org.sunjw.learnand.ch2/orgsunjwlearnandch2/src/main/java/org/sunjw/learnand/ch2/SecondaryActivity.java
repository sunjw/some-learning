package org.sunjw.learnand.ch2;

import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;

import org.sunjw.learnand.ch2.util.Ch2Util;
import org.sunjw.learnand.ch2.util.User;

import java.io.File;
import java.io.FileInputStream;
import java.io.ObjectInputStream;

public class SecondaryActivity extends ActionBarActivity {

    private static final String TAG = MainActivity.class.getName();

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_secondary);

        recoverFromFile();
	}

	private void recoverFromFile() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                User user = null;
                File dataFile = new File(Ch2Util.getDataStoreFile());
                if (dataFile.exists()) {
                    ObjectInputStream ois = null;
                    try {
                        ois = new ObjectInputStream(new FileInputStream(dataFile));
                        user = (User) ois.readObject();
                        Log.d(TAG, "persist user: " + user);
                    } catch (Exception ex) {
                        ex.printStackTrace();
                    }
                }
            }
        }).start();
    }

}
