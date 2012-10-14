pysha3
======

SHA-3 wrapper (keccak) for Python. The package is a wrapper around the
optimized reference implementation from http://keccak.noekeon.org/ . Only
the optimizations for 32 and 64bit platforms are used. The optimized SSE and
ARM assembly variants are ignored for now.

The module is a standalone version of the SHA-3 implemention of Python 3.4
(currently under development). The code in sha3module.c has been modified to
be compatible with Python 2.6 to 3.4. Python 2.5 and earlier are not
supported.


Platforms
=========

pysha3 has been successfully tested on several platforms and architectures:

 - Linux (GCC 4.3, GCC 4.6, clang 3.0) on X86, X86_64 and ARMv6
 - Windows (VS 2008, VS 2010) on X86 and X86_64
 - FreeBSD (clang) on X86 and X86_64
 - HP-UX (HP C/aC++) on IA64
 - Solaris 10 (Oracle Solaris Studio 12.3) on SPARC (big endian)
 - AIX (XLC 12.1) on PowerPC (big endian)

Thank you very much to Trend Nelson for the SnakeBite network.


Usage
=====

The `sha3` module contains several constructors for hash objects with a
PEP 247 compatible interface. The module provides `sha3_228()`, `sha3_256()`,
`sha3_384()`, and `sha3_512()`.

The `sha3` module monkey patches the `hashlib` module . The monkey patch is
automatically activated with the first import of the `sha3` module. The
`hashlib` module of Python 3.4 will support the four SHA-3 algorithms
on all platforms. Therefore you shouldn't use the sha3 module directly
and rather go through the `hashlib` interface::

  >>> import sys
  >>> import hashlib
  >>> if sys.version_info < (3, 4):
  ...    import sha3
  >>> s = hashlib.new("sha3_512")
  >>> s = hashlib.sha3_512() # alternative
  >>> s.name
  'sha3_512'
  >>> s.digest_size
  64
  >>> s.update(b"data")
  >>> s.hexdigest()
  '1065aceeded3a5e4412e2187e919bffeadf815f5bd73d37fe00d384fe29f55f08462fdabe1007b993ce5b8119630e7db93101d9425d6e352e22ffe3dcb56b825'

**Don't use SHA-3 for HMAC!** HMAC hasn't been specified for SHA-3 yet and no
test vectors are available, too.


Comments from sha3module header
===============================

The code is based on KeccakReferenceAndOptimized-3.2.zip from 29 May 2012.

The reference implementation is altered in this points:
  - C++ comments are converted to ANSI C comments.
  - All functions and globals are declared static.
  - The typedef for UINT64 is commented out.
  - brg_endian.h is removed.
  - KeccakF-1600-opt[32|64]-settings.h are commented out
  - Some unused functions are commented out to silence compiler warnings.

In order to avoid name clashes with other software I have to declare all
Keccak functions and global data as static. The C code is directly
included into this file in order to access the static functions.

Keccak can be tuned with several paramenters. I try to explain all options
as far as I understand them. The reference implementation also contains
assembler code for ARM platforms (NEON instructions).

Common
------

  `Unrolling`
    loop unrolling (24, 12, 8, 6, 4, 3, 2, 1)

  `UseBebigokimisa`
    lane complementing

64bit platforms
---------------

default settings of common options

  `Unrolling`
    24
  `UseBebigokimisa`
    enabled

Additional optimiation instructions (disabled by default):

  `UseSSE`
    use Stream SIMD extensions

    `UseOnlySIMD64`
      limit to 64bit instructions, otherwise 128bit

    `w/o UseOnlySIMD64`
      requires compiler argument `-mssse3` or `-mtune=core2` or better

  `UseMMX`
    use 64bit MMX instructions

  `UseXOP`
    use AMD's eXtended Operations (128bit SSE extension)

When neither `UseSSE`, `UseMMX` nor `UseXOP` is configured, `ROL64`
(rotate left 64) is implemented as:

  Windows
    _rotl64()

  `UseSHLD`
    use shld (shift left) asm optimization

  otherwise
    shift and xor

`UseBebigokimisa` can't be used in combination with `UseSSE`, `UseMMX` or
`UseXOP`. `UseOnlySIMD64` has no effect unless UseSSE is specified.

Tests have shown that `UseSSE` + `UseOnlySIMD64` is about three to four
times SLOWER than `UseBebigokimisa`. `UseSSE` and `UseMMX` are about two
times slower. (tested by CH and AP)

32bit platforms
---------------

default settings of common options

  `Unrolling`
    2
  `UseBebigokimisa`
    disabled

 `UseSchedule`
   `1`
     unknown

   `2`
     unknown

   `3` [default]
     unknown, no `UseBebigokimisa`, `Unrolling` must be 2

  `UseInterleaveTables`
    use two 64k lookup tables for (de)interleaving (disabled by default)

