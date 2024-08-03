/**
 * Higher level painting operation sources.
 */
#include "pyframebuffer.h"

/**
 * Short macro to convert a unsigned long int to a long int.
 * 
 * @param x The number variable to convert
 */
#define ULI_TO_LI(x) ((long int)(x))

/**
 * Sets a pixel, but only if it is on screen, else ignored.
 * 
 * @param xres The x resolution
 * @param yres The y resolution
 * @param x The x coordinate
 * @param y The y coordinate
 * @param fbnum The framebuffer number
 * @param color The color value
 */
#define SET_PIXEL_OR_IGNORE(fbnum, x, y, xres, yres, color) \
    if(x < xres && y < yres) {                              \
        pyfb_setPixel(fbnum, x, y, color);                  \
    }

/**
 * Long int abs function.
 * 
 * @param x The number to remove a minus
 * 
 * @return The input number without possible minus
 */
static inline long int li_abs(long int x) {
    if(x < 0) {
        return (long int)(x * -1);
    }

    // else
    return (long int)x;
}

void __APISTATUS_internal pyfb_drawLine(uint8_t fbnum,
                                        unsigned long int x1,
                                        unsigned long int y1,
                                        unsigned long int x2,
                                        unsigned long int y2,
                                        const struct pyfb_color* color) {
    if(y1 == y2) {
        // draw a horizontal line
        unsigned long int begin = x1 < x2 ? x1 : x2;
        unsigned long int end   = x1 < x2 ? x2 : x1;
        unsigned long int len   = end - begin + 1;
        pyfb_drawHorizontalLine(fbnum, begin, y1, len, color);
        // ready
        return;
    }

    if(x1 == x2) {
        // draw a vertical line
        unsigned long int begin = y1 < y2 ? y1 : y2;
        unsigned long int end   = y1 < y2 ? y2 : y1;
        unsigned long int len   = end - begin + 1;
        pyfb_drawVerticalLine(fbnum, x1, begin, len, color);
        // ready
        return;
    }

    // else if we get here, draw a line so
    long int li_x1 = ULI_TO_LI(x1);
    long int li_x2 = ULI_TO_LI(x2);
    long int li_y1 = ULI_TO_LI(y1);
    long int li_y2 = ULI_TO_LI(y2);

    long int dx  = li_abs(li_x2 - li_x1);
    long int dy  = li_abs(li_y2 - li_y1);
    long int sx  = (li_x1 < li_x2) ? 1 : -1;
    long int sy  = (li_y1 < li_y2) ? 1 : -1;
    long int err = dx - dy;

    while(1) {
        pyfb_setPixel(fbnum, li_x1, li_y1, color);

        if(li_x1 == li_x2 && li_y1 == li_y2) {
            break;
        }

        long int e2 = 2 * err;
        if(e2 > -dy) {
            err -= dy;
            li_x1 += sx;
        }

        if(e2 < dx) {
            err += dx;
            li_y1 += sy;
        }
    }
}

void __APISTATUS_internal pyfb_sdrawLine(uint8_t fbnum,
                                         unsigned long int x1,
                                         unsigned long int y1,
                                         unsigned long int x2,
                                         unsigned long int y2,
                                         const struct pyfb_color* color) {
    // first check if fbnum is valid
    if(fbnum >= MAX_FRAMEBUFFERS) {
        PyErr_SetString(PyExc_ValueError, "The framebuffer number is not valid");
        return;
    }

    // Ok, then lock
    pyfb_fblock(fbnum);

    // next test if the device is really in use
    if(!pyfb_fbused(fbnum)) {
        // this framebuffer is not in use, so ignore
        PyErr_SetString(PyExc_IOError, "The framebuffer is not opened");
        pyfb_fbunlock(fbnum);
        return;
    }

    // check if all values are valid
    struct pyfb_videomode_info vinfo;
    pyfb_vinfo(fbnum, &vinfo);
    unsigned long int xres = vinfo.vinfo.xres;
    unsigned long int yres = vinfo.vinfo.yres;

    if(x1 >= xres || y1 >= yres) {
        PyErr_SetString(PyExc_ValueError, "The x1y1 coordinate is not on the screen");
        pyfb_fbunlock(fbnum);
        return;
    }

    if(x2 >= xres || y2 >= yres) {
        PyErr_SetString(PyExc_ValueError, "The x2y2 coordinate is not on the screen");
        pyfb_fbunlock(fbnum);
        return;
    }

    // all is valid, so draw the line
    pyfb_drawLine(fbnum, x1, y1, x2, y2, color);

    // ready, so return
    pyfb_fbunlock(fbnum);
}

void __APISTATUS_internal pyfb_drawCircle(uint8_t fbnum,
                                          unsigned long int xm,
                                          unsigned long int ym,
                                          unsigned long int radius,
                                          struct pyfb_color* color) {
    long int x0  = ULI_TO_LI(xm);
    long int y0  = ULI_TO_LI(ym);
    long int rad = ULI_TO_LI(radius);

    long int f     = 1 - rad;
    long int ddF_x = 0;
    long int ddF_y = -2 * radius;
    long int x     = 0;
    long int y     = radius;

    struct pyfb_videomode_info vinfo;
    pyfb_vinfo(fbnum, &vinfo);

    const unsigned long int xres = vinfo.vinfo.xres;
    const unsigned long int yres = vinfo.vinfo.yres;

    SET_PIXEL_OR_IGNORE(fbnum, x0, y0 + rad, xres, yres, color);
    SET_PIXEL_OR_IGNORE(fbnum, x0, y0 - rad, xres, yres, color);
    SET_PIXEL_OR_IGNORE(fbnum, x0 + rad, y0, xres, yres, color);
    SET_PIXEL_OR_IGNORE(fbnum, x0 - rad, y0, xres, yres, color);

    while(x < y) {
        if(f >= 0) {
            y -= 1;
            ddF_y += 2;
            f += ddF_y;
        }

        x += 1;
        ddF_x += 2;
        f += ddF_x + 1;

        SET_PIXEL_OR_IGNORE(fbnum, x0 + x, y0 + y, xres, yres, color);
        SET_PIXEL_OR_IGNORE(fbnum, x0 - x, y0 + y, xres, yres, color);
        SET_PIXEL_OR_IGNORE(fbnum, x0 + x, y0 - y, xres, yres, color);
        SET_PIXEL_OR_IGNORE(fbnum, x0 - x, y0 - y, xres, yres, color);
        SET_PIXEL_OR_IGNORE(fbnum, x0 + y, y0 + x, xres, yres, color);
        SET_PIXEL_OR_IGNORE(fbnum, x0 - y, y0 + x, xres, yres, color);
        SET_PIXEL_OR_IGNORE(fbnum, x0 + y, y0 - x, xres, yres, color);
        SET_PIXEL_OR_IGNORE(fbnum, x0 - y, y0 - x, xres, yres, color);
    }
}

void pyfb_sdrawCircle(uint8_t fbnum,
                      unsigned long int xm,
                      unsigned long int ym,
                      unsigned long int radius,
                      struct pyfb_color* color) {
    // first check if fbnum is valid
    if(fbnum >= MAX_FRAMEBUFFERS) {
        PyErr_SetString(PyExc_ValueError, "The framebuffer number is not valid");
        return;
    }

    // Ok, then lock
    pyfb_fblock(fbnum);

    // next test if the device is really in use
    if(!pyfb_fbused(fbnum)) {
        // this framebuffer is not in use, so ignore
        PyErr_SetString(PyExc_IOError, "The framebuffer is not opened");
        pyfb_fbunlock(fbnum);
        return;
    }

    pyfb_drawCircle(fbnum, xm, ym, radius, color);

    // ready, so return
    pyfb_fbunlock(fbnum);
}