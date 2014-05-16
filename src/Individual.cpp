/****************************************************************
 * INDIVIDUAL.CPP
 * 
 * This file contains all the definitions of the methods of
 * individual.hpp (see this file for methods' documentation)
 * 
 * Authors: J. Barthelemy and L. Hollaert
 * Date   : 17 july 2012
 ****************************************************************/

#include "../include/Individual.hpp"

using namespace std;
using namespace repast;

Individual::Individual(repast::AgentId id, repast::AgentId hh_id,
    int municipality, char gender, int age_class, char education,
    char hh_relationship) :
    _id(id), _hh_id(hh_id), _municipality(municipality), _gender(gender), _age_class(
        age_class), _education(education), _hh_relationship(hh_relationship) {

  _age = -1;
  _house = -1;
  _driving_license = 'X';
  _sps_status = 'X';

}

Individual::Individual(repast::AgentId id, repast::AgentId hh_id,
    int municipality, char gender, int age_class, int age, char education) :
    _id(id), _hh_id(hh_id), _municipality(municipality), _gender(gender), _age_class(
        age_class), _age(age), _education(education) {

  _hh_relationship = 'X';
  _house = -1;
  _driving_license = 'X';
  _sps_status = 'X';

}

Individual::Individual(repast::AgentId id, repast::AgentId hh_id,
    int municipality, char gender, int age_class, char education,
    char hh_relationship, long house) :
    _id(id), _hh_id(hh_id), _municipality(municipality), _gender(gender), _age_class(
        age_class), _education(education), _hh_relationship(hh_relationship), _house(
        house) {

  _age = -1;
  _driving_license = 'X';
  _sps_status = 'X';

}

Individual::Individual(repast::AgentId id, repast::AgentId hh_id,
    int municipality, char gender, int age_class, int age, char education,
    char hh_relationship, long house) :
    _id(id), _hh_id(hh_id), _municipality(municipality), _gender(gender), _age_class(
        age_class), _age(age), _education(education), _hh_relationship(
        hh_relationship), _house(house) {

  _driving_license = 'X';
  _sps_status = 'X';

}


Individual::Individual(repast::AgentId id, repast::AgentId hh_id,
    int municipality, char gender, int age_class, char education,
    char sps_status, char driving_license, char hh_relationship,
    long house, std::vector<Activity> act_chain) :
    _id(id), _hh_id(hh_id), _municipality(municipality), _gender(gender),
    _age_class(age_class), _education(education), _sps_status(sps_status),
    _driving_license(driving_license), _hh_relationship(hh_relationship),
    _house(house), _act_chain(act_chain) {

  _age = -1;

}

Individual::Individual(repast::AgentId id, repast::AgentId hh_id,
    int municipality, char gender, int age_class, int age, char education,
    char sps_status, char driving_license, char hh_relationship, long house, std::vector<Activity> act_chain) :
    _id(id), _hh_id(hh_id), _municipality(municipality), _gender(gender),
    _age_class(age_class), _age(age), _education(education), _sps_status(sps_status),
    _driving_license(driving_license), _hh_relationship(hh_relationship),
    _house(house), _act_chain(act_chain) {

}



Individual::~Individual() {
}

void Individual::aging() {

  // increment age
  this->_age++;

  // computing age class
  if (this->_age < 6) {
    this->_age_class = 0;
  } else if (this->_age < 18) {
    this->_age_class = 1;
  } else if (this->_age < 40) {
    this->_age_class = 2;
  } else if (this->_age < 60) {
    this->_age_class = 3;
  } else {
    this->_age_class = 4;
  }

}

void Individual::initAge() {

  vector<long int> age_dis = Data::getInstance()->getMunAge(this->_municipality,
      this->_gender);

  int low = 0;
  int up = 0;

  switch (this->_age_class) {
    case 0: {
      low = 0;
      up = 5;
      break;
    }
    case 1: {
      low = 6;
      up = 17;
      break;
    }
    case 2: {
      low = 18;
      up = 39;
      break;
    }
    case 3: {
      low = 40;
      up = 59;
      break;
    }
    case 4: {
      low = 60;
      up = 110;
      break;
    }
  }

  this->_age = draw_discrete(age_dis, low, up);

}

bool Individual::isDying() {

  float proba_death = Data::getInstance()->getDeathProba(this->_gender, this->_age);
  float draw = rand() / float(RAND_MAX);

  if (draw <= proba_death) {
    return true;
  } else {
    return false;
  }

  return false;

}

/*Calculate thanks the random numbers if the women will give a child or not*/
bool Individual::givingbirth() {

  float proba_birth = Data::getInstance()->getBirthProba(this->_age);
  float draw = rand() / float(RAND_MAX);

  if (draw <= proba_birth) {
    //cout << "Individual is giving birth!" << endl;
    return true;
  } else {
    return false;
  }

}

//Constructor of a baby
Individual Individual::birthInd(long babyId) {

  float proba_boy = Data::getInstance()->getBirthsex(this->_age);
  float draw = rand() / float(RAND_MAX);

  int municipality;
  char gender;
  int age_class;
  int age;
  char education;
  char hh_relationship;
  long house;
  AgentId hh_id;
  AgentId ind_id(babyId, RepastProcess::instance()->rank(), MODEL_AGENT_IND_TYPE);

  if (draw <= proba_boy) {
    //cout << "Individual is a boy!" << endl;
    gender = 'H';

  } else {
    //cout << "Individual is a girl!" << endl;
    gender = 'F';
  }

  hh_id = this->getHhId();
  municipality = this->_municipality;
  age_class = 0;
  age = 0;
  education = 'O';
  hh_relationship = 'C';
  house = this->_house;

  Individual newInd(ind_id, hh_id, municipality, gender, age_class, age,
      education, hh_relationship, house);

  return newInd;

}


