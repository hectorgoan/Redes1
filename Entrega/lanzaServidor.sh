#! /bin/bash

./servidor
./cliente olivo TCP ordenes.txt &
./cliente olivo TCP ordenes1.txt &
./cliente olivo TCP ordenes2.txt &
./cliente olivo UDP ordenes.txt &
./cliente olivo UDP ordenes1.txt &
./cliente olivo UDP ordenes2.txt &
