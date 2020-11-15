# distributed_systems_homework

This is a homework from the subject Distributed systems. It contains multithreaded and multiprocess server and client in C.

## How to run it

First compile desired server type
```
gcc -o forkServer  TCPServerFork.c
gcc -o threadServer  TCPServerThread.c -lpthread
```
Then run it with
```
./forkServer
./threadServer
```
Then you need to change directory to client and compile client

```
gcc -o client  TCPClient.c
```
And run it like this
```
./client *filename*
```
If you want to test the multitreaded or multiprocess functionality you need to open a new terminal in the client directory and run command
```
./client *filename2*
```
For the test you can use two .txt files in the client directory


