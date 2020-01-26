package org.sunjw.lbstest;

import android.util.Log;
import android.widget.TextView;

import com.baidu.location.BDLocation;
import com.baidu.location.BDLocationListener;

/**
 * Created by sunjw on 2017/9/3.
 */

public class MyLocationListener implements BDLocationListener {

    private String TAG = MyLocationListener.class.getSimpleName();

    private MainActivity mainActivity;

    public MyLocationListener(MainActivity mainActivity) {
        this.mainActivity = mainActivity;
    }

    @Override
    public void onReceiveLocation(BDLocation bdLocation) {
        Log.e(TAG, "onReceiveLocation");
        StringBuilder sbCurPos = new StringBuilder();
        sbCurPos.append("纬度: ").append(bdLocation.getLatitude()).append("\n");
        sbCurPos.append("经度: ").append(bdLocation.getLongitude()).append("\n");
        sbCurPos.append("国家: ").append(bdLocation.getCountry()).append("\n");
        sbCurPos.append("省: ").append(bdLocation.getProvince()).append("\n");
        sbCurPos.append("市: ").append(bdLocation.getCity()).append("\n");
        sbCurPos.append("区: ").append(bdLocation.getDistrict()).append("\n");
        sbCurPos.append("街道: ").append(bdLocation.getStreet()).append("\n");
        sbCurPos.append("定位方式: ");
        if (bdLocation.getLocType() == BDLocation.TypeGpsLocation) {
            sbCurPos.append("GPS\n");
        } else {
            sbCurPos.append("Network\n");
        }
        sbCurPos.append("时间: ").append(currentMillSecs()).append("\n");
        Log.e(TAG, sbCurPos.toString());

        mainActivity.updatePosText(sbCurPos.toString());
        mainActivity.navigateToFirst(bdLocation);
    }

    @Override
    public void onConnectHotSpotMessage(String s, int i) {

    }

    private long currentMillSecs() {
        return System.currentTimeMillis();
    }
}
