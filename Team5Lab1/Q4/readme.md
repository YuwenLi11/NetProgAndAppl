# Team5 Lab1-Q4 Readme

## Purpose and Introduction
This program include the sources of TCP client and server
The client passes an integer to the server as a command line argument. The server receives the message, takes the square root of the integer, and sends the result back to the client. The client then receives the value and prints out both numbers.
The sever can handdle errors such as the integer is too big, the input is not a number, the input is decimal or the input is less than zero

## Files
This folder contains four files
* Makefile
* readme.md
* client.c
* server.c

## Compile
In command line, key in `make`.  Two executable file `server` and `client` will be compiled and generated.
Use `make clean` to remove generated files

## Execute
First, type `./server` to run the UDP server in terminal 1.
Second, type `./client ` plus an integer to run the client in terminal 2.

## Test case and epcept result
test case1: (Result: correct output in client's terminal)
./server
./client 100


test case2: (Result: 'Message form server: please input a number bigger than zero' in client's terminal)
./server
./client -12

test case3: (Result: 'Message form server: please input a number bigger than zero' in client's terminal)
./server
./client 0

test case4: (Result: 'Message form server: please input a number' in client's terminal)
./server
./client ab

test case5: (Result: 'Please enter one number' in client's terminal)
./server
./client

test case6: (Result: correct output in client's terminal)
./server
./client 1.23

