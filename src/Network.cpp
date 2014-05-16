/****************************************************************
 * NETWORK.CPP
 *
 * This file contains all the definitions of the methods of
 * network.hpp (see this file for methods' documentation)
 *
 * Authors: J. Barthelemy and L. Hollaert
 * Date   : 26 jun 2013
 ****************************************************************/

#include "../include/Network.hpp"


using namespace std;

// Default Constructor (binary heap)
DHeap::DHeap() : _d(2) {
}

// Constructor specifying size of the heap and D
DHeap::DHeap(unsigned long size, unsigned int d) {

  // if wrong size is specified, d is set to 2 (binary heap)
  ( d > 2 ) ? this->_d = d : this->_d = 2;

  //memory reservation
  this->heap.reserve(size);

}

// Constructor specifying root, size and d parameter of the heap
DHeap::DHeap(unsigned int d, long node_id, std::map<long, Node> &Nodes) :
    _d(d) {

  // Memory reservation
  this->heap.reserve(Nodes.size());

  // Source node's key is set to 0
  Nodes[node_id].setKey(0);
  this->insert(Nodes[node_id]);

  // Every other nodes' key are set to a large number
  map<long, Node>::iterator itr;
  for (itr = Nodes.begin(); itr != Nodes.end(); itr++) {
    if (itr->first != node_id) {
      Nodes[itr->first].setKey(std::numeric_limits<float>::max());
      this->pushBack(Nodes[itr->first]);
    }
  }

}

// Destructor
DHeap::~DHeap() {
}

// Return parent's id of a node
int DHeap::parent(int child) {

  if (child != 0) {
    return (child - 1) / this->_d;         // (child - 1) / d
  }

  return -1;

}

// Return the ith child id
int DHeap::child(int parent, int ith) {

  if( ith > this->_d ) return -1;            // assess that i in [1,d]

  int i = ( this->_d * parent + ith );       // (d * parent) + i
  return (i < (int) heap.size()) ? i : -1;

}

// Ensure that the structure is a n-ary heap (bottom-up)
void DHeap::heapifyup(int index) {

  while ((index > 0) && (parent(index) >= 0) && (heap[parent(index)] > heap[index])) {

    // swap parent and child nodes
    std::swap(heap[parent(index)],heap[index]);
    this->_map_nodeid_index[heap[parent(index)].getId()] = parent(index);
    this->_map_nodeid_index[heap[index].getId()] = index;

    // going up in the data structure
    index = parent(index);

   }

}

// Ensure that the structure is a n-ary heap (up-bottom)
void DHeap::heapifydown(int index) {

  int c = child(index,1);

  // choix du fils ayant la valeur la plus elevee
  for( int i = 2; i <= this->_d; i++ ) {

    if( c > 0 && child(index,i) > 0 && heap[c] > heap[child(index,i)] ) {
      c = child(index,i);
    }

  }

  // if left > right, swap them
  if (c > 0) {

    // swapping
    std::swap(heap[index],heap[c]);
    this->_map_nodeid_index[heap[index].getId()] = index;
    this->_map_nodeid_index[heap[c].getId()] = c;

    // going down in the data structure
    heapifydown(c);
  }

}

// Decreasing the an element's key
void DHeap::decreaseKey(int index, float key) {

  this->heap[index].setKey(key);
  heapifyup(index);

}

// Inserting an element to the heap
void DHeap::insert(Node &element) {

#ifdef __GXX_EXPERIMENTAL_CXX0X__
  heap.emplace_back(element);                     // if c++11 support is enabled
#else
  heap.push_back(element);
#endif

  _map_nodeid_index.insert(make_pair(element.getId(), heap.size() - 1));
  heapifyup(heap.size() - 1);

}

// Inserting an element at the end of the heap
void DHeap::pushBack(Node &element) {

#ifdef __GXX_EXPERIMENTAL_CXX0X__
  heap.emplace_back(element);                    // if c++11 support is enabled
#else
  heap.push_back(element);
#endif

  _map_nodeid_index.insert(make_pair(element.getId(), heap.size() - 1));

}

// Returning the root of the heap (and removing it)
const Node DHeap::deletemin() {

  Node min = heap.front();                       // extracting the first node's data of the vector
  heap[0] = heap.at(heap.size() - 1);            // putting the last element of the vector at the front
  heap.pop_back();                               // removing it

  this->_map_nodeid_index[heap[0].getId()] = 0;
  this->_map_nodeid_index.erase(min.getId());    // deleting it from the map

  heapifydown(0);                                // insure that the d-ary heap structure is preserved

  return min;

}

// Insert a node in the network
void Network::addNode(Node aNode) {

  this->_Nodes.insert(make_pair(aNode.getId(), aNode));

}

// Insert a link in the network
void Network::addLink(Link aLink) {

  this->_Links.insert(make_pair(aLink.getId(), aLink));

}

// Retrieve a set of nodes of a given distance from a source node
long Network::getDestFromSource(long source_id, float dist) {

   vector<long> result;                  // resulting set of nodes
   float        epsilon = 250.0;         // error term, unit: meters

   // Loop until at least one feasible node is found
   while (result.size() < 1) {
   //while (true) {

     // Init
     FibonacciHeap<long,float> Q;                              // Fibonnacci heap
     std::map< long, FibonacciHeapNode<long,float>* > Q_nodes; // array of pointer to the nodes of the F-heap
     std::set<long> Q_marked;                                  // set of marked nodes

     // ... root node key set to 0
     Q_nodes[source_id] = Q.insert(source_id,0);
     Q_marked.insert(source_id);

     // ... every other nodes' key are set to a large number
     map<long, Node>::iterator itr;
     for (itr = this->_Nodes.begin(); itr != this->_Nodes.end(); itr++) {
       if (itr->first != source_id) {
         Q_nodes.insert( std::pair< long, FibonacciHeapNode<long,float>* >( itr->first, Q.insert(itr->first,std::numeric_limits<float>::max()) ) );
       }
     }

     float d = 0.0;  // distance from source

     // Dijkstra loop
     while( Q.empty() == false && ( d < dist + epsilon ) ) {

       // ... extracting the node with minimum key (i.e. distance from source)

       long id_min = Q.minimum()->data();      // id
       d           = Q.minimum()->key();       // distance
       Node * i    = &this->_Nodes.at(id_min); // get a pointer to the min node
       Q_marked.insert(id_min);                // mark the node

       // ... if the current node's key is in the desirable interval [dist +/- epsilon], adding it to the result
       if ( (d > dist - epsilon) && (d < dist + epsilon) ) {
         result.push_back(id_min);
       }

       // ... updating the distances between starting node and node i's sink nodes if necessary
       for (unsigned int j = 0; j < i->getLinksOutId().size(); j++) {
	 
         long id = this->_Links[i->getLinksOutId().at(j)].getEndNodeId();

         // ... if node not already marked
         if ( Q_marked.find(id) == Q_marked.end() ) {

           float w_ij  = this->_Links[i->getLinksOutId().at(j)].getLength() + d; // new weight
           float key_j = Q_nodes[id]->key();                                     // current weight

           // ... update the weight if necessary
           if ( w_ij < key_j ) Q.decreaseKey(Q_nodes[id],w_ij);

         }

       }

       // ... removing the min node from the heap
       Q.deletemin();

     }

     // ... increasing the error if no feasible node has been found
     epsilon = epsilon * 2.0;

   }

   // Randomly returning a node
   unsigned int index = RandomGenerators::getInstance()->unif.int32() % (result.size());
   return result[index];

}

float Network::getDistanceNodes(long source_id, long dest_id) {

  float d = 0.0;                        // distance from source
  long curr_node = source_id;           // current node found by Dijkstra algorithm

  // Init
  FibonacciHeap<long,float> Q;                              // Fibonnacci heap
  std::map< long, FibonacciHeapNode<long,float>* > Q_nodes; // array of pointer to the nodes of the F-heap
  std::set<long> Q_marked;                                  // set of marked nodes

  // ... root node key set to 0
  Q_nodes[curr_node] = Q.insert(curr_node,0);
  Q_marked.insert(curr_node);

  // ... every other nodes' key are set to a large number
  map<long, Node>::iterator itr;
  for (itr = this->_Nodes.begin(); itr != this->_Nodes.end(); itr++) {
    if (itr->first != curr_node) {
      Q_nodes.insert( std::pair< long, FibonacciHeapNode<long,float>* >( itr->first, Q.insert(itr->first,std::numeric_limits<float>::max()) ) );
    }
  }

  // Dijkstra loop
  while( curr_node != dest_id ) {

    // ... extracting the node with minimum key (i.e. distance from source)

    curr_node   = Q.minimum()->data();         // id
    d           = Q.minimum()->key();          // distance
    Node * i    = &this->_Nodes.at(curr_node); // get a pointer to the min node
    Q_marked.insert(curr_node);                // mark the node

    // ... updating the distances between starting node and node i's sink nodes if necessary
    for (unsigned int j = 0; j < i->getLinksOutId().size(); j++) {

      long id = this->_Links[i->getLinksOutId().at(j)].getEndNodeId();

      // ... if node not already marked
      if ( Q_marked.find(id) == Q_marked.end() ) {

        float w_ij  = this->_Links[i->getLinksOutId().at(j)].getLength() + d; // new weight
        float key_j = Q_nodes[id]->key();                                     // current weight

        // ... update the weight if necessary
        if ( w_ij < key_j ) Q.decreaseKey(Q_nodes[id],w_ij);

      }

    }

    // ... removing the min node from the heap
    Q.deletemin();

  }

  return d;

}


// Constructor
Node::Node(long id, double x, double y, int ins) :
    _id(id), _x(x), _y(y), _ins(ins), _key(-1), _index(-1), _indicators() {
}


// Adding an indicator to a node
void Node::addIndicator(string aIndicator, long aIndicatorValue) {
  this->_indicators.insert(make_pair(aIndicator,aIndicatorValue));
}


// Overloading '<' for Node class
bool operator<(const Node &aNode1, const Node &aNode2) {
  return (aNode1._key < aNode2._key);
}


// Overloading '>' for Node class
bool operator>(const Node &aNode1, const Node &aNode2) {
  return (aNode1._key > aNode2._key);
}


// Constructor
Link::Link(long id, long start_node, long end_node, float length) :
    _id(id), _start_node_id(start_node), _end_node_id(end_node), _length(length) {
}
