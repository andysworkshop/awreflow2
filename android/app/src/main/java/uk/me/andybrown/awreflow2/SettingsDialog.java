package uk.me.andybrown.awreflow2;

import android.app.Dialog;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.TextView;


/*
 * Dialog to handle settings
 */

public class SettingsDialog {


  /*
   * Reference back to the main activity
   */

  protected MainActivity _mainActivity;


  /*
   * Constructor
   */

  public SettingsDialog(MainActivity activity) {
    _mainActivity=activity;
  }


  /*
   * Show the dialog
   */

  public void show() {

    Dialog dlg;
    LayoutInflater inflater;
    View layout;

    dlg=new Dialog(_mainActivity,android.R.style.Theme_Holo_Light_Dialog_NoActionBar);
    inflater=(LayoutInflater)_mainActivity.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
    layout=inflater.inflate(R.layout.settings_dialog,(ViewGroup)_mainActivity.findViewById(R.id.settings_dialog));
    dlg.setContentView(layout);

    setListeners(dlg);

    dlg.show();
  }


  /*
   * Set up the change listeners
   */

  protected void setListeners(Dialog dlg) {

    final SeekBar brightness,contrast,offset;
    final EditText deviceNameEditor;
    final SharedPreferences prefs;
    Button transmit;
    String deviceName;

    // set the initial values

    brightness=(SeekBar)dlg.findViewById(R.id.backlight_slider);
    contrast=(SeekBar)dlg.findViewById(R.id.contrast_slider);
    offset=(SeekBar)dlg.findViewById(R.id.sensor_offset_slider);

    offset.setMax(99 + 99);       // range is -99 to +99
    contrast.setMax(100);
    brightness.setMax(100);

    brightness.setProgress(_mainActivity.getLcdBacklight());
    contrast.setProgress(_mainActivity.getLcdContrast());
    offset.setProgress(_mainActivity.getSensorOffset() + 99);   // bump up to positive

    ((TextView)dlg.findViewById(R.id.backlight_value)).setText(Integer.toString(_mainActivity.getLcdBacklight()));
    ((TextView)dlg.findViewById(R.id.contrast_value)).setText(Integer.toString(_mainActivity.getLcdContrast()));
    ((TextView)dlg.findViewById(R.id.sensor_offset_value)).setText(Integer.toString(_mainActivity.getSensorOffset()));

    linkSlider(dlg, brightness, R.id.backlight_value, 0);
    linkSlider(dlg, contrast, R.id.contrast_value, 0);
    linkSlider(dlg,offset,R.id.sensor_offset_value,99);

    // device name setup

    prefs=PreferenceManager.getDefaultSharedPreferences(_mainActivity);
    deviceName=prefs.getString(PreferenceStrings.PREFS_DEVICE_NAME,PreferenceStrings.DEFAULT_DEVICE_NAME);
    deviceNameEditor=((EditText)dlg.findViewById(R.id.device_name_editor));
    deviceNameEditor.setText(deviceName);

    // transmit button setup

    transmit=(Button)dlg.findViewById(R.id.transmit_button);
    transmit.setOnClickListener(new View.OnClickListener() {

      @Override
      public void onClick(View v) {

        int b,c,o;
        byte[] data;
        Intent intent;
        String deviceName;

        // save the device name

        deviceName=deviceNameEditor.getText().toString();
        prefs.edit().putString(PreferenceStrings.PREFS_DEVICE_NAME,deviceName).commit();

        // get the values

        b=brightness.getProgress();
        c=contrast.getProgress();
        o=offset.getProgress()-99;

        // serialize the payload

        data=new byte[3];

        data[0]=(byte)o;      // all values are 7-bit
        data[1]=(byte)b;
        data[2]=(byte)c;

        // send the intent

        intent=new Intent(CustomIntent.SET_CONTROLLER_SETTINGS);
        intent.putExtra(CustomIntent.CONTROLLER_SETTINGS_EXTRA,data);

        _mainActivity.sendBroadcast(intent);
      }
    });
  }


  /*
   * link progress change to number
   */

  protected void linkSlider(Dialog dlg,SeekBar slider,int numberId,final int offset) {

    final TextView tv;

    tv=(TextView)dlg.findViewById(numberId);

    slider.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

      @Override
      public void onProgressChanged(SeekBar seekBar,int progress,boolean fromUser) {
        tv.setText(Integer.toString(progress-offset));
      }

      @Override public void onStartTrackingTouch(SeekBar seekBar) {}
      @Override public void onStopTrackingTouch(SeekBar seekBar) {}
    });
  }
}
