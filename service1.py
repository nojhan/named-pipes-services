#!/usr/bin/env python

import sys

if __name__ == "__main__":
    print("Start server")

    while True:
        with open(sys.argv[1]) as fin:
            datas = fin.readline()

        data = datas.strip()
        print("Received: <",data,">", file=sys.stderr)

        with open(sys.argv[2], 'w') as fout:
            fout.write(data)

        if data == "exit":
            break

    print("Stop server", file=sys.stderr)
