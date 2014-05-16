/****************************************************************
 * HOUSEHDOLD.CPP
 *
 * This file contains all the definitions of the methods of
 * Household.hpp (see this file for methods' documentation)
 *
 * Authors: J. Barthelemy and L. Hollaert
 * Date   : 17 july 2012
 ****************************************************************/

#include "../include/Household.hpp"

using namespace std;
using namespace repast;

Household::Household(repast::AgentId id, int ins,
    std::vector<repast::AgentId> list_ind, std::string type, int n_children, int n_adults) :
    _id(id), _ins(ins), _list_ind(list_ind), _type(type), _n_children(
        n_children), _n_adults(n_adults) {
  _house = -1;
}

Household::Household(repast::AgentId id, int ins,
    std::vector<repast::AgentId> list_ind, std::string type, int n_children, int n_adults,
    long house) :
    _id(id), _ins(ins), _list_ind(list_ind), _type(type), _n_children(
        n_children), _n_adults(n_adults), _house(house) {
}

Household::~Household() {
}

//retourne les individus du menage -> doit se faire sous forme de liste
//d'id d'agents! -> erreur ici!!!!!!!!!
vector<Individual *> Household::getIndividuals(SharedContext<Individual> & agentsInd) {

  vector<Individual *> individuals;

  for (unsigned int i = 0; i < this->_list_ind.size(); i++) {
    individuals.push_back(agentsInd.getAgent(_list_ind[i]));
  }

  return individuals;

}

//calcul du type de menage (suite a la modification d'un menage. exemple : décès, mariage,etc.)
void Household::computeHhType(SharedContext<Individual> & agentsInd) {

  int n_mate = 0;
  int n_children = 0;
  int n_adults = 0;

  char head_gender;

  this->getIndividuals(agentsInd)[0]->setHhRelationship('H'); // TODO faire attention aux orphelins (et autres adultes)!
  agentsInd.getAgent(this->getListInd()[0])->setHhRelationship('H');

  for (unsigned int i = 1; i < this->getIndividuals(agentsInd).size(); i++) {

    switch (this->getIndividuals(agentsInd)[i]->getHhRelationship()) {
      case 'M':
        n_mate++;
        break;
      case 'C':
        n_children++;
        break;
      case 'A':
        n_adults++;
        break;
      default:
        break;
    }
  }

  head_gender = this->getIndividuals(agentsInd)[0]->getGender();

  if (n_mate == 0) {
    if (n_children == 0) {
      if (head_gender == 'M') {
        this->_type = "IH";
      } else {
        this->_type = "IF";
      }
    } else {
      if (head_gender == 'M') {
        this->_type = "M";
      } else {
        this->_type = "W";
      }
    }
  } else {
    if (n_children == 0) {
      this->_type = "C";
    } else {
      this->_type = "F";
    }
  }

  this->_n_children = n_children;
  this->_n_adults = n_adults;

}

// supprime l'identifiant d'un individu dans la liste d'individus du ménage (suite à une modification du ménage. exemple : décès, mariage,etc.  )
void Household::removeIndFromList(AgentId aId) {

  vector<AgentId>::iterator idToRemove = find(this->_list_ind.begin(), this->_list_ind.end(), aId); // todo:erreur detectee ici (this = 0x0) menage serait supprime avant individu?
  this->_list_ind.erase(idToRemove);

}

void Household::localizeHouse(Data dataset) {

  // extracting the municipality
  this->_house = dataset.getOneNodeIdFromIns(this->_ins);

}

void Household::addBaby( AgentId aBabyId ) {

  vector<repast::AgentId>::iterator it = this->_list_ind.end();

  it = it - this->_n_adults;

  this->_list_ind.insert(it, aBabyId);


}


