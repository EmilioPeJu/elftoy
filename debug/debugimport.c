#include <Python.h>
#include <sys/ptrace.h>
#include <errno.h>
#include "debugimport.h"


static PyObject *
ptrace_attach(PyObject *sdebug, PyObject *args)
{
    pid_t pid;
    int rc;
    if (!PyArg_ParseTuple(args, "i", &pid))
        return NULL;
    rc = ptrace(PTRACE_ATTACH, pid, NULL, NULL);

    if (rc == -1)
        return  PyErr_SetFromErrno(PyExc_OSError);

    return Py_BuildValue("");
}

static PyMethodDef DebugHelperMethods[] = {
    {"ptrace_attach", ptrace_attach, METH_VARARGS, "Attach to process"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef debugmodule = {
    PyModuleDef_HEAD_INIT,
    "debug",
    NULL,
    -1,
    DebugHelperMethods
};

PyMODINIT_FUNC
PyInit_debug(void)
{
    return PyModule_Create(&debugmodule);
}
