from distutils.core import setup
from distutils.extension import Extension
from glob import glob

try:
    import sysconfig
except ImportError:
    import struct
    pointer_size = struct.calcsize("P") * 8
else:
    pointer_size = sysconfig.get_config_var('SIZEOF_VOID_P') * 8

sha3_files = ["Modules/sha3module.c",
              "Modules/keccak/KeccakNISTInterface.c",
              "Modules/keccak/KeccakSponge.c"]
if pointer_size == 32:
    sha3_files.append("Modules/keccak/KeccakF-1600-opt32.c")
elif pointer_size == 64:
    sha3_files.append("Modules/keccak/KeccakF-1600-opt64.c")
else:
    raise ValueError(pointer_size)

sha3_depends = glob("Modules/keccak/*.h") + glob("Modules/keccak/*.macros")


setup(
    name="pysha3",
    version="0.0.1",
    ext_modules=[
        Extension("_sha3", sha3_files, depends=sha3_depends)
        ],
    py_modules=["sha3"],
    author="Christian Heimes",
    author_email="christian@python.org",
    maintainer="Christian Heimes",
    maintainer_email="christian@python.org",
    url="https://bitbucket.org/tiran/pykeccak",
    keywords="sha3 sha-3 keccak hash",
    license="PSF",
    description="SHA-3 for Python 2.6 - 3.4",
    long_description=open("README.txt").read(),
    classifiers=(
        'Development Status :: 3 - Alpha',
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
