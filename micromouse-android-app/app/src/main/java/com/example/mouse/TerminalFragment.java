package com.example.mouse;

import android.media.Image;
import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.text.method.ScrollingMovementMethod;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.ToggleButton;

import java.io.IOException;


/**
 * A simple {@link Fragment} subclass.
 * Use the {@link TerminalFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class TerminalFragment extends Fragment {

    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;

    TextView terminal;
    EditText terminal_command;
    ImageButton buttonSend;
    Button buttonClear;

    public TerminalFragment() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @param param1 Parameter 1.
     * @param param2 Parameter 2.
     * @return A new instance of fragment TerminalFragment.
     */
    // TODO: Rename and change types and number of parameters
    public static TerminalFragment newInstance(String param1, String param2) {
        TerminalFragment fragment = new TerminalFragment();
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
        View view = inflater.inflate(R.layout.fragment_terminal, container, false);

        terminal = (TextView) view.findViewById(R.id.textViewTerminal);
        terminal_command = (EditText) view.findViewById(R.id.editTextTerminalCommand);
        buttonSend = (ImageButton) view.findViewById(R.id.imageButtonSend);
        buttonClear = (Button) view.findViewById(R.id.buttonClear);

        terminal.setMovementMethod(new ScrollingMovementMethod());

        buttonSend.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (MainActivity.getInstance().isBluetoothConnected()) {
                    MainActivity.getInstance().BluetoothSend(terminal_command.getText().toString());
                }
            }
        });

        buttonClear.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                terminal.setText("");
            }
        });

        new Thread(new Runnable() {
            public void run() {
                // Keep looping to listen for received messages
                while (true) {

                    if(MainActivity.recivedData.size() != 0)
                    {
                        terminal.append(MainActivity.recivedData.get(0));
                        MainActivity.recivedData.remove(0);
                    }

                }
            }
        }).start();

        return view;
    }

    public void changeText(String mText)
    {
        terminal.setText(mText);
    }
}