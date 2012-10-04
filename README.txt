pysha3
======

sha3 wrapper (keccak) for Python. The package is a wrapper around the
optimized reference implementation from http://keccak.noekeon.org/ . Only
the optimizations for 32 and 64bit platforms are used. The optimized SSE and
ARM assembly variants are ignored for now.

The module is targeted for inclusion in Python 3.4, see 
http://bugs.python.org/issue16113
