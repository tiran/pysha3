#include <string.h>
#include "pymemsets.h"

/* Windows doesn't provide EOVERFLOW. */
#ifndef EOVERFLOW
#  define EOVERFLOW E2BIG
#endif


/* ISO C11 memset_s() function
 *
 * The function implements the best effort to overwrite a memory location
 * with data in order to wipe sensitive information. memset() isn't
 * sufficient because compilers often optimize a call to memset() away, when
 * the memory segment is not accessed anymore, e.g. at the end of a function
 * body. The functions follows recommendation MSC06-C of the `CERT Secure
 * Coding Standards`.
 *
 * _Py_memset_s() comes WITHOUT warranty and does NOT guarantee any security.
 * The page holding your data might already been swapped to disk or shared
 * with a forked child process. It's still better than no wiping ...
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
 */
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

/* I have seen other tricks to accomplish the same task faster with less CPU
 * instructions.
 *
 * A patch for NetBSD creates a const volatile function
 * pointer to memset() and calls memset() by dereferencing the volatile
 * function pointer. I'm not sure if this works with all compilers.
 *
 *   static void * (* const volatile __memset_vp)(void *, int, size_t) = (memset);
 *   (*__memset_vp)(s, c, n);
 *
 *   Source: http://ftp.netbsd.org/pub/NetBSD/misc/apb/memset_s.20120224.diff
 *
 * Another trick for GCC and LLVM is inline assembly to prevent dead code
 * elimination:
 *
 *   memset(s, c, n);
 *   asm volatile("" : : "r"(s) : "memory");
 *        no code : no write : reads s : clobbers memory
 *
 *   Source: http://llvm.org/bugs/show_bug.cgi?id=15495
 *
 * Windows has SecureZeroMemory(void*, size_t).
 * http://msdn.microsoft.com/en-us/library/windows/desktop/aa366877%28v=vs.85%29.aspx
 */
