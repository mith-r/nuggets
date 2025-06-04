#!/bin/bash
# set +e
# Authors: Benjamin Lin, Tarini Gupta, Mithun Rameshkumar, Gustavo Lopez-Flemming
# CS50, Spring 2025
#Testing script for nuggets game - client

#Testing with invalid command line entries

echo "Testing too few command line entries"
./client plank

echo "Testing with too many command line entries"
./client plank 55555 Tarini Hello Bye

echo "Testing with invalid port number"
./client plank badPort Hello