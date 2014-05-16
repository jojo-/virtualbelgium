#!/bin/sh

cp ../output/$1 ../tools/od_maps/flows_bel.csv

cd ../tools/od_maps/
java -Xms10000m -jar jflowmap.jar &
