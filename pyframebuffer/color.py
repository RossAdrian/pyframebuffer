"""Color utilies"""

import sys

__all__ = ["rgb", "rgba", "from_value"]


class Color:
    """
    Class for the color on a framebuffer.
    """

    def __init__(self, color_val=0xFFFFFFFF):
        """
        Initializes a color from the color value. Use instead the
        rgb() or rgba() functions to build a instance of this
        class.
        """
        self.color_val = color_val

    def getColorValue(self):
        """
        Returns the color value as like it is used for the native
        functions.
        """
        return self.color_val


def from_value(val=0xFFFFFFFF):
    return Color(val)


def rgb(red=0xFF, green=0xFF, blue=0xFF):
    """
    Builds an instance of the color class from the red, green and blue
    value of the color.
    """
    return rgba(red, green, blue, alpha=0xFF)


def rgba(red=0xFF, green=0xFF, blue=0xFF, alpha=0xFF):
    """
    Builds an instance of the color class from the red, green and blue
    value of the color. The alpha value is only used if the framebuffer
    depth is 32 bits. Else if the framebuffer is in 16 bit mode, the
    alpha value is unused.
    """
    red &= 0xFF
    green &= 0xFF
    blue &= 0xFF
    alpha &= 0xFF
    value = 0

    if sys.byteorder == 'little':
        r = red << 24
        g = green << 16
        b = blue << 8
        a = alpha

        value = r | g | b | a
    else:
        r = red >> 24
        g = green >> 16
        b = blue >> 8
        a = alpha

        value = r | g | b | a

    return Color(value)
