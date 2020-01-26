package org.sunjw.coolweather.android.service;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.IBinder;
import android.os.SystemClock;
import android.preference.PreferenceManager;

import org.sunjw.coolweather.android.WeatherActivity;
import org.sunjw.coolweather.android.gson.Weather;
import org.sunjw.coolweather.android.util.DataUtil;
import org.sunjw.coolweather.android.util.HttpUtil;

import java.io.IOException;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.Response;

public class AutoUpdateService extends Service {

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        updateWeather();
        updateBingPic();

        AlarmManager manager = (AlarmManager) getSystemService(ALARM_SERVICE);
        int hour8 = 8 * 60 * 60 * 1000;
        long triggerAtTime = SystemClock.elapsedRealtime() + hour8;
        Intent intentUpdate = new Intent(this, AutoUpdateService.class);
        PendingIntent pi = PendingIntent.getService(this, 0, intentUpdate, 0);
        manager.cancel(pi);
        manager.set(AlarmManager.ELAPSED_REALTIME_WAKEUP, triggerAtTime, pi);

        return super.onStartCommand(intent, flags, startId);
    }

    private void updateWeather() {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(this);
        String weatherString = prefs.getString(WeatherActivity.PREF_KEY_WEATHER, null);
        if (weatherString != null) {
            Weather weather = DataUtil.handleWeatherResponse(weatherString);
            String weatherId = weather.basic.weatherId;
            String weatherUrl = WeatherActivity.API_WEATHER_ADDRESS + "?cityid=" + weatherId +
                    "&key=" + WeatherActivity.API_KEY;
            HttpUtil.sendOkHttpRequest(weatherUrl, new Callback() {
                @Override
                public void onFailure(Call call, IOException e) {
                    e.printStackTrace();
                }

                @Override
                public void onResponse(Call call, Response response) throws IOException {
                    String reponseText = response.body().string();
                    Weather weatherNew = DataUtil.handleWeatherResponse(reponseText);
                    if (weatherNew != null && "ok".equals(weatherNew.status)) {
                        SharedPreferences.Editor editor =
                                PreferenceManager.getDefaultSharedPreferences(AutoUpdateService.this).edit();
                        editor.putString(WeatherActivity.PREF_KEY_WEATHER, reponseText);
                        editor.apply();
                    }
                }
            });
        }
    }

    private void updateBingPic() {
        String requestBingPic = WeatherActivity.API_BING_PIC_ADDRESS;
        HttpUtil.sendOkHttpRequest(requestBingPic, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                e.printStackTrace();
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                String bingPic = response.body().string();
                SharedPreferences.Editor editor =
                        PreferenceManager.getDefaultSharedPreferences(AutoUpdateService.this).edit();
                editor.putString(WeatherActivity.PREF_KEY_BING_PIC, bingPic);
                editor.apply();
            }
        });
    }

}
