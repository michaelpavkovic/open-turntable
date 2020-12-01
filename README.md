# Open Turntable
This is the codebase for our UMass CS335 final project: an open source digital turntable. It is intended to be used with [Mixxx](https://mixxx.org/), which handles audio playback. MIDI commands are sent over the LPC1768's secondary USB data line. Mixxx processes them and turns them into play/pause, cue, jog wheel adjust, volume adjust, and tempo adjust commands.


## How to Build
Building can be done offline or online. Note: all PolluLedStrip code must be removed if compiling offline.
### With mbed-cli:
1. Install the [GCC ARM Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads) and enter its path in env.py.
2. Install the [MBED CLI](https://os.mbed.com/docs/mbed-os/v5.15/tools/manual-installation.html).
3. Run `make`.

### With [ide.mbed.com](https://ide.mbed.com/compiler)
1. Click Import, then import from URL.
2. Paste the URL to this repository and import.
3. Rename `PololuluLedStrip/led_strip_write_color.s` to `PololuluLedStrip/led_strip_write_color.S`.
4. Compile and move to the mounted MBED volume.

## How to run
1. Plug in LPC1768.
2. (If using offline compilation) Run `make deploy`. Make sure the microcontroller's mount point is specified in Makefile.
