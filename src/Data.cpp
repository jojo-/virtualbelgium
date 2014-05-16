/****************************************************************
 * DATA.CPP
 * 
 * This file contains all the definitions of the methods of
 * Data.hpp (see this file for methods' documentation)
 * 
 * Authors: J. Barthelemy and L. Hollaert
 * Date   : 17 july 2012
 ****************************************************************/

#include "../include/Data.hpp"

using namespace std;
using namespace repast;
using namespace tinyxml2;
using namespace boost;

void Data::read_mun_age_men() {

  vector<long> age_dis;                                           // age distribution
  int mun_id;                                                     // id of the municipality
  string a_line;                                                  // a line of data
  string filename = this->_props.getProperty("file.age_dis_men"); // path to data
  ifstream file(filename.c_str(), ios::in);                       // opening data file

  if (RepastProcess::instance()->rank() == 0) {
    cout << "... reading mun_age_men in " << filename << endl;
  }

  // file reading
  if (file) {
    while (getline(file, a_line)) {
      age_dis = split<long>(a_line, ";");                         // extracting the age distribution from the current line
      mun_id = age_dis[0];                                        // extracting municipality id
      age_dis.erase(age_dis.begin());                             // ... and deleting it from the age distribution vector
      _mun_age_men.insert(make_pair(mun_id, age_dis));            // saving data
    }
    file.close();
  } else {
    cerr << "Could not open " << filename << endl;
  }

}

void Data::read_mun_age_women() {

  vector<long> age_dis;                                             // age distribution
  int mun_id;                                                       // id of the municipality
  string a_line;                                                    // a line of data
  string filename = this->_props.getProperty("file.age_dis_women"); // path to data
  ifstream file(filename.c_str(), ios::in);                         // opening data file

  if (RepastProcess::instance()->rank() == 0) {
    cout << "... reading mun_age_women" << endl;
  }

  // file reading  
  if (file) {
    while (getline(file, a_line)) {
      age_dis = split<long>(a_line, ";"); // extracting the age distribution from the current line
      mun_id = age_dis[0];                         // extracting municipality id
      age_dis.erase(age_dis.begin()); // ... and deleting it from the age distribution vector
      _mun_age_women.insert(make_pair(mun_id, age_dis));          // saving data
    }
    file.close();
  } else {
    cerr << "Could not open " << filename << endl;
  }

}

void Data::read_death_age() {

  if (RepastProcess::instance()->rank() == 0) {
    cout << "... reading death_age_men and death_age_women" << endl;
  }

  vector<float> proba_death;                // distribution of death probability
  int age;                                                       // age of death
  string a_line;                                           // a line of the file
  string filename = this->_props.getProperty("file.mortality");  // path to data
  ifstream file(filename.c_str(), ios::in);                 // opening data file

  // file reading
  if (file) {
    while (getline(file, a_line)) {
      proba_death = split<float>(a_line, ";"); // extracting the death probability from the current line
      age = (int) proba_death[0];                              // extracting age
      proba_death.erase(proba_death.begin()); // ... and deleting it from the vector
      _death_age_men.insert(make_pair(age, proba_death[0])); // saving data (men)
      _death_age_women.insert(make_pair(age, proba_death[1])); // saving data (women)
    }
    file.close();
  } else {
    cerr << "Could not open " << filename << endl;
  }

}

void Data::read_birth_age() {

  if (RepastProcess::instance()->rank() == 0) {
    cout << "... reading birth_age" << endl;
  }

  vector<float> proba_birth;                // distribution of birth probability
  int age;                                                       // age of birth
  string a_line;                                           // a line of the file
  string filename = this->_props.getProperty("file.birth");      // path to data
  ifstream file(filename.c_str(), ios::in);                 // opening data file

  // file reading
  if (file) {
    while (getline(file, a_line)) {
      proba_birth = split<float>(a_line, ";"); // extracting the birth probability from the current line
      age = (int) proba_birth[0];                              // extracting age
      proba_birth.erase(proba_birth.begin()); // ... and deleting it from the vector
      _birth_age.insert(make_pair(age, proba_birth[0])); // saving data (birth probability)
      _birth_men.insert(make_pair(age, proba_birth[1])); // saving data (probability of a boy)
    }
    file.close();
  } else {
    cerr << "Could not open " << filename << endl;
  }

}

void Data::read_node_ins() {

  if (RepastProcess::instance()->rank() == 0) {
    cout << "... reading municipalities' nodes" << endl;
  }

  int          ins;
  long         node_id;
  vector<long> data;
  string       a_line;
  string       filename = this->_props.getProperty("file.node_ins");
  ifstream     file(filename.c_str(), ios::in);

  // file reading
  if (file) {
    while (getline(file, a_line)) {
      data = split<long>(a_line, ";");                            // reading a data line
      node_id = data[0];                                          // extracting id of the current node
      ins = data[1];                                              // extracting ins code of the municipality
      _map_ins_node.insert(make_pair(ins, node_id));              // saving data
      _map_node_ins.insert(make_pair(node_id, ins));
    }
    file.close();
  } else {
    cerr << "Could not open " << filename << endl;
  }

}

void Data::read_activity_cdb() {

  if (RepastProcess::instance()->rank() == 0) {
    cout << "... reading activities' codebook" << endl;
  }

  int codeInt;
  char codeChar;

  vector<string> data;
  string a_line;
  string filename = this->_props.getProperty("file.act_cdb");
  ifstream file(filename.c_str(), ios::in);

  if (file) {

    while (getline(file, a_line)) {

      // check if no '#" in the current line
      if (a_line.find("#") == string::npos) {

        data = split<string>(a_line, ";");
        codeChar = data[0].c_str()[0];
        codeInt = boost::lexical_cast<int>(data[1].c_str());

        this->_map_act_charToInt.insert(make_pair(codeChar, codeInt));
        this->_map_act_intToChar.insert(make_pair(codeInt, codeChar));

      }

    }

  }
  else {
    cerr << "Could not open " << filename << endl;
  }


}

void Data::read_network() {

  if (RepastProcess::instance()->rank() == 0) {
    cout << "... reading network" << endl;
  }

  // Loading XML file containing the network data.
  string filename = this->_props.getProperty("file.network");
  XMLDocument doc(filename.c_str());
  doc.loadFile(filename.c_str());

  // Parsing the node data
  XMLElement * ele = doc.FirstChildElement("network")->FirstChildElement(
      "nodes")->FirstChildElement("node");
  const XMLAttribute * attr;

  long id;
  double x;
  double y;
  int ins;

  double x_min = std::numeric_limits<float>::max();
  double y_min = std::numeric_limits<float>::max();
  double x_max = std::numeric_limits<float>::min();
  double y_max = std::numeric_limits<float>::min();

  while (ele) {

    // reading id
    attr = ele->FirstAttribute();
    id = attr->IntValue();

    // reading x coordinate
    attr = attr->Next();
    x = attr->DoubleValue();

    // determining min and max x coordinates
    if( x < x_min ) x_min = x; else if( x > x_max ) x_max = x;

    // reading y coordinate
    attr = attr->Next();
    y = attr->DoubleValue();

    // determining min and max y coordinates;
    if( y < y_min ) y_min = y; else if( y > y_max ) y_max = y;

    // finding ins code
    ins = this->_map_node_ins[id];

    // adding the node to the network
    Node currNode(id, x, y, ins);
    this->_network.addNode(currNode);

    ele = ele->NextSiblingElement("node");

  }

  // Parsing the link data
  ele = doc.FirstChildElement("network")->FirstChildElement("links")->FirstChildElement("link");

  long start_node;
  long end_node;
  float length;

  while (ele) {

    // reading id
    attr = ele->FirstAttribute();
    id = attr->IntValue();

    // reading starting node id
    attr = attr->Next();
    start_node = attr->IntValue();

    // reading ending node id
    attr = attr->Next();
    end_node = attr->IntValue();

    // ... and adding it to the list of end node of start node
    this->_network.addLinkOutToNode(start_node, id);

    // reading lenght
    attr = attr->Next();
    length = attr->FloatValue();

    // adding the link to the network
    Link currLink(id, start_node, end_node, length);
    this->_network.addLink(currLink);

    // moving to next link
    ele = ele->NextSiblingElement("link");

  }

  if (RepastProcess::instance()->rank() == 0) {
    cout << "    Network bounding box: x min " << x_min << ", x max " << x_max << ", y min " << y_min << ", y max " << y_max << endl;
  }

}

void Data::read_distribution_parameters_distance() {

  if (RepastProcess::instance()->rank() == 0) {
    std::cout << "... reading activities' distance distribution parameters" << endl;
  }

  // Opening the data file
  string filename = this->_props.getProperty("file.act_distance");
  ifstream file(filename.c_str(), ios::in);

  // Data related variables
  string a_line;                     // a line of the file
  vector<float> data;               // vector of data read
  dist_param dist;                   // parameter of the distance's distribution performed for a given activity
  int codeInt;                       // integer coding of the activity

  // Reading the data file if it exists
  if (file) {

    while (getline(file, a_line)) {

      // splitting the line
      data = split<float>(a_line, ";");

      // data extraction
      codeInt    = (int) data[0];    // integer code of purpose

      dist.mu    = data[2];          // getting mu
      dist.sigma = data[3];          // getting sigma
      dist.max   = data[5];          // getting upper bound

      // adding the distribution parameters to the simulation data
      this->_map_act_dist_par_dist.insert(make_pair(codeInt, dist));


    }

    // closing the file
    file.close();

  }
  else {
    cerr << "Could not open " << filename << endl;
  }

}




void Data::read_distribution_parameters_start_duration(){

  if (RepastProcess::instance()->rank() == 0 ) {
      cout << "... reading activities starting time x duration distribution parameters" << endl;
  }

  // Opening the data file
  string filename = this->_props.getProperty("file.act_start_duration");
  ifstream file(filename.c_str(), ios::in);

  // Data related variables
  string a_line;                     // a line of the file
  vector<float> data;               // vector of data read
  int codeInt;                       // integer coding of the activity
  int size;                          // size of the mixture

  // Data reading if file exists
  if (file) {

    // reading a line
    while (getline(file, a_line)) {

      // splitting the line
      data = split<float>(a_line, ";");

      // data extraction
      codeInt = (int) data[0];
      size    = (int) data[1];

      dist_param_mixture_2d dist;        // parameter of the mixture of bivariate distribution performed
      dist.components.resize(size);
      dist.p.resize(size);

      #ifdef DEBUGVBDATA
        cout << "DATA for " << codeInt << endl;
      #endif

      for( int i = 0; i < size; i++ ) {

        // means
        dist.components[i].mu[0] = data[2 + i];                  // mu_1
        dist.components[i].mu[1] = data[2 + size + i];           // mu_2

        // Covariance Matrix S
        dist.components[i].sigma[0] = data[2 + size*2 + i * 4];   // S_11
        dist.components[i].sigma[1] = data[4 + size*2 + i * 4];   // S_12
        dist.components[i].sigma[2] = data[5 + size*2 + i * 4];   // S_22

        // mixing proportions
        dist.p[i] = data[2 + size*2 + size*4 + i];

        #ifdef DEBUGVBDATA
          cout << "READING COMP " << i << endl;
          cout << " mu     " << dist.components[i].mu[0] << " " << dist.components[i].mu[1] << endl;
          cout << " sigma  " << dist.components[i].sigma[0] << " " << dist.components[i].sigma[1] << " " << dist.components[i].sigma[2] << endl;
          cout << " p      " << dist.p[i] << endl;
        #endif
      }

      // upper bounds
      dist.max[0] = data[2 + size*7];
      dist.max[1] = data[3 + size*7];

      #ifdef DEBUGVBDATA
        cout << " max     " << dist.max[0] << " " << dist.max[1] << endl;
      #endif

      this->_map_act_start_x_dur.insert(make_pair(codeInt,dist));

    }

    // closing file
    file.close();

  }
  else cerr << "Could not open " << filename << endl;

}

void Data::read_distribution_parameters_distance_x_duration_trip() {

  if (RepastProcess::instance()->rank() == 0 ) {
       cout << "... reading activities distance x duration of the trip distribution parameters" << endl;
   }

   // Opening the data file
   string filename = this->_props.getProperty("file.act_dist_x_dur_trip");
   ifstream file(filename.c_str(), ios::in);

   // Data related variables
   string a_line;                    // a line of the file
   vector<float> data;               // vector of data read
   int size;                         // size of the mixture
   dist_param_mixture_2d dist;       // parameter of the distribution

   // Data reading if file exists
   if (file) {

     // reading the line of data
     getline(file, a_line);
     data = split<float>(a_line, ";");
     size = (int) data[0];               // size of the mixture

     dist.components.resize(size);       // memory reservation
     dist.p.resize(size);

     for( int i = 0; i < size; i++ ) {

       // means
       dist.components[i].mu[0] = data[1 + i];                   // mu_1
       dist.components[i].mu[1] = data[1 + size + i];            // mu_2

       // Covariance matrix S
       dist.components[i].sigma[0] = data[1 + size*2 + i * 4];   // S_11
       dist.components[i].sigma[1] = data[3 + size*2 + i * 4];   // S_12
       dist.components[i].sigma[2] = data[4 + size*2 + i * 4];   // S_22

       // mixing proportions
       dist.p[i] = data[1 + size*2 + size*4 + i];

       #ifdef DEBUGVBDATA
         cout << "READING COMP " << i << endl;
         cout << " mu     " << dist.components[i].mu[0] << " " << dist.components[i].mu[1] << endl;
         cout << " sigma  " << dist.components[i].sigma[0] << " " << dist.components[i].sigma[1] << " " << dist.components[i].sigma[2] << endl;
         cout << " p      " << dist.p[i] << endl;
       #endif

     }

     // upper bounds
     dist.max[0] = data[1 + size*7];
     dist.max[1] = data[2 + size*7];

     #ifdef DEBUGVBDATA
       cout << " max     " << dist.max[0] << " " << dist.max[1] << endl;
     #endif

     this->_act_dist_x_dur_trip_dist = dist;

     // closing file
     file.close();

   }
   else cerr << "Could not open " << filename << endl;

}



void Data::read_distribution_parameters_house_tdep() {

  if (RepastProcess::instance()->rank() == 0) {
    cout << "... reading activities house departure time distribution parameters" << endl;
  }

  // Opening the data file
  string filename = this->_props.getProperty("file.act_tdep_house");
  ifstream file(filename.c_str(), ios::in);

  // Data related variables
  string a_line;                     // a line of the file
  vector<float> data;               // vector of data read
  dist_param_mixture dist;           // parameter of the distance's distribution performed for a given activity
  int codeInt;                       // integer coding of the activity
  int size;                          // size of the mixture

  // Reading the data file if it exists
  if (file) {

    while (getline(file, a_line)) {

      // splitting the line
      data = split<float>(a_line, ";");

      // data extraction
      codeInt    = (int) data[0];         // integer code of purpose
      size       = (int) data[1];         // size of the mixture

      dist.mu.resize(size);
      dist.sigma.resize(size);
      dist.p.resize(size);

      for( int i = 0; i < size; i++ ) {
        dist.mu[i]    = data[2+i];        // vector of means
        dist.sigma[i] = data[2+size+i];   // vector of standard deviation
        dist.p[i]     = data[2+2*size+i]; // vector of mixing proportions
      }

      dist.max = data[2+3*size];          // upper bound

      // adding the distribution parameters to the simulation data
      this->_map_act_tdep_par_dist.insert(make_pair(codeInt, dist));

    }
    // closing the file
    file.close();
  }
  else {
    cerr << "Could not open " << filename << endl;
  }


}

void Data::read_indicators() {

  if (RepastProcess::instance()->rank() == 0) {
      cout << "... reading municipalities' indicator" << endl;
  }

  // Opening data file

  string       filename = this->_props.getProperty("file.indicators");         // file name
  ifstream     file(filename.c_str(), ios::in);                                // opening the data file
  string       a_line;                                                         // a line of data
  vector<long> data;                                                           // data

  // Reading data file

  if ( file ) {
    while (getline(file, a_line)) {

      data = split<long>(a_line, ";");                                         // extracting the data from current line
      this->_indic_mun_size.insert(make_pair(data[0], data[1]));               // saving it in _indic_mun_size

    }
    file.close();
  } else {
    cerr << "Could not open " << filename << endl;
  }

}

void Data::read_ins_id_mun() {

  if (RepastProcess::instance()->rank() == 0) {
       cout << "... reading municipality id and ins code..." << endl;
  }

  // Opening data file

  string filename = this->_props.getProperty("file.ins_id_code");
  ifstream file(filename.c_str(), ios::in);
  string a_line;
  vector<string> data;

  int ins;    // ins code of the municipality
  int id;     // numerical id of the municipality (1 to 589)

  if (file) {

    while (getline(file, a_line)) {

        data = split<string>(a_line, ";");
        id   = boost::lexical_cast<int>(data[1].c_str()) - 1;                 // '-1' since ids start at 1
        ins  = boost::lexical_cast<int>(data[2].c_str());

        this->_map_ins_id_mun.insert(make_pair(ins, id));
        this->_map_id_mun_ins.insert(make_pair(id, ins));
    }

  }
  else {
      cerr << "Could not open " << filename << endl;
  }

}

vector<long int> Data::getMunAge(int municipality, char gender) {

  if (gender == 'M') { return _mun_age_men[municipality];   }                  // Men
  else               { return _mun_age_women[municipality]; }                  // Women

}

float Data::getDeathProba(char gender, int age) {

  if (gender == 'M') { return _death_age_men[age];   }                         // Men
  else               { return _death_age_women[age]; }                         // Women

}

float Data::getBirthProba(int age) {
  return _birth_age[age];
}

float Data::getBirthsex(int age) {
  return _birth_men[age];
}

vector<long> Data::getNodesIdFromIns(int aIns) {

  vector<long> result;

  typedef multimap<int, long>::const_iterator Iter;
  pair<Iter, Iter> it = this->_map_ins_node.equal_range(aIns);

  for (Iter i = it.first; i != it.second; i++) {
    result.push_back(i->second);
  }

  return result;

}

long Data::getOneNodeIdFromIns(int aIns) {

  long result = -1;
  vector<long> list_node_id = this->getNodesIdFromIns(aIns);

  try {
    result = list_node_id[draw_discrete(list_node_id)];
  }
  catch ( const std::exception & e )
  {
    std::cerr << e.what() << "- no nodes for ins " << aIns << endl;
  }

  return result;

}

dist_param Data::getActDistParDist(int aActivityType) {

  return this->_map_act_dist_par_dist[aActivityType];

}

dist_param_mixture Data::getActHouseTDepParDist(int aActivityType ) {

  return this->_map_act_tdep_par_dist[aActivityType];

}

dist_param_mixture Data::getActDurationCondiStartParDist(int aActivityType, int aStartTime) {

  dist_param_mixture result;

  result.mu.resize(this->_map_act_start_x_dur[aActivityType].p.size());
  result.sigma.resize(this->_map_act_start_x_dur[aActivityType].p.size());

  result.max = this->_map_act_start_x_dur[aActivityType].max[1];
  result.p   = this->_map_act_start_x_dur[aActivityType].p;

  for( unsigned int i = 0; i < this->_map_act_start_x_dur[aActivityType].p.size(); i++ ) {

    float mu_1     = this->_map_act_start_x_dur[aActivityType].components[i].mu[0];
    float mu_2     = this->_map_act_start_x_dur[aActivityType].components[i].mu[1];
    float sigma_11 = this->_map_act_start_x_dur[aActivityType].components[i].sigma[0];
    float sigma_12 = this->_map_act_start_x_dur[aActivityType].components[i].sigma[1];
    float sigma_22 = this->_map_act_start_x_dur[aActivityType].components[i].sigma[2];

    result.mu[i]    = mu_2 + (sigma_12 / sigma_11) * (log(aStartTime) - mu_1);  // log transform for aStartTime
    result.sigma[i] = sigma_22 - ( ( sigma_12 * sigma_12) / sigma_11 );

  }

  return result;

}

dist_param_mixture Data::getDurationCondiDistTripParDist(int aDistance) {

  dist_param_mixture result;

  result.mu.resize(this->_act_dist_x_dur_trip_dist.p.size());
  result.sigma.resize(this->_act_dist_x_dur_trip_dist.p.size());

  result.max = this->_act_dist_x_dur_trip_dist.max[1];
  result.p   = this->_act_dist_x_dur_trip_dist.p;

  for( unsigned int i = 0; i < this->_act_dist_x_dur_trip_dist.p.size(); i++ ) {

    float mu_1     = this->_act_dist_x_dur_trip_dist.components[i].mu[0];
    float mu_2     = this->_act_dist_x_dur_trip_dist.components[i].mu[1];
    float sigma_11 = this->_act_dist_x_dur_trip_dist.components[i].sigma[0];
    float sigma_12 = this->_act_dist_x_dur_trip_dist.components[i].sigma[1];
    float sigma_22 = this->_act_dist_x_dur_trip_dist.components[i].sigma[2];

    result.mu[i]    = mu_2 + (sigma_12 / sigma_11) * (log(aDistance) - mu_1);  // log transform for aDistance
    result.sigma[i] = sigma_22 - ( ( sigma_12 * sigma_12) / sigma_11 );

  }

  return result;

}

// Aggregate output data class

AggregateSum::AggregateSum() {
  _sum = 0;
}

void AggregateSum::increment() {
  _sum++;
}

void AggregateSum::reset() {
  _sum = 0;
}

int AggregateSum::getData() {
  return _sum;
}

// Some useful tools

long int linesCount(string filename) {

  ifstream file(filename.c_str(), ios::in);                    // opening the file
  int lines = 0;                                               // number of lines

  // file reading
  if (file) {
    while (file.ignore(numeric_limits<int>::max(), '\n')) {
      lines++;                                                 // counting lines
    }
    file.close();
  } else {
    cerr << "Could not open " << filename << endl;
  }

  return lines;

}
