======
pysha3
======

SHA-3 wrapper (keccak) for Python. The package is a wrapper around the
optimized Keccak Code Package, https://github.com/gvanas/KeccakCodePackage .

The module is a standalone version of my SHA-3 module from Python 3.6
(currently under development). The code in sha3module.c has been modified to
be compatible with Python 2.7 to 3.5. Python 2.6 and earlier are not
supported.


Updates since pysha 0.3
=======================

**pysha3 1.0 is not compatible with pysha3 0.3!**

pysha3 < 1.0 used the old Keccak implementation. During the finalization of
SHA3, NIST changed the delimiter suffix from 0x01 to 0x06. The Keccak sponge
function stayed the same. pysha3 1.0 provides the previous Keccak hash, too.


Platforms
=========

pysha3 has been successfully tested on several platforms:

 - Linux (GCC, clang) on X86, X86_64 and ARMv6 (little endian)
 - Windows (VS 2008, VS 2010, VS2015) on X86 and X86_64


Usage
=====

The `sha3` module contains several constructors for hash objects with a
PEP 247 compatible interface. The module provides SHA3, SHAKE and Keccak:

* `sha3_228()`, `sha3_256()`, `sha3_384()`, and `sha3_512()`
* `shake_128()`, `shake_256()`
* `keccak_228()`, `keccak_256()`, `keccak_384()`, and `keccak_512()`

The `sha3` module monkey patches the `hashlib` module . The monkey patch is
automatically activated with the first import of the `sha3` module. The
`hashlib` module of Python 3.6 will support the four SHA-3 algorithms and
the two SHAKE algorithms on all platforms. Therefore you shouldn't use the
sha3 module directly and rather go through the `hashlib` interface::

  >>> import sys
  >>> import hashlib
  >>> if sys.version_info < (3, 6):
  ...    import sha3
  >>> s = hashlib.sha3_512()
  >>> s.name
  'sha3_512'
  >>> s.digest_size
  64
  >>> s.update(b"data")
  >>> s.hexdigest()
  'ceca4daf960c2bbfb4a9edaca9b8137a801b65bae377e0f534ef9141c8684c0fedc1768d1afde9766572846c42b935f61177eaf97d355fa8dc2bca3fecfa754d'

  >>> s = hashlib.shake_256()
  >>> s.update(b"data")
  >>> s.hexdigest(4)
  'c73dbed8'
  >>> s.hexdigest(8)
  'c73dbed8527f5ae0'
  >>> s.hexdigest(16)
  'c73dbed8527f5ae0568679f30ecc5cb6'

  >>> import sha3
  >>> k = sha3.keccak_512()
  >>> k.update(b"data")
  >>> k.hexdigest()
  '1065aceeded3a5e4412e2187e919bffeadf815f5bd73d37fe00d384fe29f55f08462fdabe1007b993ce5b8119630e7db93101d9425d6e352e22ffe3dcb56b825'
