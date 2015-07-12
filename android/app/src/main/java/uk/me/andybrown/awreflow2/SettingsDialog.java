package uk.me.andybrown.awreflow2;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.content.Intent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
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

    final SeekBar brightness,contrast,zero;
    Button transmit;

    // set the initial values

    brightness=(SeekBar)dlg.findViewById(R.id.backlight_slider);
    contrast=(SeekBar)dlg.findViewById(R.id.contrast_slider);
    zero=(SeekBar)dlg.findViewById(R.id.oven_zero_slider);

    brightness.setProgress(_mainActivity.getLcdBacklight());
    contrast.setProgress(_mainActivity.getLcdContrast());
    zero.setProgress(_mainActivity.getOvenZero());

    ((TextView)dlg.findViewById(R.id.backlight_value)).setText(Integer.toString(_mainActivity.getLcdBacklight()));
    ((TextView)dlg.findViewById(R.id.contrast_value)).setText(Integer.toString(_mainActivity.getLcdContrast()));
    ((TextView)dlg.findViewById(R.id.oven_zero_value)).setText(Integer.toString(_mainActivity.getOvenZero()));

    linkSlider(dlg,brightness,R.id.backlight_value);
    linkSlider(dlg,contrast,R.id.contrast_value);
    linkSlider(dlg,zero,R.id.oven_zero_value);

    transmit=(Button)dlg.findViewById(R.id.transmit_button);
    transmit.setOnClickListener(new View.OnClickListener() {

      @Override
      public void onClick(View v) {

        int b,c,z;
        byte[] data;
        Intent intent;

        // get the values

        b=brightness.getProgress();
        c=contrast.getProgress();
        z=zero.getProgress();

        // serialize the payload

        data=new byte[3];

        data[0]=(byte)z;      // all values are 7-bit
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

  protected void linkSlider(Dialog dlg,SeekBar slider,int numberId) {

    final TextView tv;

    tv=(TextView)dlg.findViewById(numberId);

    slider.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

      @Override
      public void onProgressChanged(SeekBar seekBar,int progress,boolean fromUser) {
        tv.setText(Integer.toString(progress));
      }

      @Override public void onStartTrackingTouch(SeekBar seekBar) {}
      @Override public void onStopTrackingTouch(SeekBar seekBar) {}
    });
  }
}
