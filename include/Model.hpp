/****************************************************************
 * MODEL.HPP
 *
 * This file contains all the VirtualBelgium's model and
 * scheduler related classes and functions.
 *
 * Authors: J. Barthelemy and L. Hollaert
 * Date   : 17 july 2012
 ****************************************************************/

/*! \file Model.hpp
 *  \brief VirtualBelgium's models and schedulers declarations.
 */

#ifndef MODEL_HPP_
#define MODEL_HPP_

#include "Individual.hpp"
#include "Household.hpp"
#include "Data.hpp"
#include "tinyxml2.hpp"

#include "repast_hpc/SharedContext.h"
#include "repast_hpc/Schedule.h"
#include "repast_hpc/Random.h"
#include "repast_hpc/Properties.h"
#include "repast_hpc/Utilities.h"
#include "repast_hpc/io.h"
#include "repast_hpc/logger.h"
#include "repast_hpc/initialize_random.h"
#include "repast_hpc/SVDataSetBuilder.h"

#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <stdexcept>
#include <math.h>
#include <vector>
#include <iomanip>
#include <boost/serialization/access.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/unordered_set.hpp>


//! Main VirtualBelgium class.
/*!
  This class contains the scheduler and responsible for data aggregation.
  It is the core of VirtualBelgium.
 */
class Model {

 private :

  friend class ProviderReceiver;

  int                _proc;                                     //!< rank of the model's process
  repast::Properties _props;                                    //!< properties of the model
  repast::SVDataSet* _data_out;                                 //!< aggregated output data set

  AggregateSum* _deathMenSum;                                   //!< aggregated number of death (men)
  AggregateSum* _deathWomenSum;                                 //!< aggregated number of death (women)
  AggregateSum* _babyBoySum;                                    //!< aggregated number of death
  AggregateSum* _babyGirlSum;                                   //!< aggregated number of death
  AggregateSum* _menSum;                                        //!< aggregated number of men
  AggregateSum* _girlSum;                                       //!< aggregated number of women

  AggregateSum* _age0MSum;                                      //!< aggregated number of men in age class 0
  AggregateSum* _age1MSum;                                      //!< aggregated number of men in age class 1
  AggregateSum* _age2MSum;                                      //!< aggregated number of men in age class 2
  AggregateSum* _age3MSum;                                      //!< aggregated number of men in age class 3
  AggregateSum* _age4MSum;                                      //!< aggregated number of men in age class 4
  AggregateSum* _age5MSum;                                      //!< aggregated number of men in age class 5
  AggregateSum* _age6MSum;                                      //!< aggregated number of men in age class 6
  AggregateSum* _age7MSum;                                      //!< aggregated number of men in age class 7
  AggregateSum* _age8MSum;                                      //!< aggregated number of men in age class 8
  AggregateSum* _age9MSum;                                      //!< aggregated number of men in age class 9
  AggregateSum* _age10MSum;                                     //!< aggregated number of men in age class 10

  AggregateSum* _age0WSum;                                      //!< aggregated number of women in age class 0
  AggregateSum* _age1WSum;                                      //!< aggregated number of women in age class 1
  AggregateSum* _age2WSum;                                      //!< aggregated number of women in age class 2
  AggregateSum* _age3WSum;                                      //!< aggregated number of women in age class 3
  AggregateSum* _age4WSum;                                      //!< aggregated number of women in age class 4
  AggregateSum* _age5WSum;                                      //!< aggregated number of women in age class 5
  AggregateSum* _age6WSum;                                      //!< aggregated number of women in age class 6
  AggregateSum* _age7WSum;                                      //!< aggregated number of women in age class 7
  AggregateSum* _age8WSum;                                      //!< aggregated number of women in age class 8
  AggregateSum* _age9WSum;                                      //!< aggregated number of women in age class 9
  AggregateSum* _age10WSum;                                     //!< aggregated number of women in age class 10

  unsigned long int _n_activity_start_time_x_ins[589][24];      //!< Number of starting activities  time of the day by municipality
  unsigned long int _n_activity_end_time_x_ins[589][24];        //!< Number of ending activities performed / time of the day by municipality (also trip start)
  unsigned long int _origin_destination_matrix[589][589];       //!< Origin-Destination matrix between municipalities
  unsigned long int _origin_destination_matrix_mp[589][589];    //!< Origin-Destination matrix between municipalities (morning peak: 7:00 - 9:00)
  unsigned long int _origin_destination_matrix_ep[589][589];    //!< Origin-Destination matrix between municipalities (evening peak: 15:00 - 19:00)

  int _babyId;                                                  //!< Id initialized for the babies

 public :

  repast::SharedContext<Individual> agents;   //!< Shared context containing the individual agents of the simulation
  repast::SharedContext<Household>  agentsHh; //!< Shared context containing the household agents of the simulation

  //! Constructor.
  /*
   /param comm the mpi communicator
   /param props the model properties
   */
  Model( boost::mpi::communicator* comm, repast::Properties & props );

  //! Destructor.
  ~Model();
  
  //! Initialisation of the simulation's schedule.
  void initSchedule();

  //! Implements one step of the simulation.
  void step();

  //! Generates the travel demand forecasting via activity chains model.
  void computeActivityChains();

  //! Computes the socio-demographic evolution of the population.
  void computePopulationEvolution();

  //! Reset every aggregate data set to 0.
  void resetAggregateOutputs();

  //! Save the agents state of the current process' Individual shared context in a file.
  void writeIndividuals();

  //! Write the Individual agents plans to an XML file that can be processed with MATSim.
  void writeActivityChains();

  //! Save activities localization and when they are performed.
  void saveActivityLocalizationAndTime();

  //! Save origin-destination matrices for several time slots.
  void saveODMatrix();

  //! Used by Repast HPC to exchange Individual agents between process.
  /*!
    \param agent the agent to exchange
    \param out the package containing the agent to exchange
   */
  void providePackage(Individual * agent , std::vector<IndividualPackage> &out);

  //! Used by Repast HPC to exchange Households agents between process.
  /*!
    \param agent the agent to exchange
    \param out the package containing the agent to exchange
   */
  void providePackage(Household * agent, std::vector<HouseholdPackage> &out);

  //! Used by Repast HPC to create an Individual Agent from an IndividualPackage.
  /*!
    \param package a package containing an individual agent
    
    \return an individual agent
   */
  Individual * createIndividual(IndividualPackage package);

  //! Used by Repast HPC to create an Household Agent from an HouseholdPackage.
  /*!
    \param package a package containing an household agent

    \return an household agent
   */
  Household * createHousehold(HouseholdPackage package);

};

#endif /* MODEL_HPP_ */
