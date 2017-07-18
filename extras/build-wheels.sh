#!/bin/bash
set -ex

# based on https://github.com/pypa/python-manylinux-demo

DIST=/io/dist
WHEELOUT=${DIST}/wheels-$(uname -m)

for PYBIN in /opt/python/cp{27*,3[4-9]*}/bin; do
    "${PYBIN}/pip" wheel /io/ -w ${WHEELOUT}
done

for whl in ${WHEELOUT}/*.whl; do
    auditwheel repair "$whl" -w ${DIST}
done

for PYBIN in /opt/python/cp{27*,3[4-9]*}/bin; do
    "${PYBIN}/pip" install pysha3 --no-index -f ${DIST}
    (cd /io; "${PYBIN}/python" tests.py)
done

chown -R ${BUILD_UID}:${BUILD_GID} /io
