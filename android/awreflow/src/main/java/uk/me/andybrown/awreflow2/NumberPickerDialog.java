// Andy's Workshop Open Source Reflow Controller
// Copyright (c) 2015 Andy Brown. All rights Reserved.
// Please see website (http://www.andybrown.me.uk) for full license details.

package uk.me.andybrown.awreflow2;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.widget.NumberPicker;
import android.widget.RelativeLayout;


/**
 * Dialog to show a number picker
 */

public class NumberPickerDialog {


  /*
   * The dialog class
   */

  private AlertDialog _dialog;
  private NumberPicker _picker;

  /*
   * Constructor
   */

  public NumberPickerDialog(Activity parent,final String valuePrefsId,int titleResource) {

    String title;
    int value;
    final SharedPreferences prefs;
    RelativeLayout layout;
    AlertDialog.Builder builder;
    RelativeLayout.LayoutParams layoutParams;
    RelativeLayout.LayoutParams pickerParams;

    // get the values from the preferences

    prefs=PreferenceManager.getDefaultSharedPreferences(parent.getApplicationContext());
    value=(int)prefs.getFloat(valuePrefsId,1.0f);
    title=parent.getResources().getString(titleResource);

    // create the layout dynamically

    layout=new RelativeLayout(parent);
    _picker=new NumberPicker(parent);
    _picker.setMaxValue(999);
    _picker.setMinValue(0);
    _picker.setValue(value);
    _picker.setWrapSelectorWheel(false);
    _picker.setClickable(false);
    _picker.setEnabled(true);

    layoutParams=new RelativeLayout.LayoutParams(50,50);
    pickerParams=new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.WRAP_CONTENT,RelativeLayout.LayoutParams.WRAP_CONTENT);

    pickerParams.addRule(RelativeLayout.CENTER_HORIZONTAL);

    layout.setLayoutParams(layoutParams);
    layout.addView(_picker,pickerParams);
    layout.isClickable();

    // create the dialog with a builder

    builder=new AlertDialog.Builder(parent);
    builder.setTitle(title);
    builder.setView(layout);
    builder.setMessage(R.string.change_value).setCancelable(false);
    builder.setNegativeButton(android.R.string.cancel,new DialogInterface.OnClickListener() {
      @Override
      public void onClick(DialogInterface dialog,int which) {

        // clear dialog

        dialog.dismiss();

        // notify closed

        onClose(false);
      }
    });
    builder.setPositiveButton(android.R.string.ok,new DialogInterface.OnClickListener() {
      @Override
      public void onClick(DialogInterface dialog,int which) {

        int newValue;
        SharedPreferences.Editor editor;

        // save the new value

        _picker.clearFocus();
        newValue=_picker.getValue();
        editor=prefs.edit();
        editor.putFloat(valuePrefsId,(float)newValue);
        editor.commit();

        dialog.dismiss();

        // notify that OK has been processed

        onClose(true);
      }
    });

    // create the dialog ready to show

    _dialog=builder.create();
  }


  /*
   * For overriding
   */

  protected void onClose(boolean ok) {
  }

  /*
   * Show the dialog
   */

  public void show() {
    _dialog.show();
  }
}
