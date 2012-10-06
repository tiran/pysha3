pysha3
======

sha3 wrapper (keccak) for Python. The package is a wrapper around the
optimized reference implementation from http://keccak.noekeon.org/ . Only
the optimizations for 32 and 64bit platforms are used. The optimized SSE and
ARM assembly variants are ignored for now.

The module is a standalone version of the SHA-3 implemention of Python 3.4
(currently under development).


Usage
=====

  >>> import sha3
  >>> s = sha3.sha3_512()
  >>> s.name
  'sha3_512'
  >>> s.digest_size
  64
  >>> s.update(b"data")
  >>> s.hexdigest()
  '1065aceeded3a5e4412e2187e919bffeadf815f5bd73d37fe00d384fe29f55f08462fdabe1007b993ce5b8119630e7db93101d9425d6e352e22ffe3dcb56b825'

The module contains the constructors sha3_228(), sha3_256(), sha3_384 and
sha3_512().


hashlib monkeypatch
===================

The sha3 module monkey patches the hashlib module:

  >>> import hashlib
  >>> s = hashlib.new("sha3_512")
  Traceback (most recent call last):
  ...
  ValueError: unsupported hash type sha3_512

  >>> import sha3
  >>> s = hashlib.new("sha3_512")
  >>> s = hashlib.sha3_512()


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

Options:
  UseBebigokimisa, Unrolling

- Unrolling: loop unrolling (24, 12, 8, 6, 4, 3, 2, 1)
- UseBebigokimisa: lane complementing

64bit platforms
===============

Additional options:
  UseSSE, UseOnlySIMD64, UseMMX, UseXOP, UseSHLD

Optimized instructions (disabled by default):
  - UseSSE: use Stream SIMD extensions
    o UseOnlySIMD64: limit to 64bit instructions, otherwise 128bit
    o w/o UseOnlySIMD64: requires compiler agument -mssse3 or -mtune
  - UseMMX: use 64bit MMX instructions
  - UseXOP: use AMD's eXtended Operations (128bit SSE extension)

Other:
  - Unrolling: default 24
  - UseBebigokimisa: default 1

When neither UseSSE, UseMMX nor UseXOP is configured, ROL64 (rotate left
64) is implemented as:
  - Windows: _rotl64()
  - UseSHLD: use shld (shift left) asm optimization
  - otherwise: shift and xor

UseBebigokimisa can't be used in combination with UseSSE, UseMMX or
UseXOP. UseOnlySIMD64 has no effect unless UseSSE is specified.

Tests have shown that UseSSE + UseOnlySIMD64 is about three to four
times SLOWER than UseBebigokimisa. UseSSE and UseMMX are about two times
slower. (tested by CH and AP)

32bit platforms
---------------

Additional options:
  UseInterleaveTables, UseSchedule

  - Unrolling: default 2
  - UseBebigokimisa: default n/a
  - UseSchedule: ???, (1, 2, 3; default 3)
  - UseInterleaveTables: use two 64k lookup tables for (de)interleaving
    default: n/a

schedules:
  - 3: no UseBebigokimisa, Unrolling must be 2
  - 2 + 1: ???
