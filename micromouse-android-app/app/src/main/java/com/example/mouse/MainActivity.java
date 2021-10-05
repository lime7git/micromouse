package com.example.mouse;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.os.Bundle;
import android.view.View;
import android.view.inputmethod.InputMethodManager;

import androidx.appcompat.app.AppCompatActivity;
import androidx.viewpager.widget.ViewPager;

import com.example.mouse.databinding.ActivityMainBinding;
import com.example.mouse.ui.main.SectionsPagerAdapter;
import com.google.android.material.tabs.TabLayout;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {

    private ActivityMainBinding binding;
    private static MainActivity instance;

    public static final UUID mUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    public static BluetoothAdapter btAdapter = BluetoothAdapter.getDefaultAdapter();
    public static BluetoothDevice hc06 = btAdapter.getRemoteDevice("98:D3:31:FC:B1:18");
    public static BluetoothSocket btSocket = null;
    private boolean isBluetoothConnected = false;

    public static List<String> recivedData = new ArrayList<String>();

    OutputStream btFrame;
    InputStream InputStream;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        SectionsPagerAdapter sectionsPagerAdapter = new SectionsPagerAdapter(this, getSupportFragmentManager());
        ViewPager viewPager = binding.viewPager;
        viewPager.setAdapter(sectionsPagerAdapter);
        TabLayout tabs = binding.tabs;
        tabs.setupWithViewPager(viewPager);


        instance = this;
    }

    public static MainActivity getInstance() {
        return instance;
    }

    protected void BluetoothConnect() {
        int timeout = 3;

        do{
            try {
                btSocket = hc06.createRfcommSocketToServiceRecord(mUUID);
                btSocket.connect();

                InputStream = btSocket.getInputStream();
                btFrame = btSocket.getOutputStream();

            } catch (IOException e) {
                e.printStackTrace();
            }

            timeout--;
        } while ((!btSocket.isConnected()) && (timeout == 0));

        if(btSocket.isConnected())
        {
            isBluetoothConnected = true;
            new Thread(new Runnable() {
                public void run() {
                    byte[] buffer = new byte[1024];
                    int readBufferPosition = 0;

                    // Keep looping to listen for received messages
                    while (true) {
                        try
                        {
                            int bytesAvailable = InputStream.available();
                            if(bytesAvailable > 0)
                            {
                                byte[] packetBytes = new byte[bytesAvailable];
                                InputStream.read(packetBytes);
                                for(int i=0;i<bytesAvailable;i++)
                                {
                                    byte b = packetBytes[i];
                                    if(b == 10)
                                    {
                                        buffer[readBufferPosition++] = b;
                                        byte[] encodedBytes = new byte[readBufferPosition];
                                        System.arraycopy(buffer, 0, encodedBytes, 0, encodedBytes.length);
                                        final String data = new String(encodedBytes, "US-ASCII");
                                        readBufferPosition = 0;
                                        recivedData.add(data);
                                    }
                                    else
                                    {
                                        buffer[readBufferPosition++] = b;
                                    }
                                }
                            }
                        }
                        catch (IOException ex)
                        {
                            break;
                        }
                    }
                }
            }).start();
        }
            else isBluetoothConnected = false;

    }
    protected void BluetoothDisconnect() {
        try {
            btSocket.close();
            isBluetoothConnected = false;
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public boolean isBluetoothConnected()
    {
        return isBluetoothConnected;
    }


    protected void BluetoothSend(String message)
    {
        try {
            btFrame.write(message.getBytes(StandardCharsets.UTF_8));
        } catch (IOException e) {
            e.printStackTrace();
        }

    }
    protected void hideKeyboard() {
        InputMethodManager imm = (InputMethodManager) instance.getSystemService(Activity.INPUT_METHOD_SERVICE);
        //Find the currently focused view, so we can grab the correct window token from it.
        View view = instance.getCurrentFocus();
        //If no view currently has focus, create a new one, just so we can grab a window token from it
        if (view == null) {
            view = new View(instance);
        }
        imm.hideSoftInputFromWindow(view.getWindowToken(), 0);
    }

}