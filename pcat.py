#!/usr/bin/env python

import sys

if __name__ == "__main__":

    while True:
        with open(sys.argv[1]) as fin:
            line = fin.readline()
        sys.stdout.write(line)

