#include <iostream>
#include <limits>
#include <vector>
#include <cassert>
#include <queue>
#include <string.h>
#include <algorithm>
#include <type_traits>

#include "GridPathfinder.hpp"

#ifndef DBG
#define DBG(x)
#endif
#ifndef DBG_PRIO
#define DBG_PRIO(x)
#endif

struct Node {
	uint16_t x, y;
	
	Node (): x(0), y(0) {};
	Node (const uint16_t x, const uint16_t y): x(x), y(y) {};
	
	uint32_t getXY() const {
		return *((uint32_t*) &x);
	}
	
	bool operator== (const Node o) const {
//		return x == o.x && y == o.y;
		return getXY() == o.getXY();
	}
	
	bool operator!= (const Node o) const {
//		return x != o.x || y != o.y;
		return getXY() != o.getXY();
	}
	
	Node add(const uint16_t x2, const uint16_t y2) const {
		return Node (x + x2, y + y2);
	}
	
	Node operator-(const Node o) const {
		return Node (x - o.x, y - o.y);
	}
	
	bool isValid(const uint16_t mapWidth, const uint16_t mapHeight) const {
		return x != 0xFFFF && y != 0xFFFF && x != mapWidth && y != mapHeight;
	}
};

// Used in priority queue
struct NodeWithCost {
	uint32_t cost;
	uint16_t x, y;
	
	NodeWithCost (const uint32_t cost, const Node n): cost(cost), x(n.x), y(n.y) {};
	NodeWithCost (const uint32_t cost, const uint16_t x, const uint16_t y): cost(cost), x(x), y(y) {};
	
	uint32_t getXY() const {
		return *((uint32_t*) &x);
	}
	
	bool operator== (const Node o) const {
		return getXY() == o.getXY();
	}
	
	operator Node() const {
		return Node(x, y);
	}
	
	bool operator< (NodeWithCost& o) {
	//		return cost >= o.cost;
		return cost > o.cost; // faster, less squares evaluated
	}
};

std::ostream& operator<< (std::ostream& stream, const Node n) {
	stream << n.x << "," << n.y;
	return stream;
}

std::ostream& operator<< (std::ostream& stream, const NodeWithCost n) {
	stream << n.x << "," << n.y << " " << n.cost;
	return stream;
}

BooleanList::BooleanList(const uint16_t mapWidth, const uint16_t mapHeight)
: size((63 + mapWidth * mapHeight) / 64), data(new uint64_t[size]) {
	clear();
}

BooleanList::~BooleanList() {
	if (data != nullptr) {
		delete[] data;
		data = nullptr;
	}
}

bool BooleanList::operator[] (const uint32_t nodeIndex) const {
	return data[nodeIndex / 64] & (1UL << (nodeIndex & 0x3F));
}

void BooleanList::operator|= (const uint32_t nodeIndex) {
	data[nodeIndex / 64] |= (1UL << (nodeIndex & 0x3F));
}

void BooleanList::operator&= (const uint32_t nodeIndex) {
	data[nodeIndex / 64] &= ~(1UL << (nodeIndex & 0x3F));
}

void BooleanList::set(uint32_t idx, uint64_t val) {
	assert(idx < size);
	data[idx] = val;
}

void BooleanList::clear() {
	memset(data, 0, 8 * size);
}

uint32_t BooleanList::count() {
	uint32_t setBits = 0;
	
	for (size_t i=0; i < size; i++) {
		setBits += __builtin_popcountll(data[i]);
	}
	
	return setBits;
}
	

// Map of fixed size indexed by in our case the nodes map index.
template<typename T>
LinearNodeMap<T>::LinearNodeMap(const uint16_t mapWidth, const uint16_t mapHeight)
	: mapWidth(mapWidth), size(mapWidth * mapHeight), data(new T[size])
#ifdef TESTING
	  , isSet(mapWidth, mapHeight)
#endif
{
	clear();
}

template<typename T>
LinearNodeMap<T>::~LinearNodeMap() {
	if (data != nullptr) {
		delete[] data;
		data = nullptr;
	}
}

template<typename T>
T LinearNodeMap<T>::operator[] (const Node n) const {
	uint32_t nodeIndex = toMapIdx(n);
#ifdef TESTING
	assert(isSet[nodeIndex]);
#endif
	return data[nodeIndex];
}

template<typename T>
T LinearNodeMap<T>::operator[] (const uint32_t nodeIndex) const {
#ifdef TESTING
	assert(isSet[nodeIndex]);
#endif
	return data[nodeIndex];
}

template<typename T>
T& LinearNodeMap<T>::operator[] (const Node n) {
	uint32_t nodeIndex = toMapIdx(n);
#ifdef TESTING
	isSet |= nodeIndex;
#endif
	return data[nodeIndex];
}

template<typename T>
T& LinearNodeMap<T>::operator[] (const uint32_t nodeIndex) {
#ifdef TESTING
	isSet |= nodeIndex;
#endif
	return data[nodeIndex];
}

template<typename T>
void LinearNodeMap<T>::clear() {
	if constexpr (std::is_fundamental<T>::value) {
		memset(data, 0, sizeof(T) * size);
		
	} else {
		
		T empty = T();
		
		for (size_t i = 0; i < size / 4; i += 4) { // Manual loop unrolling
			data[i] = empty;
			data[i + 1] = empty;
			data[i + 2] = empty;
			data[i + 3] = empty;
		}
		
		for (size_t i = 0; i < size % 4; i++) {
			data[i] = empty;
		}
	}
}

template<typename T>
uint32_t LinearNodeMap<T>::toMapIdx(const Node n) const {
	return (uint32_t) n.y * mapWidth + n.x;
}


struct PrioBucket {
	std::vector<NodeWithCost> nodes;
	uint32_t max;
	
	PrioBucket(): nodes(), max(0) {};
	PrioBucket(const uint32_t max): max(max) {};
	
	bool operator<(const PrioBucket& o) {
		return max < o.max;
	}
};

HOTPriorityQueue::HOTPriorityQueue() {
	buckets.emplace_back(new PrioBucket());
}

HOTPriorityQueue::~HOTPriorityQueue() {
	for (Bucket* bucket : buckets) {
		delete bucket;
	}
	
	for (Bucket* bucket : freeBuckets) {
		delete bucket;
	}
}

void HOTPriorityQueue::push(NodeWithCost node) {
	DBG_PRIO(std::cout << "push" << node);
	PrioBucket& hotBucket = *buckets[hotIdx];
	
	if (node.cost <= hotBucket.max) {
		DBG_PRIO(std::cout << " hot" << std::endl);
		hotBucket.nodes.push_back(node);
		std::push_heap(hotBucket.nodes.begin(), hotBucket.nodes.end());
		
	} else if (hotBucket.nodes.size() == 0 && hotIdx == buckets.size() - 1) { // a push to empty last bucket
		DBG_PRIO(std::cout << " hot empty");
		hotBucket.max = std::max(hotBucket.max, node.cost);
		
		hotBucket.nodes.push_back(node);
		std::push_heap(hotBucket.nodes.begin(), hotBucket.nodes.end());
		
	} else {
		uint8_t idx = hotIdx;
		
		while (idx + 1U < buckets.size()) {
			idx++;
			assert(idx < 255U);
			PrioBucket& bucket = *buckets[idx];
			
			if (node.cost <= bucket.max) {
				DBG_PRIO(std::cout << " cold " << (int) idx << std::endl);
				bucket.nodes.push_back(node);
				return;
			}
		}
		
		if (idx != hotIdx) { // If last bucket allow increasing its max
			PrioBucket& bucket = *buckets[idx];
			
			if (node.cost - bucket.max < 100U && bucket.nodes.size() < 100U) {
				DBG_PRIO(std::cout << " cold " << (int) idx << " increase " << bucket.max << " > " << node.cost
								 << " " << bucket.nodes.size() << std::endl);
				bucket.max = node.cost;
				bucket.nodes.push_back(node);
				return;
			}
		}
		
		uint32_t newMax = buckets[buckets.size() - 1]->max + 1;
		
		if (hotIdx >= 64) { // If lots of unused buckets move them to free list and move used to the front
			DBG_PRIO(std::cout << " consolidate");
			
			freeBuckets.insert(freeBuckets.end(), std::make_move_iterator(buckets.begin()), std::make_move_iterator(buckets.begin() + hotIdx));
			std::ranges::move(buckets.begin() + hotIdx, buckets.end(), buckets.begin());
			buckets.erase(buckets.end() - hotIdx, buckets.end());
			hotIdx = 0;
		}
		
		if (freeBuckets.empty()) {
			buckets.emplace_back(new Bucket(newMax));
			
		} else {
			DBG_PRIO(std::cout << " reuse");
			buckets.push_back(std::move(freeBuckets[freeBuckets.size() - 1]));
			freeBuckets.pop_back();
			buckets[buckets.size() - 1]->max = newMax;
		}
		
		buckets[buckets.size() - 1]->nodes.push_back(node);
		DBG_PRIO(std::cout << " new " << buckets.size() - 1 << " " << newMax << std::endl);
	}
}

// Must call isEmpty() before using.
NodeWithCost HOTPriorityQueue::top() const {
	DBG_PRIO(std::cout << "top " << buckets[hotIdx].nodes[0] << std::endl);
	return buckets[hotIdx]->nodes[0];
}

// Must call isEmpty() before using.
void HOTPriorityQueue::pop() {
	std::vector<NodeWithCost>& nodes = buckets[hotIdx]->nodes;
	DBG_PRIO(std::cout << "pop " << nodes[0] << std::endl);
	std::pop_heap(nodes.begin(), nodes.end());
	nodes.pop_back();
}

bool HOTPriorityQueue::isEmpty() {
	size_t size = buckets[hotIdx]->nodes.size();
	DBG_PRIO(std::cout << "size " << size);
	
	if (size == 0) {
		while (hotIdx + 1U < buckets.size()) {
			hotIdx++;
			PrioBucket& bucket = *buckets[hotIdx];
			size = bucket.nodes.size();
			
			if (size > 0) {
//					DBG_PRIO(std::cout << " heapify " << (int) hotIdx << " " << bucket.nodes.size());
//					std::cout << " heapify " << (int) hotIdx << " " << bucket.nodes.size() << std::endl;
				std::make_heap(bucket.nodes.begin(), bucket.nodes.end());
				break;
			}
		}
	}
	
	DBG_PRIO(std::cout << std::endl);
	
	return size == 0;
}

void HOTPriorityQueue::clear() {
	hotIdx = 0;
	buckets[0]->max = 0;
	
	for (Bucket* bucket : buckets) {
		bucket->nodes.clear();
	}
	
	if (buckets.size() > 1) {
		freeBuckets.insert(freeBuckets.end(), std::make_move_iterator(buckets.begin() + 1), std::make_move_iterator(buckets.end()));
		buckets.erase(buckets.begin() + 1, buckets.end());
	}
}


GridAStar::GridAStar(const unsigned char* map, const uint16_t mapWidth, const uint16_t mapHeight)
	: map(map), mapWidth(mapWidth), mapHeight(mapHeight), openList(mapWidth, mapHeight), closedList(mapWidth, mapHeight),
	  sourceNodeMap(mapWidth, mapHeight), nodeCurrentCostMap(mapWidth, mapHeight)
	{
		// We need position 65535 internally for out of bounds comparisons.
		assert(mapWidth < 0xFFFF && mapHeight < 0xFFFF);
	}

uint32_t GridAStar::findPath (const uint16_t startX, const uint16_t startY, const uint16_t targetX, 
                              const uint16_t targetY, uint32_t* outBuffer, const size_t outBufferSize) {

	assert(startX < mapWidth && startY < mapHeight);
	assert(targetX < mapWidth && targetY < mapHeight);
	
	const Node startNode {startX, startY};
	const Node targetNode {targetX, targetY};
	
	if (startNode == targetNode) {
		return 0;
	}
	
	reset(); // For repeated calls
	
	nodesToEvaluate.push({heuristic(startNode, targetNode, startNode), startNode});
	openList |= toMapIdx(startNode);
	nodeCurrentCostMap[startNode] = 0;
	
	while (!nodesToEvaluate.isEmpty()) {
		NodeWithCost node = nodesToEvaluate.top();
		
		if (node == targetNode) {
			std::vector<uint32_t> path {};
			
			Node node2 = node;
			std::vector<Node> oldNodes {};
			
			while (node2 != startNode) {
				DBG(std::cout << " step " << node2 << std::endl);
//					assert(std::find(oldNodes.begin(), oldNodes.end(), node2) == oldNodes.end());
				oldNodes.push_back(node2);
				path.push_back(toMapIdx(node2));
				node2 = sourceNodeMap[node2];
			}
			
			DBG(std::cout << "found path " << path.size() << ": ");
			
			size_t length = std::min(path.size(), outBufferSize);
			for (size_t i = 0; i < length; i++) {
				uint32_t mapIdx = path[length -1 - i];
				outBuffer[i] = mapIdx;
				DBG(std::cout << mapIdx << "(" << mapIdx % mapWidth << "," << mapIdx / mapWidth << "), ");
			}
			DBG(std::cout << std::endl);
			
#ifdef TESTING
			std::cout << closedList.count() << ", ";
#endif
			
			return path.size();
		}
		
		nodesToEvaluate.pop();
		
		uint32_t nodeMapIdx = toMapIdx(node);
		openList &= nodeMapIdx;
		
		if (!closedList[nodeMapIdx]) {
			closedList |= nodeMapIdx;
			
			DBG(std::cout << "evaluate " << node << " " << node.cost << std::endl);
			
			uint32_t nodeCost = nodeCurrentCostMap[nodeMapIdx];
			evaluateNeighbour(node, nodeCost, -1,  0, targetNode, startNode);
			evaluateNeighbour(node, nodeCost,  1,  0, targetNode, startNode);
			evaluateNeighbour(node, nodeCost,  0, -1, targetNode, startNode);
			evaluateNeighbour(node, nodeCost,  0,  1, targetNode, startNode);
			
			DBG(std::cout << std::endl);
		}
	}
	
#ifdef TESTING
	std::cout << closedList.count() << ", ";
#endif
	
	return -1;
}
	
// How much the tie break is allowed to nudge node scores
#define TIE_BREAK_SPAN 31U
	
void GridAStar::evaluateNeighbour(const Node node, const uint32_t nodeCost, uint16_t x, uint16_t y, 
                                  const Node targetNode, const Node startNode) {
	Node neighbour = node.add(x, y);
	
	if (!neighbour.isValid(mapWidth, mapHeight)) {
		return;
	}
	
	DBG(std::cout << " > " << neighbour);
	
	uint32_t neighbourMapIdx = toMapIdx(neighbour);
	
	if (closedList[neighbourMapIdx]) {
		DBG(std::cout << " clear closed" << std::endl);
		return;
	}
	
	if (map[neighbourMapIdx]) {
		DBG(std::cout << " clear");
		
		Node sourceNode = node;
		uint32_t neighbourCost = nodeCost + 1 + TIE_BREAK_SPAN;
		
		DBG(std::cout << " " << neighbourCost);
		
		if (openList[neighbourMapIdx]) {
			DBG(std::cout << " open");
			uint32_t oldNeighborCost = nodeCurrentCostMap[neighbourMapIdx];
			
			if (oldNeighborCost <= neighbourCost) {
				DBG(std::cout << std::endl);
				return;
			}
			
			// Re-push node, this will be picked up first and close it which will ignore the existing queued node
			uint32_t estimatedTotalCost = neighbourCost + heuristic(neighbour, targetNode, startNode);
			nodesToEvaluate.push({estimatedTotalCost, neighbour}); 
			
			DBG(std::cout << " cheaper " << oldNeighborCost << " " << estimatedTotalCost << std::endl);
			
		} else { // new unvisited node
			
			uint32_t heuristicCost = heuristic(neighbour, targetNode, startNode);
			
			bool neighborSideWallA = false, neighborSideWallB = false;
			Node test = neighbour.add(y, x);
			if (test.isValid(mapWidth, mapHeight)) {
				neighborSideWallA = map[toMapIdx(test)];
			}
			
			test = neighbour.add(-y, -x);
			if (test.isValid(mapWidth, mapHeight)) {
				neighborSideWallB = map[toMapIdx(test)];
			}
			
			// Jump point search
			// Try to go as far as is advantageous in a straight line unless there is a change in side walls.
			uint32_t skipHeuristic;
			Node skipNode = neighbour.add(x, y);
			uint32_t skipNodeMapIdx = toMapIdx(skipNode);
			while(skipNode.isValid(mapWidth, mapHeight) && !closedList[skipNodeMapIdx] && !openList[skipNodeMapIdx] &&
						map[skipNodeMapIdx] && 
						(skipHeuristic = heuristic(skipNode, targetNode, startNode)) + 1 + TIE_BREAK_SPAN < heuristicCost) {
				
				sourceNodeMap[neighbourMapIdx] = sourceNode;
				sourceNode = neighbour;
				
				openList |= neighbourMapIdx;
				nodeCurrentCostMap[neighbourMapIdx] = neighbourCost;
				neighbourCost += 1 + TIE_BREAK_SPAN;
				
				neighbour = skipNode;
				neighbourMapIdx = skipNodeMapIdx;
				heuristicCost = skipHeuristic;
				
				// Check for changes in side walls, if so we need to break as we might need to turn here
				Node test = skipNode.add(y, x);
				if (test.isValid(mapWidth, mapHeight)) {
					if (map[toMapIdx(test)] != neighborSideWallA) {
						break;
					}
				}
				
				test = skipNode.add(-y, -x);
				if (test.isValid(mapWidth, mapHeight)) {
					if (map[toMapIdx(test)] != neighborSideWallB) {
						break;
					}
				}
				
				skipNode = skipNode.add(x, y);
				skipNodeMapIdx = toMapIdx(skipNode);
			}
			
#ifdef DBG
			if (neighbourCost - nodeCost > 1 + TIE_BREAK_SPAN) {
				DBG(std::cout << " skipped " << (neighbourCost - nodeCost - 1 - TIE_BREAK_SPAN) << " > " << neighbour);
			}
#endif
			
			openList |= neighbourMapIdx;
			uint32_t estimatedTotalCost = neighbourCost + heuristicCost;
			nodesToEvaluate.push({estimatedTotalCost, neighbour});
			
			DBG(std::cout << " new queued " << estimatedTotalCost << std::endl);
		}
		
		sourceNodeMap[neighbourMapIdx] = sourceNode;
		nodeCurrentCostMap[neighbourMapIdx] = neighbourCost;
		
	} else {
		DBG(std::cout << " blocked" << std::endl);
	}
}

// uint32_t is always enough for storing cost as max squares within uint16_t coordinate grid is 0xFFFF0001,
// and our heuristic does not overestimate noticeably.
uint32_t GridAStar::heuristic(const Node n, const Node target, const Node start) {
	// Break ties toward line from start to target
	int32_t dx1 = (int32_t) n.x - target.x;
	int32_t dy1 = (int32_t) n.y - target.y;
	int32_t dx2 = (int32_t) start.x - target.x;
	int32_t dy2 = (int32_t) start.y - target.y;
	uint32_t cross = abs(dx1 * dy2 - dx2 * dy1);
//	uint32_t tieBreaker = std::clamp(cross / 25000U, 0U, TIE_BREAK_SPAN);
		uint32_t tieBreaker = std::clamp(cross / 10000U, 0U, TIE_BREAK_SPAN); // Better performance but fails kattis
	
	// manhattan distance
	int32_t dx = (int32_t) target.x - n.x;
	int32_t dy = (int32_t) target.y - n.y;
	return (1U + TIE_BREAK_SPAN) * ((uint32_t) abs(dx) + (uint32_t) abs(dy) + tieBreaker);
}
	
	// maps x from [in_min,in_max] to [out_min,out_max]
int32_t GridAStar::mapValue(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
	
uint32_t GridAStar::toMapIdx(Node n) {
	return (uint32_t) n.y * mapWidth + n.x;
}

void GridAStar::reset() {
	sourceNodeMap.clear();
	nodeCurrentCostMap.clear();
	openList.clear();
	closedList.clear();
	nodesToEvaluate.clear();
}
