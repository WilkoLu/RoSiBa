#!/bin/bash

for file in *.c;do
	gcc "$file" -o "${file%.c}.out"
done
