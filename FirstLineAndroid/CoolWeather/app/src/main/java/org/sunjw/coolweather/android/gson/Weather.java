package org.sunjw.coolweather.android.gson;

import com.google.gson.annotations.SerializedName;

import java.util.List;

/**
 * Created by sunjw on 2017/10/31.
 */

public class Weather {

    public String status;

    public WeatherBasic basic;

    public WeatherAQI aqi;

    public WeatherNow now;

    public WeatherSuggestion suggestion;

    @SerializedName("daily_forecast")
    public List<WeatherForecast> forecastList;

}
