/* ISO C11 memset_s() function
 *
 * Section K.3.7.4.1, paragraph 4 [ISO/IEC 9899:2011] states:
 *
 *   The memset_s function copies the value of c (converted to an unsigned
 *   char) into each of the first n characters of the object pointed to by s.
 *   Unlike memset, any call to the memset_s function shall be evaluated
 *   strictly according to the rules of the abstract machine as described
 *   in (5.1.2.3). That is, any call to the memset_s function shall assume
 *   that the memory indicated by s and n may be accessible in the future
 *   and thus must contain the values indicated by c.
 *
 */

#include <string.h>
#include "pymemsets.h"


errno_t
_Py_memset_s(void *s, rsize_t smax, int c, rsize_t n)
{
    errno_t errval = 0;
    volatile unsigned char *p = s;

    /* The 1st and 2nd runtime-constraint violation abort the function. */
    if (s == NULL) {
        errval = EINVAL;
        goto err;
    }
    if (smax > RSIZE_MAX) {
        errval = E2BIG;
        goto err;
    }
    /* The 3rd and 4th runtime-constraint violation limit n to smax. */
    if (n > RSIZE_MAX) {
        errval = E2BIG;
        n = smax;
    }
    if (n > smax) {
        errval = EOVERFLOW;
        n = smax;
    }

    while (n--) {
        *p++ = (unsigned char)c;
    }

    if (errval == 0) {
        return 0;
    }
    else {
      err:
        errno = errval;
        return errval;
    }
}

#undef USE_LOOP

