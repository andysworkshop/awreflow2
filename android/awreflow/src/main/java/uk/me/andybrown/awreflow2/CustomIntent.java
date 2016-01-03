// Andy's Workshop Open Source Reflow Controller
// Copyright (c) 2015 Andy Brown. All rights Reserved.
// Please see website (http://www.andybrown.me.uk) for full license details.

package uk.me.andybrown.awreflow2;

/*
 * Custom intents used by our application
 */

public class CustomIntent {

  /*
   * Intent strings
   */

  public static final String REFLOW_STARTED = "uk.me.andybrown.awreflow2.REFLOW_STARTED";
  public static final String REFLOW_STOPPED = "uk.me.andybrown.awreflow2.REFLOW_STOPPED";
  public static final String LINK_STATUS_CHECK = "uk.me.andybrown.awreflow2.LINK_STATUS_CHECK";
  public static final String TEMPERATURE_RECEIVED = "uk.me.andybrown.awreflow2.TEMPERATURE_RECEIVED";
  public static final String TEMPERATURE_FAILED = "uk.me.andybrown.awreflow2.TEMPERATURE_FAILED";
  public static final String SET_DUTY_CYCLE = "uk.me.andybrown.awreflow2.SET_DUTY_CYCLE";
  public static final String SET_DUTY_CYCLE_FAILED = "uk.me.andybrown.awreflow2.SET_DUTY_CYCLE_FAILED";
  public static final String REFLOW_PROGRESS = "uk.me.andybrown.awreflow2.REFLOW_PROGRESS";
  public static final String GET_CONTROLLER_SETTINGS= "uk.me.andybrown.awreflow2.GET_CONTROLLER_SETTINGS";
  public static final String SET_CONTROLLER_SETTINGS = "uk.me.andybrown.awreflow2.SET_CONTROLLER_SETTINGS";
  public static final String SET_SETTINGS_FAILED = "uk.me.andybrown.awreflow2.SET_SETTINGS_FAILED";
  public static final String SET_SETTINGS_OK = "uk.me.andybrown.awreflow2.SET_SETTINGS_OK";


  /*
   * Extra intent data names
   */

  public static final String LINK_STATUS_EXTRA = "link_status";
  public static final String TEMPERATURE_RECEIVED_EXTRA = "temperature";
  public static final String TEMPERATURE_FAILED_EXTRA = "message";
  public static final String DUTY_CYCLE_EXTRA = "duty_cycle";
  public static final String DUTY_CYCLE_FAILED_EXTRA = "message";
  public static final String REFLOW_PROGRESS_EXTRA = "progress";
  public static final String REFLOW_STOPPED_EXTRA = "reason";
  public static final String CONTROLLER_SETTINGS_EXTRA = "settings";
  public static final String SET_SETTINGS_FAILED_EXTRA = "message";
  public static final String SET_SETTINGS_OK_EXTRA = "settings";
}
