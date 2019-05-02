#include <Python.h>
#include "elfimport.h"
#include "elffunctions.h"

static PyObject *get_elf_hdr(PyObject *self, PyObject *args)
{
    return PyBytes_FromString("Not Implemented");
}

static PyObject *inject_PT_NOTE_hijack_entry(PyObject *self, PyObject *args)
{
    char *bin_path;
    char *payload_path;

    if (!PyArg_ParseTuple(args, "ss", &bin_path, &payload_path)) {
            return NULL;
    }

    _inject_PT_NOTE_hijack_entry(bin_path, payload_path);

    return Py_BuildValue("");
}

static PyMethodDef ElfHelperMethods[] = {
    {"get_elf_hdr", get_elf_hdr, METH_VARARGS, "Get elf header"},
    {"inject_PT_NOTE_hijack_entry", inject_PT_NOTE_hijack_entry, METH_VARARGS,
        "Convert PT_NOTE to PT_LOAD segment, inject payload and replace entrypoint"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef elfmodule = {
    PyModuleDef_HEAD_INIT,
    "elf",
    NULL,
    -1,
    ElfHelperMethods
};

PyMODINIT_FUNC
PyInit_elf(void)
{
    return PyModule_Create(&elfmodule);
}
