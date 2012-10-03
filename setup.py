import sysconfig
from distutils.core import setup
from distutils.extension import Extension
from glob import glob

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
    name="pykeccak",
    version="0.1",
    ext_modules=[
        Extension("_sha3", sha3_files, depends=sha3_depends)
        ],
    author="Christian Heimes",
    author_email="christian@python.org",
    maintainer="Christian Heimes",
    maintainer_email="christian@python.org",
    url="https://bitbucket.org/tiran/pykeccak",
    keywords="sha3 sha-3 keccak",
    license="PSF",
    description="SHA-3 for Python 3.4",
    #long_description=open("README.txt").read(),
    classifiers=(
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: PSF',
        'Natural Language :: English',
        'Operating System :: POSIX',
        'Programming Language :: Python',
        'Programming Language :: C',
    ),
)
