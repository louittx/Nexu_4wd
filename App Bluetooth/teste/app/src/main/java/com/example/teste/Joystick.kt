package com.example.teste

import android.content.Context
import android.graphics.*
import android.util.AttributeSet
import android.view.MotionEvent
import android.view.View
import kotlin.math.*

class Joystick @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null
) : View(context, attrs) {

    private val basePaint = Paint().apply { color = Color.parseColor("#F2B87D") }
    private val handlePaint = Paint().apply { color = Color.parseColor("#EB9746") }

    private val baseCenter = PointF()
    private val handleCenter = PointF()

    private var baseRadius = 0f
    private var handleRadius = 0f

    var joystickListener: ((Int, Int) -> Unit)? = null

    override fun onSizeChanged(w: Int, h: Int, oldw: Int, oldh: Int) {
        val size = min(w, h)
        baseRadius = size / 2f
        handleRadius = baseRadius / 3f
        baseCenter.set(w / 2f, h / 2f)
        handleCenter.set(baseCenter.x, baseCenter.y)
    }

    override fun onDraw(canvas: Canvas) {
        canvas.drawCircle(baseCenter.x, baseCenter.y, baseRadius, basePaint)
        canvas.drawCircle(handleCenter.x, handleCenter.y, handleRadius, handlePaint)
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        val dx = event.x - baseCenter.x
        val dy = event.y - baseCenter.y
        val distance = hypot(dx, dy)
        val max = baseRadius - handleRadius

        when (event.action) {
            MotionEvent.ACTION_DOWN, MotionEvent.ACTION_MOVE -> {
                val ratio = if (distance > max) max / distance else 1f
                handleCenter.set(baseCenter.x + dx * ratio, baseCenter.y + dy * ratio)

                val angle = (((atan2(dy, dx) * (180 / PI))+360)%360).toInt()

                val force = ((distance / max).coerceIn(0f,1f)*255).toInt()
                joystickListener?.invoke(angle, force)
            }
            MotionEvent.ACTION_UP -> {
                handleCenter.set(baseCenter.x, baseCenter.y)
                joystickListener?.invoke(0, 0)
            }
        }
        invalidate()
        return true
    }
}