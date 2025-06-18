package com.example.teste

import android.app.Service
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.content.Intent
import android.os.Binder
import android.os.Handler
import android.os.IBinder
import android.os.Looper
import android.util.Log
import android.widget.Toast
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.cancel
import java.io.IOException
import java.util.UUID

class BluethoothService : Service(){ // create the class
    // the value private of the Service
    private val binder = LocalBinder()
    private val scope = CoroutineScope(Dispatchers.Default + SupervisorJob())
    private val MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")
    private val handler = Handler(Looper.getMainLooper())

    // the value can be called in the other pages
    var connectedThread: ConnectedThread? = null
    var incomingMessage = 0L // value message bluetooth (L = long )
    var LengthMessage = 0 // length of Message Bluetooth
    var ConectedApareil = false // value for know if the device is connect
    var NameBtConnet ="" // stoke the Name of device
    var DelayMsgBluetooth = 10

    inner class LocalBinder : Binder() {
        fun getService(): BluethoothService = this@BluethoothService // bind the class to Service
    }

    // for the activity linked to the Servicex
    override fun onBind(intent: Intent?): IBinder {
        return binder
    }

    // stop the service
    override fun onDestroy() {
        super.onDestroy()
        scope.cancel()
    }
    // start the connection to bluetooth
    internal inner class ConnectThread(private val device: BluetoothDevice) : Thread() {
        //Lazy creation of a Bluetooth socket (RFCOMM) with shared UUID
        private val mmSocket: BluetoothSocket? by lazy(LazyThreadSafetyMode.NONE) {
            device.createRfcommSocketToServiceRecord(MY_UUID)
        }

        override fun run() {
            try {
                mmSocket?.let { socket ->
                    socket.connect() // connect of device bluetooth
                    handler.post {
                        NameBtConnet = device.name // recovery the name of the device bluetooth
                        Toast.makeText(this@BluethoothService, "${NameBtConnet} is connect", Toast.LENGTH_SHORT).show() // poster the name of
                        ConectedApareil = true  // change value to say that device is connect
                    }
                    // start bluetooth after the connexion
                    connectedThread = ConnectedThread(socket)
                    connectedThread?.start()
                }
            } catch (e: IOException) {
                handler.post {
                    Toast.makeText(this@BluethoothService, "Error : Device is not compatible or connexion is not compatible.", Toast.LENGTH_LONG).show()
                }
                try {
                    cancel()
                } catch (closeException: IOException) {}
            }
        }

        fun cancel() {
            try {
                mmSocket?.close() // disconnect the device of bluetooth
                ConectedApareil = false
            } catch (e: IOException) {}
        }
    }
    inner class ConnectedThread(private val mmSocket: BluetoothSocket) : Thread() {
        private val mmInStream = mmSocket.inputStream // Message to receive
        private val mmOutStream = mmSocket.outputStream // Message to send
        private val mmBuffer = ByteArray(1024)

        override fun run() {
            while (true) {
                val numBytes = try {
                    mmInStream.read(mmBuffer) // read the message and stoke in the buffer
                } catch (e: IOException) {
                    ConectedApareil = false
                    Log.d("MonTag", "Input stream deconnecte", e)
                    break
                }
                LengthMessage = numBytes // stoke the length of message in the value
                incomingMessage = byteArrayToLong(mmBuffer, numBytes) // convert the buffer to long and stoke the value
            }
        }

        fun write(bytes: ByteArray , length: Int) {
            try {
                mmOutStream.write(bytes, 0, length) // send message to bluetooth
            } catch (e: IOException) {
                Log.e("MonTag", "Error when sending", e)
            }
        }

        fun cancel() {
            try {
                mmSocket.close() // disconnect to device
                ConectedApareil = false
            } catch (e: IOException) {}
        }
    }

    // fonction for the convert the byteArray to long
    fun byteArrayToLong(byteArray: ByteArray, length: Int): Long {
        var result = 0L
        for (i in 0 until length) {
            result = result shl 8
            result = result or (byteArray[i].toLong() and 0xFF)
        }
        return result
    }
}