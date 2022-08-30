#!/usr/bin/env python

from enum import Enum
import threading
import stat
import sys
import os

class ERROR(Enum):
    NOT_FIFO = 1

class Service:
    def __init__(self, context: str, data: str, out: str) -> None:
        self._has_current_context: bool = False
        self._mutex = threading.Lock()
        self._file_current_context: str = context
        self._file_data: str = data
        self._out: str = out
        self._current_context: str = ""

    def get_has_current_context(self) -> bool:
        return self._has_current_context

    def set_has_current_context(self, flag: bool) -> None:
        self._mutex.acquire()
        self._has_current_context = flag
        self._mutex.release()

    def update_current_context(self) -> None:
        while True:
            print("Wait for context...", file = sys.stderr)
            has_error: bool = False
            try:
                with open(self._file_current_context) as if_current_context:
                    self._current_context: str = if_current_context.readline().strip()
            except:
                has_error = True

            if not has_error:
                self.set_has_current_context(True)
                print("\tReceived context:", self._current_context, file=sys.stderr)

    def handle_data(self) -> None:
        while True:
            if self.get_has_current_context():
                print("Wait for data...", file=sys.stderr)
                has_error: bool = False
                try:
                    with open(self._file_data) as if_data:
                        data: str = if_data.readline().strip()
                except:
                    has_error = True

                if not has_error:
                    print("\tReceived data:",data, file=sys.stderr)

                    print("Do stuff...", file=sys.stderr)
                    result: str = self._current_context + ":" + data
                    print("\tdone", file=sys.stderr)

                    print("Output...", file=sys.stderr)
                    with open(self._out, 'w') as out:
                        out.write(result)

                    print("\tdone", file=sys.stderr)

def is_named_pipe_fifo(filename: str):
    st = os.stat(filename)
    return stat.S_ISFIFO(st.st_mode)

if __name__ == "__main__":

    assert(len(sys.argv) == 4)

    for i in range(1,4):
        if not is_named_pipe_fifo(sys.argv[i]):
            print("ERROR:", sys.argv[i], "is not a named pipe FIFO", file=sys.stderr)
            sys.exit(ERROR.NO_FIFO)

    print("Start server", file=sys.stderr, flush=True)
    server = Service(sys.argv[1], sys.argv[2], sys.argv[3])

    do_current_context = threading.Thread( target = server.update_current_context )
    do_tasks           = threading.Thread( target = server.handle_data )

    do_current_context.start()
    do_tasks.start()

    do_current_context.join()
    do_tasks.join()

    print("End", file=sys.stderr)

