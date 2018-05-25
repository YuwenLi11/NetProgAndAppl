# Team5 Lab1-Q3 Readme

## Purpose and Introduction
Write a TCP client program that contacts two TIME servers and reports the times they return as well as the difference between them.

In this question, we create two sockets for two TIME server, and request for the current time.  We found the TIME server lists from this websites: https://tf.nist.gov/tf-cgi/servers.cgi

## Files
This folder contains four files
* Makefile
* readme.md
* tcp_time_client.c
    * This file is the main purpose of this lab

## Compile
In command line, key in `make`.  One executable file `tcp_time_client` will be compiled and generated.
Use `make clean` to remove generated files.

## Execute
Type `./tcp_time_client [T_SERVER_IP_1] [T_SERVER_IP_2]` to send requests to two TIME server T_SERVER_IP_1 and T_SERVER_IP_2.

## Test case
We pick two arbitrary servers for this test case.
* 129.6.15.28
* 132.163.96.4

```
$ ./tcp_time_client 129.6.15.28 132.163.96.4
Send time request to 129.6.15.28...
TCP socket created
Connection established
Send time request to 132.163.96.4...
TCP socket created
Connection established
Receive from server 129.6.15.28: Mon Mar  5 20:41:30 2018
Receive from server 132.163.96.4: Mon Mar  5 20:41:31 2018
The difference between two servers are 1 seconds
```
