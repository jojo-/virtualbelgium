/****************************************************************
 * ACTIVITY.CPP
 *
 * This file contains all the definitions of the methods of
 * Activity.hpp (see this file for methods' documentation)
 *
 * Authors: J. Barthelemy and L. Hollaert
 * Date   : 17 july 2012
 ****************************************************************/

#include "../include/Activity.hpp"

using namespace std;

// A null activity constructor
Activity::Activity() {
  _type = ' ';
  _type_num = -1;
  _end_time = 0;
  _nodeId   = 0;
  _distance = -1;
  _duration = -1;
  _dur_trip = -1;
}

// A default constructor
Activity::Activity( char aType ) : _type(aType) {
  
  map<char, int> codebook = Data::getInstance()->getMapActCharToInt();
  this->_type_num = codebook[aType];

  _end_time = 0.0;
  _duration = 0.0;
  _distance = 0.0;
  _nodeId   = 0;
  _dur_trip = 0.0;

}

// This constructor generate an activity of a given type, taking place at a distance
// (derived from a distribution) from nodeId.
Activity::Activity(char aType, long nodeId, bool start, float startTime) : _type(aType) {

  // getting code-book to compute integer coding of the activity
  map<char, int> codebook = Data::getInstance()->getMapActCharToInt();
  this->_type_num = codebook[aType];

  // getting the road network's nodes
  Network net = Data::getInstance()->getNetwork();
  map<long, Node> nodes = net.getNodes();

  // Computation of the activity destination.
  if( start == true ) {

    // ... computation of the distance of the trip.
    dist_param param = Data::getInstance()->getActDistParDist(this->_type_num);
    this->_distance = 0.0;

    while( this->_distance < 1.0 ) {
      this->_distance = RandomGenerators::getInstance()->lognorm_dev.dev(param.mu, param.sigma, param.max);
    }

    // ... computation of the duration of the trip
    dist_param_mixture duration_trip_dist_par = Data::getInstance()->getDurationCondiDistTripParDist(this->_distance);
    this->_dur_trip = RandomGenerators::getInstance()->mixt_lognorm_dev.dev(duration_trip_dist_par.mu, duration_trip_dist_par.sigma, duration_trip_dist_par.p, duration_trip_dist_par.max);
    
    // ... update starting time to take account of trip duration
    startTime = startTime + this->_dur_trip;

    // ... duration of the activity given the starting time
    dist_param_mixture duration_dist_par = Data::getInstance()->getActDurationCondiStartParDist(this->_type_num, startTime);
    this->_duration = RandomGenerators::getInstance()->mixt_lognorm_dev.dev(duration_dist_par.mu, duration_dist_par.sigma, duration_dist_par.p, duration_dist_par.max);

    // ... ending time
    this->_end_time = this->_duration + startTime;

    // ... selection of a destination node id.
    this->_nodeId = net.getDestFromSource(nodeId, this->_distance);

  // Activity takes place at current node: no destination and trip duration.
  } else {

    dist_param_mixture duration_dist_par = Data::getInstance()->getActDurationCondiStartParDist(this->_type_num, startTime);
    this->_duration = RandomGenerators::getInstance()->mixt_lognorm_dev.dev(duration_dist_par.mu, duration_dist_par.sigma, duration_dist_par.p, duration_dist_par.max);
    this->_end_time = this->_duration + startTime;

    // node where the activity is taking place ...
    this->_nodeId = nodeId;

    // this need to be initialized outside the constructor!
    this->_distance = 0.0;
    this->_dur_trip = 0.0;

  }

}

// Constructor of the first activity performed by an Individual
Activity::Activity(long nodeId, int nextActivityType) : _type('m'), _type_num(2), _nodeId(nodeId) {

  // House departure time

  dist_param_mixture tdep_par_dist = Data::getInstance()->getActHouseTDepParDist(nextActivityType);
  this->_end_time = 0.0;
  while( this->_end_time < 1.0 ) { // staying at least one second
    this->_end_time = RandomGenerators::getInstance()->mixt_lognorm_dev.dev(tdep_par_dist.mu, tdep_par_dist.sigma, tdep_par_dist.p, tdep_par_dist.max) * 60.0;
  }
  this->_duration = this->_end_time;

  // Extraction of x and y coordinate.

  this->_distance = 0.0;  // at home, so no distance performed
  this->_dur_trip = 0.0;  // ... and no duration for the trip

}

// Constructor of the last activity performed by an Individual
Activity::Activity(long startNode, long endNode) {

  this->_type     = 'm';     // returning home: character coding
  this->_type_num = 2;       // returning home: integer coding
  this->_end_time = -1;      // last activity of the chain -> no end time
  this->_duration = -1;      // last activity of the chain -> no duration


  // Duration of the trip

  Network net = Data::getInstance()->getNetwork();
  float distance = net.getDistanceNodes(startNode,endNode);   // distance between startNode and endNode
  dist_param_mixture duration_trip_dist_par = Data::getInstance()->getDurationCondiDistTripParDist(distance);
  this->_dur_trip = RandomGenerators::getInstance()->mixt_lognorm_dev.dev(duration_trip_dist_par.mu, duration_trip_dist_par.sigma, duration_trip_dist_par.p, duration_trip_dist_par.max);

  // Extraction of x and y coordinate.

  this->_nodeId   = endNode;  // returning home: home node id
  this->_distance = -1;       // returning home, so the distance traveled does not really matters

}
