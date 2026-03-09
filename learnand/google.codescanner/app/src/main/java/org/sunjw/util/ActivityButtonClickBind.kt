package org.sunjw.util

import android.app.Activity
import android.view.View

/**
 * Created by sunjw on 16/8/21.
 */
class ActivityButtonClickBind(activity: Activity?) : View.OnClickListener {
    private var activity: Activity? = null

    private val buttonActionMap: MutableMap<Int?, ButtonClickAction?> =
        HashMap<Int?, ButtonClickAction?>()

    fun interface ButtonClickAction {
        fun onClick(v: View?)
    }

    init {
        this.activity = activity
    }

    override fun onClick(v: View) {
        val viewId = v.id
        val action = buttonActionMap[viewId]
        action?.onClick(v)
    }

    fun bindButtonAction(buttonId: Int, action: ButtonClickAction?) {
        val viewBtn = activity!!.findViewById<View?>(buttonId)
        if (viewBtn != null) {
            buttonActionMap[buttonId] = action
            viewBtn.setOnClickListener(this)
        }
    }
}
