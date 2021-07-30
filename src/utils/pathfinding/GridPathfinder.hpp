/*
 * Pathfinder.hpp
 *
 *  Created on: Jul 31, 2021
 *      Author: exuvo
 */

#ifndef SRC_UTILS_PATHFINDER_HPP_
#define SRC_UTILS_PATHFINDER_HPP_

struct Node;

// Used in priority queue
struct NodeWithCost;

std::ostream& operator<< (std::ostream& stream, const Node n);
std::ostream& operator<< (std::ostream& stream, const NodeWithCost n);

// Compact and fast list of booleans
class BooleanList {
public:
	BooleanList(const uint16_t mapWidth, const uint16_t mapHeight);
	~BooleanList();
	
	bool operator[] (const uint32_t nodeIndex) const;
	void operator|= (const uint32_t nodeIndex);
	void operator&= (const uint32_t nodeIndex);
	
	void set(uint32_t idx, uint64_t val);
	
	void clear();
	uint32_t count();
	
private:
	const size_t size;
	uint64_t* data = nullptr;
};

// Map of fixed size indexed by in our case the nodes map index.
template<typename T>
class LinearNodeMap {
public:
	LinearNodeMap(const uint16_t mapWidth, const uint16_t mapHeight);
	
	~LinearNodeMap();
	
	T operator[] (const Node n) const;
	T operator[] (const uint32_t nodeIndex) const;
	
	T& operator[] (const Node n);
	T& operator[] (const uint32_t nodeIndex);
	
	void clear();
	
private:
	const uint16_t mapWidth;
	const size_t size;
	T* data = nullptr;
	
#ifdef TESTING
	BooleanList isSet;
#endif
	
	uint32_t toMapIdx(const Node n) const;
};

struct PrioBucket;

// Bucket queue with first bucket being a heap.
// Implemented solely from descriptions at https://en.wikipedia.org/wiki/Bucket_queue and 
// http://theory.stanford.edu/~amitp/GameProgramming/ImplementationNotes.html#hot-queues.
class HOTPriorityQueue {
public:
	HOTPriorityQueue();
	
public:
	void push(NodeWithCost node);
	
	// Must call isEmpty() before using.
	NodeWithCost top() const;
	
	// Must call isEmpty() before using.
	void pop();
	
	bool isEmpty();
	
	void clear();
	
private:
	std::vector<PrioBucket*> buckets;
	uint8_t hotIdx = 0;
};

/**
 * Not thread safe but instances can be reused and map contents can safely be modified between calls as long as
 *  the map remains the same size.
 * It could be made thread safe by moving sourceNodeMap, nodeCurrentCostMap, nodeCostToEndMap, nodesToEvaluateSet
 *  to stack local variables inside FindPath but then you will get a lot of unnecessary memory allocations for repeated
 *  path finding calls and as such i would suggest instead having one instance per thread.
 * 
 * This implementation is optimized for positions fitting within 2 bytes
 *  and therefore does not support maps larger than 65534 (position 65535 is used internally).
 *  
 * Uses both "Jump Point Searching" and tie breaking on equal nodes to increase pathing speed.
 */
class GridAStar {
public:
	GridAStar(const unsigned char* map, const uint16_t mapWidth, const uint16_t mapHeight);
	
	uint32_t findPath (const uint16_t startX, const uint16_t startY, const uint16_t targetX, const uint16_t targetY,
	              uint32_t* outBuffer, const size_t outBufferSize);
private:
	const unsigned char* map;
	const uint16_t mapWidth, mapHeight;
	
	BooleanList openList, closedList;
	LinearNodeMap<Node> sourceNodeMap;
	LinearNodeMap<uint32_t> nodeCurrentCostMap;
	HOTPriorityQueue nodesToEvaluate;
	
	void evaluateNeighbour(const Node node, const uint32_t nodeCost, uint16_t x, uint16_t y, 
	                       const Node targetNode, const Node startNode);
	uint32_t heuristic(const Node n, const Node target, const Node start);
	int32_t mapValue(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max);
	uint32_t toMapIdx(Node n);
	void reset();
};

#endif /* SRC_UTILS_PATHFINDER_HPP_ */
