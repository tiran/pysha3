#ifndef PY_MEMSET_S_H
#define PY_MEMSET_S_H 1

#include <errno.h>
#include <string.h>
#ifndef _MSC_VER
#  include <stdint.h>
#endif

#ifndef errno_t
typedef int errno_t;
#define errno_t errno_t
#endif

#ifndef rsize_t
typedef size_t rsize_t;
#define rsize_t rsize_t
#endif

#ifndef RSIZE_MAX
/* Reasonable large value for bound check, 1 Gibibyte minus 1 Byte */
#define RSIZE_MAX ((rsize_t)1073741823U)
#endif

errno_t _Py_memset_s(void *, rsize_t, int, rsize_t);

#endif /* PY_MEMSET_S_H */
