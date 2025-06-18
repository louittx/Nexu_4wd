package com.example.teste

import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.content.ServiceConnection
import android.os.Bundle
import android.os.Handler
import android.os.IBinder
import android.os.Looper
import android.util.Log
import android.widget.Button
import android.widget.ProgressBar
import android.widget.SeekBar
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import java.nio.Buffer
import kotlin.concurrent.thread

class MainActivity3 : AppCompatActivity() {
    private lateinit var service: BluethoothService
    private var bound = false
    var isThreadRunning = true

    private val connection = object : ServiceConnection {
        override fun onServiceConnected(className: ComponentName, serviceBinder: IBinder) {
            val binder = serviceBinder as BluethoothService.LocalBinder
            service = binder.getService()
            bound = true
        }

        override fun onServiceDisconnected(arg0: ComponentName) {
            bound = false
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main3)

        Intent(this, BluethoothService::class.java).also { intent ->
            startService(intent)
            bindService(intent, connection, Context.BIND_AUTO_CREATE)
        }

        // deffined element
        val Direction: SeekBar = findViewById(R.id.SeekBarDirection)
        val ProgressBarMotorLeftDown = findViewById<ProgressBar>(R.id.ProgressBarMotorLeftDown)
        val ProgressBarMotorLeftUp = findViewById<ProgressBar>(R.id.ProgressBarMotorLeftUp)
        val ProgressBarMotorRigthDown = findViewById<ProgressBar>(R.id.ProgressBarMotorRigthDown)
        val ProgressBarMotorRigthUp = findViewById<ProgressBar>(R.id.ProgressBarMotorRigthUp)
        val ProgressBarSensorLeft = findViewById<ProgressBar>(R.id.ProgressBarSensorLeft)
        val ProgressBarSensorRight = findViewById<ProgressBar>(R.id.ProgressBarSensorRight)
        val ProgressBarSensorFront = findViewById<ProgressBar>(R.id.ProgressBarSensorFront)
        val joystick = findViewById<Joystick>(R.id.joystick)

        // defin the var and val
        var getMessage : Long = 0
        var SpeedMotor = IntArray(8)
        var Sensor = IntArray(8)
        var Angle = 0
        var Speeds = 0
        var sendMessage = ByteArray(10)
        var lengthMessage = 0
        sendMessage[0]=0x17
        var DelayMsgBt = 10


        Direction.max = 255 // vlaue max for the Bar of progression
        Direction.setProgress(255/2)// set value of bar of progression
        sendMessage[3] = (255/2).toByte()

        Handler(Looper.getMainLooper()).postDelayed({ // delay a task
            if (service != null) { // if Service bluetooth is start
                DelayMsgBt = service.DelayMsgBluetooth

            }
        }, 1)

        // creat task for the send info for the bluetooth
        thread {
            Thread.sleep(1000) // delay 1s
            println("DelayMsgBt = ${DelayMsgBt}")
            while ( isThreadRunning) {
                    Thread.sleep(DelayMsgBt.toLong()) // delay 10 ms before each message
                    println("DelayMsgBt = ${DelayMsgBt}")
                    service.connectedThread?.write(sendMessage, 4) // send message
                    getMessage = service.incomingMessage // store the get message
                    lengthMessage = service.LengthMessage// store the lengh of message
            }
        }
        // creat task for set the bar progression for the sensors
        thread {
            Thread.sleep(1000) // delay 1s
            while (isThreadRunning) {
                    Thread.sleep(100)
                    val ValueGetMessage = getMessage // stoke the message
                    val MessageLength = lengthMessage // stoke length
                    if (((ValueGetMessage shr 32) == 0x44.toLong()) && (MessageLength == 5)) { //if message = message Speeds
                        for (i in 0..3) {
                            SpeedMotor[i] = ((ValueGetMessage shr (8 * i) and 0xFF).toInt() * 100) / 255 // stoke value for the 4 Speed of motor
                        }
                        ProgressBarMotorLeftDown.setProgress(SpeedMotor[0], true) // poster the value for the motor 1
                        ProgressBarMotorLeftUp.setProgress(SpeedMotor[1], true) // poster the value for the motor 2
                        ProgressBarMotorRigthDown.setProgress(SpeedMotor[2], true) // poster the value for the motor 3
                        ProgressBarMotorRigthUp.setProgress(SpeedMotor[3], true) // poster the value for the motor 4
                    }
                    if (((ValueGetMessage shr (10 * 4)) == 0x45.toLong()) && (MessageLength == 6)) {// if the message = message Sensor
                        for (i in 0..2) {
                            Sensor[i] = (((ValueGetMessage shr (12 * i) and 0xFFF)*100/0xFFF).toInt()) // stoke value sensor
                        }
                        ProgressBarSensorLeft.setProgress(Sensor[0], true) // poster the value for the sensor 1
                        ProgressBarSensorRight.setProgress(Sensor[1], true) // poster the value for the sensor 2
                        ProgressBarSensorFront.setProgress(Sensor[2], true) // poster the value for the sensor 3
                    }
            }
        }
        // task for the vreifet if the devices conecte to bluetooth
        thread {
            Thread.sleep(1000)// delay 1s

            while (isThreadRunning) {
                Thread.sleep(1250) // delay 1s before the verifie

                if (!service.ConectedApareil) { // if device connected
                    runOnUiThread {
                        Toast.makeText(this, "Device not connected", Toast.LENGTH_SHORT).show()

                        val intent = Intent(this, MainActivity2::class.java) // set the swith the page
                        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP or Intent.FLAG_ACTIVITY_NEW_TASK) // clear MainActivitity 2 befor the swith sreen
                        startActivity(intent) // swith sreen
                        finish() // finish the thread
                    }
                    break
                }
            }
        }


        // Joystick
        joystick.joystickListener = { angle, strength -> // if joysitiq change of value
            Speeds = strength // stoke the strength
            Angle = (angle*255)/360 // convret the angle of 0 to 360 in 0 to 255
            sendMessage[1] = Speeds.toByte() // convret the valus in Byte then stoke the value in the sendMessage
            sendMessage[2] = Angle.toByte()
            println("Value speeds bluetooth = ${angle}")
            Thread.sleep(10)

        }
        // sliceur
        Direction.setOnSeekBarChangeListener(
            object : SeekBar.OnSeekBarChangeListener {
                    override fun onProgressChanged(
                        seek: SeekBar,
                        progress: Int,
                        fromUser: Boolean
                    ) { // if sliceur changed of value
                        val DirectionOutput = seek.progress // stoke the nex value of slicer
                        sendMessage[3] = DirectionOutput.toByte() // set value Direction for the sendMessage
                        Thread.sleep(10)
                    }

                    override fun onStartTrackingTouch(seek: SeekBar) {} // if the slicer is start
                    override fun onStopTrackingTouch(seek: SeekBar) { // if slicer is stop
                        Direction.setProgress(255/2) // put the slicer back halfway
                        sendMessage[3] = (255/2).toByte() // set value 127 for the sendMessage
                        Thread.sleep(10)
                    }
                })
    }
    override fun onDestroy() {
        super.onDestroy()
        isThreadRunning = false
    }
}