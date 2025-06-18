package com.example.teste

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.content.ServiceConnection
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import android.os.Handler
import android.os.IBinder
import android.os.Looper
import android.util.Log
import android.widget.ArrayAdapter
import android.widget.Button
import android.widget.EditText
import android.widget.ListView
import android.widget.Toast
import androidx.annotation.RequiresPermission
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat

class MainActivity2 : AppCompatActivity() {
    private lateinit var service: BluethoothService
    private var bound = false
    var NameBtConnet = ""
    // Start the conextion to Service
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

    private lateinit var bluetoothAdapter: BluetoothAdapter

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    override fun onCreate(savedInstanceState: Bundle?) {

        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main2)
        // Start the connexition to Service
        Intent(this, BluethoothService::class.java).also { intent ->
            startService(intent)
            bindService(intent, connection, Context.BIND_AUTO_CREATE)
        }

        val list_bt = findViewById<ListView>(R.id.list_BT) // Set listView
        val RefeshButton = findViewById<Button>(R.id.ButtonRefersh)// Set button Refesh
        val DecoButton = findViewById<Button>(R.id.ButtonDeco)// set button deconed

        var deviceNames = BtInit("Name") // Stoket in the list the name pared devices and stat the bluettoth
        var deviceAddress = BtInit("adresse") // stoket in nthe list the Address pared devices
        var BtNames = ArrayList<PostData>() // Creat the list

        for(name in deviceNames ) {
            BtNames.add(PostData(name, false)) // stok the name without not conneted
        }



        var adapter = BtAdapteur(this, android.R.layout.simple_list_item_1, BtNames) // poster teh list view
        list_bt.adapter = adapter

        list_bt.setOnItemClickListener { _, _, position, _ -> // clik in the list view, recover  position og item clik
            val name = BtNames[position] // stokte the name of item clik
            val address = deviceAddress[position] // stoke the adresse item clik
            Toast.makeText(this, "$name ($address)", Toast.LENGTH_SHORT).show() // poster the name and adreesse of item clik

            for (item in BtNames) {
                item.isVisible = false // remove of the connecting bars
            }
            name.isVisible = true // put the item clik in conction bar
            adapter.notifyDataSetChanged() // updates the listView

            // connecde of the appariel bluettoth
            val device = bluetoothAdapter.getRemoteDevice(address)

            if (device != null) { // if thre is appareil bT
                // check if bluetooth permission is granted
                if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED &&
                    ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_SCAN) == PackageManager.PERMISSION_GRANTED) {

                    if (bluetoothAdapter.isDiscovering) { // if recherche the bluetooth
                        bluetoothAdapter.cancelDiscovery() // stop the recherche
                    }
                    val connectThread = service.ConnectThread(device) // connecter of the bluetooth
                    connectThread.start()// stat the connecte of the bluetooth
                }
            }
        }

        // allow activer the bars de connextion even the swith the page
        Handler(Looper.getMainLooper()).postDelayed({ // delay a task
            if (service != null && service.ConectedApareil == true) { // if Service bluetooth is start
                NameBtConnet = service.NameBtConnet // Stok the name in the Service

                val index = BtNames.indexOfFirst { it.name == NameBtConnet } // Find the position of name connected

                if (index != -1) {
                    BtNames[index] = BtNames[index].copy(isVisible = true) // poster the appreil coneter with the bars connecet
                    adapter.notifyDataSetChanged() // uptate the liste View
                }
            }
        }, 1)


        RefeshButton.setOnClickListener { // clisk in the button Refresh
            deviceNames = BtInit("Name") // init bluettoth and stoke the lis name of apreid device
            deviceAddress = BtInit("adresse") // iniy bluetooth ans tok the list adresse od apreid devices
            adapter = BtAdapteur(this, android.R.layout.simple_list_item_1, BtNames) // set the list Viex
            list_bt.adapter = adapter // poster the list view
            Toast.makeText(this,"Refresh", Toast.LENGTH_SHORT).show()//poster "Refresh" in the sreen

            // poste the bars connected if the appareil is connect
            if (service != null && service.ConectedApareil == true) {
                NameBtConnet = service.NameBtConnet

                val index = BtNames.indexOfFirst { it.name == NameBtConnet } // cannais la psotion de notre appareil conneted

                if (index != -1) {
                    BtNames[index] = BtNames[index].copy(isVisible = true) // metre notre apppareil en visible
                    adapter.notifyDataSetChanged() // actualiser la liste
                }
            }
            else {

                for (item in BtNames) {
                    item.isVisible = false // stok the name without not conneted
                }
            }
        }


        DecoButton.setOnClickListener { // click in the button deco
            if (service.ConectedApareil == true){
                service.connectedThread?.cancel() // deconceted of the appareil connect
                for (item in BtNames) {
                    item.isVisible = false // stok the name without not conneted
                }
                adapter.notifyDataSetChanged() // uplate  the listView
                NameBtConnet = "" // stok not name for the connect appareil
            }
            else {
                Toast.makeText(this,"Not Conected to bluetooth", Toast.LENGTH_SHORT).show()
            }
        }
    }


    // fonction for initialise the bluetooth
    private fun BtInit(connection: String): List<String> {
        // actived the permision of bluetooth
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            val permissions = mutableListOf<String>()
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                permissions.add(Manifest.permission.BLUETOOTH_CONNECT)
            }
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
                permissions.add(Manifest.permission.BLUETOOTH_SCAN)
            }

            if (permissions.isNotEmpty()) {
                ActivityCompat.requestPermissions(this, permissions.toTypedArray(), 1)
                return emptyList()
            }
        }

        val bluetoothManager: BluetoothManager = getSystemService(BluetoothManager::class.java) // acesse to service
        bluetoothAdapter = bluetoothManager.adapter

        val deviceNames = mutableListOf<String>()
        val deviceAddresses = mutableListOf<String>()

        if (bluetoothAdapter == null) {
            Toast.makeText(this, "Phone not compatible", Toast.LENGTH_SHORT).show()
            return emptyList()// stope the recher of the bluetooth
        } else if (!bluetoothAdapter.isEnabled) { // if the bluetooth is not activate
            val enableBtIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            startActivityForResult(enableBtIntent, 1) // poster the affiche to ask the user to activate it
        }
        // another check for the bleutooth
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.BLUETOOTH_CONNECT), 1)
                return emptyList()
            }
        }

        val pairedDevices: Set<BluetoothDevice>? = bluetoothAdapter.bondedDevices // stoke pareid divice

        if (pairedDevices != null && pairedDevices.isNotEmpty()) { // if there is pareid device
            for (device in pairedDevices) {
                deviceNames.add(device.name ?: "Unknow device") // add name for the list name
                deviceAddresses.add(device.address) // add adresse for the list adresse
            }
        } else {
            Toast.makeText(this, "Never devies", Toast.LENGTH_SHORT).show()
        }

        return if (connection.lowercase() == "adresse") {
            deviceAddresses // return Adresse
        } else {
            deviceNames // return Names
        }
    }
}

