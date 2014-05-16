#!/usr/bin/perl

# =======================================================
# This script extract the number of trips going from/to a
# given municipality in order from the origin-destination
# matrix produced by VirtualBelgium for a given year
#
# Arguments: - INS code
#            - year of interest
#
# Author : J. Barthelemy
# Version: 26 nov 2013
# =======================================================

# Check the number of arguments

my $num_args = $#ARGV + 1;
if ($num_args != 2) {
  print "\nUsage: od_one_mucipality.pl INS_code year\n";
  exit;
}

# Reading the municipality INS code and year to treat

my $ins  = $ARGV[0];
my $year = $ARGV[1];

print "****************************\n";
print "Extracting data for $ins...\n";
print "****************************\n\n";

# Opening input file and reading header

my $file_in = "../output/origin_destination_array_$year";
print "... Reading the data from $file_in\n";
open( F_IN, "<$file_in" ) or die "Could not open $file_in\n";
my $header = <F_IN>;

# Opening output file a

$fn = $ins."_".$year;
my $file_out = "../output/od_$fn";
open( F_OUT, ">$file_out" ) or die "Could not open output file!\n";
print F_OUT $header;

# Reading input and extracting the data related to the given INS code

my $cur_line;
my $cur_origin;
my $cur_destination;
my $cur_trips;

print "... Extracting data to file $file_out\n";
while( <F_IN> ) {
    $cur_line = $_;
    chomp( $cur_line );
    ( $cur_origin, $cur_destination, $cur_trips ) = split( ",", $cur_line);

    if( ($cur_origin eq $ins ) or ($cur_destination eq $ins) ) {
	print F_OUT "$cur_origin,$cur_destination,$cur_trips\n";
    }
}

print "End!\n";
close( F_IN  );
close( F_OUT );
