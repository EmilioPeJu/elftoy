#include <Python.h>
#include <sys/ptrace.h>
#include <errno.h>
#include "debugutils.h"
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

static PyObject *
ptrace_read_memory(PyObject *sdebug, PyObject *args)
{
    pid_t pid;
    uint64_t address;
    size_t len;
    if (!PyArg_ParseTuple(args, "ill", &pid, &address, &len))
        return NULL;
    PyObject *bytes_buffer = PyBytes_FromStringAndSize(NULL, len);
    char *buffer = PyBytes_AsString(bytes_buffer);
    int rc = read_memory(pid, address, buffer, len);
    if (rc == -1)
        return  PyErr_SetFromErrno(PyExc_OSError);
    return bytes_buffer;
}

static PyMethodDef DebugHelperMethods[] = {
    {"attach", ptrace_attach, METH_VARARGS, "Attach to process"},
    {"read_memory", ptrace_read_memory, METH_VARARGS, "Read another process memory"},
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
