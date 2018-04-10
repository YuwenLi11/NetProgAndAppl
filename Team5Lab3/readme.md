# Team5 Lab3 Readme

## Purpose and Introduction
Write basic web Client and Server using BSD sockets.
The server can be requested by a browser, so as the client programmed by us.

## Files
This folder contains
* server.c
* client.c
* Makefile

* readme.md
* 1800bytes.txt
* 900bytes.txt

## Compile
Type `make`, it generates two folder `server_folder` and `client_folder`.
Use `make clean` to remove generated files

## Execute
First, in `server_folder` type `./server` to run the HTTP server in terminal 1.
Second, in `client_folder` type `./client`, you are asked to give a file_name, input either `900bytes.txt` or `1800bytes.txt`.  If you give other inputs, server will respond error status.
Third, use a browser by typing URL `localhost:5678/900bytes.txt` or `localhost:5678/1800bytes.txt`, you are able to download the two files.  If you give other inpus, the server will show the error message.

## Test case and expected result
#### Case 1
```
# Terminal 1
$ make
$ cd server_folder
$ ./server

# Terminal 2
$ cd client_folder
$ ./client
Please input file name to request: 900bytes.txt
```

#### Case 2
```
$ make
$ cd server_folder
$ ./server
```
Use browser, type `localhost:5678/900bytes.txt`
