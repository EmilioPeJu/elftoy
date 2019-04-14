#include <Python.h>
#include "elfhelper.h"

static PyObject *
get_elf_hdr(PyObject *self, PyObject *args)
{
    return PyBytes_FromString("Not Implemented");
}

static PyMethodDef ElfHelperMethods[] = {
    {"get_elf_hdr", get_elf_hdr, METH_VARARGS, "Get elf header"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef elfhelpermodule = {
    PyModuleDef_HEAD_INIT,
    "elfhelper",
    NULL,
    -1,
    ElfHelperMethods
};

PyMODINIT_FUNC
PyInit_elfhelper(void)
{
    return PyModule_Create(&elfhelpermodule);
}
