#!/usr/bin/env python3
#
import sys

while True:
    with open(sys.argv[1],'r') as fd:
        print(fd.read(), flush=True)

