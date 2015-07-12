# An android bluetooth reflow oven that anyone can build

Welcome to the github repository that contains the firmware source code to the [bluetooth reflow oven](http://andybrown.me.uk/wk/2015/07/12/awreflow2) project blog. You'll need to read the blog to understand this project before making use of this firmware.

![controller and oven](http://andybrown.me.uk/wk/wp-content/images/awreflow2/pcb_built_small.jpg)

# Build instructions

You might not need to. I've built a `.hex` file for the ATMega8L that is suitable for users on a 220-240V 50Hz mains supply. I've also built the Android `.apk` app ready to [side-load](http://www.greenbot.com/article/2452614/how-to-sideload-an-app-onto-your-android-phone-or-tablet.html) on to your Android device. The app is compatible with all mains supplies.

You can find this files in the `bin/` directory.

## But I really want to build from the source

OK, no worries. Let's deal with the app first.

### Building the app

This one should be easy. Simply download and install [Android Studio](http://developer.android.com/develop/index.html) from Google. When it's installed you can open up the project file from the `android/` directory and build it yourself.

### Building the ATMega8L firmware

I built this firmware with avr-gcc version 4.9.2 so you'll need that version or anything more recent. Linux users can use their package installer to get the compiler. 

Windows users can use the [pre-built package](http://andybrown.me.uk/wk/2015/03/08/avr-gcc-492/) that I created. That's the package that I used to create this firmware. Windows users must also have installed a Unix-like environment such as Cygwin.

I use the `scons` build system to build my projects. `cd` into the `atmega8l` directory and run the following command to build the firmware:

	$ scons mains=50
	scons: Reading SConscript files ...
	scons: done reading SConscript files.
	scons: Building targets ...
	avr-objdump -S awreflow2.elf > awreflow2.lst
	avr-size awreflow2.elf | tee awreflow2.siz
	   text    data     bss     dec     hex filename
	   7724      16       7    7747    1e43 awreflow2.elf

I've trimmed the compiler output from the above command. The `mains=50` option refers to the frequency of your mains supply in Hertz and at the time of writing I've only implemented timings for 50Hz supplies because I don't have the ability to test against the other common frequency: 60Hz. I'll gratefully accept pull requests from anyone who can supply tested timings for 60Hz.

Now the firmware's built you need to upload it. See the [main blog](http://andybrown.me.uk/wk/2015/07/12/awreflow2) article for USBASP setup instructions. Here's the command to upload the built file to the controller:

	$ scons mains=50 upload

We also need to set the fuses on the ATMega8L so that it will use our 8MHz external crystal. Here's how:

	$ scons mains=50 fuse

You might need to manually power-cycle (reset) the device after fusing. For some reason that doesn't happen automatically for me.