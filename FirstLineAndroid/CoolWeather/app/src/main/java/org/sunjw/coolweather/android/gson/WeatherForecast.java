package org.sunjw.coolweather.android.gson;

import com.google.gson.annotations.SerializedName;

/**
 * Created by sunjw on 2017/10/31.
 */

public class WeatherForecast {

    public class Temperature {
        public String max;
        public String min;
    }

    public class More {
        @SerializedName("txt_d")
        public String info;
    }

    public String date;

    @SerializedName("tmp")
    public Temperature temperature;

    @SerializedName("cond")
    public More more;

}
