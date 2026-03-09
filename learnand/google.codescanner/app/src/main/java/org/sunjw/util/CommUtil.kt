package org.sunjw.util

import kotlin.math.abs

object CommUtil {
    val currentMillis: Long
        get() = System.currentTimeMillis()

    fun averageOnDegree(degrees: FloatArray?): Float {
        val count = degrees!!.size
        if (count == 0) {
            return 0f
        }
        var maxDiff = 0f
        for (i in 0..<count) {
            for (j in i + 1..<count) {
                val diff = abs(degrees[j] - degrees[i])
                if (diff > maxDiff) {
                    maxDiff = diff
                }
            }
        }
        if (maxDiff > 180) {
            var minIdx = 0
            var min = degrees[0]
            for (i in 0..<count) {
                if (degrees[i] < min) {
                    min = degrees[i]
                    minIdx = i
                }
            }
            if (degrees[minIdx] < 0) {
                degrees[minIdx] = degrees[minIdx] + 360
                return averageOnDegree(degrees)
            }
        }
        var avg = 0f
        for (i in 0..<count) {
            avg += degrees[i]
        }
        avg /= count
        if (avg > 180) {
            avg -= 360
        }
        return avg
    }
}
