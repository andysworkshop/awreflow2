// Andy's Workshop Open Source Reflow Controller
// Copyright (c) 2015 Andy Brown. All rights Reserved.
// Please see website (http://www.andybrown.me.uk) for full license details.

package uk.me.andybrown.awreflow2;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.graphics.Point;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;


/*
 * Reflow activity handler
 */

public class ReflowActivity extends BluetoothActivity {

  /*
   * Current state
   */

  protected TrackingType _trackingType;
  protected ReflowProfile _profile;
  protected BroadcastReceiver _broadcastReceiver;


  /*
   * OnCreate
   */

  @Override
  protected void onCreate(Bundle savedInstanceState) {

    String title;
    PackageInfo pInfo;

    // call the base

    super.onCreate(savedInstanceState);

    // set the custom title

    requestWindowFeature(Window.FEATURE_CUSTOM_TITLE);
    setContentView(R.layout.activity_reflow);
    getWindow().setFeatureInt(Window.FEATURE_CUSTOM_TITLE,R.layout.window_title);

    try {
      pInfo=getPackageManager().getPackageInfo(getPackageName(),0);
      title=getResources().getString(R.string.app_name)+" v"+pInfo.versionName;
      ((TextView)findViewById(R.id.activityTitleText)).setText(title);
    }
    catch(Exception ex) {
    }

    // create the broadcast receiver for bluetooth events

    createBroadcastReceiver();

    // set the icons

    setActionIcons();
  }


  /*
   * Receive broadcast intents from the application
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

          case CustomIntent.REFLOW_STARTED:
            setActionIcons();
            break;

          case CustomIntent.REFLOW_STOPPED:
            stop(ReflowJob.StopReason.values()[intent.getIntExtra(CustomIntent.REFLOW_STOPPED_EXTRA,ReflowJob.StopReason.COMPLETED.ordinal())]);
            break;

          case CustomIntent.REFLOW_PROGRESS:
            onProgress(intent.getDoubleArrayExtra(CustomIntent.REFLOW_PROGRESS_EXTRA));
            break;

          case CustomIntent.SET_DUTY_CYCLE:
            onSetDutyCycle(intent.getIntExtra(CustomIntent.DUTY_CYCLE_EXTRA,-1));
            break;

          case CustomIntent.SET_DUTY_CYCLE_FAILED:
            onSetDutyCycleFailed(intent.getStringExtra(CustomIntent.DUTY_CYCLE_FAILED_EXTRA));
            break;
        }
      }
    };
  }


  /*
   * onResume lifecycle event
   */

  @Override
  protected void onResume() {

    SharedPreferences prefs;
    boolean leaded;
    Chart chart;
    IntentFilter ifilter;

    super.onResume();

    // create filter for interesting values

    ifilter=new IntentFilter();
    ifilter.addAction(CustomIntent.LINK_STATUS_CHECK);
    ifilter.addAction(CustomIntent.TEMPERATURE_RECEIVED);
    ifilter.addAction(CustomIntent.TEMPERATURE_FAILED);
    ifilter.addAction(CustomIntent.REFLOW_STARTED);
    ifilter.addAction(CustomIntent.REFLOW_STOPPED);
    ifilter.addAction(CustomIntent.REFLOW_PROGRESS);
    ifilter.addAction(CustomIntent.SET_DUTY_CYCLE);
    ifilter.addAction(CustomIntent.SET_DUTY_CYCLE_FAILED);

    registerReceiver(_broadcastReceiver,ifilter);

    // get profile and tracking type

    prefs=PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
    leaded=prefs.getBoolean(PreferenceStrings.PREFS_LEADED,Boolean.TRUE);

    _profile=leaded ? new LeadReflowProfile() : new LeadFreeReflowProfile();
    _trackingType=TrackingType.fromValue(prefs.getInt(PreferenceStrings.PREFS_TRACKING,TrackingType.SPLINE_CURVE.getValue()));

    // set the correct profile in the chart

    chart=(Chart)findViewById(R.id.chart);
    chart.setReflowProfile(_profile);
    chart.setTrackingType(_trackingType);

    // update the current link status

    updateLinkStatus(((ReflowApplication)getApplication()).getLinkStatus());

    // set the icons based on state

    setActionIcons();
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
   * Set the action icons based on the state
   */

  protected void setActionIcons() {

    ImageView go,stop,exit;

    go=(ImageView)findViewById(R.id.go_icon);
    stop=(ImageView)findViewById(R.id.stop_icon);
    exit=(ImageView)findViewById(R.id.exit_icon);

    switch(getReflowState()) {

      case STOPPED:
        go.setImageResource(R.drawable.go);
        stop.setImageResource(R.drawable.stop_disabled);
        exit.setImageResource(R.drawable.exit);
        break;

      case STARTED:
        go.setImageResource(R.drawable.go_disabled);
        stop.setImageResource(R.drawable.stop);
        exit.setImageResource(R.drawable.exit_disabled);
        break;
    }
  }


  /*
   * Start button clicked
   */

  public void onClickStart(View v) {

    // must be linked to the controller

    if(!verifyLinked())
      return;

    // must be stopped

    if(getReflowState()!=ReflowJob.State.STOPPED)
      return;

    // start it

    ((Chart)findViewById(R.id.chart)).clearProgress();
    getReflowJob().start();
  }


  /*
   * Stop button clicked
   */

  public void onClickStop(View v) {
    stop(ReflowJob.StopReason.USER_CANCEL);
  }


  /*
   * Stop with a reason
   */

  protected void stop(ReflowJob.StopReason reason) {

    // stop the process

    getReflowJob().stop(ReflowJob.StopReason.USER_CANCEL);

    // popup the reason

    Toast.makeText(getApplicationContext(),getResources().getString(reason.getStringId()),Toast.LENGTH_LONG).show();

    // hide the progress icons

    setProgressIconVisibility(View.INVISIBLE);

    // set the action icons

    setActionIcons();
  }


  /*
   * Set the visibility of the progress icons
   */

  protected void setProgressIconVisibility(int visibility) {

    int i;
    final int[] ids=new int[]{
            R.id.time_text_label,
            R.id.time_text,
            R.id.adjustment_text_label,
            R.id.adjustment_text,
            R.id.power_text_label,
            R.id.power_text,
            R.id.pulse
    };

    for(i=0;i<ids.length;findViewById(ids[i++]).setVisibility(visibility));
  }


  /*
   * Exit button clicked
   */

  public void onClickExit(View v) {

    Intent intent;

    if(getReflowState()==ReflowJob.State.STARTED)
      return;

    intent=new Intent(this,MainActivity.class);
    startActivity(intent);
  }


  /*
   * Update title with current link status
   */

  public void onBluetoothLinkStatusCheck(LinkStatus linkStatus) {
    updateLinkStatus(linkStatus);
  }

  protected void updateLinkStatus(final LinkStatus linkStatus) {

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
      s=String.format("%1$d\u00b0", celsius);
    else
      s="E";

    runOnUiThread(new Runnable() {
      @Override
      public void run() {
        ((TextView)findViewById(R.id.temperature_text)).setText(s);
      }
    });
  }


  /*
   * Failed to read a temperature
   */

  public void onTemperatureFailed(final String message) {

    // update the UI

    runOnUiThread(new Runnable() {
      @Override
      public void run() {
        ((TextView)findViewById(R.id.temperature_text)).setText("E");
        Toast.makeText(getApplicationContext(),"Failed to read temperature: "+message,Toast.LENGTH_LONG).show();
      }
    });
  }


  /*
   * Failed to set a new duty cycle
   */

  public void onSetDutyCycleFailed(final String message) {

    // update the UI

    runOnUiThread(new Runnable() {
      @Override
      public void run() {
        ((TextView)findViewById(R.id.power_text)).setText("E");
        Toast.makeText(getApplicationContext(),"Failed to set power: "+message,Toast.LENGTH_LONG).show();
      }
    });
  }


  /*
   * Progress update. Array is [seconds, temperature]
   */

  public void onProgress(double[] values) {

    Chart chart;
    String str;
    int seconds,colour,difference;
    TextView view;
    Point p;
    View pulse;
    ViewGroup.MarginLayoutParams mlp;

    // state check

    if(getReflowState()!=ReflowJob.State.STARTED)
      return;

    // update the current time

    findViewById(R.id.time_text_label).setVisibility(View.VISIBLE);

    seconds=(int)values[0];
    str=String.format("%d:%02d",seconds/60,seconds % 60);

    view=(TextView)findViewById(R.id.time_text);
    view.setText(str);
    view.setVisibility(View.VISIBLE);

    // get the difference between the desired temperature and the actual temperature

    difference=(int)(values[1]-_profile.getPoints(_trackingType)[seconds]+0.5);

    view=(TextView)findViewById(R.id.adjustment_text);
    view.setText(Integer.toString(difference));
    view.setVisibility(View.VISIBLE);

    // colour code the difference (red=too hot, blue=too cold, black=spot on)

    if(difference<0)
      colour=0xFF0000FF;
    else if(difference>0)
      colour=0xFFFF0000;
    else
      colour=0xFF000000;

    view.setTextColor(colour);
    findViewById(R.id.adjustment_text_label).setVisibility(View.VISIBLE);

    // update the chart

    chart=(Chart)findViewById(R.id.chart);
    chart.updateProgress(seconds,values[1]);
    chart.invalidate();

    // move the pulse and make it visible

    p=chart.reflowPointToChart(seconds,values[1]);
    pulse=findViewById(R.id.pulse);

    mlp=(ViewGroup.MarginLayoutParams)pulse.getLayoutParams();
    mlp.setMargins(p.x-(pulse.getWidth()/2),p.y-(pulse.getHeight()/2),0,0);
    pulse.requestLayout();

    pulse.setVisibility(View.VISIBLE);
  }


  /*
   * New duty cycle was set
   */

  protected void onSetDutyCycle(final int percent) {

    // check state

    if(percent==-1 || getReflowState()!=ReflowJob.State.STARTED)
      return;

    runOnUiThread(new Runnable() {
      @Override
      public void run() {

        String str;
        TextView view;

        // set the values

        str=String.format("%d%%",percent);
        view=(TextView)findViewById(R.id.power_text);
        view.setText(str);
        view.setVisibility(View.VISIBLE);

        findViewById(R.id.power_text_label).setVisibility(View.VISIBLE);
      }
    });
  }


  /*
   * Get the reflow job
   */

  protected ReflowJob getReflowJob() {
    return ((ReflowApplication)getApplication()).getReflowJob();
  }


  /*
   * Get the reflow job state
   */

  protected ReflowJob.State getReflowState() {
    return getReflowJob().getState();
  }
}
