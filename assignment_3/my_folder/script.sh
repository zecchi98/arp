#! /bin/bash
gcc -c main.c -I ../ARP_headers/
gcc -c stuff.c
gcc -L../ARP_headers -I../ARP_headers -o my_executable main.o stuff.o -larpnet
./my_executable 





