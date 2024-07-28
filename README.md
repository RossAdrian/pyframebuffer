# pyframebuffer

[![Python package](https://github.com/RossAdrian/pyframebuffer/actions/workflows/python-package.yml/badge.svg)](https://github.com/RossAdrian/pyframebuffer/actions/workflows/python-package.yml)

> A linux framebuffer primitives library written in C and Python.

The pyframebuffer library is a library written in C and Python to access over a native API the framebuffer device file. It has been developed to support graphics specially on embedded systems, like the *Raspberry Pi 4 Model B*, to remove overhead from *X* or *Wayland*, and making it easy to draw graphics with Python through an abstraced API. Means this library acts as an abstraction layer of the framebuffer device file access in C for an high-level Python API.

## Supported platforms

The library has been tested on the *Raspberry Pi 4 Model B*. Other hardware devices may be supported as the implementation uses the framebuffer device files `/dev/fbXX` from linux. Means only linux is supported and installation on an platform other than linux will fail.

Other hardware then the *Raspberry Pi 4* may be supported. But testing the framebuffer access on x86_64 computers and Qemu were not successful.

## Installation

Download the sources from [GitHub](https://github.com/RossAdrian/pyframebuffer) and run the following commands:

```sh
sudo ./setup.py install
```

Ensure you have installed the python development package (named `python-dev` or `python-devel`) of your Linux distribution
to be able to build and install this package. Recommended is the installation on a headless distribution with framebuffer
console like *Raspberry Pi OS lite* or *Ubuntu Server* to see the graphics output on the screen. Full screen display without
noisy framebuffer console output is best with running a python script with the draw operations and sleep the amount of seconds
required to see the magic of the graphics output.