/**
 * Python module implementation.
 */

#include "pyframebuffer.h"

/**
 * Python wrapper for the pyfb_open function.
 *
 * @param self This function
 * @param args The arguments, expecting a long of the fbnum
 *
 * @return The exitstatus
 */
static PyObject* pyfunc_pyfb_open(PyObject* self, PyObject* args) {
    unsigned char fbnum_c = 0;
    if(!PyArg_ParseTuple(args, "b", &fbnum_c)) {
        PyErr_SetString(PyExc_TypeError, "Expecting arguments of type byte");
        return NULL;
    }

    int exitcode = pyfb_open((uint8_t)fbnum_c);
    return PyLong_FromLong(exitcode);
}

/**
 * Python wrapper for the pyfb_close function.
 *
 * @param self The function
 * @param args The arguments, expecting long of the fbnum
 *
 * @return Just a 0
 */
static PyObject* pyfunc_pyfb_close(PyObject* self, PyObject* args) {
    unsigned char fbnum_c = 0;
    if(!PyArg_ParseTuple(args, "b", &fbnum_c)) {
        PyErr_SetString(PyExc_TypeError, "Expecting arguments of type byte");
        return NULL;
    }

    int exitcode = 0;
    pyfb_close((uint8_t)fbnum_c);
    return PyLong_FromLong(exitcode);
}

/**
 * Python wrapper for the pyfb_ssetPixel function.
 *
 * @param self The function
 * @param args The arguments, expecting long of the fbnum, long of the x coordinate, long of the y coordinate, and long of the color 32bit value
 *
 * @return Just a 0
 */
static PyObject* pyfunc_pyfb_ssetPixel(PyObject* self, PyObject* args) {
    unsigned char fbnum_c = 0;
    unsigned long int x;
    unsigned long int y;
    uint32_t color_val;

    if(!PyArg_ParseTuple(args, "bkkI", &fbnum_c, &x, &y, &color_val)) {
        PyErr_SetString(PyExc_TypeError, "Expecting arguments of type (byte, long, long, long)");
        return NULL;
    }

    // now parse the color
    struct pyfb_color color;
    pyfb_initcolor_u32(&color, color_val);

    // And invoke the target function
    pyfb_ssetPixel((uint8_t)fbnum_c, x, y, &color);

    // ready
    int exitcode = 0;
    return PyLong_FromLong(exitcode);
}

/**
 * Python wrapper for the pyfb_sdrawHorizontalLine function.
 * 
 * @param self The function
 * @param args The arguments, expecting long of the fbnum, long of the x coordinate, long of the y coordinate, long of the len coordinate and long for the color
 * 
 * @return Just a 0
 */
static PyObject* pyfunc_pyfb_sdrawHorizontalLine(PyObject* self, PyObject* args) {
    unsigned char fbnum_c;
    unsigned long int x;
    unsigned long int y;
    unsigned long int len;
    uint32_t color_val;

    if(!PyArg_ParseTuple(args, "bkkkI", &fbnum_c, &x, &y, &len, &color_val)) {
        PyErr_SetString(PyExc_TypeError, "Expecting arguments of type (byte, long, long, long, long)");
        return NULL;
    }

    // now parse the color
    struct pyfb_color color;
    pyfb_initcolor_u32(&color, color_val);

    // And invoke the target function
    pyfb_sdrawHorizontalLine((uint8_t)fbnum_c, x, y, len, &color);

    // ready
    int exitcode = 0;
    return PyLong_FromLong(exitcode);
}

/**
 * Python wrapper for the pyfb_sdrawVerticalLine function.
 * 
 * @param self The function
 * @param args The arguments, expecting long of the fbnum, long of the x coordinate, long of the y coordinate, long of the len coordinate and long for the color
 * 
 * @return Just a 0
 */
static PyObject* pyfunc_pyfb_sdrawVerticalLine(PyObject* self, PyObject* args) {
    unsigned char fbnum_c;
    unsigned long int x;
    unsigned long int y;
    unsigned long int len;
    uint32_t color_val;

    if(!PyArg_ParseTuple(args, "bkkkI", &fbnum_c, &x, &y, &len, &color_val)) {
        PyErr_SetString(PyExc_TypeError, "Expecting arguments of type (byte, long, long, long, long)");
        return NULL;
    }

    // now parse the color
    struct pyfb_color color;
    pyfb_initcolor_u32(&color, color_val);

    // and invoke the target function
    pyfb_sdrawVerticalLine((uint8_t)fbnum_c, x, y, len, &color);

    // ready
    int exitcode = 0;
    return PyLong_FromLong(exitcode);
}

/**
 * Python wrapper for the pyfb_flushBuffer function.
 * 
 * @param self The function
 * @param args The arguments, expecting long of the fbnum
 * 
 * @return The exitstatus
 */
static PyObject* pyfunc_pyfb_flushBuffer(PyObject* self, PyObject* args) {
    unsigned char fbnum_c;

    if(!PyArg_ParseTuple(args, "b", &fbnum_c)) {
        PyErr_SetString(PyExc_TypeError, "Expecting arguments of type (byte)");
        return NULL;
    }

    // Now invoke the function
    int exitcode = pyfb_flushBuffer((uint8_t)fbnum_c);
    return PyLong_FromLong(exitcode);
}

/**
 * Returns the resolution of the framebuffer.
 * 
 * @param self The function
 * @param args The arguments, expecting long of the fbnum
 * 
 * @return A python tuple of (xres, yres, bit-depth)
 */
static PyObject* pyfunc_pyfb_getResolution(PyObject* self, PyObject* args) {
    unsigned char fbnum_c;

    if(!PyArg_ParseTuple(args, "b", &fbnum_c)) {
        PyErr_SetString(PyExc_TypeError, "Expecting arguments of type (byte)");
        return NULL;
    }

    // Else build the tuple of the resolution info and return it
    struct pyfb_videomode_info vinfo;
    pyfb_svinfo((uint8_t)fbnum_c, &vinfo);

    // check if valid
    if(vinfo.fb_size_b == 0) {
        PyErr_SetString(PyExc_ValueError, "The framebuffer number is not valid");
        return NULL;
    }

    // else build the tuple
    PyObject* tuple = Py_BuildValue("III", vinfo.vinfo.xres, vinfo.vinfo.yres, vinfo.vinfo.bits_per_pixel);
    return tuple;
}

/**
 * Python wrapper for the pyfb_drawLine function.
 * 
 * @param self The function
 * @param args The arguments, expecting long of the fbnum
 * 
 * @return Just 0
 */
static PyObject* pyfunc_pyfb_sdrawLine(PyObject* self, PyObject* args) {
    unsigned char fbnum_c;
    unsigned long int x1;
    unsigned long int y1;
    unsigned long int x2;
    unsigned long int y2;
    uint32_t color_val;

    if(!PyArg_ParseTuple(args, "bkkkkI", &fbnum_c, &x1, &y1, &x2, &y2, &color_val)) {
        PyErr_SetString(PyExc_TypeError, "Expecting arguments of type (byte, long, long, long, long, long)");
        return NULL;
    }

    // now parse the color
    struct pyfb_color color;
    pyfb_initcolor_u32(&color, color_val);

    // and invoke the target function
    pyfb_sdrawLine((uint8_t)fbnum_c, x1, y1, x2, y2, &color);

    // and return just 0
    int exitcode = 0;
    return PyLong_FromLong(exitcode);
}

/**
 * Python wrapper for the pyfb_fill function.
 * 
 * @param self The function
 * @param args The arguments, expecting long of the fbnum, long of the color value
 * 
 * @return Just 0
 */
static PyObject* pyfunc_pyfb_sfill(PyObject* self, PyObject* args) {
    unsigned char fbnum_c;
    uint32_t color_val;

    if(!PyArg_ParseTuple(args, "bI", &fbnum_c, &color_val)) {
        PyErr_SetString(PyExc_TypeError, "Expecting arguments of type (byte, long)");
        return NULL;
    }

    // now parse the color
    struct pyfb_color color;
    pyfb_initcolor_u32(&color, color_val);

    // and invoke the target function
    pyfb_sfill((uint8_t)fbnum_c, &color);

    // and return just 0
    int exitcode = 0;
    return PyLong_FromLong(exitcode);
}

/**
 * Python wrapper for the pyfb_drawCircle function.
 * 
 * @param self The function
 * @param args The arguments, expecting long of the fbnum, long of the xm, long of the ym, long of the radius, long of the color
 * 
 * @return Just 0
 */
static PyObject* pyfunc_pyfb_sdrawCircle(PyObject* self, PyObject* args) {
    unsigned char fbnum_c;
    unsigned long int xm;
    unsigned long int ym;
    unsigned long int radius;
    uint32_t color_val;

    if(!PyArg_ParseTuple(args, "bkkkI", &fbnum_c, &xm, &ym, &radius, &color_val)) {
        PyErr_SetString(PyExc_TypeError, "Expecting arguments of type (byte, long, long, long, long)");
        return NULL;
    }

    // Now parse the color
    struct pyfb_color color;
    pyfb_initcolor_u32(&color, color_val);

    // and invoke the target function
    pyfb_sdrawCircle((uint8_t)fbnum_c, xm, ym, radius, &color);

    // and return just 0
    int exitcode = 0;
    return PyLong_FromLong(exitcode);
}

// The module def

/**
 * The method definitions.
 */
static PyMethodDef pyfb_methods[] = {
    {"pyfb_open", pyfunc_pyfb_open, METH_VARARGS, "Framebuffer open function"},
    {"pyfb_close", pyfunc_pyfb_close, METH_VARARGS, "Framebuffer close function"},
    {"pyfb_setPixel", pyfunc_pyfb_ssetPixel, METH_VARARGS, "Draw a pixel on the framebuffer"},
    {"pyfb_drawLine", pyfunc_pyfb_sdrawLine, METH_VARARGS, "Draw a line on the framebuffer"},
    {"pyfb_drawHorizontalLine", pyfunc_pyfb_sdrawHorizontalLine, METH_VARARGS, "Draw a horizontal line on the framebuffer"},
    {"pyfb_drawVerticalLine", pyfunc_pyfb_sdrawVerticalLine, METH_VARARGS, "Draw a vertical line on the framebuffer"},
    {"pyfb_drawCircle", pyfunc_pyfb_sdrawCircle, METH_VARARGS, "Draw a circle on the framebuffer"},
    {"pyfb_fill", pyfunc_pyfb_sfill, METH_VARARGS, "Fill the framebuffer in one color"},
    {"pyfb_flushBuffer", pyfunc_pyfb_flushBuffer, METH_VARARGS, "Flush the offscreen buffer to the framebuffer"},
    {"pyfb_getResolution", pyfunc_pyfb_getResolution, METH_VARARGS, "Returns a tupel of the framebuffer resolution"},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef module__pyfb = {PyModuleDef_HEAD_INIT,
                                          "_pyfb",
                                          "Native interface for the pyframebuffer C sources",
                                          -1,
                                          pyfb_methods};

/**
 * Module init function.
 *
 * Initializes the module and defines the MAX_FRAMEBUFFERS macro as
 * constant in Python.
 *
 * @return The module definition
 */
PyMODINIT_FUNC PyInit__pyfb(void) {
    PyObject* module = PyModule_Create(&module__pyfb);
    pyfb_init();

    // Add the MAX_FRAMEBUFFERS macro to the constants
    PyModule_AddIntMacro(module, MAX_FRAMEBUFFERS);

    return module;
}