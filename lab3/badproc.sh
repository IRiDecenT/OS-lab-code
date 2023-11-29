#! /bin/bash 

while echo "I am making files!"
do 
    mkdir adir
    cd adir 
    touch afile
    sleep 10s
done
