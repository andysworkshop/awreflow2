// Andy's Workshop Open Source Reflow Controller
// Copyright (c) 2015 Andy Brown. All rights Reserved.
// Please see website (http://www.andybrown.me.uk) for full license details.

package uk.me.andybrown.awreflow2;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.view.View;
import android.view.Window;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.Timer;
import java.util.TimerTask;


/**
 * Main user interface
 */

public class MainActivity extends BluetoothActivity {

  /*
   * Values used for the parameters editor
   */

  protected int _animationDuration;
  protected View _parametersSummaryView;
  protected View _parametersEditorView;
  protected Timer _parametersTimer;
  protected BroadcastReceiver _broadcastReceiver;

  /*
   * settings values read when the link is established
   */

  volatile protected int _sensorOffset;
  volatile protected int _lcdBacklight;
  volatile protected int _lcdContrast;


  /*
   * OnCreate
   */

  @Override
  protected void onCreate(Bundle savedInstanceState) {

    super.onCreate(savedInstanceState);

    requestWindowFeature(Window.FEATURE_CUSTOM_TITLE);
    setContentView(R.layout.activity_main);
    getWindow().setFeatureInt(Window.FEATURE_CUSTOM_TITLE,R.layout.window_title);

    // cache the values required for animation

    _animationDuration=getResources().getInteger(android.R.integer.config_shortAnimTime);
    _parametersSummaryView=findViewById(R.id.parameters_summary);
    _parametersEditorView=findViewById(R.id.parameters_editor);

    _parametersEditorView.setVisibility(View.GONE);

    // set the profile check marks

    setProfileCheck();

    // set the PID numbers

    setPidNumbers();

    // set the tracking value and settings

    _sensorOffset=-999;
    _lcdBacklight=-999;
    _lcdContrast=-999;

    setTrackingValue();
    setSettingsValue();

    // create the broadcast receiver

    createBroadcastReceiver();

    // if available but not enabled, request that it is enabled

    enableBluetooth();
  }


  /*
   * If bluetooth is switched off then enable it
   */

  protected void enableBluetooth() {

    Intent intent;
    BluetoothAdapter bluetoothAdapter;

    bluetoothAdapter=BluetoothAdapter.getDefaultAdapter();

    if(bluetoothAdapter!=null && !bluetoothAdapter.isEnabled()) {
      intent=new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
      startActivityForResult(intent,ActivityCodes.REQUEST_ENABLE_BT.getValue());
    }
  }


  /*
   * Pause lifecycle event
   */

  @Override
  protected void onPause() {

    super.onPause();
    unregisterReceiver(_broadcastReceiver);
  }


  /*
   * onResume lifecycle event
   */

  @Override
  protected void onResume() {

    super.onResume();

    IntentFilter ifilter;

    // create filter for interesting values

    ifilter=new IntentFilter();
    ifilter.addAction(CustomIntent.LINK_STATUS_CHECK);
    ifilter.addAction(CustomIntent.TEMPERATURE_RECEIVED);
    ifilter.addAction(CustomIntent.TEMPERATURE_FAILED);
    ifilter.addAction(CustomIntent.GET_CONTROLLER_SETTINGS);
    ifilter.addAction(CustomIntent.SET_SETTINGS_OK);

    registerReceiver(_broadcastReceiver, ifilter);
  }


  /*
   * Create the broadcast receiver
   */

  protected void createBroadcastReceiver() {

    _broadcastReceiver=new BroadcastReceiver() {

      /*
       * Process a broadcast intent
       */

      @Override
      public void onReceive(Context context,Intent intent) {

        switch(intent.getAction()) {

          case CustomIntent.LINK_STATUS_CHECK:
            onBluetoothLinkStatusCheck(
              LinkStatus.values()[intent.getIntExtra(CustomIntent.LINK_STATUS_EXTRA,LinkStatus.INTERNAL_ERROR.ordinal())]
            );
            break;

          case CustomIntent.TEMPERATURE_RECEIVED:
            onTemperatureReceived(intent.getByteArrayExtra(CustomIntent.TEMPERATURE_RECEIVED_EXTRA));
            break;

          case CustomIntent.TEMPERATURE_FAILED:
            onTemperatureFailed(intent.getStringExtra(CustomIntent.TEMPERATURE_FAILED_EXTRA));
            break;

          case CustomIntent.GET_CONTROLLER_SETTINGS:
            onGetControllerSettings(intent.getByteArrayExtra(CustomIntent.CONTROLLER_SETTINGS_EXTRA));
            break;

          case CustomIntent.SET_SETTINGS_OK:
            onSetControllerSettings(intent.getByteArrayExtra(CustomIntent.SET_SETTINGS_OK_EXTRA));
            break;
        }
      }
    };
  }


  /*
   * Set the PID numbers in the buttons
   */

  protected void setPidNumbers() {

    SharedPreferences prefs;
    Resources res;
    String str;
    int p,i,d;

    prefs=PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
    res=getResources();

    // the PID values are floats with no decimal places on show

    p=(int)prefs.getFloat(PreferenceStrings.PREFS_PROPORTIONAL,1.0f);
    i=(int)prefs.getFloat(PreferenceStrings.PREFS_INTEGRAL,1.0f);
    d=(int)prefs.getFloat(PreferenceStrings.PREFS_DERIVATIVE,1.0f);

    str=String.format(res.getString(R.string.parameters_text),p,i,d);

    ((TextView)findViewById(R.id.parameters_text)).setText(str);

    // and on the editor...

    ((TextView)findViewById(R.id.proportional_value)).setText(Integer.toString(p));
    ((TextView)findViewById(R.id.integral_value)).setText(Integer.toString(i));
    ((TextView)findViewById(R.id.derivative_value)).setText(Integer.toString(d));
  }


  /*
   * Set the tracking text value
   */

  protected void setTrackingValue() {

    SharedPreferences prefs;
    int id;

    prefs=PreferenceManager.getDefaultSharedPreferences(getApplicationContext());

    if(prefs.getInt(PreferenceStrings.PREFS_TRACKING,TrackingType.SPLINE_CURVE.getValue())==TrackingType.SPLINE_CURVE.getValue())
      id=R.string.spline_curve;
    else
      id=R.string.linear;

    ((TextView)findViewById(R.id.tracking_value)).setText(id);
  }


  /*
   * Set the settings value
   */

  protected void setSettingsValue() {

    String str;
    Resources res;

    res=getResources();

    if(hasSettings())
      str=String.format(res.getString(R.string.settings_text),_sensorOffset,_lcdBacklight,_lcdContrast);
    else
      str=res.getString(R.string.settings_unavailable);

    // set the new value to the view text

    ((TextView)findViewById(R.id.settings_text)).setText(str);
  }


  /*
   * Check if settings have been read from the controller
   */

  protected boolean hasSettings() {
    return _sensorOffset!=-999;
  }


  /*
   * Set/clear the checkbox on the profile buttons
   */

  protected void setProfileCheck() {

    SharedPreferences prefs;
    boolean leaded;
    Resources res;
    String str;

    prefs=PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
    leaded=prefs.getBoolean(PreferenceStrings.PREFS_LEADED,Boolean.TRUE);

    // check marks

    findViewById(R.id.leaded_checked).setVisibility(leaded ? View.VISIBLE : View.INVISIBLE);
    findViewById(R.id.unleaded_checked).setVisibility(leaded ? View.INVISIBLE : View.VISIBLE);

    // 'leaded' 'lead free' in the reflow description

    res=getResources();
    str=String.format(res.getString(R.string.reflow_text),leaded ? "leaded" : "lead free");

    ((TextView)findViewById(R.id.reflow_text)).setText(str);
  }


  /*
   * Lead button clicked
   */

  public void onClickLead(View v) {

    SharedPreferences.Editor editor=PreferenceManager.getDefaultSharedPreferences(getApplicationContext()).edit();

    editor.putBoolean(PreferenceStrings.PREFS_LEADED, true);
    editor.commit();

    setProfileCheck();
  }

  /*
   * Unleaded clicked
   */

  public void onClickUnleaded(View v) {

    SharedPreferences.Editor editor=PreferenceManager.getDefaultSharedPreferences(getApplicationContext()).edit();

    editor.putBoolean(PreferenceStrings.PREFS_LEADED, false);
    editor.commit();

    setProfileCheck();
  }


  /*
   * Parameters panel clicked
   */

  public void onClickParametersPanel(View v) {

    // hide the summary

    _parametersSummaryView.animate()
            .alpha(0f)
            .setDuration(_animationDuration)
            .setListener(new AnimatorListenerAdapter() {
              @Override
              public void onAnimationEnd(Animator animation) {
                _parametersSummaryView.setVisibility(View.GONE);
              }
            });

    // show the editor but invisible

    _parametersEditorView.setAlpha(0);
    _parametersEditorView.setVisibility(View.VISIBLE);

    // animate up the editor and start a revert timer

    _parametersEditorView.animate()
            .alpha(1f)
            .setDuration(_animationDuration)
            .setListener(new AnimatorListenerAdapter() {
              @Override
              public void onAnimationEnd(Animator animation) {
                startParametersTimer();
              }
            });
  }


  /*
   * Proportional clicked
   */

  public void onClickProportional(View v) {

    NumberPickerDialog npd;

    _parametersTimer.cancel();

    npd=new NumberPickerDialog(this,PreferenceStrings.PREFS_PROPORTIONAL,R.string.proportional) {
      @Override
      protected void onClose(boolean ok) {

        if(ok)
          setPidNumbers();

        startParametersTimer();
      }
    };

    npd.show();
  }


  /*
   * Integral clicked
   */

  public void onClickIntegral(View v) {

    NumberPickerDialog npd;

    _parametersTimer.cancel();

    npd=new NumberPickerDialog(this,PreferenceStrings.PREFS_INTEGRAL,R.string.integral) {
      @Override
      protected void onClose(boolean ok) {

        if(ok)
          setPidNumbers();

        startParametersTimer();
      }
    };

    npd.show();
  }


  /*
   * Derivative clicked
   */

  public void onClickDerivative(View v) {

    NumberPickerDialog npd;

    _parametersTimer.cancel();

    npd=new NumberPickerDialog(this,PreferenceStrings.PREFS_DERIVATIVE,R.string.derivative) {
      @Override
      protected void onClose(boolean ok) {

        if(ok)
          setPidNumbers();

        startParametersTimer();
      }
    };

    npd.show();
  }


  /*
   * Tracking button clicked
   */

  public void onClickTracking(View v) {

    AlertDialog dialog;
    AlertDialog.Builder builder;
    final SharedPreferences prefs;
    final TrackingType[] trackingType=new TrackingType[1];

    // keep the parameters on show

    _parametersTimer.cancel();

    // get the preference

    prefs=PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
    trackingType[0]=TrackingType.fromValue(
      prefs.getInt(PreferenceStrings.PREFS_TRACKING,TrackingType.SPLINE_CURVE.getValue())
    );

    // build the dialog

    builder=new AlertDialog.Builder(this);
    builder.setTitle("Select Tracking Mode");

    // ok button

    builder.setPositiveButton(android.R.string.ok,new DialogInterface.OnClickListener() {
      public void onClick(DialogInterface dialog, int item) {

        SharedPreferences.Editor editor;

        // write back the edited value

        editor=prefs.edit();
        editor.putInt(PreferenceStrings.PREFS_TRACKING,trackingType[0].getValue());
        editor.commit();

        setTrackingValue();
        startParametersTimer();
        dialog.dismiss();
      }});

      // cancel button

      builder.setNegativeButton(android.R.string.cancel,new DialogInterface.OnClickListener() {
        public void onClick(DialogInterface dialog, int item) {
          startParametersTimer();
          dialog.dismiss();
      }});

    // radio buttons

    builder.setSingleChoiceItems(
      R.array.tracking_array,
      trackingType[0]==TrackingType.LINEAR ? 0 : 1,
      new DialogInterface.OnClickListener() {

        public void onClick(DialogInterface dialog, int item) {
          switch(item) {

            case 0:
              trackingType[0]=TrackingType.LINEAR;
              break;

            case 1:
              trackingType[0]=TrackingType.SPLINE_CURVE;
              break;
          }
        }
      });

    // create and show the dialog

    dialog=builder.create();
    dialog.show();
  }


  /*
   * The settings button was clicked
   */

  public void onClickSettings(View v) {

    SettingsDialog dlg;

    // can't do this if settings have not been read from the controller

    if(!hasSettings()) {
      Toast.makeText(getApplicationContext(),getResources().getString(R.string.settings_unread_text),Toast.LENGTH_LONG).show();
      return;
    }

    dlg=new SettingsDialog(this);
    dlg.show();
  }


  /*
   * Start an idle timer for the parameters editor
   */

  private void startParametersTimer() {

    _parametersTimer=new Timer();
    _parametersTimer.schedule(new TimerTask() {
      @Override
      public void run() {

        // ensure not again

        _parametersTimer.cancel();

        runOnUiThread(new Runnable() {
          @Override
          public void run() {

            // revert to the parameters summary

            _parametersEditorView.animate()
              .alpha(0f)
              .setDuration(_animationDuration)
              .setListener(new AnimatorListenerAdapter() {
                @Override
                public void onAnimationEnd(Animator animation) {
                  _parametersEditorView.setVisibility(View.GONE);
                }
              });

            _parametersSummaryView.setAlpha(0);
            _parametersSummaryView.setVisibility(View.VISIBLE);

            _parametersSummaryView.animate()
              .alpha(1f)
              .setDuration(_animationDuration)
              .setListener(null);

          }
        });
      }
    }, 5000);
  }


  /*
   * Reflow button clicked
   */

  public void onClickReflow(final View v) {

    if(!verifyLinked())
      return;

    startReflow(v);
  }


  /*
   * Start the reflow process
   */

  protected void startReflow(View v) {

    Intent intent;

    intent=new Intent(this,ReflowActivity.class);
    startActivity(intent);
  }


  /*
   * Update title with current link status
   */

  public void onBluetoothLinkStatusCheck(final LinkStatus linkStatus) {

    runOnUiThread(new Runnable() {
      @Override
      public void run() {

        TextView text;
        ImageView icon;

        text=(TextView)findViewById(R.id.bluetoothText);
        icon=(ImageView)findViewById(R.id.bluetoothIcon);

        switch(linkStatus) {

          case NOT_SUPPORTED:
            icon.setImageResource(R.drawable.xbluetooth);
            text.setText("Not supported");
            break;

          case DISABLED:
            icon.setImageResource(R.drawable.xbluetooth);
            text.setText("Switched off");
            break;

          case ENABLED:
            icon.setImageResource(R.drawable.xbluetooth);
            text.setText("Not paired");
            break;

          case PAIRED:
            icon.setImageResource(R.drawable.xbluetooth);
            text.setText("No link");
            break;

          case LINKED:
            icon.setImageResource(R.drawable.bluetooth);
            text.setText("Ready");
            break;
        }
      }
    });
  }


  /*
   * temperature was read OK
   */

  public void onTemperatureReceived(byte[] response) {

    String errorText;
    int celsius=0;

    // decode the response

    switch(response[2]) {

      case 0:
        errorText="not ready";
        break;

      case 1:
        celsius=response[0] & 0xff | ((response[1] << 8) & 0xff00);
        errorText=null;
        break;

      case 2:
        errorText="vcc short";
        break;

      case 3:
        errorText="gnd short";
        break;

      case 4:
        errorText="no tcouple";
        break;

      default:
        errorText=String.format("unknown response %1$d",(int)response[2]);
        break;
    }

    // update the status display

    final String s;

    if(errorText==null)
      s=String.format(getResources().getString(R.string.temperature_text),celsius);
    else
      s=String.format(getResources().getString(R.string.temperature_text_error),errorText);

    runOnUiThread(new Runnable() {
      @Override
      public void run() {
        ((TextView)findViewById(R.id.temperature_text)).setText(s);
      }
    });
  }


  /*
   * Received the 3-byte settings from the controller:
   *   [0] = sensor offset
   *   [1] = LCD backlight percentage
   *   [2] = LCD contrast setting (0..127)
   */

  public void onGetControllerSettings(byte[] settings) {

    // these 3 settings are 7-bit values so no special treatment necessary when promoting to int

    _sensorOffset=settings[0];
    _lcdBacklight=settings[1];
    _lcdContrast=settings[2];

    // update the UI display

    runOnUiThread(new Runnable() {
      @Override
      public void run() {
        setSettingsValue();
      }
    });
  }


  /*
   * Settings have changed, the application needs to transmit them
   */

  public void onSetControllerSettings(byte[] settings) {

    // update the UI

    onGetControllerSettings(settings);
  }


  /*
   * Failed to read a temperature
   */

  public void onTemperatureFailed(String message) {

    final String errorText;

    // update the UI

    errorText=String.format("communication failure (%1$s)",message);
    runOnUiThread(new Runnable() {
      @Override
      public void run() {
        ((TextView)findViewById(R.id.temperature_text)).setText(errorText);
      }
    });
  }


  public int getLcdBacklight() { return _lcdBacklight; }
  public int getLcdContrast() { return _lcdContrast; }
  public int getSensorOffset() { return _sensorOffset; }
}
