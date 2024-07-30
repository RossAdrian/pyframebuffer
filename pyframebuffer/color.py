"""Color utilies"""

import sys

__all__ = ["rgb", "rgba", "from_value", "getColorValue"]


class Color:
    """
    Class for the color on a framebuffer.
    """

    def __init__(self, color_val=0xFFFFFFFF):
        """
        Initializes a color from the color value. Use instead the
        rgb() or rgba() functions to build a instance of this
        class.

        @param color_val The color value as number
        """
        self.color_val = color_val

    def getColorValue(self):
        """
        Returns the color value as like it is used for the native
        functions.

        @return The color value as number
        """
        return self.color_val


def from_value(val=0xFFFFFFFF):
    """
    Creates the Color object from the color value as 32 bit number
    """
    return Color(val)


def rgb(red=0xFF, green=0xFF, blue=0xFF):
    """
    Builds an instance of the color class from the red, green and blue
    value of the color.

    @param red The red channel from 0 to 255
    @param green The green channel from 0 to 255
    @param blue The blue channel from 0 to 255

    @return The Color object representing the output color
    """
    return rgba(red, green, blue, alpha=0xFF)


def rgba(red=0xFF, green=0xFF, blue=0xFF, alpha=0xFF):
    """
    Builds an instance of the color class from the red, green and blue
    value of the color. The alpha value is only used if the framebuffer
    depth is 32 bits. Else if the framebuffer is in 16 bit mode, the
    alpha value is unused.

    @param red The red channel from 0 to 255
    @param green The green channel from 0 to 255
    @param blue The blue channel from 0 to 255
    @param alpha The alpha channel from 0 to 255

    @return The Color object representing the output color
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


def getColorValue(val):
    """
    Returns the color value. Accepts as input value a 32 bit color number,
    or an instance of the color object.

    @param val The value representing a color

    @return A 32 bit color number
    """
    if type(val) is Color:
        return val.color_val
    # else
    return val
