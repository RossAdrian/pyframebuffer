"""
Core Python sources of the pyframebuffer module.
"""
from pyframebuffer.color import getColorValue
import _pyfb as fb  # type: ignore

import functools
import inspect

__all__ = ["openfb", "MAX_FRAMEBUFFERS", "fbuser"]
MAX_FRAMEBUFFERS = fb.MAX_FRAMEBUFFERS


class Framebuffer:
    """
    The object representing the framebuffer. This object is private as it
    is wrapped by the openfb() function.

    The usage to open a framebuffer is as following:

    @code{.py}
    from pyframebuffer.color import rgb
    import pyframebuffer as fb

    color = rgb(255, 0, 0)

    # -- With the Context API
    with fb.openfb(0) as framebuffer:
        framebuffer.drawPixel(100, 100, color)
        framebuffer.update()
        # continue drawing something to the framebuffer

    # If exiting the context, the framebuffer is closed cleanly

    # -- With the Decorator API
    @fb.fbuser
    def compositor(framebuffer):
        framebuffer.drawPixel(100, 100, color)
        framebuffer.update()

    # And now call the function
    # 0 is the framebuffer number
    compositor(0)
    @endcode
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
        except:
            fb.pyfb_close(self.fbnum)

        self.opened = True
        # Ready
        return self

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

    def getResolution(self):
        """
        Returns the framebuffer resolution in a tuple of structure (xres, yres, depth).

        @return The tuple with the framebuffer resolution values
        """
        return (self.xres, self.yres, self.depth)

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

    def fill(self, color):
        """
        Fills the complete framebuffer with one color. Using as color 0x00000000
        or rgba(0, 0, 0, 0) is equivalent to clear the framebuffer (fill the
        framebuffer with black).

        @param color The color value or Color object
        """
        color = getColorValue(color)
        for i in range(0, self.yres):
            fb.pyfb_drawHorizontalLine(0, 0, i, self.xres, color)

    def drawLine(self, x1, y1, x2, y2, color):
        """
        Draws a line from the Point (x1 | y1) to
        (x2 | y2).

        @param x1 The x1 coordinate
        @param y1 The y1 coordinate
        @param x2 The x2 coordinate
        @param y2 The y2 coordinate
        @param color The color value or object
        """
        color = getColorValue(color)
        fb.pyfb_drawLine(self.fbnum, x1, y1, x2, y2, color)

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

    def drawCircle(self, xm, ym, radius, color):
        """
        Draws a circle on the offscreen buffer.

        @param xm The x coordinate of the middle
        @param ym The y coordinate of the middle
        @param radius The radius of the circle
        @param color The color value of Color object
        """
        color = getColorValue(color)
        fb.pyfb_drawCircle(self.fbnum, xm, ym, radius, color)


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


def fbuser(fn):
    """
    Decorator to open a framebuffer via the Decorator API.

    @param fn The function to wrap

    @return The wrapper function
    """
    @functools.wraps(fn)
    def wrapper(*args, **kwargs):
        # first check the function signature
        signature = inspect.signature(fn)
        first_param = next(iter(signature.parameters), None)

        # declare the return value variable
        ret_val = None

        if first_param not in kwargs:
            # got here because target argument is not used keyworded
            # so manipulate the args tuple
            argl = list(args)
            fbnum = argl[0]
            with openfb(fbnum) as fb_obj:
                argl[0] = fb_obj
                args = tuple(argl)
                ret_val = fn(*args, **kwargs)
        else:
            # got here because target argument is keyworded
            # so modify the dict
            fbnum = int(kwargs[first_param])
            with openfb(fbnum) as fb_obj:
                kwargs[first_param] = fb_obj
                ret_val = fn(*args, **kwargs)

        return ret_val

    return wrapper
