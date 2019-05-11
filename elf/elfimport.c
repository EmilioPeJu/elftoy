#include <Python.h>
#include "elfimport.h"
#include "elffunctions.h"

static PyObject *py_inject_note_segment_hijack_entry(PyObject *self, PyObject *args)
{
    char *bin_path;
    char *payload_path;

    if (!PyArg_ParseTuple(args, "ss", &bin_path, &payload_path)) {
            return NULL;
    }

    int rc = _inject_note_segment_hijack_entry(bin_path, payload_path);

    return Py_BuildValue("");
}

static PyObject *py_inject_note_segment(PyObject *self, PyObject *args)
{
    char *bin_path;
    char *payload_path;

    if (!PyArg_ParseTuple(args, "ss", &bin_path, &payload_path)) {
            return NULL;
    }

    Elf64_Addr addr = _inject_note_segment(bin_path, payload_path);

    return Py_BuildValue("K", addr);
}

static PyObject *py_hijack_entry(PyObject *self, PyObject *args)
{
    Elf64_Addr new_addr;
    char *bin_path;

    if (!PyArg_ParseTuple(args, "sK", &bin_path, &new_addr)) {
            return NULL;
    }

    Elf64_Addr addr = _hijack_entry(bin_path, new_addr);

    return Py_BuildValue("K", addr);
}

static PyObject *py_hijack_got(PyObject *self, PyObject *args)
{
    Elf64_Addr new_addr;
    char *bin_path;
    char *symbol_name;

    if (!PyArg_ParseTuple(args, "ssK", &bin_path, &symbol_name, &new_addr)) {
            return NULL;
    }

    Elf64_Addr addr = _hijack_got(bin_path, symbol_name, new_addr);

    return Py_BuildValue("K", addr);
}

static PyObject *py_patch_jmp(PyObject *self, PyObject *args)
{
    Elf64_Addr orig_entry;
    Elf64_Addr addr;
    char *bin_path;

    if (!PyArg_ParseTuple(args, "sKK", &bin_path,
                          &orig_entry, &addr)) {
            return NULL;
    }

    _patch_jmp(bin_path, orig_entry, addr);

    return Py_BuildValue("");
}

static PyMethodDef ElfHelperMethods[] = {
    {"inject_note_segment_hijack_entry", py_inject_note_segment_hijack_entry, METH_VARARGS,
        "Convert note segment to loadable segment, inject payload and replace entrypoint"},
    {"inject_note_segment", py_inject_note_segment, METH_VARARGS,
        "Convert note segment to loadable segment"},
    {"hijack_entry", py_hijack_entry, METH_VARARGS,
        "Change entry point"},
    {"hijack_got", py_hijack_got, METH_VARARGS,
        "Replace got entry given its symbol name"},
    {"patch_jmp", py_patch_jmp, METH_VARARGS,
        "Patch jmp 0 found after the given address"},
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
