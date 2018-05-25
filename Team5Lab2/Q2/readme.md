# Team5 Lab2-Q2 Readme

## Purpose and Introduction
This program include the sources of the following Connectionless file servers
* Iterative server
* Concurrent Multiprocessing server with one process per request
* Concurrent Multithreading server with one thread per request
* Concurrent Pre-forked Multiprocessing server
* Concurrent Pre-threaded Multithreading server
and two data files size are 900bytes and 1800bytes

The client passes an file name to the server as a command line argument.  The server receives the message, and check the file then sends it if it is exist.

## Files
This folder contains five folders and server files
* Iterative_upd
* MultiProcessing_upd
* MultiThreading_upd
* PreForked_upd
* PreThreaded_upd
(Each folder has own makefile to make `server` and `client` folder)

* readme.md
* 1800bytes.txt
* 900bytes.txt

## Compile
## Compile
In each folder, type `make`, it generates two folder `server` and `client`.
Use `make clean` to remove generated files

## Execute
First, type `./server` to run the TCP server in terminal 1.
(For Pre-allocated server type `./server` plus `an Integer` to indicate the number of children to pre_allocated)
Second, type `./client ` plus `file name` to run the client in terminal 2.

## Test case and expected result
test case1: (none Pre_allocated server)
```
$ ./server       # server's terminal
$ ./client 900bytes.txt   # client's terminal
```

test case2: (Pre_allocated server)
```
$./server 5     # server's terminal
$./client 900bytes.txt   # client's terminal
```
