#!/bin/sh

# ===============================================================
# This script merge transport demand forecasting 
# outputs files into a single one
#
# Author : J. Barthelemy
# Version: 12 jul 2013
# ===============================================================

#cat file | tail -n +3 | head -n -1 > trimmed_file
#The tail -n +3 takes everything from the 3rd line down to the end of the file, and head -n -1 takes everything except the last line of the file.
if [ -f ../output/activity_chains_2001.xml ]
then
    cp ../output/activity_chains_2001.xml ../output/activity_chains_2001_OLD
    cp ../output/activity_stat_2001 ../output/activity_stat_2001_OLD
fi

nfiles=$(ls ../output/activity_chains_*_2001.xml -l | wc -l)
last=$(($nfiles-2))
final=$(($nfiles-1))

cat ../output/activity_chains_0_2001.xml | head -n -1 > ../output/activity_chains_2001.xml
rm  ../output/activity_chains_0_2001.xml
for (( i=1; i<=$last; i++ ))
do
    cat ../output/activity_chains_${i}_2001.xml | tail -n +4 | head -n -1 >> ../output/activity_chains_2001.xml
    rm  ../output/activity_chains_${i}_2001.xml
done
cat ../output/activity_chains_${final}_2001.xml | tail -n +4 >> ../output/activity_chains_2001.xml
rm  ../output/activity_chains_${final}_2001.xml

cat ../output/activity_stat_*_2001 > ../output/activity_stat_2001
rm  ../output/activity_stat_*_2001
