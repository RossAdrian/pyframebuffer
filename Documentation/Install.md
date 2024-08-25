# Install pyframebuffer

There are several ways to install pyframebuffer:
- [Install with pip](#install-with-pip)
- [Install with setuptools](#install-with-setuptools)
- [Install for builroot system](#install-for-buildroot-system)

## Install with pip

You can install pyframebuffer via pip with the following commands:

```sh
# Install a specific tag from GitHub
PYFB_VERSION=0.0.1
pip install https://github.com/RossAdrian/pyframebuffer/archive/refs/tags/$(PYFB_VERSION).zip

# Install the actual HEAD from the repository
pip install git+https://github.com/RossAdrian/pyframebuffer.git
```

## Install with setuptools

We recommend to install pyframebuffer via pip, as this makes your pyframebuffer installation
managed by pip, making a upgrade as easy as remove the old package and install the new one.

But sometimes, often in embedded systems, you don't have pip available. In this case, you can
install pyframebuffer via setuptools.

First, get the source code from the [pyframebuffer GitHub repository](https://github.com/RossAdrian/pyframebuffer)
and unzip it. Next, change into the project directory, and run:

```sh
./setup.py
```

That's it! With this command, it builds and installs pyframebuffer. But make sure, you have a
compiler like gcc or clang available. For embedded systems, it is recommended to involve building
pyframebuffer directly into the system build workflow. For this, you may want to have a look at
[how to build pyframebuffer with buildroot](Install.md#install-for-buildroot-system).

## Install for buildroot system

If you need pyframebuffer for your embedded system, it is worth to have a look at
[our buildroot fork](https://github.com/rossonlinesolutions/buildroot), which contains additional
packages, including pyframebuffer. Just configure it, and choose under third-party python modules
`python-pyframebuffer`.