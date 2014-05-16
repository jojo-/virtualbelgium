/*
 * Activity.hpp
 *
 *  Created on: Aug 14, 2012
 *      Author: jojo
 */

/*! \file Activity.hpp
    \brief VirtualBelgium's activity-based model related classes and methods.
 */

#ifndef ACTIVITY_HPP_
#define ACTIVITY_HPP_

#include "Data.hpp"
#include "Random.hpp"
#include "Network.hpp"
#include "repast_hpc/RepastProcess.h"

//! \brief A class representing activities.
/*!
  This class represents an activity performed by an individual. An activity
  is defined by a type, a duration, an end time and a localization. The methods
  implemented allows to generate an activity and determining its characteristics
 */
class Activity {

private:

  char   _type;               //!< activity type (char encoding).
  int    _type_num;           //!< activity type (numerical encoding).
  float  _end_time;           //!< end time of the activity (in seconds).
  float  _duration;           //!< duration of the activity (in seconds).
  float  _distance;           //!< distance to reach the activity (in meters).
  float  _dur_trip;           //!< duration of the trip to reach activity localization (in seconds)
  long   _nodeId;             //!< id of the node where the activity occurs.

public:

  //! Constructor
  /*!
   This constructor generate a default activity, i.e. no type, no end time, localized at (0,0).
   */
  Activity();

  //! Default constructor
  /*!
   This constructor generate an activity of type aType, and randomly initialize the end time and localization.

   /param aType the activity type
   */
  Activity(char aType);

  //! Constructor
  /*!
    This constructor generate an activity of type aType and compute its end time and localization.

    \param aType the desired type of activity.
    \param node the network's node id of the previous activity.
    \param start a boolean indicating whether the node is where the activity is taking place (false) or the
          node is the starting place from where the activity's destination is computed (true).
    \param startTime starting time of the activity (used to compute the end time)
   */
  Activity(char aType, long node, bool start, float startTime);

  //! Constructor of the first activity
  /*!
    This constructor should be used to generate the first activity of an agent, i.e.
    being at home. The house departure time depends on the type of the first activity
    to be performed after leaving the house.

    \param nodeId the node id where the activity take place
    \param nextActivityType type of the first activity performed by the individual after leaving home
  */
  Activity(long nodeId, int nextActivityType);

  //! Constructor of the last activity
  /*!
    Constructor of the last activity performed by an Individual, i.e. returning home.

    \param endNode id node where is last activity takes place
    \param startNode id node left to reach endNode
   */
  Activity(long startNode, long endNode);

  //! Destructor
  virtual ~Activity() {};

  //! Return the end time of the activity.
  /*!
    \return the end time of the activity
   */
  float getEndTime() const {
    return _end_time;
  }

  //! Set the end time of an activity.
  /*!
    \param endTime end time of the activity
   */
  void setEndTime(float endTime) {
    _end_time = endTime;
  }

  //! Return the character type of the activity.
  /*!
    \return type of the activity, character coding
   */
  char getType() const {
    return _type;
  }

  //! Set the character type of the activity.
  /*!
    \param type character type of the activity
   */
  void setType(char type) {
    _type = type;
  }

  //! Return the node id where the activity is performed.
  /*!
    \return a node id
   */
  long getNodeId() const {
    return _nodeId;
  }

  //! Set the node id where the activity is performed.
  /*!
    \param nodeId the node id of the activity
   */
  void setNodeId(long nodeId) {
    _nodeId = nodeId;
  }

  //! Return the activity type (integer coding).
  /*!
    \return the type of the activity, integer coding
   */
  int getTypeNum() const {
    return _type_num;
  }

  //! Set the activity type (integer coding).
  /*!
    \param typeNum an activity type
   */
  void setTypeNum(int typeNum) {
    _type_num = typeNum;
  }

  //! Return the distance performed to reach the activity localization.
  /*!
    \return a distance
   */
  float getDistance() const {
    return _distance;
  }

  //! Set the distance performed to reach the activity localization.
  /*!
    \param distance the distance
   */
  void setDistance(float distance) {
    _distance = distance;
  }

  //! Return the duration of the activity.
  /*!
    \return a duration
   */
  float getDuration() const {
    return _duration;
  }

  //! Set the duration of the trip to reach the activity localization.
  /*!
    \param duration the duration
   */
  void setDuration(float duration) {
    _duration = duration;
  }

  //! Return the duration of the trip performed to reach the activity localization.
  /*!
    \return a trip duration
   */
  float getDurationTrip() const {
    return _dur_trip;
  }

  //! Set the duration of the trip performed to reach the activity localization.
  /*!
    \param durationTrip the duration of the trip
   */
  void setDurationTrip(float durationTrip) {
    _dur_trip = durationTrip;
  }

};

#endif /* ACTIVITY_HPP_ */
