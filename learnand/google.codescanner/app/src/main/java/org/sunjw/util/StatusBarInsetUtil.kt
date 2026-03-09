package org.sunjw.util

import android.util.TypedValue
import android.view.View
import android.view.ViewGroup.MarginLayoutParams
import androidx.appcompat.app.AppCompatActivity

/**
 * Created by sunjw on 4/11/24.
 */
object StatusBarInsetUtil {
    private fun getStatusBarHeightPx(activity: AppCompatActivity): Int {
        val metrics = activity.getResources().displayMetrics
        var statusBarHeightPx =
            TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 25f, metrics).toInt()
        val statusBarHeightId =
            activity.getResources().getIdentifier("status_bar_height", "dimen", "android")
        if (statusBarHeightId > 0) {
            statusBarHeightPx = activity.getResources().getDimensionPixelSize(statusBarHeightId)
        }
        return statusBarHeightPx
    }

    private fun getActionBarHeightPx(activity: AppCompatActivity): Int {
        var actionBarHeight = 0
        val tv = TypedValue()
        if (activity.getTheme().resolveAttribute(android.R.attr.actionBarSize, tv, true)) {
            actionBarHeight = TypedValue.complexToDimensionPixelSize(
                tv.data,
                activity.getResources().displayMetrics
            )
        }
        return actionBarHeight
    }

    fun updateControlMarginFullScreen(
        activity: AppCompatActivity,
        viewId: Int,
        withActionBar: Boolean
    ) {
        var statusBarHeightPx = getStatusBarHeightPx(activity)
        if (withActionBar) {
            val actionBarHeightPx = getActionBarHeightPx(activity)
            statusBarHeightPx += actionBarHeightPx
        }
        val controlView = activity.findViewById<View>(viewId)
        val controlLayoutParams = controlView.layoutParams as MarginLayoutParams
        controlLayoutParams.setMargins(
            controlLayoutParams.leftMargin,
            controlLayoutParams.topMargin + statusBarHeightPx,
            controlLayoutParams.rightMargin,
            controlLayoutParams.bottomMargin
        )
        controlView.setLayoutParams(controlLayoutParams)
    }

    fun updateControlPaddingFullScreen(activity: AppCompatActivity, viewId: Int) {
        val statusBarHeightPx = getStatusBarHeightPx(activity)
        val controlView = activity.findViewById<View>(viewId)
        controlView.setPadding(
            controlView.getPaddingLeft(),
            controlView.paddingTop + statusBarHeightPx,
            controlView.getPaddingRight(),
            controlView.paddingBottom
        )
    }
}
