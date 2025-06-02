#!bin/bash
#testing.sh to test invalid arguments for server

# Testing with no arguments
./server
if [ $? -eq 1 ]
then
    echo "Test passed: caught too few parameters"
fi


# Testing with more than 2 arguments
./server ../maps/main.txt 10 hello
if [ $? -eq 2 ]
then
    echo "Test passed: caught too many parameters"
fi


#Testing server with invalid mapFile
./server ../maps/hello.txt
if [ $? -eq 4 ] 
then
    echo "Test passed: caught invalid mapFile"
fi


#Testing server with invalid seed

./server ../maps/main.txt -1
if [ $? -eq 5 ]
then
    echo "Test passed: caught invalid optional seed"
fi

./server ../maps/main.txt hi
if [ $? -eq 5 ] 
then
    echo "Test passed: caught invalid optional seed"
fi