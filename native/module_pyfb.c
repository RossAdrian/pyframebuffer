/**
 * Python module implementation.
 */

#include "pyframebuffer.h"

#include <Python.h>

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

// The module def

/**
 * The method definitions.
 */
static PyMethodDef pyfb_methods[] = {
    {"pyfb_open", pyfunc_pyfb_open, METH_VARARGS, "Framebuffer open function"},
    {"pyfb_close", pyfunc_pyfb_close, METH_VARARGS, "Framebuffer close function"},
    {"pyfb_setPixel", pyfunc_pyfb_ssetPixel, METH_VARARGS, "Draw a pixel on the framebuffer"},
    {"pyfb_drawHorizontalLine", pyfunc_pyfb_sdrawHorizontalLine, METH_VARARGS, "Draw a horizontal line on the framebuffer"},
    {"pyfb_drawVerticalLine", pyfunc_pyfb_sdrawVerticalLine, METH_VARARGS, "Draw a vertical line on the framebuffer"},
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

    // Add the MAX_FRAMEBUFFERS macro to the constants
    PyModule_AddIntMacro(module, MAX_FRAMEBUFFERS);

    return module;
}