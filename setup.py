from distutils.core import setup
from distutils.extension import Extension

setup_info = dict(
    name="pykeccak",
    version="0.1",
    ext_modules=[
        Extension("_sha3",
                  ["Modules/sha3module.c",
                   "Modules/keccak/KeccakNISTInterface.c",
                   "Modules/keccak/KeccakSponge.c",
                   "Modules/keccak/KeccakF-1600-opt64.c",
                   ]),
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

setup(**setup_info)
