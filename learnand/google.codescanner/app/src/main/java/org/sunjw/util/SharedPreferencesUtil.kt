package org.sunjw.util

import android.content.Context
import android.content.SharedPreferences
import androidx.core.content.edit

/**
 * Created by sunjw on 16/8/21.
 */
object SharedPreferencesUtil {
    private val TAG = SharedPreferencesUtil::class.java.getCanonicalName()

    private fun getSharedPreferences(context: Context, preferenceName: String?): SharedPreferences {
        return context.getSharedPreferences(preferenceName, Context.MODE_PRIVATE)
    }

    fun hasSavedPreference(context: Context, preferenceName: String?, key: String?): Boolean {
        val sharedPreferences = getSharedPreferences(context, preferenceName)
        return sharedPreferences.contains(key)
    }

    fun getPreferenceString(
        context: Context,
        preferenceName: String?,
        key: String?,
        defaultVal: String?
    ): String? {
        val sharedPreferences = getSharedPreferences(context, preferenceName)
        return sharedPreferences.getString(key, defaultVal)
    }

    fun putPreferenceString(
        context: Context,
        preferenceName: String?,
        key: String?,
        value: String?
    ) {
        val sharedPreferences = getSharedPreferences(context, preferenceName)
        sharedPreferences.edit(commit = true) {
            putString(key, value)
        }
    }
}
