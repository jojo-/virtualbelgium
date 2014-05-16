/****************************************************************
 * INDIVIDUAL.HPP
 * 
 * This file contains all the Individuals related classes and 
 * functions.
 * 
 * Authors: J. Barthelemy and L. Hollaert
 * Date   : 17 july 2012
 ****************************************************************/

/*! \file Individual.hpp
 *  \brief Individual agents related classes and methods declarations.
 */


#ifndef INDIVIDUAL_HPP_
#define INDIVIDUAL_HPP_

#include "repast_hpc/TDataSource.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/Schedule.h"
#include "repast_hpc/Random.h"
#include "repast_hpc/Properties.h"
#include "repast_hpc/Utilities.h"
#include "repast_hpc/io.h"
#include "repast_hpc/logger.h"
#include "repast_hpc/initialize_random.h"

#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <stdexcept>
#include <math.h>
#include <vector>
#include <boost/serialization/access.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>

#include "Data.hpp"
#include "Random.hpp"
#include "Activity.hpp"

//! \brief The package structure for Individual agents.
/*!
  A structure used for passing individual agents from one process to another.
 */
struct IndividualPackage {

  friend class boost::serialization::access ;

  //! Serializing procedure of the package.
  /*!
    \param ar an Archive in which the individual will be stored
    \param version version of the package
   */
  template <class Archive>
  void serialize ( Archive & ar , const unsigned int version ) {
    ar & id;
    ar & hh_id;
    ar & municipality;
    ar & gender;
    ar & age_class;
    ar & age;
    ar & education;
    ar & hh_relationship;
    ar & sps_status;
    ar & driving_license;
    ar & act_chain;
    ar & house;
    ar & proc;
    ar & agent_type;
  }

  int              id;                //!< id of the individual
  repast::AgentId  hh_id;             //!< Repast agent id of the individual
  int              municipality;      //!< ins code of the individual's municipality
  char             gender;            //!< gender of the individual
  int              age_class;         //!< age class of the individual
  int              age;               //!< age of the individual
  char             education;         //!< education level of the individual
  char             hh_relationship;   //!< household relationship status of the individual
  char             sps_status;        //!< socio-professional status of the individual
  char             driving_license;   //!< driving license ownership of the individual
  std::vector<Activity> act_chain;    //!< activity chain of the individual
  long             house;             //!< house of the individual, i.e. a node id
  int              proc;              //!< initial individual process
  int              agent_type;        //!< individual agent_type

  //! Return individual's agent id.
  /*!
    \return an individual agent id
   */
  repast::AgentId getId () const {
    return repast::AgentId( id, proc, agent_type );
  }

};

//! The individual agent class
/*!
  This class implements the individual agents in VirtualBelgium.
  Each individual is characterized by
  - an individual id of the type repast::AgentId;
  - an household of the type repast::AgentId;
  - a gender;
  - an age class;
  - an education level;
  - a socio-professional status;
  - a driving license ownership;
  - a house;
  - an activity chain.
*/
class Individual : public repast::Agent {

  // Repast need this line
  friend class boost::serialization::access;

private :

  repast::AgentId _id;                    //!< Individual's Repast::AgentId.
  repast::AgentId _hh_id;                 //!< Household's Repast::AgentId.
  int  _municipality;                     //!< Municipality of the individual.
  char _gender;                           //!< Gender of the individual (Male, Female).
  int  _age_class;                        //!< Age class of the individual.
  int  _age;                              //!< Actual age of the individual.
  char _education;                        //!< Education level (No diploma, Primary school, High School, Master degree).
  char _sps_status;                       //!< Socio-professional status.
  char _driving_license;                  //!< Driving license ownership.
  char _hh_relationship;                  //!< Household status relationship.
  long _house;                            //!< Network's node's id of the individual's house.
  std::vector<Activity> _act_chain;       //!< Activity chain of the individual.

public :

  //! Constructor (initialize every attributes but house, sps status, age and activity chain).
  /*!
    \param id a Repast agent id
    \param hh_id the individual's household Repast agent id
    \param municipality the ins code of the agent's municipality
    \param gender the gender of the individual
    \param age_class the age class of the individual
    \param education an education level
    \param hh_relationship an household relationship status
   */
  Individual( repast::AgentId id , repast :: AgentId hh_id, int municipality,
	      char gender, int age_class, char education, char hh_relationship );

  //! Constructor (initialize every attributes but house, sps status, household relationship status and activity chain).
  /*!
    \param id a Repast agent id
    \param hh_id the individual's household Repast agent id
    \param municipality the ins code of the agent's municipality
    \param gender the gender of the individual
    \param age_class the age class of the individual
    \param age the individual's age
    \param education an education level
   */
  Individual( repast::AgentId id , repast :: AgentId hh_id, int municipality,
	       char gender, int age_class, int age, char education );

  //! Constructor (initialize every attributes but age, sps status and activity chain).
  /*!
    \param id a Repast agent id
    \param hh_id the individual's household Repast agent id
    \param municipality the ins code of the agent's municipality
    \param gender the gender of the individual
    \param age_class the age class of the individual
    \param education an education level
    \param hh_relationship an household relationship status
    \param house a road network node id
   */
  Individual( repast::AgentId id, repast::AgentId hh_id, int municipality,
           char gender, int age_class, char education, char hh_relationship,
           long house );

  //! Constructor (initialize every attributes but sps status and activity chain).
  /*!
    \param id a Repast agent id
    \param hh_id the individual's household Repast agent id
    \param municipality the ins code of the agent's municipality
    \param gender the gender of the individual
    \param age_class the age class of the individual
    \param age the individual's age
    \param education an education level
    \param hh_relationship an household relationship status
    \param house a road network node id
   */
  Individual( repast::AgentId id, repast::AgentId hh_id, int municipality,
           char gender, int age_class, int age, char education, char hh_relationship,
           long house );

  //! Constructor (initialize every attributes but age).
  /*!
    \param id a Repast agent id
    \param hh_id the individual's household Repast agent id
    \param municipality the ins code of the agent's municipality
    \param gender the gender of the individual
    \param age_class the age class of the individual
    \param education an education level
    \param sps_status socio-professional status
    \param driving_license driving license ownership
    \param hh_relationship an household relationship status
    \param house a road network node id
    \param act_chain an activity chain
   */
  Individual( repast::AgentId id, repast::AgentId hh_id, int municipality,
             char gender, int age_class, char education, char sps_status,
             char driving_license, char hh_relationship, long house,
             std::vector<Activity> act_chain );

  //! Constructor (initialize every attributes).
  /*!
    \param id a Repast agent id
    \param hh_id the individual's household Repast agent id
    \param municipality the ins code of the agent's municipality
    \param gender the gender of the individual
    \param age_class the age class of the individual
    \param age the individual's age
    \param education an education level
    \param sps_status socio-professional status
    \param driving_license driving license ownership
    \param hh_relationship an household relationship status
    \param house a road network node id
    \param act_chain an activity chain
   */
  Individual( repast::AgentId id, repast::AgentId hh_id, int municipality,
              char gender, int age_class, int age, char education, char sps_status,
              char driving_license, char hh_relationship, long house, std::vector<Activity> act_chain );

  //! Destructor.
  virtual ~Individual() ;

  //! Return individual's gender.
  /*!
    \return a gender
   */
  char getGender() const {
    return _gender;
  }

  //! Set individual's gender.
  /*!
    \param val a new gender
   */
  void setGender( char val ) {
    _gender = val ;
  }

  //! Return individual's age class.
  /*!
    \return an age class
   */
  int getAgeClass() const {
    return _age_class;
  }

  //! Set individual's age class.
  /*!
    \param val a new age class
   */
  void setAgeClass( int val ) {
    _age_class = val ;
  }

  //! Return individual's age.
  /*!
    \return an age
   */
  int getAge() const {
    return _age;
  }

  //! Set individual's age.
  /*!
    \param val a new age
   */
  void setAge( int val ) {
    _age = val ;
  }

  //! Return the ins code of individual's municipality.
  /*!
    \return a ins code
   */
  int getMunicipality() const {
    return _municipality;
  }

  //! Set individual's muncipality.
  /*!
    \param val a new municipality ins code
   */
  void setMunicipality( int val ) {
    _municipality = val ;
  }

  //! Return individual's education level.
  /*!
    \return an education level
   */
  char getEducation() const {
    return _education;
  }

  //! Set individual's education level.
  /*!
    \param val a new education level
   */
  void setEducation( char val ) {
    _education = val ;
  }

  //! Return individual's socio-professional status.
  /*!
    \return individual's socio-professional status
   */
  char getSpsStatus() const {
    return _sps_status;
  }

  //! Set individual's socio-professional status.
  /*!
    \param val a new socio-professional status
   */
  void setSpsStatus( char val ) {
    _sps_status = val;
  }

  //! Return individual's driving license ownership.
  /*!
    \return a driving license ownership
   */
  char getDrivingLicense() const {
    return _driving_license;
  }

  //! Set individual's driving license ownership.
  /*!
    \param val a new driving license ownership
   */
  void setDrivingLicense( char val ) {
    _driving_license = val ;
  }

  //! Return individual's household relationship status (head, mate, child, adult).
  /*!
    \return a household relationship status
   */
  char getHhRelationship() const {
    return _hh_relationship;
  }

  //! Set individual's household relationship status (head, mate, child, adult).
  /*!
    \param val a household relationship status
   */
  void setHhRelationship( char val ) {
    _hh_relationship = val ;
  }

  //! Return individual's activity chain.
  /*!
    \return a vector of Activity objects (see Activity class)
   */
  std::vector<Activity> getActChain() const {
    return _act_chain;
  }

  //! Set individual's activity chain.
  /*!
    \param val a vector of Activity objects (see Activity class)
   */
  void setActChain( std::vector<Activity> val ) {
    _act_chain = val;
  }

  //! Add an activity to individual's activity chain.
  /*!
    \param val an Activity (see Activity class)
   */
  void addActChain( Activity val ) {
    _act_chain.push_back(val);
  }

  //! Return Repast AgentId of the individual's household.
  repast::AgentId & getHhId() {
    return _hh_id ;
  }

  //! Return Repast AgentId of the individual's household.
  const repast::AgentId & getHhId() const {
    return _hh_id ;
  }

  //! Set the Repast AgentId of the individual's household.
  /*!
    \param val a Repast AgentId designing an household
   */
  void setHhId( repast::AgentId val ) {
    _hh_id = val;
  }    

  //! Return the house localization (i.e. a node id, see Network class).
  /*!
    \return a node id
   */
  long getHouse() const {
    return _house;
  }

  //! Set individual's house localization (i.e. a node id, see Network class).
  /*!
    \param val a node id
   */
  void setHouse(long val) {
    _house = val;
  }

  //! Return the individual Repast agent id (required by Repast).
  /*!
    \return the individual Repast agent id
   */
  repast::AgentId & getId() {
    return _id ;
  }

  //! Return the individual Repast agent id (required by Repast).
  /*!
    \return the individual Repast agent id
   */
  const repast::AgentId & getId() const {
    return _id ;
  }

  //! Incrementing the age of an Individual.
  void aging();

  //! Initialize the age of an Individual.
  void initAge();

  //! Death process.
  bool isDying();

  //! Determine whether the individual is giving birth to a baby.
  bool givingbirth();

  //! Generate a baby by the individual.
  Individual birthInd(long babyId);

};

#endif /* INDIVIDUAL_HPP_ */
