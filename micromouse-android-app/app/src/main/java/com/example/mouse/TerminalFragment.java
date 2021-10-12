package com.example.mouse;

import static androidx.core.content.ContextCompat.getSystemService;

import android.app.Activity;
import android.content.ClipData;
import android.content.Context;
import android.media.Image;
import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.text.method.ScrollingMovementMethod;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.Spinner;
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
    Spinner spinnerModes;

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
        terminal.setMovementMethod(new ScrollingMovementMethod());
        terminal_command = (EditText) view.findViewById(R.id.editTextTerminalCommand);
        buttonSend = (ImageButton) view.findViewById(R.id.imageButtonSend);
        buttonClear = (Button) view.findViewById(R.id.buttonClear);



        spinnerModes = (Spinner) view.findViewById(R.id.spinnerModeSelect);
// Create an ArrayAdapter using the string array and a default spinner layout
        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(this.getContext(),
                R.array.Modes, android.R.layout.simple_spinner_item);
// Specify the layout to use when the list of choices appears
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
// Apply the adapter to the spinner
        spinnerModes.setAdapter(adapter);

        spinnerModes.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parentView,
                                       View selectedItemView, int position, long id) {
                try {

                    String select_item = (String) parentView.getItemAtPosition(position);
                    Integer mode = 0;

                    switch (select_item)
                    {
                        case "Init" :
                            mode = 0;
                            break;
                        case "Critical" :
                            mode = 1;
                            break;
                        case "Stop" :
                            mode = 2;
                            break;
                        case "Idle" :
                            mode = 3;
                            break;
                        case "Run" :
                            mode = 4;
                            break;
                        case "Test" :
                            mode = 5;
                            break;
                        case "PID" :
                            mode = 6;
                            break;
                        case "Manual" :
                            mode = 7;
                            break;
                        default:
                            mode = 2;
                            break;
                    }


                    MainActivity.getInstance().BluetoothSend(String.format("$STATE=%d#", mode));
                    MainActivity.getInstance().BluetoothSend("$STATE=?#");
                }
                catch (Exception e) {
                    e.printStackTrace();
                }
            }
            @Override
            public void onNothingSelected(AdapterView<?> parentView) {

            }

        });

        buttonSend.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (MainActivity.getInstance().isBluetoothConnected()) {
                    MainActivity.getInstance().BluetoothSend(terminal_command.getText().toString());
                    MainActivity.getInstance().hideKeyboard();
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