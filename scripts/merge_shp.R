#!/usr/bin/Rscript

#####################################################
#                                                   #
# R script generating shapefiles for VirtualBelgium #
#                                                   #
# Requires: R, Rscpit and the rgdal library         #
# Version : 14 jun 2013                             #
# Author  : J. Barthelemy                           #
#                                                   #
#####################################################

# importing the required libraries
library(rgdal)

# reading base shapefile 
shp <- readOGR("../data/network", "belgium_mun")

# sorting it by ins code
sort.shp <- shp[order(shp$ADMUKEY), ]

# reading VirtualBelgium data
data <- read.csv('../output/activity_mun_start_time_2001', header=TRUE, sep=';')
sort.data <- data[order(data$ADMUKEY),]

# merging shp and data
sort.shp@data <- merge(sort.shp@data, sort.data, by="ADMUKEY")

# writing final output
writeOGR(sort.shp,".","../output/activity_start", driver="ESRI Shapefile", overwrite_layer=TRUE)
