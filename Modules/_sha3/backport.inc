#include "../pymemsets.h"

#define SHA3_clearstate(state) \
    _Py_memset_s(&(state), sizeof(SHA3_state), 0, sizeof(SHA3_state))

#ifndef Py_UNUSED
#ifdef __GNUC__
#define Py_UNUSED(name) _unused_ ## name __attribute__((unused))
#else
#define Py_UNUSED(name) _unused_ ## name
#endif
#endif /* Py_UNUSED */

#if PY_MAJOR_VERSION >= 3
#define Hashname_FromString PyUnicode_FromString
#define Hashsize_FromLong PyLong_FromLong
#else
#define Hashname_FromString PyString_FromString
#define Hashsize_FromLong PyInt_FromLong
#endif /* PY_MAJOR_VERSION >= 3 */

#if (PY_MAJOR_VERSION < 3) || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION < 5)
static PyObject*
_Py_strhex(const char* argbuf, const Py_ssize_t arglen)
{
    static const char *hexdigits = "0123456789abcdef";

    PyObject *retval;
#if PY_MAJOR_VERSION >= 3
    Py_UCS1 *retbuf;
#else
    char *retbuf;
#endif
    Py_ssize_t i, j;

    assert(arglen >= 0);
    if (arglen > PY_SSIZE_T_MAX / 2)
        return PyErr_NoMemory();

#if PY_MAJOR_VERSION >= 3
    retval = PyUnicode_New(arglen * 2, 127);
    if (!retval)
            return NULL;
    retbuf = PyUnicode_1BYTE_DATA(retval);
#else
    retval = PyString_FromStringAndSize(NULL, arglen * 2);
    if (!retval)
            return NULL;
    retbuf = PyString_AsString(retval);
    if (!retbuf) {
            Py_DECREF(retval);
            return NULL;
    }
#endif
    /* make hex version of string, taken from shamodule.c */
    for (i=j=0; i < arglen; i++) {
        unsigned char c;
        c = (argbuf[i] >> 4) & 0xf;
        retbuf[j++] = hexdigits[c];
        c = argbuf[i] & 0xf;
        retbuf[j++] = hexdigits[c];
    }

    return retval;
}
#else
#include "pystrhex.h"
#endif /* < 3.5 */
