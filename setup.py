from distutils.core import setup
from distutils.extension import Extension
from glob import glob

exts = []
sha3_depends =  []
sha3_depends.extend(glob("Modules/_sha3/keccak/*.c"))
sha3_depends.extend(glob("Modules/_sha3/keccak/*.h"))
sha3_depends.extend(glob("Modules/_sha3/keccak/*.macros"))
exts.append(Extension("_sha3", ["Modules/_sha3/sha3module.c"],
                      depends=sha3_depends))

setup(
    name="pysha3",
    version="0.1",
    ext_modules=exts,
    py_modules=["sha3"],
    author="Christian Heimes",
    author_email="christian@python.org",
    maintainer="Christian Heimes",
    maintainer_email="christian@python.org",
    url="https://bitbucket.org/tiran/pykeccak",
    keywords="sha3 sha-3 keccak hash",
    license="PSFL (Keccak: CC0 1.0 Universal)",
    description="SHA-3 for Python 2.6 - 3.4",
    long_description=open("README.txt").read(),
    classifiers=(
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: Python Software Foundation License',
        'Natural Language :: English',
        'Operating System :: POSIX',
        'Operating System :: Microsoft :: Windows',
        'Programming Language :: C',
        "Programming Language :: Python",
        "Programming Language :: Python :: 2",
        "Programming Language :: Python :: 2.6",
        "Programming Language :: Python :: 2.7",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.2",
        "Programming Language :: Python :: 3.3",
        #"Programming Language :: Python :: 3.4",
        'Topic :: Security :: Cryptography',
    ),
)
