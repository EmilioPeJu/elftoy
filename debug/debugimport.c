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
ptrace_detach(PyObject *sdebug, PyObject *args)
{
    pid_t pid;
    int rc;
    if (!PyArg_ParseTuple(args, "i", &pid))
        return NULL;
    rc = ptrace(PTRACE_DETACH, pid, NULL, NULL);

    if (rc == -1)
        return  PyErr_SetFromErrno(PyExc_OSError);

    return Py_BuildValue("");
}

static PyObject *
ptrace_cont(PyObject *sdebug, PyObject *args)
{
    pid_t pid;
    int rc;
    if (!PyArg_ParseTuple(args, "i", &pid))
        return NULL;
    rc = ptrace(PTRACE_CONT, pid, NULL, NULL);

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

static PyObject *
ptrace_write_memory(PyObject *sdebug, PyObject *args)
{
    pid_t pid;
    uint64_t address;
    size_t len;
    Py_ssize_t count;
    const char *buffer;
    if (!PyArg_ParseTuple(args, "ils#", &pid, &address, &buffer, &count))
        return NULL;
    len = count & 0xffffffff;
    int rc = write_memory(pid, address, buffer, len);
    if (rc == -1)
        return  PyErr_SetFromErrno(PyExc_OSError);
    return Py_BuildValue("");
}

static PyMethodDef DebugHelperMethods[] = {
    {"attach", ptrace_attach, METH_VARARGS, "Attach to process"},
    {"detach", ptrace_detach, METH_VARARGS, "Detach from process"},
    {"cont", ptrace_cont, METH_VARARGS, "Continue"},
    {"read_memory", ptrace_read_memory, METH_VARARGS, "Read another process memory"},
    {"write_memory", ptrace_write_memory, METH_VARARGS, "Write another process memory"},
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
