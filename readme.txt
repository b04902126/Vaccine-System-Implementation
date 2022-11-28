b04902126 柳相宇

Makefile is equipped with a default action of compiling both read_server and write_server (default action - make). Alternatively, you can compile just read_server (make read_server) or just write_server (make write_server). Using the command 'make clean' will remove both read_server and write_server, if they exist.

I placed all source code in 'server.c'.

My implementation does not allow writing preference order such as 1 1 1 or 2 2 2 that is out of the rule. Then the server will shut down and output error message.

Locking is implemented fcntl's flock
I/O Multiplexing is implemented using select()

