package com.example.teste

import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.content.ServiceConnection
import android.os.Bundle
import android.os.Handler
import android.os.IBinder
import android.os.Looper
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import android.widget.Toast
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat

class MainActivity4 : AppCompatActivity() {
    private lateinit var service: BluethoothService
    private var bound = false

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
        setContentView(R.layout.activity_main4)

        Intent(this, BluethoothService::class.java).also { intent ->
            startService(intent)
            bindService(intent, connection, Context.BIND_AUTO_CREATE)
        }

        // defined element
        val ButtonUpdates = findViewById<Button>(R.id.ButtonUpdatesTime)
        val SetDataTime = findViewById<EditText>(R.id.DataTime)
        val TextValue = findViewById<TextView>(R.id.PosterValueSet)


        ButtonUpdates.setOnClickListener {
            val TextDataTime = SetDataTime.text.toString()
            if (TextDataTime.trim().isEmpty() || !TextDataTime.all { it.isDigit() }) {
                Toast.makeText(this,"Not data or data is false", Toast.LENGTH_SHORT).show()
            }
            else{
                service.DelayMsgBluetooth = TextDataTime.toInt()
                //Toast.makeText(this,"${service.DelayMsgBluetooth}", Toast.LENGTH_SHORT).show()
                TextValue.text = "Value = ${service.DelayMsgBluetooth}"
            }
        }
        Handler(Looper.getMainLooper()).postDelayed({ // delay a task
            if (service != null) { // if Service bluetooth is start
                TextValue.text = "Value = ${service.DelayMsgBluetooth}"
            }
        }, 1)
    }
}