// Andy's Workshop Open Source Reflow Controller
// Copyright (c) 2015 Andy Brown. All rights Reserved.
// Please see website (http://www.andybrown.me.uk) for full license details.

package uk.me.andybrown.awreflow2;

import android.app.Activity;
import android.widget.Toast;


/*
 * Base activity for all ours
 */

abstract public class BluetoothActivity extends Activity {

  /*
   * get the current link status
   */

  protected LinkStatus getLinkStatus() {
    return ((ReflowApplication)getApplication()).getLinkStatus();
  }


  /*
   * Return true if linked
   */

  protected boolean verifyLinked() {

    switch(getLinkStatus()) {

      case NOT_SUPPORTED:
        Toast.makeText(getApplicationContext(),"This device does not have bluetooth",Toast.LENGTH_LONG).show();
        return false;

      case DISABLED:
        Toast.makeText(getApplicationContext(),"Bluetooth is switched off",Toast.LENGTH_LONG).show();
        return false;

      case ENABLED:
        Toast.makeText(getApplicationContext(),"Bluetooth is not paired with the oven controller",Toast.LENGTH_LONG).show();
        return false;

      case PAIRED:
        Toast.makeText(getApplicationContext(), "Bluetooth data link not active", Toast.LENGTH_LONG).show();
        return false;
    }
    // OK

    return true;
  }
}
