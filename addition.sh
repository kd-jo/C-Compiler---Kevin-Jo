#!/bin/bash
echo $USER
rm quad.q
bin/pcc addition.c
echo
echo "TEST SA3 $1"
echo
echo "Start Interpreter"
echo "================="
echo
bin/inter quad.q
echo $USER
