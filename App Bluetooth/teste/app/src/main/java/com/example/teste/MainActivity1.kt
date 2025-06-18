package com.example.teste

import android.Manifest
import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.content.ServiceConnection
import android.media.audiofx.Virtualizer
import android.os.Bundle
import android.os.IBinder
import android.view.View
import android.widget.Button
import android.widget.Toast
import androidx.annotation.RequiresPermission
import androidx.appcompat.app.AppCompatActivity

class MainActivity1 : AppCompatActivity() {
    //Starte the Service bluetooth
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

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    override fun onCreate(savedInstanceState: Bundle?) {
        // Start the code when the page is visible
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        // strat the connexion to Service
        Intent(this, BluethoothService::class.java).also { intent ->
            startService(intent)
            bindService(intent, connection, Context.BIND_AUTO_CREATE)
        }

        val LeafSearchtBt = findViewById<Button>(R.id.ButtonUpdatesTime) // Set button
        val LeafController = findViewById<Button>(R.id.ButtonLeafController) // Set button
        val LeafSetting = findViewById<Button>(R.id.ButtonLeafSetting)

        LeafSearchtBt.setOnClickListener { view: View->  // when clik in the buttonSearchtBt
            val SearchBtLeaf = Intent(this, MainActivity2::class.java)// Set the switch the MainActivity2
            startActivity(SearchBtLeaf)// Switch the page
        }
        LeafController.setOnClickListener { view: View-> // when clik in the button LeafController
            if (service.ConectedApareil == true) { // if the connexion to appareil bt
                val ControllerLeaf = Intent(this, MainActivity3::class.java) // set swith the MainActivity3
                startActivity(ControllerLeaf) // switch
            }
            else {
                Toast.makeText(this,"Not Conected to bluetooth", Toast.LENGTH_SHORT).show() // poster " Not Connected to bluetooth"
            }
        }
        LeafSetting.setOnClickListener {
            val SettingLeaf = Intent(this, MainActivity4::class.java)
            startActivity(SettingLeaf)
        }
    }
}