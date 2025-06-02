#!bin/bash
#testing.sh to test invalid arguments for server

# Testing with no arguments
./server

# Testing with more than 2 arguments
./server ../maps/main.txt 10 hello

#Testing server with invalid mapFile
./server ../maps/hello.txt

#Testing server with invalid port
./server ../maps/main.txt -1
./server ../maps/main.txt hi