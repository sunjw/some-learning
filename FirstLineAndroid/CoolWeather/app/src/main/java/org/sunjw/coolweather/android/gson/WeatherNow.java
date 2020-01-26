package org.sunjw.coolweather.android.gson;

import com.google.gson.annotations.SerializedName;

/**
 * Created by sunjw on 2017/10/31.
 */

public class WeatherNow {

    public class More {
        @SerializedName("txt")
        public String info;
    }

    @SerializedName("tmp")
    public String temperature;

    @SerializedName("cond")
    public More more;

}
