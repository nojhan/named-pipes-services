Named pipes services
====================

Examples (in C++ and Python) of how to design services that use named pipes FIFO as I/O.

Instead of implementing heavy web services or complex low-level network code,
just read/write from/to files, and be done with it.


Introduction
------------

### Rationale

The problem of making two programs *communicate* is among the ones
that generated the largest litterature and code in all computer science
(along with cache invalidation, naming things, and web frameworks).

When facing such a problem, a programer immediatly thinks "I'll use middleware".
If you don't really now what a middleware is, be at ease, nobody really knows.
Nowadays, you may have eared of their latest avatar: *web services*.
As our programmer is going to realize, one now have *two* problems.
The burden of writing, using, and maintaining code using middleware is always huge.
Because they are made to handle a **tremendous** number of features and complex situations
—most of which involve adversary users, users being bots, or both.

But most of the time, the actual problem does not really involve these situations.
At least not at the beginning (… which means probably never).
People familiar with middleware history would argue that their key feature
is not just *messages passing* but *remote call*, which involves *object serialization*.
But most of the time, the messages are pretty simple anyway, and using a middleware
to implement a serialization of a list of instances having three members of fundamental types
is not a good use of your time.

If you are building up (firsts versions of) communicating programs
that will run on a (safe) local network,
and for which the exchanged messages are known and simple,
then I have good news:
**you don't have to use web services** (or any kind of middleware).

**YOU JUST NEED TO KNOW HOW TO READ/WRITE FROM/TO (SPECIAL) FILES**.


### Overview

The basic idea is that, instead of programming the network interface
to your service with low level sockets or any high level library,
you can just implement query/answer mechanisms using **named pipes**.

Named pipes are special *FIFO* files that are blocking on I/O
and implements a very basic form of message passing,
without having to bother with polling.
Moreover, they are very easy to use, as they are just files
in which you read/write.

Once you made your service on top of named pipes,
it is easy to wrap it within an interface made with other languages/tools.
For instance, it is very easy to expose it on the network using common tools like `socat`.

Be warned that this is not secure, though, you should only use this for testing
purpose in a secured local network.


### Principle

The theoretical principle can be represented by this UML sequence diagram:
```
          Named pipes
         ┌─────┴─────┐
┌──────┐ ┌───┐   ┌───┐ ┌───────┐
│Client│ │OUT│   │IN │ │Service│
└───┬──┘ └─┬─┘   └─┬─┘ └───┬───┘
    │      │       │       │
    │      │       │┌──────╢
    │      │  block││ wait ║
    │ask   │       │└─────→║
    ├─────────────→│       │
    ╟─────┐│       ├──────→│
    ║wait ││block  │       ║process
    ║←────┘│       │       ║
    │      │←──────────────┤
    │←─────┤       │   tell│
    │      │       │       │
```

Notes:
- the service is started first and is waiting for the input,
  but as processes are blocking, the starting order does not always matter.
- there are two pipes, here (one for the input and one for the output),
  for the sake of simplicity, but you may just as well use only one.


Build and run
-------------

Python code does not need to be built.

To build the C++ code on Linux, just call:
```sh
./build.sh
```

You may use the `run_*` scripts to see how to run the examples.
For instance, for the most complex one:
```
./run_service2.sh
```


Examples
--------

To create the named pipes under Linux or MacOS, use the `mkfifo` command, as shown in the `build.sh`
script.

Creating named pipes on windows is more complex, you may want to look at the
[related Stack Overflow question](https://stackoverflow.com/questions/3670039/is-it-possible-to-open-a-named-pipe-with-command-line-in-windows)


### Trivial example: a `cat` service

The `pcat` executable implements a service that reads from a named pipe and
print its content on the standard output.
It's just like a `cat` command, but that would not end after the first read, but
will continue reading from the pipe instead.

This kind of service is just a simple loop that iterates over blocking I/O calls
on the named pipes, thus having zero CPU cost for polling.

The file `run_pcat.sh` shows how to run the example.

Note: if this example prints "Hello World!" multiple times, that's because you
did not created the `data` file as a named pipe, but as a regular file. Hence,
instead of emptying its content after reading, it keeps reading the same
content.

The `pcat.py` is the same example, but in Python instead of C++.


### Simple service

The first example `./service in out` implements a service
that reads from a named pipe `in` and writes to another one `out`.

Once launched, the service will wait for the pipes to be consummed,
for instance with two commands.
The first one writes input in the input pipe:
```sh
echo "data" > in
```
The second one reads the result:
```sh
cat out
```

The file `run_service1.sh` shows how to run this example.

Note that you can use the same pipe for input and output: `./service1 data data`.


### Service with two dependent inputs

The second example `./service2 context data out` shows a service
which depends on an initialization phase that set up a "context",
after which it is possible to consume some "data".

The service use two threads, one to poll the context and one to poll the data
and do something with it.

The script `run_service2.sh` shows how to test it.
Run it and it should show `Context: data` as a last line.

Use `Ctrl-C` to close the remaining `cat` process.


Furthermore
-----------

### Expose such services on network

If you want to expose such a service as a network server, just use socat.

For example, to get _data_ query from the network for `service1`:
```sh
socat -v -u TCP-LISTEN:8423,reuseaddr PIPE:./data
```
(see `run_socat_server.sh` for a complete example).

You can test it by sending something on the connection:
```sh
echo "Hello World!" > /dev/tcp/127.0.0.1/8423
```

Conversely, to send automatically back the answer to some server:
```sh
socat -v -u PIPE:./out TCP2:8424:host
```

Be aware that `socat` will terminate as soon as it receives the end of the message.
Thus, if you want to establish a permanent gate, you will have to use the `fork`
option:
```sh
socat TCP-LISTEN:8478,reuseaddr,fork PIPE:/./data
```


### Troubleshooting

If you witness strange behavior while debugging your own services (like prints
that do not occur in the correct terminal), double check that yo don't have some
remaining detached processes that would not have been killed.

For instance, if:
```sh
ps aux | grep pcat
```
returns anything, you would need to `killall pcat`, or else several concurent
processes would read the same pipe, which leads to (almost) undefined behavior.


Author & license
----------------

Author: nojhan@nojhan.net

License: AGPLv3

