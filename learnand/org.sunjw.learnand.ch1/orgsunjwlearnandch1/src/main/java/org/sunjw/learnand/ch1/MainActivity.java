package org.sunjw.learnand.ch1;

import android.support.v7.app.ActionBarActivity;
import android.util.Log;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class MainActivity extends ActionBarActivity implements OnClickListener {

	public static final String TAG = "MainActivity";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		Log.d(TAG, "onCreate");

		setContentView(R.layout.activity_main);

		Button btn = null;
		btn = (Button) findViewById(R.id.buttonStartSecondary);
		btn.setOnClickListener(this);
		btn = (Button) findViewById(R.id.buttonTaskMode);
		btn.setOnClickListener(this);
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
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
	protected void onPause() {
		super.onPause();
		Log.d(TAG, "onPause");
	}

	@Override
	protected void onStop() {
		super.onStop();
		Log.d(TAG, "onStop");
	}

	@Override
	protected void onNewIntent(Intent intent) {
		super.onNewIntent(intent);
		Log.d(TAG, "onNewIntent, time=" + intent.getLongExtra("time", 0));
	}

	@Override
	public void onClick(View v) {
		Intent intent = null;

		switch (v.getId()) {
		case R.id.buttonStartSecondary:
			intent = new Intent(this, SecondaryActivity.class);
			startActivity(intent);
			break;
		case R.id.buttonTaskMode:
			intent = new Intent(this, MainActivity.class);
			intent.putExtra("time", System.currentTimeMillis());
			startActivity(intent);
			break;
		}
	}
}
