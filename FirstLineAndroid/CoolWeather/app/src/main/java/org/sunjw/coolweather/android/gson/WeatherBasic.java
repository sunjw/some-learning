package org.sunjw.coolweather.android.gson;

import com.google.gson.annotations.SerializedName;

/**
 * Created by sunjw on 2017/10/31.
 */

public class WeatherBasic {

    public class Update {
        @SerializedName("loc")
        public String updateTime;
    }

    @SerializedName("city")
    public String cityName;

    @SerializedName("id")
    public String weatherId;

    public Update update;

}
