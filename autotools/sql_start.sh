#!/bin/bash

cd ../build
cmake ..
make 
../bin/main

#while read line
#do
#echo $line
#done < sqls.txt

#cat < sqls.txt
