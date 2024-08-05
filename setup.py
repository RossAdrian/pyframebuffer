#!/usr/bin/env python3
from setuptools import setup, Extension
import platform
import pathlib


def check_platform():
    print("Try installing on platform: ", platform.system())
    if platform.system() != "Linux":
        raise RuntimeError("This package does only support Linux")


check_platform()

source_paths = list(pathlib.Path('native').glob("*.c"))
src = []

for i in source_paths:
    src.append(str(i))

setup(name="pyframebuffer",
      version="0.0.1",
      author="Adrian Roß",
      author_email="adrian.ross@ross-agentur.de",
      description="A linux framebuffer primitives library written in C and Python.",
      download_url="https://github.com/RossAdrian/pyframebuffer/archive/refs/heads/main.zip",
      license="MIT",
      long_description="A linux library written in C and Python to draw with graphic primitives to the framebuffer.",
      maintainer="Adrian Roß",
      maintainer_email="adrian.ross@ross-agentur.de",
      url="https://github.com/RossAdrian/pyframebuffer",
      packages=["pyframebuffer"],
      ext_modules=[Extension("_pyfb", src)])
