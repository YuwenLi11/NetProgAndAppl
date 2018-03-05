# Team5 Lab1-Q2 Readme

## Purpose
Write a UDP client program for DAYTIME service.

## Files
This folder contains four files
* Makefile
* readme.md
* udp_client.c
    * This file is the main purpose of this lab
* udp_server.c
    * For test, this file enable client to request DAYTIME.

## Compile
In command line, key in `$ make`.  Two executable file `udp_server` and `udp_client` will be compiled and generated.
Use `$ make clean` to remove generated files

## Execute
First, type `./udp_server` to run the udp server in terminal 1.
Second, type `./udp_client` to run the client in terminal 2.  Here the client send the UDP packet to the server, and the server responds DAYTIME to the client.

## Test case
Run `udp_server` and `udp_client` in different terminal windows.


```
$ ./udp_server  # terminal 1
UDP socket created
Bind successful, listen to port 5678
```

```
$ ./udp_client  # terminal 2
UDP socket created
Sent reqeust to server
Receive DAYTIME from server: "Mon Mar  6 01:23:45 2018"
```
