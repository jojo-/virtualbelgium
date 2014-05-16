#!/bin/bash

# ================================================
#
# This script runs VirtualBelgium on the ISCF 
# cluster.
#
# Author:  J. Barthelemy
# Version: 26/11/2012
#
# ================================================

#$ -l vf=1G
#$ -l h_cpu=02:30:00
#$ -l dl360
#$ -cwd
#$ -pe openmpi 50
#$ -N VirtualBelgium
#$ -m bea
#$ -M jojo@math.fundp.ac.be

# ou -l sl 230

module load openmpi
module load boost
module load repasthpc

cd ./bin/
mpiexec -np $NSLOTS ./vbel config.props model.props
