package com.example.mouse;

import android.content.pm.ActivityInfo;
import android.graphics.drawable.GradientDrawable;
import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.ImageButton;
import android.widget.SeekBar;
import android.widget.ToggleButton;

import com.google.android.material.snackbar.Snackbar;

import java.nio.charset.StandardCharsets;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link ControlFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class ControlFragment extends Fragment {

    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;

    Button buttonLed;
    Button buttonStop;
    ImageButton buttonUp;
    ImageButton buttonDown;
    ImageButton buttonLeft;
    ImageButton buttonRight;
    SeekBar seekBarSpeed;
    SeekBar seekBarTurn;
    ImageButton buttonRotateRight;
    ImageButton buttonRotateLeft;

    int speed = 50;
    float turn = 0.5F;

    public ControlFragment() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @param param1 Parameter 1.
     * @param param2 Parameter 2.
     * @return A new instance of fragment ControlFragment.
     */
    // TODO: Rename and change types and number of parameters
    public static ControlFragment newInstance(String param1, String param2) {
        ControlFragment fragment = new ControlFragment();
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
        View view = inflater.inflate(R.layout.fragment_control, container, false);

        buttonStop = (Button) view.findViewById(R.id.buttonStop);
        buttonLed = (Button) view.findViewById(R.id.buttonLed);
        buttonUp = (ImageButton) view.findViewById(R.id.buttonArrowUp);
        buttonDown = (ImageButton) view.findViewById(R.id.buttonArrowDown);
        buttonLeft = (ImageButton) view.findViewById(R.id.buttonArrowLeft);
        buttonRight = (ImageButton) view.findViewById(R.id.buttonArrowRight);
        seekBarSpeed = (SeekBar) view.findViewById(R.id.seekBarSpeed);
        seekBarTurn = (SeekBar) view.findViewById(R.id.seekBarTurn);
        buttonRotateRight = (ImageButton) view.findViewById(R.id.imageButtonRotateRight);
        buttonRotateLeft = (ImageButton) view.findViewById(R.id.imageButtonRotateLeft);

        if(!(MainActivity.getInstance().isBluetoothConnected()))
        {
            buttonLed.setEnabled(false);
            buttonStop.setEnabled(false);
            buttonUp.setEnabled(false);
            buttonDown.setEnabled(false);
            buttonLeft.setEnabled(false);
            buttonRight.setEnabled(false);
            seekBarSpeed.setEnabled(false);
            seekBarTurn.setEnabled(false);
            buttonRotateRight.setEnabled(false);
            buttonRotateLeft.setEnabled(false);
        }

        buttonLed.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                MainActivity.getInstance().BluetoothSend("$LED=4,2#");
            }
        });

        buttonStop.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                MainActivity.getInstance().BluetoothSend("$MOTOR=0,0#");
            }
        });

        buttonUp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                MainActivity.getInstance().BluetoothSend(String.format("$MOTOR=%d,%d#", speed, speed));
            }
        });

        buttonDown.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                MainActivity.getInstance().BluetoothSend(String.format("$MOTOR=%d,%d#", -speed, -speed));
            }
        });

        buttonLeft.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                MainActivity.getInstance().BluetoothSend(String.format("$MOTOR=%d,%d#", (int)(speed * turn), speed));
            }
        });

        buttonRight.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                MainActivity.getInstance().BluetoothSend(String.format("$MOTOR=%d,%d#", speed,(int)(speed * turn)));
            }
        });

        seekBarSpeed.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress,boolean fromUser) {
                speed = progress;
            }

            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        seekBarTurn.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress,boolean fromUser) {
                turn = (float)progress / 100.0F;
            }

            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        buttonRotateLeft.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                MainActivity.getInstance().BluetoothSend(String.format("$MOTOR=%d,%d#", -speed, speed));
            }
        });

        buttonRotateRight.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                MainActivity.getInstance().BluetoothSend(String.format("$MOTOR=%d,%d#", speed, -speed));
            }
        });

        return view;
    }

}