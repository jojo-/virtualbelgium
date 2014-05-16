#!/bin/bash

ulimit -s 100000

cd ./bin/
mpirun -np $1 ./vbel config.props model.props
cd ..
