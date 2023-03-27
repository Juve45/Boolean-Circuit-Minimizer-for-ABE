#!/bin/bash

if [ $# -eq 0 ]
then
  echo 'enter script to run: replace/ factorize/ generate'
  exit 1
elif [ $# -gt 1 ]
then
  echo 'too many arguments'
  exit 2
else
  g++ -std=c++17 -Wall -Wextra sources/*.cpp start/$1.cpp -o main && ./main
fi
