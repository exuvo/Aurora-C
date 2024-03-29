// *********************************************************************************
// QuadTree where each node is a point
// *********************************************************************************
#ifndef QUADTREE_POINT_HPP
#define QUADTREE_POINT_HPP

#include <stdint.h>

#include "utils/SmallList.hpp"
#include "utils/FreeList.hpp"

// Represents a rectangle for the quadtree storing a center and half-size.
struct QuadPointCRect
{
    int32_t mid_x;
    int32_t mid_y;
    int32_t size_x;
    int32_t size_y;
};

// Represents an element node in the quadtree.
struct QuadPointEltNode
{
    // Points to the next element in the leaf node. A value of -1
    // indicates the end of the list.
    int32_t next;

    // Stores the element index.
    int32_t element;
};

// Represents an element in the quadtree.
struct QuadPointElt
{
    uint32_t id;
    int32_t mx;
    int32_t my;
};

// Represents a node in the quadtree.
struct QuadPointNode
{
    // Points to the first child if this node is a branch or the first element
    // if this node is a leaf.
    int32_t first_child;

    // Stores the number of elements in the node or -1 if it is not a leaf.
    uint16_t count;
};

// Stores temporary data about a node during a search.
struct QuadPointNodeData
{
    QuadPointCRect rect;
    int32_t index;
    uint8_t depth;
};
typedef SmallList<QuadPointNodeData> QuadPointNodeList;

// Function signature used for traversing a tree node.
struct QuadtreePoint;
typedef void QuadtreePointNodeFunc(QuadtreePoint* qt, void* user_data, int32_t node, uint8_t depth, int32_t mx, int32_t my, int32_t sx, int32_t sy);

struct QuadtreePoint
{
    // Creates a quadtree with the requested extents and max depth.
    QuadtreePoint(int32_t width, int32_t height, uint16_t start_max_elements, uint8_t max_depth);

    // Inserts a new element to the tree.
    // Returns an index to the new element.
    int32_t insert(uint32_t id, int32_t x, int32_t y);

    // Removes the specified element from the tree.
    void remove(int32_t element);

    // Outputs a list of elements found in the specified rectangle.
    SmallList<uint32_t> query(const std::array<int32_t, 4> rect, int32_t omit_element);

    // Return the data for the root node.
    QuadPointNodeData root_data() const;

    // Cleans up the tree, removing empty leaves.
    bool cleanup();
    
    // Cleans up the tree, removing empty leaves and consolidating mostly empty child nodes.
    bool cleanupFull();
    
    // Traverses all the nodes in the tree, calling 'branch' for branch nodes and 'leaf' for leaf nodes.
    void traverse(void* user_data, QuadtreePointNodeFunc* branch, QuadtreePointNodeFunc* leaf);

    // Stores all the nodes in the quadtree. The first node in this
    // sequence is always the root.
    SmallList<QuadPointNode> nodes;

    // Stores all the element data in the quadtree.
    FreeList<QuadPointElt> elts;

    // Stores all the elements in the quadtree.
    FreeList<QuadPointEltNode> elt_nodes;

    // Stores the width of the quadtree extents.
    QuadPointCRect root_rect;

    // Stores the first free node in the quadtree to be reclaimed as 4
    // contiguous nodes at once.
    int32_t free_node = -1;

    // Stores the maximum depth allowed for the quadtree.
    uint8_t max_depth;
    
    // Maximum allowed elements in a leaf before the leaf is subdivided/split unless the leaf is at the maximum allowed tree depth.
    uint16_t max_elements;
    
    static constexpr auto NOT_LEAF = std::numeric_limits<decltype(QuadPointNode::count)>::max();
    
	private:
    void leaf_insert(QuadtreePoint& tree, const QuadPointNodeData& node_data, int32_t element);
    void node_insert(QuadtreePoint& tree, const QuadPointNodeData& node_data, int32_t element);
};

#endif
