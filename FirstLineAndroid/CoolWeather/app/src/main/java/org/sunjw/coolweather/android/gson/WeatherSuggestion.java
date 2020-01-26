package org.sunjw.coolweather.android.gson;

import com.google.gson.annotations.SerializedName;

/**
 * Created by sunjw on 2017/10/31.
 */

public class WeatherSuggestion {

    public class Comfort {
        @SerializedName("txt")
        public String info;
    }

    public class CarWash {
        @SerializedName("txt")
        public String info;
    }

    public class Sport {
        @SerializedName("txt")
        public String info;
    }

    @SerializedName("comf")
    public Comfort comfort;

    @SerializedName("cw")
    public CarWash carWash;

    public Sport sport;

}
