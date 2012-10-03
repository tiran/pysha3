/* SHA3 module
 *
 * This module provides an interface to the SHA3 algorithm
 *
 * See below for information about the original code this module was
 * based upon. Additional work performed by:
 *
 *  Andrew Kuchling (amk@amk.ca)
 *  Greg Stein (gstein@lyra.org)
 *  Trevor Perrin (trevp@trevp.net)
 *
 *  Copyright (C) 2012   Christian Heimes (christian@python.org)
 *  Licensed to PSF under a Contributor Agreement.
 *
 */

/* SHA3 objects */

#include "Python.h"
#include "hashlib.h"
#include "keccak/KeccakNISTInterface.h"

/* hard coded for now */
#define SHA3_DIGESTSIZE 512
/* 1600 / 8 ??? */
#define SHA3_BLOCKSIZE 200

#define SHA3_state hashState
#define SHA3_init Init
#define SHA3_process Update
#define SHA3_done Final


/* The structure for storing SHA3 info */

typedef struct {
    PyObject_HEAD

    hashState hash_state;
} SHA3object;


static PyTypeObject SHA3type;


static SHA3object *
newSHA3object(void)
{
    return (SHA3object *)PyObject_New(SHA3object, &SHA3type);
}


/* Internal methods for a hash object */

static void
SHA3_dealloc(PyObject *ptr)
{
    PyObject_Del(ptr);
}


/* External methods for a hash object */

PyDoc_STRVAR(SHA3_copy__doc__, "Return a copy of the hash object.");

static PyObject *
SHA3_copy(SHA3object *self, PyObject *unused)
{
    SHA3object *newobj;

    if (Py_TYPE(self) == &SHA3type) {
        if ( (newobj = newSHA3object())==NULL)
            return NULL;
    } else {
        if ( (newobj = newSHA3object())==NULL)
            return NULL;
    }

    newobj->hash_state = self->hash_state;
    return (PyObject *)newobj;
}

PyDoc_STRVAR(SHA3_digest__doc__,
"Return the digest value as a string of binary data.");

static PyObject *
SHA3_digest(SHA3object *self, PyObject *unused)
{
    unsigned char digest[SHA3_DIGESTSIZE];
    SHA3_state temp;

    temp = self->hash_state;
    SHA3_done(&temp, digest);
    return PyBytes_FromStringAndSize((const char *)digest, SHA3_DIGESTSIZE);
}

PyDoc_STRVAR(SHA3_hexdigest__doc__,
"Return the digest value as a string of hexadecimal digits.");

static PyObject *
SHA3_hexdigest(SHA3object *self, PyObject *unused)
{
    unsigned char digest[SHA3_DIGESTSIZE];
    SHA3_state temp;
    PyObject *retval;
    Py_UCS1 *hex_digest;
    int i, j;

    /* Get the raw (binary) digest value */
    temp = self->hash_state;
    SHA3_done(&temp, digest);

    /* Create a new string */
    retval = PyUnicode_New(SHA3_DIGESTSIZE * 2, 127);
    if (!retval)
            return NULL;
    hex_digest = PyUnicode_1BYTE_DATA(retval);

    /* Make hex version of the digest */
    for(i=j=0; i<SHA3_DIGESTSIZE; i++) {
        unsigned char c;
        c = (digest[i] >> 4) & 0xf;
        hex_digest[j++] = Py_hexdigits[c];
        c = (digest[i] & 0xf);
        hex_digest[j++] = Py_hexdigits[c];
    }
    assert(_PyUnicode_CheckConsistency(retval, 1));
    return retval;
}

PyDoc_STRVAR(SHA3_update__doc__,
"Update this hash object's state with the provided string.");

static PyObject *
SHA3_update(SHA3object *self, PyObject *args)
{
    PyObject *obj;
    Py_buffer buf;

    if (!PyArg_ParseTuple(args, "O:update", &obj))
        return NULL;

    GET_BUFFER_VIEW_OR_ERROUT(obj, &buf);

    SHA3_process(&self->hash_state, buf.buf, buf.len);

    PyBuffer_Release(&buf);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef SHA3_methods[] = {
    {"copy",      (PyCFunction)SHA3_copy,      METH_NOARGS,  SHA3_copy__doc__},
    {"digest",    (PyCFunction)SHA3_digest,    METH_NOARGS,  SHA3_digest__doc__},
    {"hexdigest", (PyCFunction)SHA3_hexdigest, METH_NOARGS,  SHA3_hexdigest__doc__},
    {"update",    (PyCFunction)SHA3_update,    METH_VARARGS, SHA3_update__doc__},
    {NULL,        NULL}         /* sentinel */
};

static PyObject *
SHA3_get_block_size(PyObject *self, void *closure)
{
    return PyLong_FromLong(SHA3_BLOCKSIZE);
}

static PyObject *
SHA3_get_name(PyObject *self, void *closure)
{
    return PyUnicode_FromStringAndSize("SHA3", 3);
}

static PyObject *
SHA3_get_digest_size(PyObject *self, void *closure)
{
    return PyLong_FromLong(SHA3_DIGESTSIZE);
}


static PyGetSetDef SHA3_getseters[] = {
    {"block_size",
     (getter)SHA3_get_block_size, NULL,
     NULL,
     NULL},
    {"name",
     (getter)SHA3_get_name, NULL,
     NULL,
     NULL},
    {"digest_size",
     (getter)SHA3_get_digest_size, NULL,
     NULL,
     NULL},
    {NULL}  /* Sentinel */
};

static PyTypeObject SHA3type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "_SHA3.SHA3",               /*tp_name*/
    sizeof(SHA3object), /*tp_size*/
    0,                  /*tp_itemsize*/
    /* methods */
    SHA3_dealloc,       /*tp_dealloc*/
    0,                  /*tp_print*/
    0,                  /*tp_getattr*/
    0,                  /*tp_setattr*/
    0,                  /*tp_reserved*/
    0,                  /*tp_repr*/
    0,                  /*tp_as_number*/
    0,                  /*tp_as_sequence*/
    0,                  /*tp_as_mapping*/
    0,                  /*tp_hash*/
    0,                  /*tp_call*/
    0,                  /*tp_str*/
    0,                  /*tp_getattro*/
    0,                  /*tp_setattro*/
    0,                  /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT, /*tp_flags*/
    0,                  /*tp_doc*/
    0,                  /*tp_traverse*/
    0,                  /*tp_clear*/
    0,                  /*tp_richcompare*/
    0,                  /*tp_weaklistoffset*/
    0,                  /*tp_iter*/
    0,                  /*tp_iternext*/
    SHA3_methods,       /* tp_methods */
    NULL,               /* tp_members */
    SHA3_getseters,      /* tp_getset */
};


/* The single module-level function: new() */

PyDoc_STRVAR(SHA3_new__doc__,
"Return a new SHA3 hash object; optionally initialized with a string.");

static PyObject *
SHA3_new(PyObject *self, PyObject *args, PyObject *kwdict)
{
    static char *kwlist[] = {"string", NULL};
    SHA3object *new;
    PyObject *data_obj = NULL;
    Py_buffer buf;

    if (!PyArg_ParseTupleAndKeywords(args, kwdict, "|O:new", kwlist,
                                     &data_obj)) {
        return NULL;
    }

    if (data_obj)
        GET_BUFFER_VIEW_OR_ERROUT(data_obj, &buf);

    if ((new = newSHA3object()) == NULL) {
        if (data_obj)
            PyBuffer_Release(&buf);
        return NULL;
    }

    SHA3_init(&new->hash_state, SHA3_DIGESTSIZE);

    if (PyErr_Occurred()) {
        Py_DECREF(new);
        if (data_obj)
            PyBuffer_Release(&buf);
        return NULL;
    }
    if (data_obj) {
        SHA3_process(&new->hash_state, buf.buf, buf.len);
        PyBuffer_Release(&buf);
    }

    return (PyObject *)new;
}


/* List of functions exported by this module */

static struct PyMethodDef SHA3_functions[] = {
    {"sha3",(PyCFunction)SHA3_new, METH_VARARGS|METH_KEYWORDS,SHA3_new__doc__},
    {NULL,      NULL}            /* Sentinel */
};


/* Initialize this module. */

#define insint(n,v) { PyModule_AddIntConstant(m,n,v); }


static struct PyModuleDef _SHA3module = {
        PyModuleDef_HEAD_INIT,
        "_sha3",
        NULL,
        -1,
        SHA3_functions,
        NULL,
        NULL,
        NULL,
        NULL
};

PyMODINIT_FUNC
PyInit__sha3(void)
{
    Py_TYPE(&SHA3type) = &PyType_Type;
    if (PyType_Ready(&SHA3type) < 0)
        return NULL;
    return PyModule_Create(&_SHA3module);
}
