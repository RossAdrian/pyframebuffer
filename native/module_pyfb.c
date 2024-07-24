/**
 * Python module implementation.
 */

#include <Python.h>

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
    if(!PyArg_ParseTuple(args, "b", &fbnum)) {
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
    if(!PyArg_ParseTuple(args, "b", &fbnum)) {
        PyErr_SetString(PyExc_TypeError, "Expecting arguments of type byte");
        return NULL;
    }

    int exitcode = 0;
    pyfb_close((uint8_t)fbnum_c);
    return PyLong_FromLong(exitcode);
}


// The module def

/**
 * The method definitions.
 */
static PyMethodDef pyfb_methods[] = {
    {"pyfb_open", pyfunc_pyfb_open, METH_VARARGS, "Framebuffer open function"},
    {"pyfb_close", pyfunc_pyfb_close, METH_VARARGS, "Framebuffer close function"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef module__pyfb = {
    PyModuleDef_HEAD_INIT,
    "_pyfb",
    "Native interface for the pyframebuffer C sources",
    -1,
    pyfb_methods
};

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