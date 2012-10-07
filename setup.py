#!/usr/bin/env python
import sys
import os
import subprocess
from glob import glob
from distutils.core import setup, Command
from distutils.extension import Extension


class TestCommand(Command):
    """Hack for setup.py with implicit build_ext -i
    """
    user_options = []

    def initialize_options(self):
        pass

    def finalize_options(self):
        # hack inplace build into build_ext
        options = self.distribution.command_options
        extoptions = options.setdefault("build_ext", {})
        extoptions["inplace"] = ("hack", True)

    def remove_ext(self):
        """Remove extensions

        All Python 2.x versions share the same library name. Remove the
        file to fix version mismatch errors.
        """
        root = os.getcwd()
        for fname in os.listdir(root):
            if fname.endswith(("so", "dylib", "pyd")):
                os.unlink(os.path.join(root, fname))

    def run(self):
        if sys.version_info[0] < 3:
            self.remove_ext()

        # force a build with build_ext -i
        self.run_command("build")

        errno = subprocess.check_call([sys.executable, "tests.py"])
        raise SystemExit(errno)


exts = []
sha3_depends =  []
sha3_depends.extend(glob("Modules/_sha3/keccak/*.c"))
sha3_depends.extend(glob("Modules/_sha3/keccak/*.h"))
sha3_depends.extend(glob("Modules/_sha3/keccak/*.macros"))
exts.append(Extension("_sha3", ["Modules/_sha3/sha3module.c"],
                      depends=sha3_depends))

long_description = []
with open("README.txt") as f:
    long_description.append(f.read())
with open("CHANGES.txt") as f:
    long_description.append(f.read())

setup(
    name="pysha3",
    version="0.2.2",
    ext_modules=exts,
    py_modules=["sha3"],
    cmdclass = {"test": TestCommand},
    author="Christian Heimes",
    author_email="christian@python.org",
    maintainer="Christian Heimes",
    maintainer_email="christian@python.org",
    url="https://bitbucket.org/tiran/pykeccak",
    keywords="sha3 sha-3 keccak hash",
    license="PSFL (Keccak: CC0 1.0 Universal)",
    description="SHA-3 (Keccak) for Python 2.6 - 3.4",
    long_description="\n".join(long_description),
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
