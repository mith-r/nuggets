#!/bin/bash
# set +e
# Authors: Benjamin Lin, Tarini Gupta, Mithun Rameshkumar, Gustavo Lopez-Flemming
# CS50, Spring 2025
#Testing script for nuggets game - client

#Testing with invalid command line entries

#Testing too few command line entries
./client plank

#Testing with too many command line entries
./client plank 55555 Tarini Hello Bye

#Testing with invalid port number
./client plank badPort Hello

#Testing with valid command line entries

#Testing player 
./client plank 5555 Tarini

#Testing spectator
./client plank 5555