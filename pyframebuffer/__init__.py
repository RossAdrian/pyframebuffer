"""
Core Python sources of the pyframebuffer module.
"""
from pyframebuffer.color import getColorValue

import _fb as fb  # type: ignore

__all__ = ["openfb", "MAX_FRAMEBUFFERS"]
MAX_FRAMEBUFFERS = fb.MAX_FRAMEBUFFERS


class Framebuffer:
    """
    The object representing the framebuffer. This object is private as it
    is wrapped by the openfb() function.

    The usage to open a framebuffer is as following:

    \\code{.py}
    from pyframebuffer.color import rgb
    import pyframebuffer as fb

    color = rgb(255, 0, 0)

    with fb.openfb(0) as framebuffer:
        framebuffer.drawPixel(100, 100, color)
        framebuffer.update()
        # continue drawing something to the framebuffer


    # If exiting the context, the framebuffer is closed cleanly
    \\endcode
    """

    def __init__(self, fbnum):
        """
        Constructor for the Framebuffer object. Note that the constructor
        does not openes the framebuffer. It only assigns all data. To open
        the framebuffer, use it instead in a context.

        @param fbnum The framebuffer number
        """
        self.fbnum = fbnum
        self.xres = None
        self.yres = None
        self.depth = None
        self.opened = False

    def __enter__(self):
        """
        The enter function for a context. This function openes a framebuffer
        device file and fills the resolution informations.
        """
        exitcode = fb.pyfb_open(self.fbnum)
        if exitcode != 0:
            return  # not getting here because native sources throw an error

        # Now get the framebuffer informations
        try:
            (xres, yres, depth) = fb.pyfb_getResolution(self.fbnum)
            self.xres = xres
            self.yres = yres
            self.depth = depth
        finally:
            fb.pyfb_close(self.fbnum)

        self.opened = True
        # Ready

    def __exit__(self, exc_type, exc_value, traceback):
        """
        Exits a context and closes the framebuffer device file
        cleanly, if it is opened.

        @param exc_type Ignored
        @param exc_value Ignored
        @param traceback Ignored
        """
        if self.opened is True:
            fb.pyfb_close(self.fbnum)

        self.opened = False
        self.xres = None
        self.yres = None
        self.depth = None

    def getXRes(self):
        """
        Returns the X resolution of the framebuffer.

        @return The X resolution as number, or None if the framebuffer is not opened
        """
        if self.opened is True:
            return self.xres
        return None

    def getYRes(self):
        """
        Returns the Y resolution of the framebuffer.

        @return The Y resolution as number, or None if the framebuffer is not opened
        """
        if self.opened is True:
            return self.yres
        return None

    def getDepth(self):
        """
        Returns the framebuffer depth in bits.

        @return The framebuffer depth in bits as number, or None if the framebuffer is not opened
        """
        if self.opened is True:
            return self.depth
        return None

    def update(self):
        """
        Updates the framebuffer by flushing the offscreen buffer to the framebuffer. This method
        MUST be callen in order to display something to the screen! Only by explicitly calling this
        function, the actual frame becomes updated.
        """
        fb.pyfb_flushBuffer(self.fbnum)

    def drawPixel(self, x, y, color):
        """
        Draws a pixel on the offscreen buffer.

        @param x The x coordinate
        @param y The y coordinate
        @param color The color value or Color object
        """
        color = getColorValue(color)
        fb.pyfb_setPixel(self.fbnum, x, y, color)

    def drawHorizontalLine(self, x, y, len, color):
        """
        Draws a horizontal line on the offscreen buffer.

        @param x The starting X coordinate
        @param y The row on which to draw the line
        @param len The length of the line in pixel (towards right)
        @param color The color value or Color object
        """
        color = getColorValue(color)
        fb.pyfb_drawHorizontalLine(self.fbnum, x, y, len, color)

    def drawVerticalLine(self, x, y, len, color):
        """
        Draws a vertical line on the offscreen buffer.

        @param x The column on which to draw the line
        @param y The starting y coordinate
        @param len The length of the line in pixel (towards down)
        @param color The color value or Color object
        """
        color = getColorValue(color)
        fb.pyfb_drawVerticalLine(self.fbnum, x, y, len, color)


def openfb(num):
    """
    Opens the framebuffer device file determined by the suffix
    number of the device file. Means with invoking this function
    with num=0, this means that the device file /dev/fb0 will
    be opened.

    @param num The framebuffer number to open

    @return The Framebuffer object
    """
    return Framebuffer(fbnum=num)
