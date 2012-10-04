pysha3
======

sha3 wrapper (keccak) for Python. The package is a wrapper around the
optimized reference implementation from http://keccak.noekeon.org/ . Only
the optimizations for 32 and 64bit platforms are used. The optimized SSE and
ARM assembly variants are ignored for now.

The module is targeted for inclusion in Python 3.4, see 
http://bugs.python.org/issue16113

Usage
=====

  >>> import sha3
  >>> s = sha3.sha3()
  >>> s.name
  'sha3_512'
  >>> s.digest_size
  64
  >>> s.update(b"data")
  >>> s.hexdigest()
  '1065aceeded3a5e4412e2187e919bffeadf815f5bd73d37fe00d384fe29f55f08462fdabe1007b993ce5b8119630e7db93101d9425d6e352e22ffe3dcb56b825'

The module contains the constructors sha3_228(), sha3_256(), sha3_384 and
sha3_512(). sha3() is an alias for sha3_512().
