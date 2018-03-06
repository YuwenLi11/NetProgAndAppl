This program include the sources of TCP client and server
The client passes an integer to the server as a command line argument. The server receives the message, takes the square root of the integer, and sends the result back to the client. The client then receives the value and prints out both numbers.
The sever can handdle errors such as the integer is too big, the input is not a number, the input is decimal or the input is less than zero

please input “make” to compile the source files

test case1: (Result: correct output)
./server
./client 100


test case2: (Result: Message form server: please input a number bigger than zero)
./server
./client -12

test case3: (Result: Message form server: please input a number bigger than zero)
./server
./client 0

test case4: (Result:Message form server: please input a number)
./server
./client ab

test case5: (Result:Please enter one number)
./server
./client

test case6: (Result: correct output)
./server
./client 1.23

