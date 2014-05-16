/****************************************************************
 * HOUSEHOLD.HPP
 * 
 * This file contains all the households related class and methods.
 * The households are as agents containing individual agents and
 * localised in a municipality.
 * 
 * Authors: J. Barthelemy and L. Hollaert
 * Date   : 17 july 2012
 ****************************************************************/

/*! \file Household.hpp
 *  \brief Household agents related classes and methods declarations.
 */

#ifndef HOUSEHOLD_HPP_
#define HOUSEHOLD_HPP_


#include "repast_hpc/SVDataSet.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/Random.h"
#include "repast_hpc/io.h"
#include "repast_hpc/logger.h"
#include "repast_hpc/initialize_random.h"
#include "repast_hpc/Utilities.h"

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

#include "Individual.hpp"
#include "Data.hpp"

class Individual;


//! \brief The package structure for Household agents.
/*!
  A structure used for passing households agents from one process to another.
 */
struct HouseholdPackage {

  friend class boost::serialization::access ;

  //! Serializing procedure of the package.
  /*!
    \param ar an Archive in which the household will be stored
    \param version version of the package
   */
  template <class Archive>
  void serialize ( Archive & ar , const unsigned int version ) {
    ar & id;
    ar & ins;
    ar & list_ind;
    ar & type;
    ar & n_children;
    ar & n_adults;
    ar & house;
    ar & proc;
    ar & agent_type;
  }

  int id;                                      //!< id of the household
  int ins;                                     //!< ins code of the household's municipality
  std::vector<repast::AgentId> list_ind;       //!< ids of households' members
  std::string type;                            //!< household type
  int n_children;                              //!< number of children
  int n_adults;                                //!< number of additional adults
  long house;                                  //!< household's housing unit node id
  int proc;                                    //!< initial household process
  int agent_type;                              //!< household agent type

  //! Return household's agent id.
  /*!
    \return an household agent id
   */
  repast::AgentId getId() const {
    return repast::AgentId( id , proc , agent_type );
  }

};

//! The household agent class
/*!
  This class implements the households agents in VirtualBelgium.
  Each household is characterized by
  - an id of the type repast::AgentId;
  - a type;
  - a list of household members;
  - a number of children;
  - a number of additional adults (i.e. household head and mate not included);
  - a housing unit;
  - a municipality.
*/
class Household : public repast::Agent {

  friend class boost::serialization::access;

private :

  repast::AgentId         _id;                 //!< household's unique id
  int                     _ins;                //!< household's municipality ins code
  std::vector<repast::AgentId> _list_ind;      //!< household's members id
  std::string                  _type;          //!< household's type
  int                     _n_children;         //!< household's number of children (< 18 years old)
  int                     _n_adults;           //!< household's number of additional adults
  long                    _house;              //!< id of the network's node where the household is living

public :

  //! Constructor.
  /*!
    \param id a Repast agent id
    \param ins a ins code
    \param list_ind a list households members
    \param type a household type
    \param n_children the number of children (< 18 years old)
    \param n_adults number of adults
   */
  Household( repast::AgentId id, int ins, std::vector<repast::AgentId> list_ind,
      std::string type, int n_children, int n_adults );

  //! Constructor.
  /*!
    \param id a Repast agent id
    \param ins a ins code
    \param list_ind a list households members
    \param type a household type
    \param n_children the number of children (< 18 years old)
    \param n_adults number of adults
    \param house a node id (see network class)
   */
  Household( repast::AgentId id, int ins, std::vector<repast::AgentId> list_ind,
      std::string type, int n_children, int n_adults, long house );

  //! Destructor.
  virtual ~Household () ;

  //! Return the current household type.
  /*!
    \return current household's type
   */
  std::string getType () const {
    return _type;
  }

  //! Set household's type.
  /*!
    \param val new household type
   */
  void setType ( std::string val ) {
    _type = val ;
  }

  //! Get the number of children.
  /*!
    \return current number of children
   */
  int getNChildren () const {
    return _n_children;
  }

  //! Set the number of children.
  /*!
    \param val new number of children
   */
  void setNChildren ( int val ) {
    _n_children = val ;
  }

  //! Return the number of additional adults.
  /*!
    \return current number of additional adults
   */
  int getNAdults () const {
    return _n_adults;
  }

  //! Set the number of additional adults.
  /*!
    \param val new number of additional adults in the household
   */
  void setNAdults ( int val ) {
    _n_adults = val ;
  }
  
  //! Return a list of the household's member.
  /*!
    \return a vector of repast individual agent id
   */
  std::vector<repast::AgentId> & getListInd () {
    return _list_ind ;
  }

  //! Set the list of household members.
  /*!
    \param val a new list of individual repast agent id
   */
  void setListInd ( std::vector<repast::AgentId> val ) {
    _list_ind = val;
  }

  //! Return the node id of the household's home.
  /*!
    \return a node id (see Network class)
   */
  long getHouse() const {
    return _house;
  }

  //! Set the node id of the household's home.
  /*!
    \param val a node id (see Network class)
   */
  void setHouse(long val) {
    _house = val;
  }

  //! Return the ins code of the household's municipality.
  /*!
    \return a ins code
   */
  int getIns() const {
    return _ins;
  }

  //! Set the household ins code.
  /*!
    \param val a ins code
   */
  void setIns( int val ) {
    _ins = val;
  }

  //! Return the household repast agent id (required by Repast).
  /*!
    \return the household repast agent id
   */
  repast::AgentId & getId () {
    return _id ;
  }

  //! Return the household repast agent id (required by Repast).
  /*!
    \return the household repast agent id
   */
  const repast::AgentId & getId () const {
    return _id ;
  }

  //! Return a vector of pointers to the individual agents belonging to the household.
  /*!
    \param agentsInd the Repast shared context containing the individuals
   */
  std::vector<Individual * > getIndividuals( repast::SharedContext<Individual> & agentsInd );

  //! Determines the household type.
  /*!
    \param agentsInd the Repast shared context containing the individuals
   */
  void computeHhType( repast::SharedContext<Individual> & agentsInd );

  //! Removes an individual from the household.
  /*!
    \param aId a repast individual agent id of the household's member to remove
   */
  void removeIndFromList( repast::AgentId aId );

  //! Choose randomly a node of the network inside of the household's municipality as the household's house.
  /*!
    \param dataset simulation input data (see Data class)
   */
  void localizeHouse( Data dataset );

  //! Adding a baby to the household.
  /*!
    \param aBabyId the Repast individual agent id of the baby to add
   */
  void addBaby( repast::AgentId aBabyId );

};

#endif /* HOUSEHOLD_HPP_ */
