/****************************************************************
 * MODEL.CPP
 *
 * This file contains all the definitions of the methods of
 * Model.hpp (see this file for methods' documentation)
 *
 * Authors: J. Barthelemy and L. Hollaert
 * Date   : 17 july 2012
 ****************************************************************/

#include "../include/Model.hpp"

using namespace repast;
using namespace std;
using namespace tinyxml2;

Model::Model(boost::mpi::communicator* world, Properties & props ) : agents(world), agentsHh(world) {

  // Reading properties, rank of the process and input filenames ----

  this->_props = props;
  this->_proc  = RepastProcess::instance()->rank();

  if (this->_proc == 0) {
    cout << "... creation model!" << endl;
  }

  string filename_ind = this->_props.getProperty("file.ind");          // filename for base individual agents data
  string filename_hh  = this->_props.getProperty("file.hh");           // filename for base household agents data

  // MPI related variables ------------------------------------------

  vector<int> hhCounts;                                                // vector of number of household agents by process
  int         worldSize = RepastProcess::instance()->worldSize();      // number of process
  long int    totalHh   = linesCount(filename_hh);                     // total number of household agents to generate
  double      hhPerP    = totalHh / worldSize;                         // number of agents by process

  if (_proc == 0) {
    hhCounts.assign(worldSize, hhPerP);                                // assigning hhPerP for each process
    int diff = totalHh - hhPerP * worldSize;                           // compute the difference between the true totalHh and the computed number of agent
    hhCounts[worldSize - 1] = hhCounts[worldSize - 1] + diff;          // assign the difference to the last process
  }

  // MPI -----------------------------------------------------------
  // Task repartition between the various process available
  // numHh = number of household by process (computed by scatter)

  int numHh = 0;
  boost::mpi::scatter(*world, hhCounts, numHh, 0);

  // Agents and SharedContext generation ---------------------------

  props.putProperty("number.individuals", linesCount(filename_ind));       // logging the total number of individual agents

  // opening files
  ifstream file_hh(filename_hh.c_str(), ios::in);
  ifstream file_ind(filename_ind.c_str(), ios::in);

  if (file_hh && file_ind) {
    if (this->_proc == 0) {
      cout << "Reading individuals and households base files" << endl;
    }
  } else {
    cerr << "Error while opening individuals and households base files" << endl;
  }

  int lines = 0;

  // reading files
  if (file_hh && file_ind) {

    // finding the right input line in the data file
    while (lines < _proc * hhPerP) {
      file_hh.ignore(numeric_limits<int>::max(), '\n');
      lines++;
    }

    // ... households
    for (int i = 0; i < numHh; i++) {

      int a_ins;
      string a_municipality;
      string a_long_hhtype;
      int a_n_children;
      int a_n_adults;
      int a_hhid;
      long a_house;
      vector<int> a_list_ind_id;
      vector<AgentId> a_list_ind_agentid;

      int a_id_temp;

      // reading household characteristics
      file_hh >> a_hhid >> a_ins >> a_municipality >> a_long_hhtype >> a_n_children >> a_n_adults;

      // compute household repast::AgentId
      AgentId hh_id(a_hhid, _proc, MODEL_AGENT_HH_TYPE);

      // compute household localization
      a_house = Data::getInstance()->getOneNodeIdFromIns(a_ins);

      // compute number of household members
      int n_ind = 0;
      if (a_long_hhtype == "C" || a_long_hhtype == "F") {
        n_ind = 2 + a_n_children + a_n_adults;
      } else {
        n_ind = 1 + a_n_children + a_n_adults;
      }

      // generating households' individuals and its individual id list
      for (int j = 0; j < n_ind; j++) {

        file_hh >> a_id_temp;
        a_list_ind_id.push_back(a_id_temp);

        // skipping lines until the first individual of current household is found in the file
        long int a_id;
        file_ind >> a_id;
        if ((i == 0) && (j == 0)) {
          while (a_id != a_list_ind_id[0]) {
            file_ind.ignore(numeric_limits<int>::max(), '\n');
            file_ind >> a_id;
          }
        }

        // Generating Individuals

        int    a_ins;            // ins code of the municipality
        string a_municipality;   // municipality
        char   a_hhtype;         // household type
        char   a_gender;         // gender
        char   a_spstatus;       // socio-professional status
        char   a_dip;            // education level
        char   a_drvlic;         // driving license
        int    a_agecl;          // age class
        string a_actchain = "";  // activity chain (empty if individual is a baby, i.e. age class 0

        // extracting data from input file
        file_ind >> a_ins >> a_municipality >> a_hhtype >> a_gender >> a_spstatus >> a_dip >> a_drvlic >> a_agecl;

        // reading activity chain if the current individual is not a baby
        vector<Activity> act_chain_vect;
        if ( a_agecl > 0 ) {

          file_ind >> a_actchain;                                      // getting activity chain from data the input population file

          // generating the different activity extracted from a_actchain if the chain is different from 'x'

          if( a_actchain[0] != 'x') {

            Activity first_act('m');
            act_chain_vect.push_back(first_act);

            for (unsigned int k = 0; k < a_actchain.size(); k++) {
              Activity curr_act(a_actchain[k]);                          // ... calling Activity constructor
              act_chain_vect.push_back(curr_act);                        // ... adding the resulting activity to the vector of activities
            }

          }

        }

        // computation of household relationship
        char a_hh_rel = 'X';

        if (j == 0) { // first individual of the household is the household's head
          a_hh_rel = 'H';
        } else {
          // ... member of a family or a couple
          if (a_long_hhtype == "C" || a_long_hhtype == "F") {
            if (j == 1) {
              a_hh_rel = 'M';   // mate
            } else if (j > 1 && j < 2 + a_n_children) {
              a_hh_rel = 'C';   // children
            } else {
              a_hh_rel = 'A';   // additional adults
            }
          }
          // ... member of an other household type
          else if (a_long_hhtype == "M" || a_long_hhtype == "W") {
            if (j > 0 && j < 1 + a_n_children) {
              a_hh_rel = 'C';   // children
            } else {
              a_hh_rel = 'A';   // adults
            }
          }
        }

        AgentId ind_id(a_id, this->_proc, MODEL_AGENT_IND_TYPE); // generating andividual's AgentId
        Individual ind_temp(ind_id, hh_id, a_ins, a_gender, a_agecl, a_dip, a_spstatus, a_drvlic, a_hh_rel, a_house, act_chain_vect);
        ind_temp.initAge();                        // initialize individual's age
        agents.addAgent(new Individual(ind_temp)); // adding the agent to the Individual context
        a_list_ind_agentid.push_back(ind_id);      // saving individual's AgentId in the household currently build

      }

      Household hh_temp(hh_id, a_ins, a_list_ind_agentid, a_long_hhtype, a_n_children, a_n_adults, a_house);
      agentsHh.addAgent(new Household(hh_temp));

    }

    file_hh.close();
    file_ind.close();

  } else {
    cerr << "Could not open " << filename_ind << " or " << filename_hh << endl;
  }

  if (this->_proc == 0) {
    cout << "... end of agents generation" << endl;
  }

  // Aggregate data output ------------------------------------------

  std::string fileOutputName("../output/sim_out.csv");
  SVDataSetBuilder builder( fileOutputName.c_str(), ",", RepastProcess::instance()->getScheduleRunner().schedule() );

  this->_deathMenSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_dead_men", _deathMenSum, std::plus<int>()) );

  this->_deathWomenSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_dead_women", _deathWomenSum, std::plus<int>()));

  this->_babyBoySum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_baby_boy", _babyBoySum, std::plus<int>()));

  this->_babyGirlSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_baby_girl", _babyGirlSum, std::plus<int>()));

  this->_menSum = new AggregateSum();
  builder.addDataSource(createSVDataSource("n_men", _menSum, std::plus<int>()));

  this->_girlSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_girl", _girlSum, std::plus<int>()));

  this->_age0MSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_age_m0", _age0MSum, std::plus<int>()));

  this->_age1MSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_age_m1", _age1MSum, std::plus<int>()));

  this->_age2MSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_age_m2", _age2MSum, std::plus<int>()));

  this->_age3MSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_age_m3", _age3MSum, std::plus<int>()));

  this->_age4MSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_age_m4", _age4MSum, std::plus<int>()));

  this->_age5MSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_age_m5", _age5MSum, std::plus<int>()));

  this->_age6MSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_age_m6", _age6MSum, std::plus<int>()));

  this->_age7MSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_age_m7", _age7MSum, std::plus<int>()));

  this->_age8MSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_age_m8", _age8MSum, std::plus<int>()));

  this->_age9MSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_age_m9", _age9MSum, std::plus<int>()));

  this->_age10MSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_age_m10", _age10MSum, std::plus<int>()));

  this->_age0WSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_age_f0", _age0WSum, std::plus<int>()));

  this->_age1WSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_age_f1", _age1WSum, std::plus<int>()));

  this->_age2WSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_age_f2", _age2WSum, std::plus<int>()));

  this->_age3WSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_age_f3", _age3WSum, std::plus<int>()));

  this->_age4WSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_age_f4", _age4WSum, std::plus<int>()));

  this->_age5WSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_age_f5", _age5WSum, std::plus<int>()));

  this->_age6WSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_age_f6", _age6WSum, std::plus<int>()));

  this->_age7WSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_age_f7", _age7WSum, std::plus<int>()));

  this->_age8WSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_age_f8", _age8WSum, std::plus<int>()));

  this->_age9WSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_age_f9", _age9WSum, std::plus<int>()));

  this->_age10WSum = new AggregateSum();
  builder.addDataSource( createSVDataSource("n_age_f10", _age10WSum, std::plus<int>()));

  this->_data_out = builder.createDataSet();

  // Initialize Id for future babies ---------------------------------

  this->_babyId = 20000000 + (this->_proc + 1) * 1000000;

  if (this->_proc == 0) {
    cout << "... end of model initialization" << endl;
  }

}

Model::~Model() {
}

void Model::initSchedule() {

  ScheduleRunner & runner = RepastProcess::instance()->getScheduleRunner();

  // Stop at a given tick

  double start = strToDouble(this->_props.getProperty("par.start"));
  double stop  = strToDouble(this->_props.getProperty("par.end"));
  runner.scheduleStop(stop);

  // Call the step method on the Model every tick

  runner.scheduleEvent(start, 1, Schedule::FunctorPtr(new MethodFunctor<Model>(this, &Model::step)));

  // Schedule the aggregate data recording and writing

  runner.scheduleEvent(start, 1, Schedule::FunctorPtr(new MethodFunctor<DataSet>(_data_out, &DataSet::record)));
  runner.scheduleEndEvent(Schedule::FunctorPtr(new MethodFunctor<DataSet>(_data_out, &DataSet::write)));
  runner.scheduleEndEvent(Schedule::FunctorPtr(new MethodFunctor<DataSet>(_data_out, &DataSet::close)));

}

void Model::providePackage(Individual * agent, std::vector<IndividualPackage> &out) {

  AgentId id = agent->getId();
  IndividualPackage package = { id.id(), agent->getHhId(),
      agent->getMunicipality(), agent->getGender(), agent->getAgeClass(),
      agent->getAge(), agent->getEducation(), agent->getSpsStatus(),
      agent->getDrivingLicense(), agent->getHhRelationship(),
      agent->getActChain(), id.currentRank(), id.agentType() };
  out.push_back(package);

}

void Model::providePackage(Household * agent, std::vector<HouseholdPackage> &out) {

  AgentId id = agent->getId();
  HouseholdPackage package = { id.id(), agent->getIns(),
      agent->getListInd(), agent->getType(), agent->getNChildren(),
      agent->getNAdults(), agent->getHouse(), id.currentRank(), id.agentType() };

  out.push_back(package);

}

Individual * Model::createIndividual(IndividualPackage package) {

  return new Individual(package.getId(), package.hh_id, package.municipality,
      package.gender, package.age_class, package.age, package.education,
      package.sps_status, package.driving_license, package.hh_relationship,
      package.house, package.act_chain);

}

Household * Model::createHousehold(HouseholdPackage package) {

  return new Household(package.getId(), package.ins, package.list_ind,
      package.type, package.n_children, package.n_adults, package.house);

}

void Model::step() {

  double tick = RepastProcess::instance()->getScheduleRunner().currentTick();            // current tick (step) of the simulation
  if ( this->_proc == 0 ) cout << "... starting year " << tick << endl;

  // Reset aggregate outputs

  resetAggregateOutputs();

  // Spatial evolution

  if ( this->_props.getProperty("evo.activity") == "y" ) {

    computeActivityChains();

  }

  // Temporal evolution

  if( this->_props.getProperty("evo.age"  ) == "y" ||
      this->_props.getProperty("evo.death") == "y" ||
      this->_props.getProperty("evo.birth") == "y" ) {

    computePopulationEvolution();

  }

  if ( this->_props.getProperty("par.debug") == "y" ) {

    ostringstream screen_output;
    screen_output << "... END YEAR PROCESS " << tick << endl;
    cout << screen_output.str();

  }

  if ( this->_proc == 0 ) cout << "... end year " << tick << endl;

}

void Model::computeActivityChains() {

  // Model attributes

  repast::SharedContext<Individual>::const_local_iterator it_beg = agents.localBegin();   // initial individual agent
  repast::SharedContext<Individual>::const_local_iterator it_end = agents.localEnd();     // final individual agent
  Network net = Data::getInstance()->getNetwork();                                        // road network

  #ifdef DEBUGVB
    unsigned long debug_n_agents_done = 0;
  #endif

  // Main loop over every individuals

  while (it_beg != it_end) {

    #ifdef DEBUGVB
      debug_n_agents_done++;
      ostringstream screen_output;
      screen_output << "### computeActivityChains - Processing agent " << debug_n_agents_done << " / " << agents.size() <<  " by " << this->_proc;
      screen_output << " " << (*it_beg)->getId().id() << " " << endl;
      cout << screen_output.str();
    #endif

    if ( (*it_beg)->getAgeClass() > 0 && (*it_beg)->getActChain().size() > 0 ) {         // skipping babies and individuals with empty activity chain

      // Variables

      vector<Activity> act_chain_vect = (*it_beg)->getActChain();    // vector of initial activities
      vector<Activity> final_act_chain_vect;                         // vector of final, fully characterized, activities
      long start_act_node = (*it_beg)->getHouse();                   // starting place of the activity chain (the household's house)
      float distance = 0;                                            // distance to reach next activity
      float dur_trip = 0;                                            // duration trip to next activity
      bool start = false;                                            // flag set to 'false' if activity = staying home, 'true' otherwise

      // Generating the first activity: being at home

      Activity home(start_act_node, act_chain_vect[1].getTypeNum());
      final_act_chain_vect.push_back(home);
      float startTime = home.getEndTime();                           // ... leaving home time (seconds)

      // Generating all but last activities

      for (unsigned int k = 1; k < act_chain_vect.size() - 1; k++) {

        // ... going back to the house
        //if( act_chain_vect[k].getType() == 'm' ) {
        if( act_chain_vect[k].getType() == this->_props.getProperty("par.act_home")[0] ) {

          start              = false;
          long prev_act_node = start_act_node;
          start_act_node     = (*it_beg)->getHouse();
          distance           = net.getDistanceNodes(start_act_node,prev_act_node);

          // check if distance performed > 1m and compute trip duration...
          if ( distance > 1.0 ) {
            dist_param_mixture duration_trip_dist_par = Data::getInstance()->getDurationCondiDistTripParDist(distance);
            dur_trip = RandomGenerators::getInstance()->mixt_lognorm_dev.dev(duration_trip_dist_par.mu, duration_trip_dist_par.sigma, duration_trip_dist_par.p, duration_trip_dist_par.max);
          }
          // ... otherwise trip duration is set to 0
          else {
            dur_trip = 0.0;
          }
          startTime = startTime + dur_trip;

        }
        // ... others activities
        else {
          start = true;
        }

        // activity generation ( size of the act_chain is decremented by 2 because we don't take into account the house returning and leaving house
        Activity curr_act(act_chain_vect[k].getType(), start_act_node, start, startTime); // ... calling Activity constructor

        // if returning home, adding the characteristics not initialized by the constructor
        if( start == false) {
          curr_act.setDistance(distance);
          curr_act.setDurationTrip(dur_trip);
        }

        final_act_chain_vect.push_back(curr_act);                      // ... adding the resulting activity to the vector of activities
        start_act_node = curr_act.getNodeId();                         // ... the starting node of next activity is the destination node of the current activity
        startTime = curr_act.getEndTime();                             // ... starting time of the next activity is given by the ending time of the current activity

      }

      // Generating last activity, i.e. returning home

      Activity returnHouse(start_act_node, (*it_beg)->getHouse());     // creating the returning home activity
      final_act_chain_vect.push_back(returnHouse);                     // ... and adding it to the activity chain of the current individual

      // Updating activity chain of current individual

      (*it_beg)->setActChain(final_act_chain_vect);

    }

    // Next individual

    it_beg++;

  }

  // Saving results

  this->writeActivityChains();
  this->saveActivityLocalizationAndTime();
  this->saveODMatrix();

  // !!! Waiting every processes before going further (before beginning evolution)

  RepastProcess::instance()->getCommunicator()->barrier();

}

void Model::computePopulationEvolution(){

  // Models attributes and flag

  bool curr_ind_birth;                                                                    // indicate whether the current individual is giving birth

  // Agents

  repast::SharedContext<Individual>::const_local_iterator it_beg = agents.localBegin();   // initial individual agent
  repast::SharedContext<Individual>::const_local_iterator it_end = agents.localEnd();     // final individual agent

  // Main loop over all the Individual agents

  #ifdef DEBUGVB
    unsigned long debug_n_agents_done = 0;
  #endif

  while (it_beg != it_end) {

    #ifdef DEBUGVB
      debug_n_agents_done++;
      ostringstream screen_output;
      screen_output << "### computePopulationEvolution - Processing agent " << debug_n_agents_done << " / " << agents.size() <<  " by " << this->_proc;
      screen_output << " " << (*it_beg)->getId().id() << " " << endl;
      cout << screen_output.str();
    #endif


    // Check if current individual is dying (if model enabled)
    bool curr_ind_dead = false;
    if (this->_props.getProperty("evo.death") == "y") {
      curr_ind_dead = (*it_beg)->isDying();
    }

    // ... individual is alive: evolution process
    if (curr_ind_dead == false) {

      if ((*it_beg)->getGender() == 'H') {
        this->_menSum->increment();
      } else {
        this->_girlSum->increment();
      }

      // individuals' aging process (if ( model enabled)
      if (this->_props.getProperty("evo.age") == "y") {
        (*it_beg)->aging();
      }

      if ((*it_beg)->getGender() == 'H') {
        if ((*it_beg)->getAge() < 6) {
          this->_age0MSum->increment();
        } else if ((*it_beg)->getAge() < 16) {
          this->_age1MSum->increment();
        } else if ((*it_beg)->getAge() < 26) {
          this->_age2MSum->increment();
        } else if ((*it_beg)->getAge() < 36) {
          this->_age3MSum->increment();
        } else if ((*it_beg)->getAge() < 46) {
          this->_age4MSum->increment();
        } else if ((*it_beg)->getAge() < 56) {
          this->_age5MSum->increment();
        } else if ((*it_beg)->getAge() < 66) {
          this->_age6MSum->increment();
        } else if ((*it_beg)->getAge() < 76) {
          this->_age7MSum->increment();
        } else if ((*it_beg)->getAge() < 86) {
          this->_age8MSum->increment();
        } else if ((*it_beg)->getAge() < 96) {
          this->_age9MSum->increment();
        } else {
          this->_age10MSum->increment();
        }
      } else {
        if ((*it_beg)->getAge() < 6) {
          this->_age0WSum->increment();
        } else if ((*it_beg)->getAge() < 16) {
          this->_age1WSum->increment();
        } else if ((*it_beg)->getAge() < 26) {
          this->_age2WSum->increment();
        } else if ((*it_beg)->getAge() < 36) {
          this->_age3WSum->increment();
        } else if ((*it_beg)->getAge() < 46) {
          this->_age4WSum->increment();
        } else if ((*it_beg)->getAge() < 56) {
          this->_age5WSum->increment();
        } else if ((*it_beg)->getAge() < 66) {
          this->_age6WSum->increment();
        } else if ((*it_beg)->getAge() < 76) {
          this->_age7WSum->increment();
        } else if ((*it_beg)->getAge() < 86) {
          this->_age8WSum->increment();
        } else if ((*it_beg)->getAge() < 96) {
          this->_age9WSum->increment();
        } else {
          this->_age10WSum->increment();
        }
      }

      // individuals' giving birth process (if model enabled)
      if (this->_props.getProperty("evo.birth") == "y") {

        //Initialization
        curr_ind_birth = false;

        // check if the individual is a women, aged between 15 and 49, and is the head of the household or the mate
        if (((*it_beg)->getGender() == 'F') && ((*it_beg)->getAge() < 50)
            && ((*it_beg)->getAge() > 14)
            && (((*it_beg)->getHhRelationship() == 'H')
                || ((*it_beg)->getHhRelationship() == 'M'))) {
          //Will the women give birth...?
          curr_ind_birth = (*it_beg)->givingbirth();
        }

        // ... if a women will give birth
        if (curr_ind_birth == true) {
          //Adding all the characteristics (boy or girl, age,...) to the baby
          Individual newInd = (*it_beg)->birthInd(this->_babyId);

          //cout << this->_babyId << endl;
          this->_babyId++;

          //Increment the number of babyGirl (if it is a girl) or babyBoy (if it is a boy)
          if (newInd.getGender() == 'H') {
            this->_babyBoySum->increment();
            this->_menSum->increment();
          } else {
            this->_babyGirlSum->increment();
            this->_girlSum->increment();
          }

          //Add the new baby to the agents
          agents.addAgent(new Individual(newInd));

          //Add the baby to the household
          agentsHh.getAgent((*it_beg)->getHhId())->addBaby(newInd.getId());

          //Recompute the household type
          agentsHh.getAgent((*it_beg)->getHhId())->computeHhType(agents);

        }
      }
    }

    // ... individual is dead: removing it from the simulation
    else {

      agentsHh.getAgent((*it_beg)->getHhId())->removeIndFromList((*it_beg)->getId());

      // ... check if household is empty and removes it from the simulation if necessary
      if (agentsHh.getAgent((*it_beg)->getHhId())->getListInd().size() > 0) {
        agentsHh.getAgent((*it_beg)->getHhId())->computeHhType(agents); // a mettre dans removeIndFromList
      } else {
        agentsHh.removeAgent((*it_beg)->getHhId());
      }

      if ((*it_beg)->getGender() == 'H') { _deathMenSum->increment();   }     // counting the number of dead men
      else                               { _deathWomenSum->increment(); }     // counting the number of dead women

      agents.removeAgent((*it_beg)->getId());

    }

    it_beg++;

  }

  // Saving results

  this->writeIndividuals();

  // Waiting every processes before going further

   RepastProcess::instance()->getCommunicator()->barrier();

}

void Model::resetAggregateOutputs() {

  // Aggregated data source reset

  _deathMenSum->reset();    _deathWomenSum->reset();
  _babyBoySum->reset();     _babyGirlSum->reset();
  _menSum->reset();         _girlSum->reset();
  _age0MSum->reset();       _age0WSum->reset();
  _age1MSum->reset();       _age1WSum->reset();
  _age2MSum->reset();       _age2WSum->reset();
  _age3MSum->reset();       _age3WSum->reset();
  _age4MSum->reset();       _age4WSum->reset();
  _age5MSum->reset();       _age5WSum->reset();
  _age6MSum->reset();       _age6WSum->reset();
  _age7MSum->reset();       _age7WSum->reset();
  _age8MSum->reset();       _age8WSum->reset();
  _age9MSum->reset();       _age9WSum->reset();
  _age10MSum->reset();      _age10WSum->reset();

  // Reset of the aggregate count of activities by municipality and time of day

  memset(_n_activity_start_time_x_ins,0,sizeof(_n_activity_start_time_x_ins));
  memset(_n_activity_end_time_x_ins,0,sizeof(_n_activity_end_time_x_ins));
  memset(_origin_destination_matrix,0,sizeof(_origin_destination_matrix));
  memset(_origin_destination_matrix_mp,0,sizeof(_origin_destination_matrix));
  memset(_origin_destination_matrix_ep,0,sizeof(_origin_destination_matrix));

}

void Model::writeIndividuals() {

  double tick = RepastProcess::instance()->getScheduleRunner().currentTick();

  if (this->_proc == 0) { cout << "... writing the population in a file" << endl; }

  int count = 0;

  repast::SharedContext<Individual>::const_local_iterator it_beg = agents.localBegin();
  repast::SharedContext<Individual>::const_local_iterator it_end = agents.localEnd();

  // Output file creation
  ostringstream oss;
  oss << "../output/individuals_" << this->_proc << "_" << tick;
  string filename = oss.str();
  ofstream file(filename.c_str(), ios::out);

  // Loop over all the Individual agents
  while (it_beg != it_end) {

    AgentId aId   = (*it_beg)->getId();
    AgentId aHhId = (*it_beg)->getHhId();

    file << aId.id()                       << " " << (*it_beg)->getMunicipality() << " " << (*it_beg)->getHouse() << " ";
    file << (*it_beg)->getGender()         << " " << (*it_beg)->getAgeClass()     << " ";
    file << (*it_beg)->getAge()            << " " << (*it_beg)->getEducation()    << " ";
    file << (*it_beg)->getHhRelationship() << " " << aHhId.id() << " ";

    file << agentsHh.getAgent(aHhId)->getListInd().size();

    file << "\n";

    if( (*it_beg)->getHhRelationship() == 'C' && agentsHh.getAgent(aHhId)->getListInd().size() == 1 ) {
      cerr << "*********************** ERROR :" << (*it_beg)->getHhRelationship() << aId << " " << aHhId << endl;
    }

    it_beg++;
    count++;

  }

  file.close();

}

void Model::writeActivityChains() {

  if (this->_proc == 0) { cout << "... writing the activity chains in a file" << endl; }

  // current tick
  double tick = RepastProcess::instance()->getScheduleRunner().currentTick();

  // output file for activity outputs
  ostringstream oss2;
  oss2 << "../output/activity_stat_" << this->_proc << "_" << tick;
  string filename2 = oss2.str();
  ofstream file2(filename2.c_str(), ios::out);

  // xml document initialization
  XMLDocument * doc = new XMLDocument();

  // declaration and document type definition of xml file
  doc->insertEndChild( doc->newDeclaration("xml version=\"1.0\" encoding=\"utf-8\"") );
  doc->insertEndChild( doc->newUnknown("DOCTYPE plans SYSTEM \"http://www.matsim.org/files/dtd/plans_v4.dtd\""));

  // plans
  XMLNode * plans = doc->insertEndChild(doc->newElement("plans") );

  // loop over all individuals
  repast::SharedContext<Individual>::const_local_iterator it_beg = agents.localBegin();
  repast::SharedContext<Individual>::const_local_iterator it_end = agents.localEnd();

  while ( it_beg != it_end ) {

    // Extracting individual's activity chain, if the age class is > 0

    if( (*it_beg)->getAgeClass() > 0 && (*it_beg)->getActChain().size() > 0 ) {

      XMLElement * xel_person = doc->newElement("person");
      xel_person->SetAttribute("id",(*it_beg)->getId().id());
      xel_person->SetAttribute("employed","no");
      XMLNode * person = plans->insertEndChild(xel_person);

      XMLElement * xel_plan = doc->newElement("plan");
      xel_plan->SetAttribute("selected","yes");
      XMLNode * plan = person->insertEndChild(xel_plan);

      XMLElement * xel_act;
      XMLElement * xel_leg;

      // activity chains
      for( unsigned int i = 0; i < (*it_beg)->getActChain().size() - 1 ; i++ ) {

        xel_act = doc->newElement("act");
        xel_act->SetAttribute("type",(*it_beg)->getActChain()[i].getType());
        xel_act->SetAttribute("x",(double)Data::getInstance()->getNetwork().getNodes().at((*it_beg)->getActChain()[i].getNodeId()).getX() );
        xel_act->SetAttribute("y",(double)Data::getInstance()->getNetwork().getNodes().at((*it_beg)->getActChain()[i].getNodeId()).getY() );
        xel_act->SetAttribute("end_time",secToTime((*it_beg)->getActChain()[i].getEndTime()).c_str());
        plan->insertEndChild(xel_act);

        xel_leg = doc->newElement("leg");
        xel_leg->SetAttribute("mode","car");
        plan->insertEndChild(xel_leg);

        // activity recording (but we discard the first, staying home, activity)
        if( i > 0 ) {
          file2 << (*it_beg)->getActChain()[i].getTypeNum() << " " << (*it_beg)->getActChain()[i].getDistance() << " " << (*it_beg)->getActChain()[i].getDurationTrip() << " ";
          file2 << (*it_beg)->getActChain()[i].getDuration() << " " << ( (*it_beg)->getActChain()[i].getEndTime() - (*it_beg)->getActChain()[i].getDuration() ) << " ";
          file2 << (*it_beg)->getActChain().size() << " " << (i+1) << endl;
        }

      }

      // last activity: returning home
      unsigned int last =  (*it_beg)->getActChain().size() -1;
      xel_act = doc->newElement("act");
      xel_act->SetAttribute("type","m");
      xel_act->SetAttribute("x",(double)Data::getInstance()->getNetwork().getNodes().at((*it_beg)->getActChain()[last].getNodeId()).getX());
      xel_act->SetAttribute("y",(double)Data::getInstance()->getNetwork().getNodes().at((*it_beg)->getActChain()[last].getNodeId()).getY());

      file2 << (*it_beg)->getActChain()[last].getTypeNum() << " " << (*it_beg)->getActChain()[last].getDistance() << " " << (*it_beg)->getActChain()[last].getDurationTrip() << " ";
      file2 << (*it_beg)->getActChain()[last].getDuration() << " " << ( (*it_beg)->getActChain()[last-1].getEndTime() + (*it_beg)->getActChain()[last].getDurationTrip() ) << " ";
      file2 << (*it_beg)->getActChain().size() << " " << (last+1) << endl;

      plan->insertEndChild(xel_act);

    }

    it_beg++;

  }

  // output path
  ostringstream oss;
  oss << "../output/activity_chains_" << this->_proc << "_" << tick << ".xml" ;
  string filename = oss.str();

  // writing activity chain file
  doc->saveFile(filename.c_str());

  // freeing memory
  delete doc;

  // closing file
  file2.close();

}

void Model::saveActivityLocalizationAndTime(){

  if (this->_proc == 0) { cout << "... saving activity localization and time" << endl; }

  // iterators over the agents
  repast::SharedContext<Individual>::const_local_iterator it_beg = agents.localBegin();
  repast::SharedContext<Individual>::const_local_iterator it_end = agents.localEnd();

  int mun_id;                                        // id of a municipality (1 -- 589)
  int mun_ins;                                       // ins code of a municipality
  int time_start;                                    // starting time of the activity
  int time_end;                                      // ending time of the activity
  unsigned long int local_start[589][24] = {{ 0 }};  // 589 municipality, 24 hours + 1 garbage (for the activities starting after midnight)
  unsigned long int local_end[589][24]   = {{ 0 }};  // 589 municipality, 24 hours + 1 garbage (for the activities starting after midnight)


  // Reading data

  while ( it_beg != it_end ) {

    // loop over the individual activity chain
    for( unsigned int i = 0; i < (*it_beg)->getActChain().size(); i++ ) {

      // municipality of the activity
      mun_ins = Data::getInstance()->getNetwork().getNodes().at((*it_beg)->getActChain()[i].getNodeId()).getIns();

      // if no correct mun_ins is found due to incorrect data (or node outside Belgium), current activity is discarded
      if ( mun_ins != 0 ) {

        mun_id = Data::getInstance()->getMapInsIdMun().at(mun_ins);

        // activity starting time (skipping the first one, i.e. being at home)

        if( i > 0 ) {

          if( i < (*it_beg)->getActChain().size() - 1 ) {
            time_start = secToHour((*it_beg)->getActChain()[i].getEndTime() - (*it_beg)->getActChain()[i].getDuration());
          }
          else {
            time_start = secToHour((*it_beg)->getActChain()[i-1].getEndTime() + (*it_beg)->getActChain()[i].getDurationTrip());
          }

          // correcting starting time
          while( time_start > 23 ) {
            time_start = time_start - 24;
          }

          // saving starting time
          local_start[mun_id][time_start]++;

        }

        // activity ending time (skipping the last one, i.e. being at home)

        if( i < (*it_beg)->getActChain().size() - 1 ) {
          time_end = secToHour((*it_beg)->getActChain()[i].getEndTime());

          // correction ending time
          while( time_end > 23 ) {
            time_end = time_end - 24;
          }

          // saving ending time
          local_end[mun_id][time_end]++;

        }

      }

    }

    it_beg++;

  }

  // Gathering the data from other processes

  boost::mpi::communicator* comm = RepastProcess::instance()->getCommunicator();
  comm->barrier();
  boost::mpi::reduce(*comm, *local_start, 589*24, *_n_activity_start_time_x_ins, std::plus<int>(), 0);
  boost::mpi::reduce(*comm, *local_end, 589*24, *_n_activity_end_time_x_ins, std::plus<int>(), 0);

  // Saving it in a file (only for root process)

  if( this->_proc == 0 ) {

    ostringstream oss_start;
    oss_start << "../output/activity_mun_start_time" << "_" << RepastProcess::instance()->getScheduleRunner().currentTick();
    string filename_start = oss_start.str();
    ofstream file_start(filename_start.c_str(), ios::out);

    ostringstream oss_end;
    oss_end << "../output/activity_mun_end_time" << "_" << RepastProcess::instance()->getScheduleRunner().currentTick();
    string filename_end = oss_end.str();
    ofstream file_end(filename_end.c_str(), ios::out);

    file_start << "ADMUKEY;H0;H1;H2;H3;H4;H5;H6;H7;H8;H9;H10;H11;H12;H13;H14;H15;H16;H17;H18;H19;H20;H21;H22;H23" << endl;
    file_end   << "ADMUKEY;H0;H1;H2;H3;H4;H5;H6;H7;H8;H9;H10;H11;H12;H13;H14;H15;H16;H17;H18;H19;H20;H21;H22;H23" << endl;

    for(unsigned int m = 0; m < 589; m++ ) {

      file_start << Data::getInstance()->getMapIdMunIns().at(m);
      file_end << Data::getInstance()->getMapIdMunIns().at(m);

      for(unsigned int h = 0; h < 24; h++ ) {
        file_start << ";" << _n_activity_start_time_x_ins[m][h];
        file_end   << ";" << _n_activity_end_time_x_ins[m][h];
      }

      file_start << endl;
      file_end << endl;

    }

    file_start.close();
    file_end.close();

  }

}

void Model::saveODMatrix(){

  if (this->_proc == 0) { cout << "... saving origin-destination matrices" << endl; }

  // iterators over the local agents
  repast::SharedContext<Individual>::const_local_iterator it_beg = agents.localBegin();
  repast::SharedContext<Individual>::const_local_iterator it_end = agents.localEnd();

  int mun_id_start;                                  // id of starting municipality (1 -- 589)
  int mun_id_end;                                    // id of destination municipality (1 -- 589)
  int mun_ins_start;                                 // ins code of starting municipality
  int mun_ins_end;                                   // ins code of ending municipality
  int time_start;                                    // starting time of a trip
  unsigned long int local_od[589][589]    = {{ 0 }}; // origin-destination matrix between the 589 municipality
  unsigned long int local_od_mp[589][589] = {{ 0 }}; // origin-destination matrix between the 589 municipality (morning peak)
  unsigned long int local_od_ep[589][589] = {{ 0 }}; // origin-destination matrix between the 589 municipality (evening peak)

  // Reading data

  while ( it_beg != it_end ) {

    // loop over the individual activity chain
    for( unsigned int i = 1; i < (*it_beg)->getActChain().size() ; i++ ) {

      // municipalities of the trip
      mun_ins_start = Data::getInstance()->getNetwork().getNodes().at((*it_beg)->getActChain()[i-1].getNodeId()).getIns();
      mun_ins_end   = Data::getInstance()->getNetwork().getNodes().at((*it_beg)->getActChain()[i].getNodeId()).getIns();

      // if no correct mun_ins_start and mun_ins_end are found due to incorrect data (or node outside Belgium), current activity is discarded
      if ( mun_ins_start > 0  && mun_ins_end > 0 ) {

        mun_id_start = Data::getInstance()->getMapInsIdMun().at(mun_ins_start);
        mun_id_end   = Data::getInstance()->getMapInsIdMun().at(mun_ins_end);

        local_od[mun_id_start][mun_id_end]++;

        // check current if trip belongs to morning or evening trip
        time_start = secToHour((*it_beg)->getActChain()[i].getEndTime() - (*it_beg)->getActChain()[i].getDuration());
        if      ( time_start > 6  && time_start < 10 ) local_od_mp[mun_id_start][mun_id_end]++;
        else if ( time_start < 14 && time_start < 20 ) local_od_ep[mun_id_start][mun_id_end]++;

      }

    }

    it_beg++;

  }

  // Gathering the data from other processes

  boost::mpi::communicator* comm = RepastProcess::instance()->getCommunicator();
  comm->barrier();
  boost::mpi::reduce(*comm, *local_od,    589*589, *_origin_destination_matrix,    std::plus<int>(), 0);
  boost::mpi::reduce(*comm, *local_od_mp, 589*589, *_origin_destination_matrix_mp, std::plus<int>(), 0);
  boost::mpi::reduce(*comm, *local_od_ep, 589*589, *_origin_destination_matrix_ep, std::plus<int>(), 0);

  // Saving it in a file (only for root process)

  if( this->_proc == 0 ) {

    ostringstream oss_od;
    oss_od << "../output/origin_destination" << "_" << RepastProcess::instance()->getScheduleRunner().currentTick();
    string filename_od = oss_od.str();
    ofstream file_od(filename_od.c_str(), ios::out);

    ostringstream oss_od_mp;
    oss_od_mp << "../output/origin_destination_mp" << "_" << RepastProcess::instance()->getScheduleRunner().currentTick();
    string filename_od_mp = oss_od_mp.str();
    ofstream file_od_mp(filename_od_mp.c_str(), ios::out);

    ostringstream oss_od_ep;
    oss_od_ep << "../output/origin_destination_ep" << "_" << RepastProcess::instance()->getScheduleRunner().currentTick();
    string filename_od_ep= oss_od_ep.str();
    ofstream file_od_ep(filename_od_ep.c_str(), ios::out);

    ostringstream oss_od_array;
    oss_od_array << "../output/origin_destination_array" << "_" << RepastProcess::instance()->getScheduleRunner().currentTick();
    string filename_od_array= oss_od_array.str();
    ofstream file_od_array(filename_od_array.c_str(), ios::out);

    // writing header: O/D INS_1 INS_2 ... INS_589

    file_od    << "O/D;";
    file_od_mp << "O/D;";
    file_od_ep << "O/D;";

    for(unsigned int m = 0; m < 589; m++ ) {
      file_od    << Data::getInstance()->getMapIdMunIns().at(m);
      file_od_mp << Data::getInstance()->getMapIdMunIns().at(m);
      file_od_ep << Data::getInstance()->getMapIdMunIns().at(m);
    }

    file_od << endl;
    file_od_mp << endl;
    file_od_ep << endl;

    file_od_array << "Origin,Dest,Trips" << endl;

    // writing data

    for(unsigned int ms = 0; ms < 589; ms++ ) {

      file_od    << Data::getInstance()->getMapIdMunIns().at(ms);
      file_od_mp << Data::getInstance()->getMapIdMunIns().at(ms);
      file_od_ep << Data::getInstance()->getMapIdMunIns().at(ms);

      for(unsigned int me = 0; me < 589; me++ ) {

        file_od    << ";" << _origin_destination_matrix[ms][me];
        file_od_mp << ";" << _origin_destination_matrix_mp[ms][me];
        file_od_ep << ";" << _origin_destination_matrix_ep[ms][me];

        file_od_array << Data::getInstance()->getMapIdMunIns().at(ms) << "," << Data::getInstance()->getMapIdMunIns().at(me) << "," << _origin_destination_matrix[ms][me] << endl;

      }

      file_od    << endl;
      file_od_mp << endl;
      file_od_ep << endl;

    }

    // closing files

    file_od.close();
    file_od_mp.close();
    file_od_ep.close();
    file_od_array.close();

  }

}


