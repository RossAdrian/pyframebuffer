# pyframebuffer

[![Python package](https://github.com/RossAdrian/pyframebuffer/actions/workflows/python-package.yml/badge.svg)](https://github.com/RossAdrian/pyframebuffer/actions/workflows/python-package.yml)

> A linux framebuffer primitives library written in C and Python.

The pyframebuffer library is a library written in C and Python to access over a native API the framebuffer device file. It has been developed to support graphics specially on embedded systems, like the *Raspberry Pi 4 Model B*, to remove overhead from *Xorg* or *Wayland*, and making it easy to draw graphics with Python through an abstraced API. Means this library acts as an abstraction layer of the framebuffer device file access in C for an high-level Python API.

## Supported platforms

The library has been tested on the *Raspberry Pi 4 Model B*. Other hardware devices may be supported as the implementation uses the framebuffer device files `/dev/fbXX` from linux. Means only linux is supported and installation on an platform other than linux will fail.

On x86_64, running tests on Qemu have been succeeded. Starting the VM with option `-vga cirrus` and configuring the framebuffer with at least depth 16 via the
command `fbset -fb /dev/fb0 -g 640 480 640 480 16`, running graphic applications with *pyframebuffer* succeeds.

Other hardware have not been tested so far, but it should work while a framebuffer device file is available. Also both, little and big endian machines are supported.
Only limitation is that *pyframebuffer* only works when the display isn't allready used by another window system like *Xorg* or *Wayland*.

## Installation

### Prerequisites

Ensure you have installed the python development package (named `python-dev` or `python-devel`) of your Linux distribution
to be able to build and install this package. Recommended is the installation on a headless distribution with framebuffer
console like *Raspberry Pi OS lite* or *Ubuntu Server* to see the graphics output on the screen. Full screen display without
noisy framebuffer console output is best with running a python script which performs the drawing operations and sleeps the
amount of seconds required to see the magic of the graphics output.

To install this package, create a [virtual environment](https://docs.python.org/3/tutorial/venv.html), and activate it.

Before running, make sure you have permissions to access the framebuffer device files.

### Install

Download the sources from [GitHub](https://github.com/RossAdrian/pyframebuffer) and activate your virtual environment. Then
change to the projects root directory and run:

```sh
pip install .
```

Or else, install directly with pip:

```sh
# By version
PYFB_VERSION=0.0.1
pip install https://github.com/RossAdrian/pyframebuffer/archive/refs/tags/$(PYFB_VERSION).zip

# or for actual HEAD
pip install git+https://github.com/RossAdrian/pyframebuffer.git
```

For other installation options, see [the installation documentations](Documentation/Install.md).

## Usage

After installing, there are two APIs to use pyframebuffer:

### The pyframebuffer Context API

Here an example snippet:

```py
# Import the library
from pyframebuffer.color import rgb
from pyframebuffer import openfb

# Create a color to draw with
color_red = rgb(255, 0, 0) # red

# Select a framebuffer to draw on (determined by it's device file number)
framebuffer_number = 0

# Open a framebuffer context
with openfb(framebuffer_number) as fb:
    # get framebuffer resolution
    (xres, yres, depth) = fb.getResolution()

    # draw something to the framebuffer
    # here: drawing a red pixel in the middle of the screen
    xpos = int(xres / 2)
    ypos = int(yres / 2)
    fb.drawPixel(xpos, ypos, color_red)

    # And when the frame is ready, update the framebuffer to become all changes
    # visible on the screen
    fb.update()

# When we get back here out of the context, the framebuffer device file is closed
# cleanly. But closing the framebuffer device file does not flushes the last frame
# to the screen. For that, please use the update() function.
```

### The pyframebuffer Decorator API

Here an example snippet aswell:

```py
# Import the library
from pyframebuffer.color import rgb
from pyframebuffer import fbuser

# Create a color to draw with
color_red = rgb(255, 0, 0) # red

# Select a framebuffer to draw on (determined by it's device file number)
framebuffer_number = 0

# Create a framebuffer user function
# becomes the framebuffer object as first argument
@fbuser
def compositor(fb):
    # get framebuffer resolution
    (xres, yres, depth) = fb.getResolution()

    # draw something to the framebuffer
    # here: drawing a red pixel in the middle of the screen
    xpos = int(xres / 2)
    ypos = int(yres / 2)
    fb.drawPixel(xpos, ypos, color_red)

    # And when the frame is ready, update the framebuffer to become all changes
    # visible on the screen
    fb.update()

# And now call the framebuffer user function to draw to the framebuffer
compositor(framebuffer_number)

# Yes, the argument to the framebuffer user function is a <int>
# The decorator itself manages the context and make sure that the first argument
# is replaced by the Framebuffer object.
```

## Documentations

To generate the documentations, install `doxygen` and run the following command in the projects root directory:

```sh
doxygen
```

Then open the `html/index.html` file in your browser to see the generated documentations.

For a overview, see the [Project Documentations](./Documentation/README.md).

## Contributing

See [Contributing Page](./CONTRIBUTING.md) for guidelines and development environment setup.

## License

MIT © [Adrian Roß](https://github.com/RossAdrian)