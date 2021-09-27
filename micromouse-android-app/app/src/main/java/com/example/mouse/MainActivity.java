package com.example.mouse;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.os.Bundle;

import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.google.android.material.snackbar.Snackbar;
import com.google.android.material.tabs.TabLayout;

import androidx.viewpager.widget.ViewPager;
import androidx.appcompat.app.AppCompatActivity;

import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ProgressBar;

import com.example.mouse.ui.main.SectionsPagerAdapter;
import com.example.mouse.databinding.ActivityMainBinding;

import java.io.IOException;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {

    private ActivityMainBinding binding;
    private static MainActivity instance;

    public static final UUID mUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    public static BluetoothAdapter btAdapter = BluetoothAdapter.getDefaultAdapter();
    public static BluetoothDevice hc06 = btAdapter.getRemoteDevice("98:D3:31:FC:B1:18");
    public static BluetoothSocket btSocket = null;
    private boolean isBluetoothConnected = false;


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

            } catch (IOException e) {
                e.printStackTrace();
            }

            timeout--;
        } while ((!btSocket.isConnected()) && (timeout == 0));

        if(btSocket.isConnected()) isBluetoothConnected = true;
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
            OutputStream btFrame = btSocket.getOutputStream();
            btFrame.write(message.getBytes(StandardCharsets.UTF_8));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}