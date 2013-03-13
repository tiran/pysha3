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
        self.rootdir = os.getcwd()

    def finalize_options(self):
        pass

    def remove_ext(self):
        """Remove extensions

        All Python 2.x versions share the same library name. Remove the
        file to fix version mismatch errors.
        """
        for fname in os.listdir(self.rootdir):
            if fname.endswith(("so", "dylib", "pyd", "sl")):
                os.unlink(os.path.join(self.rootdir, fname))

    def get_lib_dirs(self):
        """Get version, platform and configuration dependend lib dirs

        Distutils caches the build command object on the distribution object.
        We can retrieve the object to retrieve the paths to the directories
        inside the build directory.
        """
        build = self.distribution.command_obj["build"]
        builddirs = set()
        for attrname in 'build_platlib', 'build_lib', 'build_purelib':
            builddir = getattr(build, attrname, None)
            if not builddir:
                continue
            builddir = os.path.abspath(os.path.join(self.rootdir, builddir))
            if not os.path.isdir(builddir):
                continue
            builddirs.add(builddir)
        return builddirs

    def run(self):
        self.remove_ext()
        # force a build with build_ext
        self.run_command("build")
        # get lib dirs from build object
        libdirs = self.get_lib_dirs()
        # add lib dirs to Python's search path
        env = os.environ.copy()
        env["PYTHONPATH"] = os.pathsep.join(libdirs)
        # and finally run the test command
        errno = subprocess.check_call([sys.executable, "tests.py"], env=env)
        raise SystemExit(errno)


exts = []
sha3_depends = ["Modules/hashlib.h", "Modules/pymemsets.h"]
sha3_depends.extend(glob("Modules/_sha3/keccak/*.c"))
sha3_depends.extend(glob("Modules/_sha3/keccak/*.h"))
sha3_depends.extend(glob("Modules/_sha3/keccak/*.macros"))
exts.append(Extension("_sha3",
                      ["Modules/_sha3/sha3module.c", "Modules/pymemsets.c"],
                      depends=sha3_depends))

long_description = []
with open("README.txt") as f:
    long_description.append(f.read())
with open("CHANGES.txt") as f:
    long_description.append(f.read())

setup(
    name="pysha3",
    version="0.4dev",
    ext_modules=exts,
    py_modules=["sha3"],
    cmdclass={"test": TestCommand},
    author="Christian Heimes",
    author_email="christian@python.org",
    maintainer="Christian Heimes",
    maintainer_email="christian@python.org",
    url="https://bitbucket.org/tiran/pykeccak",
    keywords="sha3 sha-3 keccak hash",
    platforms="POSIX, Windows",
    license="PSFL (Keccak: CC0 1.0 Universal)",
    description="SHA-3 (Keccak) for Python 2.6 - 3.4",
    long_description="\n".join(long_description),
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: Python Software Foundation License",
        "License :: CC0 1.0 Universal (CC0 1.0) Public Domain Dedication",
        "Natural Language :: English",
        "Operating System :: MacOS :: MacOS X",
        "Operating System :: POSIX",
        "Operating System :: POSIX :: AIX",
        "Operating System :: POSIX :: BSD",
        "Operating System :: POSIX :: HP-UX",
        "Operating System :: POSIX :: Linux",
        "Operating System :: POSIX :: SunOS/Solaris",
        "Operating System :: Microsoft :: Windows",
        "Programming Language :: C",
        "Programming Language :: Python",
        "Programming Language :: Python :: 2",
        "Programming Language :: Python :: 2.6",
        "Programming Language :: Python :: 2.7",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.2",
        "Programming Language :: Python :: 3.3",
        # "Programming Language :: Python :: 3.4",
        "Topic :: Security :: Cryptography",
    ],
)
