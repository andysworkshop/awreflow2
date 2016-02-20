// Andy's Workshop Open Source Reflow Controller
// Copyright (c) 2015 Andy Brown. All rights Reserved.
// Please see website (http://www.andybrown.me.uk) for full license details.

package uk.me.andybrown.awreflow2;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;


/*
 * Timed job to handle a reflow
 */

public class ReflowJob {

  /*
   * Possible states
   */

  public enum State {
    STARTED,
    STOPPED
  }


  /*
   * Reasons for stopping
   */

  public enum StopReason {
    COMPLETED(R.string.completed),
    USER_CANCEL(R.string.user_cancel),
    LINK_FAILED(R.string.link_failed);

    int _id;

    StopReason(int id) {
      _id=id;
    }

    public int getStringId() {
      return _id;
    }
  }


  protected State _state;
  protected int _lastTemperature;

  double[] _points;

  protected long _startTime;
  protected boolean _paused;
  protected Pid _pid;
  protected BroadcastReceiver _broadcastReceiver;
  protected ReflowApplication _app;


  /*
   * Constructor
   */

  public ReflowJob(ReflowApplication app) {

    // set variables

    _app=app;
    setState(State.STOPPED);

    // create the broadcast receiver

    createBroadcastReceiver();
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
            onLinkStatusCheck(
                    LinkStatus.values()[intent.getIntExtra(CustomIntent.LINK_STATUS_EXTRA,LinkStatus.INTERNAL_ERROR.ordinal())]
            );
            break;

          case CustomIntent.TEMPERATURE_RECEIVED:
            onTemperatureReceived(intent.getByteArrayExtra(CustomIntent.TEMPERATURE_RECEIVED_EXTRA));
            break;
        }
      }
    };
  }


  /*
   * Start a new reflow job. This must be paired with a stop() call.
   */

  public void start() {

    ReflowProfile profile;
    SharedPreferences prefs;
    boolean leaded;
    TrackingType trackingType;
    float p,i,d;

    // cannot start while running

    if(_state==State.STARTED)
      return;

    // get the profile and tracking type

    prefs=PreferenceManager.getDefaultSharedPreferences(_app);
    leaded=prefs.getBoolean(PreferenceStrings.PREFS_LEADED,Boolean.TRUE);

    profile=leaded ? new LeadReflowProfile() : new LeadFreeReflowProfile();
    trackingType=TrackingType.fromValue(prefs.getInt(PreferenceStrings.PREFS_TRACKING,TrackingType.SPLINE_CURVE.getValue()));

    // get points from profile and tracking type

    _points=profile.getPoints(trackingType);

    // get P/I/D constants

    p=prefs.getFloat(PreferenceStrings.PREFS_PROPORTIONAL,1.0f);
    i=prefs.getFloat(PreferenceStrings.PREFS_INTEGRAL,1.0f);
    d=prefs.getFloat(PreferenceStrings.PREFS_DERIVATIVE,1.0f);

    // initialise PID algorithm

    _pid=new Pid(p,i,d);

    // reset last temperature variable

    _lastTemperature=-1;

    // create filter for interesting values

    IntentFilter ifilter;

    ifilter=new IntentFilter();
    ifilter.addAction(CustomIntent.TEMPERATURE_RECEIVED);
    ifilter.addAction(CustomIntent.TEMPERATURE_FAILED);
    ifilter.addAction(CustomIntent.SET_DUTY_CYCLE_FAILED);
    ifilter.addAction(CustomIntent.LINK_STATUS_CHECK);

    _app.registerReceiver(_broadcastReceiver,ifilter);

    // initially paused

    _paused=true;
    setState(State.STARTED);

    // notify that we have started

    notifyStarted();
  }


  /*
   * Stop the timer immediately
   */

  public void stop(StopReason reason) {

    // must be running

    if(_state!=State.STARTED)
      return;

    // unsubscribe from events

    _app.unregisterReceiver(_broadcastReceiver);

    // set the stopped state to prevent the timer task from running again

    setState(State.STOPPED);

    // stop the oven

    setDutyCycle(0);

    // notify that we stopped

    notifyStopped(reason);
  }


  /*
   * Notify that reflow has stopped
   */

  protected void notifyStopped(StopReason reason) {

    Intent intent;

    intent=new Intent(CustomIntent.REFLOW_STOPPED);
    intent.putExtra(CustomIntent.REFLOW_STOPPED_EXTRA,reason.ordinal());

    _app.sendBroadcast(intent);
  }


  /*
   * Notify that reflow has started
   */

  protected void notifyStarted() {

    Intent intent;

    intent=new Intent(CustomIntent.REFLOW_STARTED);
    _app.sendBroadcast(intent);
  }


  /*
   * Timer ticked callback
   */

  public void onTick() {

    int seconds;
    double desiredTemperature;
    byte percent;

    synchronized(this) {

      // check the state

      if(_state!=State.STARTED)
        return;

      // are we paused at T-0 ?

      if(_paused) {

        // if no temperature received yet then keep waiting

        if(_lastTemperature==-1)
          return;

        // we can unpause if the temperature at T-0 is at least 25

        if(_lastTemperature>=25) {
          _startTime=System.currentTimeMillis();
          _paused=false;
        } else
          setDutyCycle(30);       // set the oven to 30% to slowly raise to 25C
      } else {

        // get the number of whole seconds since we started

        seconds=(int)(System.currentTimeMillis()-_startTime)/1000;

        // have we finished ?

        if(seconds>=_points.length)
          stop(StopReason.COMPLETED);
        else {

          // get the desired temperature

          desiredTemperature=_points[seconds];

          // update the algorithm and get the oven duty cycle percent

          percent=_pid.update(desiredTemperature,_lastTemperature);

          // double check the state and set the duty cycle if not stopped

          if(_state==State.STARTED)
            setDutyCycle(percent);

          // notify the progress

          notifyProgress(seconds);
        }
      }
    }
  }


  /*
   * Notify the current progress
   */

  protected void notifyProgress(int seconds) {

    Intent intent;

    intent=new Intent(CustomIntent.REFLOW_PROGRESS);
    intent.putExtra(CustomIntent.REFLOW_PROGRESS_EXTRA,new double [] { seconds, _lastTemperature });

    _app.sendBroadcast(intent);
  }


  /*
   * Set a new percentage duty cycle
   */

  protected void setDutyCycle(int percent) {

    Intent intent;

    intent=new Intent(CustomIntent.SET_DUTY_CYCLE);
    intent.putExtra(CustomIntent.DUTY_CYCLE_EXTRA,percent);

    _app.sendBroadcast(intent);
  }


  /*
   * New link status received
   */

  protected void onLinkStatusCheck(LinkStatus linkStatus) {

    // if the link fails then we must stop

    if(linkStatus!=LinkStatus.LINKED)
      stop(StopReason.LINK_FAILED);
  }


  /*
   * New temperature response received
   */

  protected void onTemperatureReceived(byte[] response) {

    Intent intent;
    String reason;

    if(response[2]==1)
      _lastTemperature=response[0] & 0xff | ((response[1] << 8) & 0xff00);
    else {

      // create a message containing the fault string

      intent=new Intent(CustomIntent.TEMPERATURE_FAILED);

      switch(response[2]) {

        case 2:
          reason=_app.getResources().getString(R.string.vcc_short_fault);
          break;

        case 3:
          reason=_app.getResources().getString(R.string.gnd_short_fault);
          break;

        case 4:
          reason=_app.getResources().getString(R.string.oc_fault);
          break;

        default:
          reason=_app.getResources().getString(R.string.unknown_fault);
          break;
      }

      // send the message

      intent.putExtra(CustomIntent.TEMPERATURE_FAILED_EXTRA,reason);
      _app.sendBroadcast(intent);
    }
  }


  /*
   * Get the current state
   */

  public State getState() {
    return _state;
  }


  /*
   * Set a new state
   */

  public void setState(State newState) {
    synchronized(this) {
      _state=newState;
    }
  }
}
