# Team5 Lab1-Q4 Readme

## Purpose and Introduction
This program include the sources of the following Connection-oriented file servers
* Iterative server
* Concurrent Multiprocessing server with one process per request 
* Concurrent Multithreading server with one thread per request 
* Concurrent Pre-forked Multiprocessing server
* Concurrent Pre-threaded Multithreading server
and two data files size are 900bytes and 1800bytes

The client passes an file name to the server as a command line argument.  The server receives the message, and check the file then sends it if it is exist.

## Files
This folder contains four files
* Makefile
* readme.md
* server_Iterative_tcp.c
* server_MultiProcessing_tcp.c	
* server_MultiThreading_tcp.c
* server_PreForked_tcp.c
* server_PreThreaded_tcp.c
* 1800bytes.txt
* 900bytes.txt

## Compile
Please make sure that the server source file and client source file are in the same directory

In command line, key in `make`.  Two executable file `server` and `client` will be compiled and generated in the different folder: 'client_folder', 'server_folder'. 

Please move the test data files into 'server_folder'

Use `make clean` to remove generated files

## Execute
First, type `./server` to run the TCP server in terminal 1.
(For Pre-allocated server type './server' plus an Integer to indicate the number of chilren to pre_allocated)
Second, type `./client ` plus 'file name' to run the client in terminal 2.

## Test case and expected result
test case1: (none Pre_allocated server)
```
$ ./server       # server's terminal
$ ./client 900bytes.txt   # client's terminal
```

test case2: (Pre_allocated server)
```
./server 5     # server's terminal
./client 900bytes.txt   # client's terminal
```
