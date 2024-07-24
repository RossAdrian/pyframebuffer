from setuptools import setup, Extension
import platform


def check_platform():
    if platform.system() != "Linux":
        raise RuntimeError("This package does only support Linux")
    print("Installing on platform: ", platform.system())


check_platform()

sources = [
    "framebuffers.c",
    "module_pyfb.c"
]

# Add prefix
src_prefix = "native/"
src = []

for i in sources:
    src.append(src_prefix + i)


setup(name="pyframebuffer",
      version="1.0",
      author="Adrian Roß",
      author_email="adrian.ross@ross-agentur.de",
      description="A linux framebuffer primitives library written in C and Python.",
      download_url="https://github.com/RossAdrian/pyframebuffer/archive/refs/heads/main.zip",
      license="MIT",
      long_description="A linux library written in C and Python to draw with graphic primitives to the framebuffer.",
      maintainer="Adrian Roß",
      maintainer_email="adrian.ross@ross-agentur.de",
      url="https://github.com/RossAdrian/pyframebuffer",
      ext_modules=[Extension("_pyfb", src)])
