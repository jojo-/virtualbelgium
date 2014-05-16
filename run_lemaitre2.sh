#!/bin/bash
##################################################################
# Define the job name
#SBATCH --job-name=VirtualBelgium
#
# Advised: your Email here, for job notification
#SBATCH --mail-user=jojo@math.fundp.ac.be
#SBATCH --mail-type=ALL
#     (ALL = BEGIN, END, FAIL, REQUEUE)
#
# Set a pattern for the output file.
##SBATCH --output=<filename pattern>
#  By default both standard output and  standard  error are 
# directed to a file of the name "slurm-%j.out", where the "%j" 
# is replaced with the job allocation number.   The filename 
# pattern may contain one or more replacement symbols, which are 
# a percent sign "%" followed by a letter (e.g. %j).
#
# Supported replacement symbols are:
#     %j     Job allocation number.
#     %N     Main node name.  
#
##################################################################
# The requested run-time
#
#SBATCH --time=0-20
# Acceptable time formats include "minutes", "minutes:seconds", 
# "hours:minutes:seconds", "days-hours", "days-hours:minutes" 
# and "days-hours:minutes:seconds"
#
# Slurm will kill your job after the requested period.
# The default time limit is the partition’s time limit.
#
# Note that the lower the requested run-time, the higher the
# chances to get scheduled to 'fill in the gaps' between other
# jobs. 
#
##################################################################
# Requested number of cores. Choose either of, or both of
#
#SBATCH --ntasks=100
#SBATCH --cpus-per-task=1
#
# Set a to be the number of process you want to launch and b the
# number of threads per process. Typically, for an MPI job with 8 
# processes, set a=8, b=1. For an OpenMP job with 12 threads, set
# a=1, b=12. For a hybrid 8-process, 12-threads per process MPI 
# job with OpenMP 'inside', set a=8, b=12.
# 
# You can also set 
##SBATCH --ntasks-per-node=c
##SBATCH --exclusive
# to force your jobs to run at most at c at a time on a single 
# node. The --exclusive option reservers the whole node for your
# job. Remove one '#' before them to activate. 
#
##################################################################
# Requested memory for each core
#
#SBATCH --mem-per-cpu=1024 
#
# Set the memory requirements for the job in MB. Your job will be
# allocated exclusive access to that amount of RAM. In the case it
# overuses that amount, Slurm will kill it. The default value is 
# around 2GB per core.
#
# Note that the lower the requested memory, the higher the
# chances to get scheduled to 'fill in the gaps' between other
# jobs. 
#
##################################################################
# Specific features and resources
#
##SBATCH --constraint="feature1&feature2" 
#
#  Constraint the job to nodes which have feature1 and feature 2. 
#  --constraint=L5520 will direct the job to nodes with Intel Xeon
#  L5520 processors, provided the cluster configuration offers it.
#
##SBATCH --gres="resource:2"
#
#  Make sure that 2 ''resources'' are available on the node before
#  allocating a job to the node. For example, --gres=gpu:2 will 
#  reserve 2 gpu's on the node for the job, provided that the cluster
#  configuration offers it. 
#
#  Remove one '#' before them to activate.
#
#         See manpage for sbatch for other options.
#
##################################################################
# Launch job
#
# Note that the environment variables that were set when you
# submitted your job are copied and transmitted to the computing 
# nodes. It nevertheless is good practice to reset them 
# explicitly in the script.
#
# It is also good practice to use the environment variables such
# as $HOME, $TMP, etc rather than explicit paths to ensure smooth
# portability of the script. 
#
#
### Simple sequential job
# If you have a simple non-parallel job, just launch it. 
# So if it is called myprog.exe, just write :

# ./myprog.exe
 
### Parallel job without communications 
# (like job arrays in SGE)
# The following line will launch the number of processes that was 
# requested with the --ntasks option on the nodes that were allocated
# by Slurm.
 
# srun myprog.exe
 
### OpenMP job
# Set the number of treads equal to those requested with the 
# --cpus-per-task option.
 
#export OMP_NUM_THREADS=b
#./myprog.exe
 
### MPI job
# The MPI implementations are aware of the Slurm environment. The
# following line will launch the number of processes that was 
# requested with the --ntasks option on the nodes that were allocated
# by Slurm.
 
#module load repasthpc/1.0.1
module load module load hdf5/1.8.5-patch1

module load repasthpc/2.0

cd ./bin/
#mpirun ./vbel config.props model.props
~/rhpc/ext/MPICH/bin/mpirun ./vbel config.props model.props
 
### Hybrid MPI/OpenMP job
# Mix the above. Note that due to an unsolved misunderstanding between
# Slurm and OpenMPI, the SLURM_CPUS_PER_TASK environment variable must
# be unset. 
 
#export OMP_NUM_THREADS=b
#unset SLURM_CPUS_PER_TASK
#mpirun myprog.exe
 
# end of job
