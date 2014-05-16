/****************************************************************
 * NETWORK.HPP
 *
 * This file contains all the data related class and methods.
 *
 * Authors: J. Barthelemy
 * Date   : 08 april 2013
 ****************************************************************/

/*! \file Network.hpp
    \brief Road network related class and methods (Heap, Node, Link and Network classes).
 */

#ifndef NETWORK_HPP_
#define NETWORK_HPP_

#include <map>
#include <vector>
#include <iostream>
#include <functional>
#include <limits>
#include <utility>
#include <sstream>
#include <set>
#include "Random.hpp"
#include "FiboHeap.hpp"

//! A node class.
/*!
 This class intends to represents the nodes of a road network.
 Additionaly to the basic attributes (id, coordinates, outgoing links),
 some members of the node are related to the municipality to which it belongs
 (ins, indicators) and (key, index) and Dijkstra shortest path algorithm 
 (key, index).
 */
class Node {

private:

  long   _id;                       //!< id of the node.
  double _x;                        //!< x coordinate.
  double _y;                        //!< y coordinate.
  int    _ins;                      //!< ins code of the node, corresponding to the municipality to which the node belongs.


  std::vector<long> _links_out_id;  //!< set of outgoing links.
  float             _key;           //!< key value of a node, corresponding to the distance between the node and a source one
  int               _index;         //!< given the value of key, correspond to the index of a node inside a D-Heap



  std::map<std::string,long> _indicators;    //!< a map of indicators related to the municipality to which the node belongs.


public:

  //! Default Constructor.
  Node() : _id(0), _x(0), _y(0), _ins(0), _links_out_id(), _key(-1), _index(-1), _indicators() {};

  //! Constructor.
  /*!
    \param id a node id
    \param x x coordinate
    \param y y coordinate
    \param ins ins code of node's municipality
   */
  Node(long id, double x, double y, int ins);

  //! Destructor.
  virtual ~Node() {};

  //! Return the id of the node.
  /*!
    \return the node id
   */
  long getId() const {
    return _id;
  }

  //! Set the id of the node.
  /*!
    \param id a new node id
   */
  void setId(long id) {
    _id = id;
  }

  //! Return the INS code of the node (corresponding to the municipality to which the node belongs).
  /*!
    \return a municipality ins code
   */
  int getIns() const {
    return _ins;
  }

  //! Set the INS code of the node (corresponding to the municipality to which the node belongs).
  /*!
    \param ins a new ins code
   */
  void setIns(int ins) {
    _ins = ins;
  }

  //! Return the set of outgoing links of the node.
  /*!
    \return a vector of link id
   */
  const std::vector<long>& getLinksOutId() const {
    return _links_out_id;
  }

  //! Setter for the set of outgoing links of the node.
  /*!
    \param linksOutId a new set of outgoing links id
   */
  void setLinksOutId(const std::vector<long>& linksOutId) {
    _links_out_id = linksOutId;
  }

  //! Add an link to the set of outgoing links.
  /*!
    \param linkId a link id
   */
  void addLinkOutId( long linkId ) {
    _links_out_id.push_back(linkId);
  }

  //! Get the key value (used by Dijkstra algorithm).
  /*!
    \return node's key value
   */
  float getKey() const {
    return _key;
  }

  //! Setting the key value (used by Dijkstra algorithm).
  /*!
    \param key a new key value
   */
  void setKey(float key) {
    _key = key;
  }

  //! Return the x coordinate.
  /*!
    \return x coordinate
   */
  double getX() const {
    return _x;
  }

  //! Set the x coordinate.
  /*!
    \param x a new node's x coordinate
   */
  void setX(double x) {
    _x = x;
  }

  //! Return the y coordinate.
  /*!
    \return y coordinate
   */
  double getY() const {
    return _y;
  }

  //! Set the y coordinate.
  /*!
    \param y a new node's y coordinate
   */
  void setY(double y) {
    _y = y;
  }

  //! Get the index value (used by Dijkstra algorithm).
  /*!
    \return node's index value 
   */
  int getIndex() const {
    return _index;
  }

  //! Set the index value (used by Dijkstra algorithm).
  /*!
    \param index a new index value
   */
  void setIndex(int index) {
    _index = index;
  }

  //! Return the indicators of the node.
  /*!
    \return the map of node's service indicators used to geo-localize activities
   */
  const std::map<std::string, long>& getIndicators() const {
    return _indicators;
  }

  //! Set the indicators of the node.
  /*!
    \param indicators a new map of services indicators
   */
  void setIndicators(const std::map<std::string, long>& indicators) {
    _indicators = indicators;
  }

  //! Add an indicator and its value to the set of node's indicator.
  /*!
    \param aIndicator an indicator id
    \param aIndicatorValue the value associated with the indicator
   */
  void addIndicator(std::string aIndicator, long aIndicatorValue );

  //! Overloading '<' operator.
  /*!
    Operator '<' compare nodes' key attribute.

    /param aNode1 the first node
    /param aNode2 the second node
   */
  friend bool operator<(const Node &aNode1, const Node &aNode2);

  //! Overloading '>' operator.
  /*!
    Operator '>' compare nodes' key attribute.

    /param aNode1 the first node
    /param aNode2 the second node
   */
  friend bool operator>(const Node &aNode1, const Node &aNode2);

  //! Overloading '<' operator.
  /*!
    Operator '<' compare nodes' key attribute.

    /param aNode1 the first node
    /param aNode2 the second node
   */
  bool operator<(Node &aNode) {
    return this->_key < aNode.getKey();
  }

  //! Overloading '>' operator.
  /*!
    Operator '>' compare nodes' key attribute.

    /param aNode1 the first node
    /param aNode2 the second node
   */
  bool operator>(Node &aNode) {
    return this->_key > aNode.getKey();
  }

};


//! A link class.
/*!
 This class intends to represents the links of a road network.
 A link is represented by an id, a source node, a sink node and
 a length (in meters).
 */
class Link {

private:

  long  _id;                 //!< id of the link.
  long  _start_node_id;      //!< source node's id.
  long  _end_node_id;        //!< sink node's id.
  float _length;             //!< length of the link (unit: meters).

public:

  //! Default constructor.
  Link() : _id(0), _start_node_id(0), _end_node_id(0), _length(0) {};

  //! Constructor.
  /*!
    \param id link's id
    \param start_node link's source node
    \param end_node link's sink node
    \param length links length
   */
  Link(long id, long start_node, long end_node, float length);

  //! Destructor.
  virtual ~Link() {};

  //! Return the sink node's id.
  /*!
    \return a node id
   */
  long getEndNodeId() const {
    return _end_node_id;
  }

  //! Set the sink node's id.
  /*!
    \param endNodeId a node id
   */
  void setEndNodeId(long endNodeId) {
    _end_node_id = endNodeId;
  }

  //! Return the link's id.
  /*!
    \return link's id
   */
  long getId() const {
    return _id;
  }

  //! Set the link's id.
  /*!
    \param id the link id
   */
  void setId(long id) {
    _id = id;
  }

  //! Get the length (in meters) of the link.
  /*!
    \return link's length
   */
  float getLength() const {
    return _length;
  }

  //! Set the length of the link.
  /*!
    \param length a link length
   */
  void setLength(float length) {
    _length = length;
  }

  //! Return the source node's id.
  /*!
    /return a node id
   */
  long getStartNodeId() const {
    return _start_node_id;
  }

  //! Set the source node's id.
  /*!
    \param startNodeId a node id
   */
  void setStartNodeId(long startNodeId) {
    _start_node_id = startNodeId;
  }

};


//! A n-ary tree data structure used by Dijkstra's shortest path algorithm
/*!
  DHeap is a n-ary tree that stores priorities (or priority-element pairs) at the nodes.
  It has the following properties:
  - All levels except last level are full. Last level is left filled.
  - Priority of a node is at least as large as that of its parent.
  A heap can be thought of as a priority queue. The most important node will always
  be at the root of the tree.
  
  References: http://www.cprogramming.com/tutorial/computersciencetheory/heap.html
 */
class DHeap {

private:

  std::vector<Node> heap;                  //!< heap of Node.
  std::map<long,int> _map_nodeid_index;    //!< map of the node's id and their respective index.
  int _d;                                  //!< degree of the heap, i.e. maximum number of childxs

  //! Return the index of the parent of child node.
  /*!
    \param child id
   */
  int parent(int child);

  //! Return the index of i-th child of a node.
  /*!
    \param parent parent id
    \param ith index of the child
   */
  int child(int parent, int ith);

  //! Ensure the heap invariant (bottom -> up), used to add a node to the heap.
  /*!
    To add a node, it is inserted at the last empty space and heapifyup process is done.
    When a node is added, its key is compared to its parent. If parent key is smaller than
    the current node it is swapped. The process is repeated till the heap property is met.

    \param index starting heap index of the procedure
  */
  void heapifyup(int index);

  //! Ensure the heap invariant (up -> bottom), used during removal of a node.
  /*!
    When a node is removed which is always the root (lowest in priority) the last available
    node in heap is replaced as the root and heapifydown process is done.
    The key of parent node is compared with the children. If any of the children have lower
    priority it is swapped with the parent. The process is repeated for the newly swapped
    node till the heap property is met again.

    \param index starting heap index of the procedure
  */
  void heapifydown(int index);

public:

  //! Default constructor.
  DHeap();

  //! Constructor specifying heap maximum size.
  /*!
    /param size heap's size (preallocating memory)
    /param d the maximum number of child for each nodes
   */
  DHeap(unsigned long size, unsigned int d);

  //! Constructor specifying heap maximum size and its root node (with key = 0).
  /*!
    \param d degree of the heap
    \param node_id root node id
    \param Nodes set of the network's nodes (used to get its size)
   */
  DHeap(unsigned int d, long node_id, std::map<long, Node> &Nodes);

  //! Destructor.
  virtual ~DHeap();

  //! Insert a node in the heap.
  /*!
    \param element the node to insert in the heap.
   */
  void insert(Node &element);

  //! Insert a node at the end of the heap.
  /*!
    \param element the node to be inserted at the end of the heap.
   */
  void pushBack(Node &element);

  //! Return the node's id with maximum priority (i.e. the first of the heap) and removes it.
  /*!
    \return the node associated with the minimum key
   */
  const Node deletemin();

  //! Decrease the key of a given Node and update the heap structure.
  /*!
    \param index index of the node whose key is to be decreased
    \param key new key value of the node 
   */
  void decreaseKey( int index, float key );

  //! Return the map <node id, node index>.
  /*!
    \return the heap's map <node id, node index>
   */
  std::map<long, int> getMapNodeidIndex() const {
    return _map_nodeid_index;
  }

  //! Compute the size of the heap.
  /*!
    \return size of the heap
   */
  int size() {
    return heap.size();
  }

};

//! A Network class.
/*!
  This class implements a network consisting of a set of nodes and links.
  Refers to the Node and Link class for more informations.
 */
class Network {

private:

  std::map<long, Node> _Nodes;                                    //!< Nodes of the network (see Node class)
  std::map<long, Link> _Links;                                    //!< Links of the network (see Link class)

  double min_x;                                                   //!< Minimum x coordinate
  double max_x;                                                   //!< Maximum x coordinate
  double min_y;                                                   //!< Minimum y coordinate
  double max_y;                                                   //!< Maximum y coordinate

public:

  //! Constructor.
  Network() {

    min_x = 0.0;
    max_x = 0.0;
    min_y = 0.0;
    max_y = 0.0;

  };

  //! Destructor.
  virtual ~Network() {};

  //! Returns the network's links.
  /*!
    \return the network's map <links id, links>
   */
  const std::map<long, Link>& getLinks() const {
    return _Links;
  }

  //! Setting the network's links.
  /*!
    \param links a map of <links id, links>
   */
  void setLinks(const std::map<long, Link>& links) {
    _Links = links;
  }

  //! Return the network's nodes.
  /*!
    \return the networks's map <nodes id, nodes>
   */
  const std::map<long, Node>& getNodes() const {
    return _Nodes;
  }

  //! Setting the network's nodes.
  /*!
    \param nodes a map of <nodes id, nodes>
   */
  void setNodes(const std::map<long, Node>& nodes) {
    _Nodes = nodes;
  }

  //! Add a link to the set of outgoing link of a node.
  /*
   /param nodeId the source node's id
   /param linkId the id of link to be added
   */
  void addLinkOutToNode(long nodeId, long linkId) {
    _Nodes[nodeId].addLinkOutId(linkId);
  }

  //! Add a node to the network.
  /*!
   /param aNode the node to add
   */
  void addNode(Node aNode);

  //! Add a link to the network.
  /*!
    /param aLink the node to add
   */
  void addLink(Link aLink);

  //! Compute the set of destination nodes at a given distance from a source node
  /*!
   The computation of the node at a given distance +/- epsilon from a source node
   is done using a Dijkstra shortest path algorithm relying on a D-ary heap data
   structure.

   The error term 'epsilon' can be increased since we can end up in an sparse area
   of the network (in term of node density) and it could then be impossible to find
   feasible nodes.

   \param source_id the source node's id
   \param dist the distance (in meters) desired between the source and the feasible destinations

   \return a set of node at distance dist (in meters) from the source node
   */
  long getDestFromSource(long source_id, float dist);

  //! Compute the distance between two nodes in the network.
  /*!
    \param source_id source node
    \param dest_id destination node
    
    \return a distance between the source and destination nodes
   */
  float getDistanceNodes(long source_id, long dest_id);

  //! Return the maximum x coordinate
  /*!
    \return maximum x coordinate
   */
  double getMaxX() const {
    return max_x;
  }

  //! Set maximum x coordinate
  /*!
    \param maxX maximum x coordinate
   */
  void setMaxX(double maxX) {
    max_x = maxX;
  }

  //! Return the maximum y coordinate
  /*!
    \return maximum y coordinate
   */
  double getMaxY() const {
    return max_y;
  }

  //! Set maximum x coordinate
  /*!
    \param maxY maximum y coordinate
   */
  void setMaxY(double maxY) {
    max_y = maxY;
  }

  //! Return the minimum x coordinate
  /*!
    \return minimum x coordinate
   */
  double getMinX() const {
    return min_x;
  }

  //! Set minimum x coordinate
  /*!
    \param minX minimum x coordinate
   */
  void setMinX(double minX) {
    min_x = minX;
  }

  //! Return the minimum y coordinate
  /*!
    \return minimum y coordinate
   */
  double getMinY() const {
    return min_y;
  }

  //! Set minimum y coordinate
  /*!
    \param minY minimum y coordinate
   */
  void setMinY(double minY) {
    min_y = minY;
  }

};

#endif /* NETWORK_HPP_ */
