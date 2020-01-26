package org.sunjw.broadcastbestpractice;

import android.content.Intent;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.Toast;

public class LoginActivity extends BaseActivity {

    private static final String KEY_REMEMBER_PASSWORD = "remember_password";
    private static final String KEY_ACCOUNT = "account";
    private static final String KEY_PASSWORD = "password";

    private SharedPreferences pref = null;
    private SharedPreferences.Editor prefEditor = null;

    private EditText etAccount = null;
    private EditText etPassword = null;

    private CheckBox cbRememberPass = null;
    private Button btLogin = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        pref = PreferenceManager.getDefaultSharedPreferences(this);

        etAccount = (EditText) findViewById(R.id.et_account);
        etPassword = (EditText) findViewById(R.id.et_password);

        cbRememberPass = (CheckBox) findViewById(R.id.cb_remember_pass);
        btLogin = (Button) findViewById(R.id.bt_login);

        boolean isRemeberPass = pref.getBoolean(KEY_REMEMBER_PASSWORD, false);
        cbRememberPass.setChecked(isRemeberPass);
        if (isRemeberPass) {
            String account = pref.getString(KEY_ACCOUNT, "");
            String password = pref.getString(KEY_PASSWORD, "");
            etAccount.setText(account);
            etPassword.setText(password);
        }

        btLogin.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                doLogin();
            }
        });
    }

    private void doLogin() {
        String account = etAccount.getText().toString();
        String password = etPassword.getText().toString();
        if (account.equals("admin") && password.equals("123456")) {
            prefEditor = pref.edit();
            if (cbRememberPass.isChecked()) {
                prefEditor.putBoolean(KEY_REMEMBER_PASSWORD, true);
                prefEditor.putString(KEY_ACCOUNT, account);
                prefEditor.putString(KEY_PASSWORD, password);
            } else {
                prefEditor.putBoolean(KEY_REMEMBER_PASSWORD, false);
                prefEditor.putString(KEY_ACCOUNT, "");
                prefEditor.putString(KEY_PASSWORD, "");
            }
            prefEditor.apply();

            Intent intent = new Intent(LoginActivity.this, MainActivity.class);
            startActivity(intent);
            finish();
        } else {
            Toast.makeText(LoginActivity.this, "Account or password is invalid",
                    Toast.LENGTH_SHORT).show();
        }
    }
}
