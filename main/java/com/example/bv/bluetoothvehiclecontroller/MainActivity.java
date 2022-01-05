package com.example.bv.bluetoothvehiclecontroller;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Set;
import java.util.UUID;

import android.Manifest;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Build;
import android.os.ParcelUuid;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.ArraySet;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import static com.example.bv.bluetoothvehiclecontroller.R.id.tvPD;
import static java.lang.Thread.currentThread;
import static java.lang.Thread.sleep;

public class MainActivity extends AppCompatActivity implements AdapterView.OnItemClickListener {
    private static final String TAG = "debugging";

    ArrayAdapter<String> listAdapter;
    ListView listView;
    BluetoothAdapter btAdapter;
    Set<BluetoothDevice> devicesArray;
    ArrayList<String> pairedDevices;
    ArrayList<BluetoothDevice> availableDevices;
    public static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    protected static final int SUCCESS_CONNECT = 0;
    protected static final int MESSAGE_READ = 1;
    protected static final int FORWARD = 2;
    protected static final int BACK = 3;
    protected static final int LEFT = 4;
    protected static final int RIGHT = 5;
    protected static final int STOP_COMMAND = 6;
    protected static final int SEARCH_COMMAND = 7;
    protected static final int REQUEST_CONTROL_COMMAND = 8;
    protected static final int A1_COMMAND = 9;
    protected static final int A2_COMMAND = 10;
    protected static final int A3_COMMAND = 11;
    protected static final int B1_COMMAND = 12;
    protected static final int B2_COMMAND = 13;
    protected static final int B3_COMMAND = 14;
    protected static final int C1_COMMAND = 15;
    protected static final int C2_COMMAND = 16;
    protected static final int C3_COMMAND = 17;
    ConnectedThread connectedThread;
    boolean receiverFlag = false;

    boolean controller_activity_Flag = false;
    boolean main_controller_Flag = false;

    Handler mHandler = new Handler()
    {
        @Override
        public void handleMessage(Message msg)
        {
            // TODO Auto-generated method stub
            Log.i(TAG, "in handler");
            super.handleMessage(msg);
            switch(msg.what)
            {
                case SUCCESS_CONNECT:
                    // DO something
                    connectedThread = new ConnectedThread((BluetoothSocket)msg.obj);
                    //Toast.makeText(getApplicationContext(), "CONNECT", Toast.LENGTH_LONG).show();
                    if(main_controller_Flag)
                    {
                        String mainControlActivityString = "mca.";
                        connectedThread.write(mainControlActivityString.getBytes());
                        setContentView(R.layout.main_controller_activity);
                    }
                    else
                    {
                        String controllerActivityString = "controller.";
                        connectedThread.write(controllerActivityString.getBytes());
                        setContentView(R.layout.controller_activity);
                    }
                    Log.i(TAG, "connected");
                    break;
                case MESSAGE_READ:
                    Log.i(TAG, "Attempted to display message");
                    byte[] readBuf = (byte[])msg.obj;
                    String message = new String(readBuf);

                    if(main_controller_Flag)
                    {
                        setTextViewText(message);
                    }
                    /*if(!message.contains("reached"))
                    {
                        Toast.makeText(getApplicationContext(), "Still moving", Toast.LENGTH_SHORT).show();
                    }
                    Toast.makeText(getApplicationContext(), message, Toast.LENGTH_LONG).show();*/
                    break;
                case SEARCH_COMMAND:
                    Log.d(TAG, "Start Command being handled");
                    String search = "search.";
                    connectedThread.write(search.getBytes());
                    connectedThread.run();
                    break;
                case REQUEST_CONTROL_COMMAND:
                    Log.d(TAG, "Request Control Command being handled");
                    String rqstCntrl = "remote.";
                    connectedThread.write(rqstCntrl.getBytes());
                    connectedThread.run();
                    break;
                case A1_COMMAND:
                    Log.d(TAG, "A1 Command being handled");
                    String A1 = "1:1.";
                    connectedThread.write(A1.getBytes());
                    connectedThread.run();
                    break;
                case A2_COMMAND:
                    Log.d(TAG, "Request Control Command being handled");
                    String A2 = "2:1.";
                    connectedThread.write(A2.getBytes());
                    connectedThread.run();
                    break;
                case A3_COMMAND:
                    Log.d(TAG, "Request Control Command being handled");
                    String A3 = "3:1.";
                    connectedThread.write(A3.getBytes());
                    connectedThread.run();
                    break;
                case B1_COMMAND:
                    Log.d(TAG, "Request Control Command being handled");
                    String B1 = "1:2.";
                    connectedThread.write(B1.getBytes());
                    connectedThread.run();
                    break;
                case B2_COMMAND:
                    Log.d(TAG, "Request Control Command being handled");
                    String B2 = "2:2.";
                    connectedThread.write(B2.getBytes());
                    connectedThread.run();
                    break;
                case B3_COMMAND:
                    Log.d(TAG, "Request Control Command being handled");
                    String B3 = "3:2.";
                    connectedThread.write(B3.getBytes());
                    connectedThread.run();
                    break;
                case C1_COMMAND:
                    Log.d(TAG, "Request Control Command being handled");
                    String C1 = "1:3.";
                    connectedThread.write(C1.getBytes());
                    connectedThread.run();
                    break;
                case C2_COMMAND:
                    Log.d(TAG, "Request Control Command being handled");
                    String C2 = "2:3.";
                    connectedThread.write(C2.getBytes());
                    connectedThread.run();
                    break;
                case C3_COMMAND:
                    Log.d(TAG, "Request Control Command being handled");
                    String C3 = "3:3.";
                    connectedThread.write(C3.getBytes());
                    connectedThread.run();
                    break;
                case FORWARD:
                    Log.d(TAG, "Forward Command being handled");
                    String fs = "forward.";
                    connectedThread.write(fs.getBytes());
                    connectedThread.run();
                    //Toast.makeText(getApplicationContext(), "Stop Command Sent", Toast.LENGTH_LONG).show();
                    break;
                case BACK:
                    Log.d(TAG, "Back Command being handled");
                    String bs = "back.";
                    connectedThread.write(bs.getBytes());
                    connectedThread.run();
                    //Toast.makeText(getApplicationContext(), "Stop Command Sent", Toast.LENGTH_LONG).show();
                    break;
                case LEFT:
                    Log.d(TAG, "Left Command being handled");
                    String ls = "left.";
                    connectedThread.write(ls.getBytes());
                    connectedThread.run();
                    //Toast.makeText(getApplicationContext(), "Stop Command Sent", Toast.LENGTH_LONG).show();
                    break;
                case RIGHT:
                    Log.d(TAG, "Right Command being handled");
                    String rs = "right.";
                    connectedThread.write(rs.getBytes());
                    connectedThread.run();
                    //Toast.makeText(getApplicationContext(), "Stop Command Sent", Toast.LENGTH_LONG).show();
                    break;
                case STOP_COMMAND:
                    Log.d(TAG, "Stop Command being handled");
                    String ss = "stop.";
                    connectedThread.write(ss.getBytes());
                    //Toast.makeText(getApplicationContext(), "Stop Command Sent", Toast.LENGTH_LONG).show();
                    break;
            }
        }
    };

    private final BroadcastReceiver btReceiver = new BroadcastReceiver()
    {
        @Override
        public void onReceive(Context context, Intent intent)
        {
            String action = intent.getAction();
            //Toast.makeText(getApplicationContext(), "Device Found", Toast.LENGTH_SHORT).show();
            Log.d(TAG,"btReceiver created");
            if (action.equals(BluetoothDevice.ACTION_FOUND))
            {
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

                if(availableDevices.size() > 0)
                {
                    for (int i = 0; i < availableDevices.size(); i++)
                    {
                        Log.d(TAG, "Checking devices");
                        if (device.getAddress().toString().equals(availableDevices.get(i).toString()))
                        {
                            Log.d(TAG, "This is not the device you are looking for");
                            break;
                        }
                        else
                        {
                            availableDevices.add(device);
                            break;
                        }
                    }
                }
                else
                {
                    availableDevices.add(device);
                }

                Log.d(TAG,"added device to devices");
                String s = "";
                for (int a = 0; a < pairedDevices.size(); a++)
                {
                    Log.d(TAG,"Entered for loop");
                    if (device.getName().equals(pairedDevices.get(a)))
                    {
                        //append
                        s = "(Available)";
                        pairedDevices.set(a, pairedDevices.get(a)+" "+s);
                        Log.d(TAG,"Marked available device");
                    }
                }
                listAdapter.notifyDataSetChanged();
                Log.d(TAG,"devices added");
            }
        }
    };

    @Override
    protected void onDestroy()
    {
        super.onDestroy();

        if (receiverFlag)
        {
            unregisterReceiver(btReceiver);
        }
        connectedThread.cancel();
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        init();

        //Toast.makeText(getApplicationContext(), "OnCreate", Toast.LENGTH_SHORT).show();
        if(btAdapter==null){
            Toast.makeText(getApplicationContext(), "No bluetooth detected", Toast.LENGTH_SHORT).show();
            finish();
        }
        else{
            if(!btAdapter.isEnabled()){
                turnOnBT();
            }

            getPairedDevices();
            createListView();
        }
    }

    private void init()
    {
        // TODO Auto-generated method stub
        pairedDevices = new ArrayList<>();
        btAdapter = BluetoothAdapter.getDefaultAdapter();
        availableDevices = new ArrayList<BluetoothDevice>();

        checkBTPermissions();
    }

    private void turnOnBT()
    {
        // TODO Auto-generated method stub
        Intent intent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
        startActivityForResult(intent, 1);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        // TODO Auto-generated method stub
        super.onActivityResult(requestCode, resultCode, data);
        if(resultCode == RESULT_CANCELED)
        {
            Toast.makeText(getApplicationContext(), "Bluetooth must be enabled to continue", Toast.LENGTH_SHORT).show();
            finish();
        }
        if(resultCode == RESULT_OK)
        {
            getPairedDevices();
            createListView();
        }
    }

    private void getPairedDevices()
    {
        // TODO Auto-generated method stub
        Log.d(TAG,"Got Paired Devices");
        pairedDevices.clear();
        devicesArray = btAdapter.getBondedDevices();
        if(devicesArray.size()>0)
        {
            for(BluetoothDevice device:devicesArray){
                pairedDevices.add(device.getName());
            }
        }
    }

    private void createListView()
    {
        // TODO Auto-generated method stub
        listView = (ListView) findViewById(R.id.listView);
        listAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, pairedDevices);
        listView.setAdapter(listAdapter);
        listView.setOnItemClickListener(this);
    }

    private void setTextViewText(String text)
    {
        TextView myTextView = (TextView)findViewById(R.id.statusTextView);
        myTextView.setText(text);
    }

    public void getPairedDevicesBtn(View view)
    {
        main_controller_Flag = true;
        //getPairedDevices();
        //createListView();
        Log.d(TAG,"Refreshed screen");
    }

    public void scanButtonBtn(View view)
    {
        Log.d(TAG,"scanButton Pressed");
        main_controller_Flag = false;

        if(btAdapter.isDiscovering())
        {
            btAdapter.cancelDiscovery();

            btAdapter.startDiscovery();
            IntentFilter actionFoundIntent = new IntentFilter(BluetoothDevice.ACTION_FOUND);
            registerReceiver(btReceiver, actionFoundIntent);
            receiverFlag = true;
        }
        if(!btAdapter.isDiscovering())
        {
            //checkBTPermissions();

            btAdapter.startDiscovery();
            IntentFilter discoverDeviceIntent = new IntentFilter(BluetoothDevice.ACTION_FOUND);
            registerReceiver(btReceiver, discoverDeviceIntent);
            receiverFlag = true;
            //Toast.makeText(getApplicationContext(), "Some Text", Toast.LENGTH_SHORT).show();
        }
    }

    public void searchBtn(View view)
    {
        Log.d(TAG, "Request Control Button Pressed");
        mHandler.obtainMessage(SEARCH_COMMAND).sendToTarget();
    }

    public void remoteCntrlBtn(View view)
    {
        Log.d(TAG, "Request Control Button Pressed");
        mHandler.obtainMessage(REQUEST_CONTROL_COMMAND).sendToTarget();
    }

    public void A1Btn(View view)
    {
        Log.d(TAG, "A1 Button Pressed");
        mHandler.obtainMessage(A1_COMMAND).sendToTarget();
    }

    public void A2Btn(View view)
    {
        Log.d(TAG, "A2 Button Pressed");
        mHandler.obtainMessage(A2_COMMAND).sendToTarget();
    }

    public void A3Btn(View view)
    {
        Log.d(TAG, "A3 Button Pressed");
        mHandler.obtainMessage(A3_COMMAND).sendToTarget();
    }

    public void B1Btn(View view)
    {
        Log.d(TAG, "B1 Button Pressed");
        mHandler.obtainMessage(B1_COMMAND).sendToTarget();
    }

    public void B2Btn(View view)
    {
        Log.d(TAG, "B2 Button Pressed");
        mHandler.obtainMessage(B2_COMMAND).sendToTarget();
    }

    public void B3Btn(View view)
    {
        Log.d(TAG, "B3 Button Pressed");
        mHandler.obtainMessage(B3_COMMAND).sendToTarget();
    }

    public void C1Btn(View view)
    {
        Log.d(TAG, "C1 Button Pressed");
        mHandler.obtainMessage(C1_COMMAND).sendToTarget();
    }

    public void C2Btn(View view)
    {
        Log.d(TAG, "C2 Button Pressed");
        mHandler.obtainMessage(C2_COMMAND).sendToTarget();
    }

    public void C3Btn(View view)
    {
        Log.d(TAG, "C3 Button Pressed");
        mHandler.obtainMessage(C3_COMMAND).sendToTarget();
    }

    public void frdBtn(View view)
    {
        Log.d(TAG, "Forward Button Pressed");
        //String s = "forward";
        //connectedThread.write(s.getBytes());
        //connectedThread.run();
        mHandler.obtainMessage(FORWARD).sendToTarget();
    }

    public void backBtn(View view)
    {
        Log.d(TAG, "Back Button Pressed");
        //String s = "back";
        //connectedThread.write(s.getBytes());
        //connectedThread.run();
        mHandler.obtainMessage(BACK).sendToTarget();
    }

    public void lftBtn(View view)
    {
        Log.d(TAG, "Left Button Pressed");
        //String s = "left";
        //connectedThread.write(s.getBytes());
        //connectedThread.run();
        mHandler.obtainMessage(LEFT).sendToTarget();
    }

    public void rgtBtn(View view)
    {
        Log.d(TAG, "Right Button Pressed");
        //String s = "right";
        //connectedThread.write(s.getBytes());
        //connectedThread.run();
        mHandler.obtainMessage(RIGHT).sendToTarget();
    }

    public void stpBtn(View view)
    {
        Log.d(TAG, "Stop Button Pressed");
        //String s = "stop";
        //connectedThread.write(s.getBytes());
        mHandler.obtainMessage(STOP_COMMAND).sendToTarget();
    }



    @Override
    public void onItemClick(AdapterView<?> arg0, View arg1, int arg2, long arg3)
    {
        // TODO Auto-generated method stub
        if(btAdapter.isDiscovering())
        {
            btAdapter.cancelDiscovery();
        }
        if(listAdapter.getItem(arg2).contains("Available"))
        {
            Log.d(TAG, "in click listener");
            //Toast.makeText(getApplicationContext(), pairedDevices.get(arg2), Toast.LENGTH_LONG).show();
            for(int x =0; x < availableDevices.size(); x++)
            {
                if (pairedDevices.get(arg2).contains(availableDevices.get(x).getName()))
                {
                    //Toast.makeText(getApplicationContext(), availableDevices.get(x).getName(), Toast.LENGTH_LONG).show();
                    BluetoothDevice selectedDevice = availableDevices.get(x);
                    ConnectThread connect = new ConnectThread(selectedDevice);
                    connect.start();
                    //Toast.makeText(getApplicationContext(), "Entered if in click listener", Toast.LENGTH_LONG).show();
                    Log.d(TAG, "Entered if in click listener");
                }
            }

        }
        else
        {
            Toast.makeText(getApplicationContext(), "device is not available", Toast.LENGTH_LONG).show();
        }
    }

    private class ConnectThread extends Thread
    {

        private final BluetoothSocket mmSocket;
        private final BluetoothDevice mmDevice;

        public ConnectThread(BluetoothDevice device)
        {
            // Use a temporary object that is later assigned to mmSocket,
            // because mmSocket is final
            BluetoothSocket tmp = null;
            mmDevice = device;
            Log.d(TAG, "construct");
            // Get a BluetoothSocket to connect with the given BluetoothDevice
            try {
                // MY_UUID is the app's UUID string, also used by the server code
                //ParcelUuid[] uuid = device.getUuids();  //MY_UUID
                tmp = device.createRfcommSocketToServiceRecord(MY_UUID);
            } catch (IOException e) {
                Log.d(TAG, "get socket failed");

            }
            mmSocket = tmp;
        }

        public void run()
        {
            // Cancel discovery because it will slow down the connection
            btAdapter.cancelDiscovery();
            Log.d(TAG, "connect - run");
            try {
                // Connect the device through the socket. This will block
                // until it succeeds or throws an exception
                mmSocket.connect();
                Log.d(TAG, "connect - succeeded");
            } catch (IOException connectException) {	Log.i(TAG, "connect failed");
                // Unable to connect; close the socket and get out
                try {
                    mmSocket.close();
                } catch (IOException closeException) { }
                return;
            }

            // Do work to manage the connection (in a separate thread)

            mHandler.obtainMessage(SUCCESS_CONNECT, mmSocket).sendToTarget();
        }



        //Will cancel an in-progress connection, and close the socket
        public void cancel()
        {
            try {
                mmSocket.close();
            } catch (IOException e) { }
        }
    }

    private class ConnectedThread extends Thread
    {
        private final BluetoothSocket mmSocket;
        private final InputStream mmInStream;
        private final OutputStream mmOutStream;
        private int bufferLen;
        private byte[] bufferArray = new byte[1024];
        private int noOfBytes;
        private String readString;
        private int bufferArrayIndex = 0;

        public ConnectedThread(BluetoothSocket socket)
        {
            Log.d(TAG, "Entered ConnectedThread");
            mmSocket = socket;
            InputStream tmpIn = null;
            OutputStream tmpOut = null;

            // Get the input and output streams, using temp objects because
            // member streams are final
            try {
                tmpIn = socket.getInputStream();
                tmpOut = socket.getOutputStream();
            } catch (IOException e) { }

            mmInStream = tmpIn;
            mmOutStream = tmpOut;
        }

        public void run()
        {
            Log.d(TAG, "Entered run");
            //byte[] bufferArray;  // buffer store for the stream
            //int noOfBytes; // bytes returned from read()

            try {
                Log.d(TAG, "Trying to receive message");
                //bufferLen = mmInStream.available();
                //Log.d(TAG, "bufferLen = "+ bufferLen);
                // Read from the InputStream
                //bufferArray = new byte[1024];
                do
                {
                    noOfBytes = mmInStream.read(bufferArray, bufferArrayIndex, 15);
                    bufferArrayIndex = noOfBytes + bufferArrayIndex;

                    //int availableBytes = mmInStream.available();
                    Log.d(TAG, "noOfBytes = "+noOfBytes);
                    //Toast.makeText(getApplicationContext(),"noOfBytes = "+noOfBytes,Toast.LENGTH_SHORT).show();


                    readString = new String(bufferArray);
                    //Toast.makeText(getApplicationContext(), readString, Toast.LENGTH_LONG).show();
                }while(!readString.contains("."));

                // Send the obtained bytes to the UI activity
                mHandler.obtainMessage(MESSAGE_READ, noOfBytes, -1, bufferArray).sendToTarget();
                //Toast.makeText(getApplicationContext(),string,Toast.LENGTH_SHORT).show();
                byte[] clearBuffer = new byte[1024];;
                bufferArray = clearBuffer;
                Log.d(TAG, "Message read");
                //break;

            } catch (IOException e) {
                Log.d(TAG, "Exception caught");
                e.printStackTrace();
            }

        }

        //Call this from the main activity to send data to the remote device
        public void write(byte[] bytes)
        {
            try {
                mmOutStream.write(bytes);
            } catch (IOException e) { }
        }

        //Call this from the main activity to shutdown the connection
        public void cancel()
        {
            try {
                mmSocket.close();
            } catch (IOException e) { }
        }
    }

    private void checkBTPermissions() {
        if(Build.VERSION.SDK_INT > Build.VERSION_CODES.LOLLIPOP){
            int permissionCheck = this.checkSelfPermission("Manifest.permission.ACCESS_FINE_LOCATION");
            permissionCheck += this.checkSelfPermission("Manifest.permission.ACCESS_COARSE_LOCATION");
            if (permissionCheck != 0) {

                this.requestPermissions(new String[]{Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.ACCESS_COARSE_LOCATION}, 1001); //Any number
            }
        }else{
            //Toast.makeText(getApplicationContext(), "No need to add permissions", Toast.LENGTH_SHORT).show();
        }
    }
}
