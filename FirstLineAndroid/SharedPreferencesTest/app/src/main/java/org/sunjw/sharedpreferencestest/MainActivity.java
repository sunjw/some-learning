package org.sunjw.sharedpreferencestest;

import android.content.SharedPreferences;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

public class MainActivity extends AppCompatActivity {

    private static final String SHARED_PERF_DATA = "data";
    private static final String SHARED_PERF_NAME = "name";
    private static final String SHARED_PERF_AGE = "age";
    private static final String SHARED_PERF_MARRIED = "married";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button btnSaveData = (Button) findViewById(R.id.btn_save_data);
        btnSaveData.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                SharedPreferences.Editor editor = getSharedPreferences(SHARED_PERF_DATA, MODE_PRIVATE).edit();
                editor.putString(SHARED_PERF_NAME, "Tom");
                editor.putInt(SHARED_PERF_AGE, 28);
                editor.putBoolean(SHARED_PERF_MARRIED, true);
                editor.apply();
            }
        });

        Button btnRestoreData = (Button) findViewById(R.id.btn_restore_data);
        btnRestoreData.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                SharedPreferences pref = getSharedPreferences(SHARED_PERF_DATA, MODE_PRIVATE);
                String name = pref.getString(SHARED_PERF_NAME, "");
                int age = pref.getInt(SHARED_PERF_AGE, 0);
                boolean married = pref.getBoolean(SHARED_PERF_MARRIED, false);
                Log.d("MainActivity", "name=[" + name + "], " +
                        "age=" + age + ", married=" + married);
            }
        });
    }
}
