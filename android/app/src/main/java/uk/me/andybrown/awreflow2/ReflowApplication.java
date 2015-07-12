// Andy's Workshop Open Source Reflow Controller
// Copyright (c) 2015 Andy Brown. All rights Reserved.
// Please see website (http://www.andybrown.me.uk) for full license details.

package uk.me.andybrown.awreflow2;

import android.app.Application;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Method;


/*
 * Application subclass
 */

public class ReflowApplication extends Application implements Runnable {

  protected static final String OVEN_DEVICE_NAME = "HC-06";

  private static final byte ACK = 0;

  private static final int LINK_CHECK_INTERVAL = 3000;
  private static final int REFLOW_GET_TEMPERATURE_INTERVAL = 1000;
  private static final int NON_REFLOW_GET_TEMPERATURE_INTERVAL = 3000;

  /*
   * Bluetooth adaptor and receiver
   */

  protected BluetoothSocket _socket;
  protected InputStream _socketInput;
  protected OutputStream _socketOutput;
  protected BluetoothAdapter _bluetoothAdapter;

  final protected Thread _thread=new Thread(this);

  protected int _sequenceNumber=0;

  volatile protected LinkStatus _linkStatus;
  volatile protected ReflowJob _reflowJob;
  volatile protected int _setPercent;
  volatile protected byte[] _newSettings;


  /*
   * Constructor
   */

  @Override
  public void onCreate() {

    IntentFilter ifilter;

    // call the superclass

    super.onCreate();

    // set members

    _setPercent=-1;

    ifilter=new IntentFilter();
    ifilter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
    ifilter.addAction(BluetoothDevice.ACTION_ACL_DISCONNECTED);
    ifilter.addAction(CustomIntent.SET_DUTY_CYCLE);
    ifilter.addAction(CustomIntent.SET_CONTROLLER_SETTINGS);

    // create the broadcast receiver for bluetooth events

    registerReceiver(new BroadcastReceiver() {
      @Override
      public void onReceive(Context context,Intent intent) {

        switch(intent.getAction()) {

          case CustomIntent.SET_DUTY_CYCLE:
            onSetDutyCycle(intent.getIntExtra(CustomIntent.DUTY_CYCLE_EXTRA,-1));
            break;

          case CustomIntent.SET_CONTROLLER_SETTINGS:
            onSetControllerSettings(intent.getByteArrayExtra(CustomIntent.CONTROLLER_SETTINGS_EXTRA));
            break;

          case BluetoothAdapter.ACTION_STATE_CHANGED:

            switch(intent.getIntExtra(BluetoothAdapter.EXTRA_STATE,BluetoothAdapter.ERROR)) {

              case BluetoothAdapter.STATE_OFF:
                onDisabled();
                break;

              case BluetoothAdapter.STATE_ON:
                onEnabled();
                break;
            }
            break;

          case BluetoothDevice.ACTION_ACL_DISCONNECTED:
            onDisconnected();
            break;
        }
      }
    },ifilter);

    // create adaptor

    if((_bluetoothAdapter=BluetoothAdapter.getDefaultAdapter())==null)
      setLinkStatus(LinkStatus.NOT_SUPPORTED);
    else if(_bluetoothAdapter.isEnabled())
      setLinkStatus(LinkStatus.ENABLED);
    else
      setLinkStatus(LinkStatus.DISABLED);

    // create the reflow job

    _reflowJob=new ReflowJob(this);

    // start the monitoring thread

    _thread.start();
  }

  
  /*
   * Thread method
   */

  @Override
  public void run() {

    long lastLinkCheckTime,lastTemperatureCheckTime;
    int percent,temperaturePollingInterval;

    lastLinkCheckTime=lastTemperatureCheckTime=0;

    for(;;) {

      try {

        // sleep for a second

        Thread.sleep(1000);

        // pass to the reflow job

        _reflowJob.onTick();

        // don't cache these time calls - it's important that they're fresh

        if(System.currentTimeMillis()-lastLinkCheckTime>LINK_CHECK_INTERVAL) {
          checkLinkStatus();
          lastLinkCheckTime=System.currentTimeMillis();
        }

        // if there's a new temperature to set, do it

        percent=_setPercent;
        if(percent!=-1) {

          setDutyCycleCommand(percent);

          // if the desired temperature has not changed then clear it out
          // to prevent re-sending the same value

          synchronized(this) {
            if(percent==_setPercent)
              _setPercent=-1;
          }

          // the duy cycle response includes the last temperature

          lastTemperatureCheckTime=System.currentTimeMillis();
        }
        else
        {
          temperaturePollingInterval=_reflowJob==null ? REFLOW_GET_TEMPERATURE_INTERVAL : NON_REFLOW_GET_TEMPERATURE_INTERVAL;

          if(System.currentTimeMillis()-lastTemperatureCheckTime>temperaturePollingInterval) {
            readTemperatureCommand();
            lastTemperatureCheckTime=System.currentTimeMillis();
          }
        }

        // if new settings need to be set then do so

        if(_newSettings!=null) {

          try {
            setOvenZeroCommand(_newSettings[0]);
            setBacklightCommand(_newSettings[1]);
            setContrastCommand(_newSettings[2]);

            notifySetSettingsOk(_newSettings);
          }
          catch(Exception ex) {
            notifySetSettingsFailed(ex.getMessage());
          }

          _newSettings=null;
        }
      }
      catch(Exception ex) {
        Log.d(getClass().getName(), ex.toString());
      }
    }
  }


  /*
   * Check the link status
   */

  protected void checkLinkStatus() throws Exception {

    switch(_linkStatus) {

      case ENABLED:
        closeConnection();
        if(getPairedDevice()!=null)
          _linkStatus=LinkStatus.PAIRED;
        break;

      case PAIRED:
        closeConnection();
        openConnection();
        break;
    }

    // notify the subscribers

    notifyLinkStatus();
  }


  /*
   * Close the connection socket
   */

  protected void closeConnection() throws IOException {

    if(_socket!=null) {
      _socket.close();
      _socket=null;
    }
  }


  /*
   * Open a connection to the device
   */

  protected void openConnection() throws Exception {

    BluetoothDevice device;
    Method m;
    byte[] response;

    // get the paired device

    if((device=getPairedDevice())==null) {
      setLinkStatus(LinkStatus.ENABLED);
      return;
    }

    // use reflection to call the connect method that doesn't require android on the other end

    m=device.getClass().getMethod("createRfcommSocket", new Class[]{int.class});
    _socket=(BluetoothSocket)m.invoke(device,1);

    // connect to the device (blocking) throws exception on fail

    _socket.connect();

    _socketInput=_socket.getInputStream();
    _socketOutput=_socket.getOutputStream();

    // send the command that gets the status

    writeCommand(CommandId.READ_SETTINGS, null);

    // read the 3-byte response

    response=new byte[3];
    readBytes(response);

    // worked, update status

    setLinkStatus(LinkStatus.LINKED);

    // notify the settings have been received

    notifySettings(response);
  }


  /*
   * Set a new duty cycle. The response is the current temperature
   */

  protected void setDutyCycleCommand(int percent) throws Exception {

    byte[] response;

    try {

      // validate

      if(_socket==null || _linkStatus!=LinkStatus.LINKED)
        throw new Exception("Not connected to the controller");

      // write the command

      writeCommand(CommandId.SET_DUTY_CYCLE,new byte[] { (byte)percent} );

      // read the 3-byte response

      response=new byte[3];
      readBytes(response);

      // notify the subscribers

      notifyDutyCycle(percent);
      notifyTemperatureReceived(response);
    }
    catch(Exception ex) {
      notifyDutyCycleFailed(ex.getMessage());
    }
  }


  /*
   * Set the oven zero
   */

  protected void setOvenZeroCommand(int zero) throws Exception {
    setSettingsCommand(CommandId.SET_OVEN_ZERO,zero);
  }

  /*
   * Set the LCD backlight
   */

  protected void setBacklightCommand(int value) throws Exception {
    setSettingsCommand(CommandId.SET_LCD_BACKLIGHT,value);
  }


  /*
   * Set the LCD contrast
   */

  protected void setContrastCommand(int value) throws Exception {
    setSettingsCommand(CommandId.SET_LCD_CONTRAST,value);
  }


  /*
   * Set any of the settings values
   */

  protected void setSettingsCommand(CommandId cmd,int value) throws Exception {

    byte[] response;

    // validate

    if(_socket==null || _linkStatus!=LinkStatus.LINKED)
      throw new Exception("Not connected to the controller");

    // write the command

    writeCommand(cmd,new byte[] { (byte)value} );

    // read the 3-byte response

    response=new byte[3];
    readBytes(response);

    // notify the subscribers

    notifyTemperatureReceived(response);
  }


  /*
   * Read the temperature
   */

  protected void readTemperatureCommand() {

    byte[] response;

    try {

      // validate

      if(_socket==null || _linkStatus!=LinkStatus.LINKED)
        throw new Exception("Not connected to the controller");

      // write the command

      writeCommand(CommandId.READ_TEMPERATURE,null);

      // read the 3-byte response

      response=new byte[3];
      readBytes(response);

      // notify the subscribers

      notifyTemperatureReceived(response);
    }
    catch(Exception ex) {
      notifyTemperatureFailed(ex.getMessage());
    }
  }


  /*
   * Read some bytes
   */

  protected void readBytes(byte[] buffer) throws Exception {

    int i,b;

    for(i=0;i<buffer.length;i++) {

      if((b=_socketInput.read())==-1)
        throw new Exception("Unexpected end of stream while reading response data");

      buffer[i]=(byte)b;
    }
  }


  /*
   * Write a command to the bluetooth device and read the response header
   */

  protected void writeCommand(CommandId id,byte[] data) throws Exception {

    byte[] requestHeader;
    int i,b;

    // update the 16-bit sequence number

    if(_sequenceNumber==65535)
      _sequenceNumber=0;
    else
      _sequenceNumber++;

    // create the 5-byte header

    requestHeader=new byte[5];

    requestHeader[0]=(byte)0xaa;
    requestHeader[1]=0x55;
    requestHeader[2]=id.getId();
    requestHeader[3]=(byte)(_sequenceNumber >> 8);
    requestHeader[4]=(byte)(_sequenceNumber & 0xff);

    // send the request

    _socketOutput.write(requestHeader);

    if(data!=null && data.length>0)
      _socketOutput.write(data);

    // first 5 bytes must match

    for(i=0;i<5;i++) {

      if((b=_socketInput.read())==-1)
        throw new Exception("Unexpected end of input while reading response");

      if(requestHeader[i]!=(byte)b)
        throw new Exception("Invalid response received for request: "+id.getId());
    }

    if((b=_socketInput.read())==-1)
      throw new Exception("Unexpected end of input while reading response");

    // 6th byte must be an ACK

    if(b!=ACK)
      throw new Exception("The device failed to process the request");
  }


  /*
   * Bluetooth adapter enabled
   */

  public void onEnabled() {
    setLinkStatus(LinkStatus.ENABLED);
  }


  /*
   * Bluetooth adapter disabled
   */

  public void onDisabled() {
    setLinkStatus(LinkStatus.DISABLED);
  }


  /*
   * Device has disconnected
   */

  public void onDisconnected() {

    try {
      closeConnection();
    }
    catch(Exception ex) {
    }

    setLinkStatus(LinkStatus.ENABLED);
  }


  /*
   * Check if we're paired with the oven
   */

  protected BluetoothDevice getPairedDevice() {

    if(_bluetoothAdapter!=null) {

      for(BluetoothDevice device : _bluetoothAdapter.getBondedDevices())
        if(device.getName().equals(OVEN_DEVICE_NAME))
          return device;

    }

    return null;
  }


  /*
   * Set a new percentage
   */

  public void onSetDutyCycle(int percent) {

    synchronized(this) {
      _setPercent=percent;
    }
  }


  /*
   * Get the link status
   */

  public LinkStatus getLinkStatus() {
    return _linkStatus;
  }


  /*
   * Set a new link status
   */

  public void setLinkStatus(LinkStatus status) {
    synchronized(this) {
      if(_linkStatus!=status) {
        _linkStatus=status;
        notifyLinkStatus();
      }
    }
  }


  /*
   * Notify a new link status
   */

  protected void notifyLinkStatus() {

    Intent intent;

    intent=new Intent(CustomIntent.LINK_STATUS_CHECK);
    intent.putExtra("link_status",_linkStatus.ordinal());

    sendBroadcast(intent);
  }


  /*
   * Notify that the settings have been read
   */

  protected void notifySettings(byte[] response) {

    Intent intent;

    intent=new Intent(CustomIntent.GET_CONTROLLER_SETTINGS);
    intent.putExtra(CustomIntent.CONTROLLER_SETTINGS_EXTRA,response);

    sendBroadcast(intent);
  }


  /*
   * Notify temperature received
   */

  protected void notifyTemperatureReceived(byte[] response) {

    Intent intent;

    intent=new Intent(CustomIntent.TEMPERATURE_RECEIVED);
    intent.putExtra(CustomIntent.TEMPERATURE_RECEIVED_EXTRA,response);

    sendBroadcast(intent);
  }


  /*
   * Notify temperature failed
   */

  protected void notifyTemperatureFailed(String message) {

    Intent intent;

    intent=new Intent(CustomIntent.TEMPERATURE_FAILED);
    intent.putExtra(CustomIntent.TEMPERATURE_FAILED_EXTRA,message);

    sendBroadcast(intent);
  }


  /*
   * Notify duty cycle set
   */

  protected void notifyDutyCycle(int percent) {

    Intent intent;

    intent=new Intent(CustomIntent.SET_DUTY_CYCLE);
    intent.putExtra(CustomIntent.DUTY_CYCLE_EXTRA,percent);

    sendBroadcast(intent);
  }


  /*
   * Notify duty cycle failed
   */

  protected void notifyDutyCycleFailed(String message) {

    Intent intent;

    intent=new Intent(CustomIntent.SET_DUTY_CYCLE_FAILED);
    intent.putExtra(CustomIntent.DUTY_CYCLE_FAILED_EXTRA,message);

    sendBroadcast(intent);
  }


  /*
   * Notify set sttings failed
   */

  protected void notifySetSettingsFailed(String message) {

    Intent intent;

    intent=new Intent(CustomIntent.SET_SETTINGS_FAILED);
    intent.putExtra(CustomIntent.SET_SETTINGS_FAILED_EXTRA,message);

    sendBroadcast(intent);
  }


  /*
   * Notify that the settings have been set OK
   */

  protected void notifySetSettingsOk(byte[] settings) {

    Intent intent;

    intent=new Intent(CustomIntent.SET_SETTINGS_OK);
    intent.putExtra(CustomIntent.SET_SETTINGS_OK_EXTRA,settings);

    sendBroadcast(intent);
  }


  /*
   * Get the reflow job
   */

  public ReflowJob getReflowJob() {
    return _reflowJob;
  }


  /*
   * New controller settings need to be set
   */

  protected void onSetControllerSettings(byte[] settings) {
    _newSettings=settings;
  }
}
