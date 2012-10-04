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


/* #define SHA3_BLOCKSIZE 1600 / 8 */
#define SHA3_MAX_DIGESTSIZE 64 /* 512 / 8 */
#define SHA3_state hashState
#define SHA3_init Init
#define SHA3_process Update
#define SHA3_done Final
#define SHA3_copystate(dest, src) memcpy(&dest, &src, sizeof(SHA3_state));

/* The structure for storing SHA3 info */

typedef struct {
    PyObject_HEAD
    int hashbitlen;
    SHA3_state hash_state;
} SHA3object;

static PyTypeObject SHA3type;


static SHA3object *
newSHA3object(int hashbitlen)
{
    SHA3object *newobj;

    /* check hashbitlen */
    switch(hashbitlen) {
        /* supported hash length */
        case 224:
            break;
        case 256:
            break;
        case 384:
            break;
        case 512:
            break;
        /* case 0: (arbitrarily-long output) isn't supported by this module */
        /* everything else is an error */
        default:
            PyErr_SetString(PyExc_ValueError,
                    "hashbitlen must be one of 224, 256, 384 or 512.");
            return NULL;
    }

    newobj = (SHA3object *)PyObject_New(SHA3object, &SHA3type);
    newobj->hashbitlen = hashbitlen;
    return newobj;
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

    if ((newobj = newSHA3object(self->hashbitlen)) == NULL) {
        return NULL;
    }
    SHA3_copystate(newobj->hash_state, self->hash_state);
    return (PyObject *)newobj;
}

PyDoc_STRVAR(SHA3_digest__doc__,
"Return the digest value as a string of binary data.");

static PyObject *
SHA3_digest(SHA3object *self, PyObject *unused)
{
    unsigned char digest[SHA3_MAX_DIGESTSIZE];
    SHA3_state temp;

    SHA3_copystate(temp, self->hash_state);
    if (SHA3_done((hashState*)&temp, digest) != SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError,
                        "internal error in SHA3 Final()");
        return NULL;
    }
    return PyBytes_FromStringAndSize((const char *)digest,
                                      self->hashbitlen / 8);
}

PyDoc_STRVAR(SHA3_hexdigest__doc__,
"Return the digest value as a string of hexadecimal digits.");

static PyObject *
SHA3_hexdigest(SHA3object *self, PyObject *unused)
{
    unsigned char digest[SHA3_MAX_DIGESTSIZE];
    SHA3_state temp;
    PyObject *retval;
    Py_UCS1 *hex_digest;
    int digestlen, i, j;

    /* Get the raw (binary) digest value */
    SHA3_copystate(temp, self->hash_state);
    if (SHA3_done((hashState*)&temp, digest) != SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "internal error in SHA3 Final()");
        return NULL;
    }

    /* Create a new string */
    digestlen = self->hashbitlen / 8;
    retval = PyUnicode_New(digestlen * 2, 127);
    if (!retval)
            return NULL;
    hex_digest = PyUnicode_1BYTE_DATA(retval);

    /* Make hex version of the digest */
    for(i=j=0; i < digestlen; i++) {
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

    /* add new data, the function takes the length in bits not bytes */
    if (SHA3_process((hashState*)&self->hash_state, buf.buf, buf.len * 8) != SUCCESS) {
        PyBuffer_Release(&buf);
        PyErr_SetString(PyExc_RuntimeError,
                        "internal error in SHA3 Update()");
        return NULL;
    }

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

/* static PyObject *
SHA3_get_block_size(SHA3object *self, void *closure)
{
    return PyLong_FromLong(SHA3_BLOCKSIZE);
}*/

static PyObject *
SHA3_get_name(SHA3object *self, void *closure)
{
    char *name;
    switch (self->hashbitlen) {
        case 224:
            name = "SHA3-224";
            break;
        case 256:
            name = "SHA3-256";
            break;
        case 384:
            name = "SHA3-384";
            break;
        case 512:
            name = "SHA3-512";
            break;
        default:
            /* can never be reached */
            PyErr_SetString(PyExc_ValueError, "invalid hashbitlen");
    };
    return PyUnicode_FromStringAndSize(name, 7);
}

static PyObject *
SHA3_get_digest_size(SHA3object *self, void *closure)
{
    return PyLong_FromLong(self->hashbitlen / 8);
}


static PyGetSetDef SHA3_getseters[] = {
    /*{"block_size", (getter)SHA3_get_block_size, NULL, NULL, NULL},*/
    {"name", (getter)SHA3_get_name, NULL, NULL, NULL},
    {"digest_size", (getter)SHA3_get_digest_size, NULL, NULL, NULL},
    {NULL}  /* Sentinel */
};

static PyTypeObject SHA3type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "_sha3.SHA3",       /* tp_name */
    sizeof(SHA3object), /* tp_size */
    0,                  /* tp_itemsize */
    /*  methods  */
    SHA3_dealloc,       /* tp_dealloc */
    0,                  /* tp_print */
    0,                  /* tp_getattr */
    0,                  /* tp_setattr */
    0,                  /* tp_reserved */
    0,                  /* tp_repr */
    0,                  /* tp_as_number */
    0,                  /* tp_as_sequence */
    0,                  /* tp_as_mapping */
    0,                  /* tp_hash */
    0,                  /* tp_call */
    0,                  /* tp_str */
    0,                  /* tp_getattro */
    0,                  /* tp_setattro */
    0,                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT, /* tp_flags */
    0,                  /* tp_doc */
    0,                  /* tp_traverse */
    0,                  /* tp_clear */
    0,                  /* tp_richcompare */
    0,                  /* tp_weaklistoffset */
    0,                  /* tp_iter */
    0,                  /* tp_iternext */
    SHA3_methods,       /* tp_methods */
    NULL,               /* tp_members */
    SHA3_getseters,     /* tp_getset */
};


/* constructor helper */
static PyObject *
SHA3_factory(PyObject *data_obj, int hashbitlen)
{
    SHA3object *new;
    Py_buffer buf;

    if (data_obj)
        GET_BUFFER_VIEW_OR_ERROUT(data_obj, &buf);

    if ((new = newSHA3object(hashbitlen)) == NULL) {
        goto error;
    }

    if (SHA3_init((hashState*)&new->hash_state, hashbitlen) != SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError,
                        "internal error in SHA3 Update()");
        goto error;
    }

    if (data_obj) {
        if (SHA3_process(&new->hash_state, buf.buf, buf.len * 8) != SUCCESS) {
            PyErr_SetString(PyExc_RuntimeError,
                            "internal error in SHA3 Update()");
            goto error;
        }
        PyBuffer_Release(&buf);
    }

    return (PyObject *)new;

  error:
    if (data_obj) {
        PyBuffer_Release(&buf);
    }
    return NULL;

}

PyDoc_STRVAR(sha3_new__doc__,
"sha3([string], *, hashbitlen=512) -> SHA3 object\n\
\n\
Return a new SHA3 hash object.\n\
hashbitlen must be one of 224, 256, 384 or 512.");

static PyObject *
sha3_new(PyObject *self, PyObject *args, PyObject *kwdict)
{
    static char *kwlist[] = {"string", "hashbitlen", NULL};
    int hashbitlen = 512;
    PyObject *data_obj = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwdict, "|O$i:sha3", kwlist,
                                     &data_obj, &hashbitlen)) {
        return NULL;
    }
    return SHA3_factory(data_obj, hashbitlen);
}

PyDoc_STRVAR(sha3_224__doc__,
"sha3_224([string]) -> SHA3 object\n\
\n\
Return a new SHA3 hash object with a hashbit length of 224 bits (28 bytes).");

static PyObject *
sha3_224(PyObject *self, PyObject *args, PyObject *kwdict)
{
    static char *kwlist[] = {"string", NULL};
    PyObject *data_obj = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwdict, "|O:sha3_224", kwlist,
                                     &data_obj)) {
        return NULL;
    }
    return SHA3_factory(data_obj, 224);
}


PyDoc_STRVAR(sha3_256__doc__,
"sha3_256([string]) -> SHA3 object\n\
\n\
Return a new SHA3 hash object with a hashbit length of 256 bits (32 bytes).");

static PyObject *
sha3_256(PyObject *self, PyObject *args, PyObject *kwdict)
{
    static char *kwlist[] = {"string", NULL};
    PyObject *data_obj = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwdict, "|O:sha3_256", kwlist,
                                     &data_obj)) {
        return NULL;
    }
    return SHA3_factory(data_obj, 256);
}

PyDoc_STRVAR(sha3_384__doc__,
"sha3_384([string]) -> SHA3 object\n\
\n\
Return a new SHA3 hash object with hashbitlen of 384 bits (28 bytes).");

static PyObject *
sha3_384(PyObject *self, PyObject *args, PyObject *kwdict)
{
    static char *kwlist[] = {"string", NULL};
    PyObject *data_obj = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwdict, "|O:sha3_384", kwlist,
                                     &data_obj)) {
        return NULL;
    }
    return SHA3_factory(data_obj, 384);
}

PyDoc_STRVAR(sha3_512__doc__,
"sha3_512([string]) -> SHA3 object\n\
\n\
Return a new SHA3 hash object with hashbitlen of 512 bits (64 bytes).");

static PyObject *
sha3_512(PyObject *self, PyObject *args, PyObject *kwdict)
{
    static char *kwlist[] = {"string", NULL};
    PyObject *data_obj = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwdict, "|O:sha3_512", kwlist,
                                     &data_obj)) {
        return NULL;
    }
    return SHA3_factory(data_obj, 512);
}


/* List of functions exported by this module */
static struct PyMethodDef SHA3_functions[] = {
    {"sha3", (PyCFunction)sha3_new, METH_VARARGS|METH_KEYWORDS, sha3_new__doc__},
    {"sha3_224", (PyCFunction)sha3_224, METH_VARARGS|METH_KEYWORDS, sha3_224__doc__},
    {"sha3_256", (PyCFunction)sha3_256, METH_VARARGS|METH_KEYWORDS, sha3_256__doc__},
    {"sha3_384", (PyCFunction)sha3_384, METH_VARARGS|METH_KEYWORDS, sha3_384__doc__},
    {"sha3_512", (PyCFunction)sha3_512, METH_VARARGS|METH_KEYWORDS, sha3_512__doc__},
    {NULL,      NULL}            /* Sentinel */
};

/* Initialize this module. */
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
