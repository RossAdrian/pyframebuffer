# pyframebuffer

> A linux framebuffer primitives library written in C and Python.

The pyframebuffer library is a library written in C and Python to access over a native API the framebuffer device file. It has been developed to support graphics specially on embedded systems, like the *Raspberry Pi 4 Model B*, to remove overhead from *X* or *Wayland*, and making it easy to draw graphics with Python through an abstraced API. Means this library acts as an abstraction layer of the framebuffer device file access in C for an high-level Python API.

## Supported platforms

The library has been tested on the *Raspberry Pi 4 Model B*. Other hardware devices may be supported as the implementation uses the framebuffer device files `/dev/fbXX` from linux. Means only linux is supported and installation on an platform other than linux will fail.

Other hardware then the *Raspberry Pi 4* may be supported. But testing the framebuffer access on x86_64 computers and Qemu were not successful.

