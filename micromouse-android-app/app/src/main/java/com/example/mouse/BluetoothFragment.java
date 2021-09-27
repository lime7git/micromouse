package com.example.mouse;

import static com.example.mouse.MainActivity.btSocket;
import static com.example.mouse.MainActivity.hc06;
import static com.example.mouse.MainActivity.mUUID;

import android.annotation.SuppressLint;
import android.app.ProgressDialog;
import android.graphics.Color;
import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.ToggleButton;

import com.google.android.material.snackbar.Snackbar;

import java.io.IOException;
import java.util.UUID;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link BluetoothFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class BluetoothFragment extends Fragment {

    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;

    private ProgressBar progressBarConnecting;

    public BluetoothFragment() {
        // Required empty public constructor
    }
    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @param param1 Parameter 1.
     * @param param2 Parameter 2.
     * @return A new instance of fragment BluetoothFragment.
     */
    // TODO: Rename and change types and number of parameters
    public static BluetoothFragment newInstance(String param1, String param2) {
        BluetoothFragment fragment = new BluetoothFragment();
        Bundle args = new Bundle();
        args.putString(ARG_PARAM1, param1);
        args.putString(ARG_PARAM2, param2);
        fragment.setArguments(args);
        return fragment;
    }
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            mParam1 = getArguments().getString(ARG_PARAM1);
            mParam2 = getArguments().getString(ARG_PARAM2);
        }

    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_bluetooth, container, false);

        ToggleButton toggleButtonConnect = (ToggleButton) view.findViewById(R.id.toggleButtonConnect);
        if(MainActivity.getInstance().isBluetoothConnected()) toggleButtonConnect.setChecked(true);
            else toggleButtonConnect.setChecked(false);

        toggleButtonConnect.setOnCheckedChangeListener(this::onCheckedChanged);

        return view;
    }

    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        Snackbar snackbar;

        if (isChecked) {

            MainActivity.getInstance().BluetoothConnect();

            if(!(MainActivity.getInstance().isBluetoothConnected()))
            {
                buttonView.setChecked(false);
                snackbar = Snackbar.make(buttonView, "Unable to connect!", Snackbar.LENGTH_SHORT);
            }
            else snackbar = Snackbar.make(buttonView, "Connected!", Snackbar.LENGTH_SHORT);
        }
        else
        {
            MainActivity.getInstance().BluetoothDisconnect();
            snackbar = Snackbar.make(buttonView, "Disconnected!", Snackbar.LENGTH_SHORT);
        }

        snackbar.show();
    }


}