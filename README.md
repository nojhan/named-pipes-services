Named pipes services
====================

Examples of how to design C++ services that use Linux' named pipes FIFO as I/O.

Rationale
---------

The basic idea is that, instead of programming the network interface
to your service with low level sockets or any high level library,
you can just implement query/answer mechanisms using named pipes.

Named pipes are special FIFO files that are blocking on I/O
and implements a very basic form of message passing,
without having to bother with polling.
Moreover, they are very easy to use, are they are just files
in which you read/write.

Once you made your service on top of named pipes,
it is easy to wrap it within an interface made with other languages/tools.
For instance, it is very easy to expose it on the network using common Linux tools like `socat`.


Build and run
-------------

```sh
./build.sh
./run.sh
```

Examples
--------

Two examples are provided.

### Simple service

The first example `./service in out` implements a service
that reads from a named pipe `in` and writes to another one `out`.
To create the named pipes, use the `mkfifo` command.

The service is just a simple loop that iterates over blocking I/O calls
on the named pipes, thus having zero CPU cost for polling.

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

Note that you can use the same pipe for input and output: `./service data data`.


### Service with two dependent inputs

The second example `./service2 context data out` shows a service
which depends on an initialization phase that set up a "context",
after which it is possible to consume some "data".

The service use two threads, one to poll the context and one to poll the data
and do something with it.

The script `run.sh` shows how to test it.
Run it and it should show `Context: data` as a last line.


Furthermore
-----------

If you want to expose such a service as a network server, just use socat.

For example, to get _data_ query from the network for `service2`:
```sh
socat -v -u TCP-LISTEN:8423,reuseaddr PIPE:./data
```

You can test it by sending something on the connection:
```sh
echo "data" > /dev/tcp/127.0.0.1/8423
```

Conversely, to send automatically back the answer to some server:
```sh
socat -v -u PIPE:./out TCP2:8424:host
```

Be aware that `socat` will terminate as soon as it receives the end of the message.
Thus, if you want to establish a permanent gate, you will have to automatically restart it:
```sh
while true; do socat TCP-LISTEN:8478,reuseaddr PIPE:/./data || break; done
```


Author & license
----------------

Author: nojhan@nojhan.net

License: AGPLv3

