package org.sunjw.learnand.ch1;

import android.support.v7.app.ActionBarActivity;
import android.util.Log;
import android.content.Intent;
import android.os.Bundle;
import android.os.PersistableBundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class SecondaryActivity extends ActionBarActivity implements OnClickListener {

	public static final String TAG = "SecondaryActivity";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		Log.d(TAG, "onCreate");

		setContentView(R.layout.activity_secondary);

		Button btn = null;
		btn = (Button) findViewById(R.id.buttonMainActivity);
		btn.setOnClickListener(this);
	}

	@Override
	protected void onStart() {
		super.onStart();
		Log.d(TAG, "onStart");
	}

	@Override
	protected void onResume() {
		super.onResume();
		Log.d(TAG, "onResume");
	}

	@Override
	public void onSaveInstanceState(Bundle outState) {
		// TODO Auto-generated method stub
		super.onSaveInstanceState(outState);

		Log.d(TAG, "onSaveInstanceState");
		outState.putString("extra_test", "test");
	}

	@Override
	public void onRestoreInstanceState(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onRestoreInstanceState(savedInstanceState);

		String testSaved = savedInstanceState.getString("extra_test");
		Log.d(TAG, "onRestoreInstanceState, [extra_test: \"" + testSaved + "\"");
	}

	@Override
	public void onClick(View v) {
		Intent intent = null;

		switch (v.getId()) {
		case R.id.buttonMainActivity:
			intent = new Intent(this, MainActivity.class);
			intent.putExtra("time", System.currentTimeMillis());
			startActivity(intent);
			break;
		}
	}
}
