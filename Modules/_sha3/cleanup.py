#!/usr/bin/env python
# Copyright (C) 2012   Christian Heimes (christian@python.org)
# Licensed to PSF under a Contributor Agreement.
#
# cleanup Keccak sources

import os
import re

CPP1 = re.compile("^//(.*)")
CPP2 = re.compile("\ //(.*)")

HERE = os.path.dirname(os.path.abspath(__file__))
KECCAK = os.path.join(HERE, "keccak")

def getfiles():
    for name in os.listdir(KECCAK):
        name = os.path.join(KECCAK, name)
        if os.path.isfile(name):
            yield name

def cleanup(f):
    buf = []
    for line in f:
        if line.startswith(("void ", "int ", "HashReturn ", "const UINT64 ")):
            buf.append("static " + line)
            continue
        line = CPP1.sub(r"/* \1 */", line)
        line = CPP2.sub(r" /* \1 */", line)
        buf.append(line)
    return "".join(buf)

for name in getfiles():
    with open(name) as f:
        res = cleanup(f)
    with open(name, "w") as f:
        f.write(res)

