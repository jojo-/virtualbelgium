/****************************************************************
 * DATA.HPP
 * 
 * This file contains all the data related class and methods.
 * 
 * Authors: J. Barthelemy and L. Hollaert
 * Date   : 17 july 2012
 ****************************************************************/

/*! \file Data.hpp
 *  \brief Data related class and methods.
 */

#ifndef DATA_HPP_
#define DATA_HPP_

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <limits>
#include <string>
#include <stdexcept>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <math.h>
#include "repast_hpc/Properties.h"
#include "repast_hpc/RepastProcess.h"
#include "Network.hpp"
#include "tinyxml2.hpp"
#include "Random.hpp"
#include "repast_hpc/TDataSource.h"
#include "repast_hpc/SVDataSet.h"


const int MODEL_AGENT_IND_TYPE = 0;     //!< constant for the individual agent type
const int MODEL_AGENT_HH_TYPE  = 1;     //!< constant for the household agent type


//! \brief Singleton class for the Data class.
template <typename T>
class Singleton {

protected:

  //! Constructor.
  Singleton () {}

  //! Destructor.
  ~Singleton () {}

public:

  //! Generates a singleton instance of a T object.
  /*!
    \param aProps model properties of VirtualBelgium
   */
  static void makeInstance ( repast::Properties aProps ) {
    if (NULL == _singleton)  {
      _singleton = new T(aProps);
    }
  }

  //! Return the generated, unique, instance of a T object (if already instanciated).
  /*!
    \return the unique instance of a T object
   */
  static T *getInstance () {
    if (NULL == _singleton)    {
      std::cerr << "No instance already created!" << std::endl;
    }
    return (static_cast<T*> (_singleton));
  }

  //! Free the memory.
  static void kill () {
    if (NULL != _singleton) {
      delete _singleton;
      _singleton = NULL;
    }
  }

private:

  static T *_singleton;    //!< unique instance of the Data class

};

//! Initialize the singleton to NULL.
template <typename T> T *Singleton<T>::_singleton = NULL;

//! \brief A data class.
/*!
 A data class reading and producing all the inputs required by
 the various models of Virtual Belgium. This class is implemented
 using a singleton design pattern.
 */
class Data : public Singleton<Data> {

  friend class Singleton<Data>;

private:

  std::map<int, std::vector<long> >    _mun_age_men;              //!< men's age distribution by municipality
  std::map<int, std::vector<long> >    _mun_age_women;            //!< women's age distribution by municipality
  std::map<int, float>                 _death_age_men;            //!< death's probability for a man by age
  std::map<int, float>                 _death_age_women;          //!< death's probability for a woman by age
  std::map<int, float>	               _birth_age;		            //!< birth's probability by women's age
  std::map<int, float>		             _birth_men;	              //!< birth's probability to have a boy
  std::multimap<int, long>             _map_ins_node;             //!< set of node by municipalities' ins code
  std::map<long,int>                   _map_node_ins;             //!< ins code of node
  std::map<int,char>                   _map_act_intToChar;        //!< activities' code-book (from integer to character encoding)
  std::map<char,int>                   _map_act_charToInt;        //!< activities' code-book (from character to integer encoding)
  std::map<int,dist_param>             _map_act_dist_par_dist;    //!< distribution parameters for activities' distance (log normal)
  std::map<int,dist_param_mixture>     _map_act_tdep_par_dist;    //!< distribution parameters for activities' house departure time (log normal)
  std::map<int, dist_param_mixture_2d> _map_act_start_x_dur;      //!< distribution parameters for activities' starting time x log(duration)
  dist_param_mixture_2d                _act_dist_x_dur_trip_dist; //!< distribution parameters for log(distance) x log(duration of the trip)
  Network                              _network;                  //!< road network
  std::map<int, long>                  _indic_mun_size;           //!< size indicator of a municipality
  std::map<int, int>                   _map_ins_id_mun;           //!< map of ins code (key) x id of municipality (value)
  std::map<int, int>                   _map_id_mun_ins;           //!< map of id of municipality (key) x ins code (value)
  repast::Properties                   _props;                    //!< properties of simulation

public:

  //! Constructor.
  /*!    
   The constructor initialize all the private members of a Data object.

   \param aProps the properties of VirtualBelgium, including paths to data files
   */
  Data( repast::Properties aProps ) {

    if (repast::RepastProcess::instance()->rank() == 0) {
      std::cout << "Data reading" << std::endl;
    }

    // Models properties

    this->_props = aProps;

    // Socio-demographics data

    read_mun_age_men();
    read_mun_age_women();
    read_death_age();
    read_birth_age();

    // Network data

    read_node_ins();
    read_network();
    read_indicators();
    read_ins_id_mun();

    // Activity model data

    read_activity_cdb();
    read_distribution_parameters_distance();
    read_distribution_parameters_start_duration();
    read_distribution_parameters_distance_x_duration_trip();
    read_distribution_parameters_house_tdep();


  }

  //! Destructor.
  virtual ~Data() {};

  //! Read the age's distribution by municipality for the men.
  void read_mun_age_men();

  //! Read the age's distribution by municipality for the women.
  void read_mun_age_women();

  //! Read the death probability by age and gender.
  void read_death_age();

  //! Read the birth probability by age and the probability to have a boy or a girl.
  void read_birth_age();

  //! Read the municipalities's node.
  void read_node_ins();

  //! Read the activities' codebook.
  void read_activity_cdb();

  //! Read the road network.
  void read_network();

  //! Read the distribution parameters for activities' distance.
  void read_distribution_parameters_distance();

  //! Read the distribution parameters for activities' house time departure.
  void read_distribution_parameters_house_tdep();

  //! Read the distribution parameters for activities' starting time x duration.
  void read_distribution_parameters_start_duration();

  //! Read the distribution parameters for distance x duration of a trip.
  void read_distribution_parameters_distance_x_duration_trip();

  //! Read the various indicators used by the activity localization model.
  void read_indicators();

  //! Read the codebooks of ins code and municipality id (1 to 589).
  void read_ins_id_mun();

  //! Get the age's distribution of a municipality for a given gender.
  /*!
   \param municipality the INS code of a municipality
   \param gender       the gender of an individual

   \return a vector of age's distribution
   */
  std::vector<long int> getMunAge(int municipality, char gender);

  //! Compute and return the death's probability of an individual.
  /*!
   \param gender the gender of the individual
   \param age    the age of the individual

   \return a probability of dying
   */
  float getDeathProba(char gender, int age);

  //! Compute and return the birth's probability of an individual by women's age.
  /*!
   \param age the age of the women

   \return a probability of having a child
   */
  float getBirthProba(int age);

  //! Compute and return the probability to be a boy or girl.
  /*!
   \param age the age of the mother

   \return a probability to be a boy
   */
  float getBirthsex(int age);

  //! Return the list of node of a given municipality identified by its INS code.
  /*!
   \param aIns the INS code of the municipality of interest

   \return a vector containing the node's id of the municipality
   */
  std::vector<long> getNodesIdFromIns(int aIns);

  //! Return one node of a given municipality identified by its INS code.
  /*!
    \param aIns the INS code of the municipality of interest

    \return the node's id of the municipality
   */
  long getOneNodeIdFromIns(int aIns);

  //! Return the distance's distribution's parameter of a given activity type.
  /*!
   \param aActivityType the type of an activity (integer coding)

   \return a mixture of univariate log-normal distributions
   */
  dist_param getActDistParDist(int aActivityType );



  //! Return the house departure time distribution's parameter for a given activity type.
  /*!
   \param aActivityType the type of an activity (integer coding)

   \return a mixture of univariate log-normal distributions
  */
  dist_param_mixture getActHouseTDepParDist(int aActivityType );


  //! Return the activity duration distribution's parameter conditional to a starting time for a given activity type.
  /*!
   \param aActivityType type of an activity, integer coding
   \param aStartTime starting time of an activity

   \return a mixture of univariate log-normal distributions
  */
  dist_param_mixture getActDurationCondiStartParDist(int aActivityType, int aStartTime);

  //! Return a journey duration distribution's parameters conditional to the journey distance.
  /*!
   \param aDistance the distance performed

   \return a mixture of univariate log-normal distributions
   */
  dist_param_mixture getDurationCondiDistTripParDist(int aDistance);

  //! Return the road network.
  /*!
   \return the road network
   */
  const Network & getNetwork() const {
    return _network;
  }

  //! Return the character to integer activity code-book.
  /*!
   \return character to integer activity code-book
   */
  const std::map<char, int>& getMapActCharToInt() const {
    return _map_act_charToInt;
  }

  //! Return the integer to character activity code-book.
  /*!
   \return integer to character activity code-book.
   */
  const std::map<int, char>& getMapActIntToChar() const {
    return _map_act_intToChar;
  }

  //! Return the codebook of municipalities ins code to their id (1 to 589).
  /*!
   \return the codebook of municipalities ins code to their id (1 to 589)
   */
  const std::map<int, int>& getMapInsIdMun() const {
    return _map_ins_id_mun;
  }

  //! Return the codebook of municipalities id (1 to 589) to their ins code.
  /*!
   \return the codebook of municipalities id (1 to 589) to their ins code
   */
  const std::map<int, int>& getMapIdMunIns() const {
    return _map_id_mun_ins;
  }

};


//! \brief Aggregate output data class.
/*!
  This class is responsible for gathering aggregate data from the simulation.
 */
class AggregateSum : public repast::TDataSource<int> {

private :
  int _sum;   //!< the aggregate sum computed over all processes used to run the simulation

public :

  //! Constructor.
  AggregateSum();

  //! Destructor.
  virtual ~AggregateSum() {};

  //! Increment sum.
  void increment();

  //! Reset sum to 0.
  void reset();

  //! Return the current state of sum.
  /*!
    \return the value of sum
   */
  int getData();

};


/////////////////////////////
//  Some useful routines.  //
/////////////////////////////

//! Convert a number of seconds to the format hour:min:sec.
/*!
 \param n_sec the number of seconds to convert

 \return a string in the hour:min:sec format
 */
inline std::string secToTime( float n_sec ) {

  unsigned long n_sec_int = (unsigned long) floor(n_sec);

  std::ostringstream result;

  unsigned int hour = n_sec_int / 3600;
  unsigned int min  = (n_sec_int / 60) % 60;
  unsigned int sec  = n_sec_int % 60;

  result << hour << ":" << min << ":" << sec;

  return result.str();

};

//! Convert a number of seconds to hours.
/*!
 \param n_sec the number of seconds to convert

 \return the hours
 */
inline unsigned int secToHour( float n_sec ) {
  return (unsigned long)floor(n_sec / 3600);
};

//! Convert a number of seconds to half-hours.
/*!
 \param n_sec the number of seconds to convert

 \return the number of half hours since midnight
 */
inline unsigned int secToHalfHour( float n_sec ) {
  return (unsigned long)floor(n_sec / 1800);
};

//! Compute the number of lines in a file.
/*!
 \param filename the path to the file to analyze

 \return the number of lines in the file
 */
long int linesCount(std::string filename);

//! Decompose a string according to a separator into a vector of type T.
/*! 
 \param msg the string to decompose
 \param separators separators used for the decomposition

 \return a vector of type T
 */
template<typename T> std::vector<T> split(const std::string & msg,
                                     const std::string & separators) {

  std::vector<T> result;                                         // resulting vector
  T         token;                                               // one token of the string
  boost::char_separator<char> sep(separators.c_str());           // separator

  boost::tokenizer<boost::char_separator<char> > tok(msg, sep);  // token's generation

  // string decomposition into token
  for (boost::tokenizer<boost::char_separator<char> >::const_iterator i = tok.begin(); i != tok.end(); i++) {
    std::stringstream s(*i);
    token = boost::lexical_cast<T>(s.str());
    result.push_back(token);
  }

  // returning the resulting decomposition
  return result;
}

#endif /* DATA_HPP_ */
