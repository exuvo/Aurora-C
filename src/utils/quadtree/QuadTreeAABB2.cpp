
// *********************************************************************************
// QuadAABB2Tree.cpp
// *********************************************************************************
#include <array>
#include <utility>

#include "QuadTreeAABB2.hpp"

static bool intersect(const std::array<int32_t, 4> ltrb1, const std::array<int32_t, 4> ltrb2)
{
  return ltrb2[0] <= ltrb1[2] && ltrb2[2] >= ltrb1[0] &&
         ltrb2[1] <= ltrb1[3] && ltrb2[3] >= ltrb1[1];
}

void QuadtreeAABB2::leaf_insert(QuadtreeAABB2& tree, const QuadAABB2NodeData& node_data, int32_t element)
{
    QuadAABB2Node* node = &tree.nodes[node_data.index];

    // Insert the element node to the leaf.
    const QuadAABB2EltNode new_elt_node = {node->first_child, element};
    node->first_child = tree.elt_nodes.insert(new_elt_node);

    // If the leaf is full, split it.
    if (node->count == max_elements && node_data.depth < tree.max_depth)
    {
        // Pop off all the previous elements.
        SmallList<int> elts;
        while (node->first_child != -1)
        {
            const int index = node->first_child;
            node->first_child = tree.elt_nodes[node->first_child].next;
            elts.push_back(tree.elt_nodes[index].element);
            tree.elt_nodes.erase(index);
        }

        // Start by allocating 4 child nodes.
        if (node->first_child != -1)
            tree.free_node = tree.nodes[tree.free_node].first_child;
        else
        {
            node->first_child = static_cast<int>(tree.nodes.size());
            tree.nodes.resize(tree.nodes.size() + 4);
        }
        node = &tree.nodes[node_data.index];

        // Initialize the new child nodes.
        for (int j=0; j < 4; ++j)
        {
            tree.nodes[node->first_child+j].first_child = -1;
            tree.nodes[node->first_child+j].count = 0;
        }

        // Transfer the elements in the former leaf node to its new children.
        node->count = -1;
        for (uint32_t j=0; j < elts.size(); ++j)
            node_insert(tree, node_data, elts[j]);
    }
    else
        ++node->count;
}

static QuadAABB2NodeData child_data(int32_t mx, int32_t my, int32_t sx, int32_t sy, int32_t index, uint8_t depth)
{
    const QuadAABB2NodeData cd = {{mx, my, sx, sy}, index, depth};
    return cd;
}

static QuadAABB2NodeList find_leaves(const QuadtreeAABB2& tree, const QuadAABB2NodeData& root, std::array<int32_t, 4> rect)
{
    QuadAABB2NodeList leaves, to_process;
    to_process.push_back(root);
    while (to_process.size() > 0)
    {
        const QuadAABB2NodeData nd = to_process.pop_back();

        // If this node is a leaf, insert it to the list.
        if (tree.nodes[nd.index].count != QuadtreeAABB2::NOT_LEAF)
            leaves.push_back(nd);
        else
        {
            // Otherwise push the children that intersect the rectangle.
            const int mx = nd.rect.mid_x, my = nd.rect.mid_y;
            const int sx = nd.rect.size_x, sy = nd.rect.size_y;
            const int hx = sx >> 1, hy = sy >> 1;
            const int fc = tree.nodes[nd.index].first_child;
            const int dp = nd.depth + 1;

            if (rect[1] <= my)
            {
                if (rect[0] <= mx)
                    to_process.push_back(child_data(mx-hx, my-hy, hx, hy, fc+0, dp));
                if (rect[2] > mx)
                    to_process.push_back(child_data(mx+hx, my-hy, hx, hy, fc+1, dp));
            }
            if (rect[3] > my)
            {
                if (rect[0] <= mx)
                    to_process.push_back(child_data(mx-hx, my+hy, hx, hy, fc+2, dp));
                if (rect[2] > mx)
                    to_process.push_back(child_data(mx+hx, my+hy, hx, hy, fc+3, dp));
            }
        }
    }
    return leaves;
}

static std::array<int32_t, 4> getNodeLTRB(QuadtreeAABB2& tree, int32_t element) {
	int mx = tree.elts[element].mx;
	int my = tree.elts[element].my;
	return { mx - tree.element_sx, my - tree.element_sy, mx + tree.element_sx, my + tree.element_sy };
}

void QuadtreeAABB2::node_insert(QuadtreeAABB2& tree, const QuadAABB2NodeData& node_data, int32_t element)
{
    // Find the leaves and insert the element to all the leaves found.
    const QuadAABB2NodeList leaves = find_leaves(tree, node_data, getNodeLTRB(tree, element));
    for (uint32_t j=0; j < leaves.size(); ++j)
        leaf_insert(tree, leaves[j], element);
}

QuadtreeAABB2::QuadtreeAABB2(int32_t width, int32_t height, int32_t element_width, int32_t element_height, uint16_t start_max_elements, uint8_t imax_depth): max_depth(imax_depth)
{
	max_elements = start_max_elements;
    const QuadAABB2Node root_node = {-1, 0};
    nodes.push_back(root_node);

    root_rect.size_x = width >> 1;
    root_rect.size_y = height >> 1;
    root_rect.mid_x = root_rect.size_x;
    root_rect.mid_y = root_rect.size_y;
    
    element_sx = element_width / 2;
		element_sy = element_height / 2;
}

int QuadtreeAABB2::insert(uint32_t id, int32_t x, int32_t y)
{
    const QuadAABB2Elt new_elt = {id, x, y};
    const int element = elts.insert(new_elt);
    node_insert(*this, root_data(), element);
    return element;
}

void QuadtreeAABB2::remove(int32_t element)
{
    // Find the leaves.
    const QuadAABB2NodeList leaves = find_leaves(*this, root_data(), getNodeLTRB(*this, element));

    // For each leaf node, remove the element node.
    for (uint32_t j=0; j < leaves.size(); ++j)
    {
        const QuadAABB2NodeData& nd = leaves[j];
        QuadAABB2Node& node = nodes[nd.index];

        // Walk the list until we find the element node.
        int* link = &node.first_child;
        while (*link != -1 && elt_nodes[*link].element != element)
        {
            link = &elt_nodes[*link].next;
            assert(*link != -1);
        }

        if (*link != -1)
        {
            // Remove the element node.
            const int elt_node_index = *link;
            *link = elt_nodes[*link].next;
            elt_nodes.erase(elt_node_index);
            --node.count;
        }
    }
    // Remove the element.
    elts.erase(element);
}

SmallList<uint32_t> QuadtreeAABB2::query(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t omit_element)
{
    // Find the leaves.
    const std::array<int, 4> rect = {x1, y1, x2, y2};
    const QuadAABB2NodeList leaves = find_leaves(*this, root_data(), rect);

    // For each leaf node, look for elements that intersect.
    SmallList<uint32_t> elements;
    temp.resize(elts.range(), false);
    for (uint32_t j=0; j < leaves.size(); ++j)
    {
        const QuadAABB2NodeData& nd = leaves[j];
        QuadAABB2Node& node = nodes[nd.index];

        // Walk the list and add elements that intersect.
        int elt_node_index = node.first_child;
        while (elt_node_index != -1)
        {
            const int element = elt_nodes[elt_node_index].element;
            if (!temp[element] && element != omit_element && intersect(getNodeLTRB(*this, element), rect))
            {
                elements.push_back(element);
                temp[element] = true;
            }
            elt_node_index = elt_nodes[elt_node_index].next;
        }
    }
    // Unmark the elements that were inserted.
    for (uint32_t j=0; j < elements.size(); ++j)
        temp[elements[j]] = false;
    return elements;
}

bool QuadtreeAABB2::cleanup()
{
	bool changed = false;
	
    // Only process the root if it's not a leaf.
    // We use a 'to process' stack to avoid recursion.
    SmallList<int> to_process;
    if (nodes[0].count == NOT_LEAF)
        to_process.push_back(0);

    while (to_process.size() > 0)
    {
        const int node_index = to_process.pop_back();
        QuadAABB2Node& node = nodes[node_index];

        // Loop through the children.
        int num_empty_leaves = 0;
        for (int j=0; j < 4; ++j)
        {
            const int child_index = node.first_child + j;
            const QuadAABB2Node& child = nodes[child_index];
            if (child.count == 0)
                ++num_empty_leaves;
            else if (child.count == NOT_LEAF)
                to_process.push_back(child_index);
        }

        // If all the children were empty leaves, remove them and
        // make this node the new empty leaf.
        if (num_empty_leaves == 4)
        {
            // Push all 4 children to the free list.
            nodes[node.first_child].first_child = free_node;
            free_node = node.first_child;

            // Make this node the new empty leaf.
            node.first_child = -1;
            node.count = 0;
            
            changed = true;
        }
    }
    
    return changed;
}

bool QuadtreeAABB2::cleanupFull()
{
	bool changed = false;
	
    // Only process the root if it's not a leaf.
    // We use a 'to process' stack to avoid recursion.
    SmallList<int32_t> to_process;
    if (nodes[0].count == NOT_LEAF)
        to_process.push_back(0);

    while (to_process.size() > 0)
    {
        const int node_index = to_process.pop_back();
        QuadAABB2Node& node = nodes[node_index];

        int num_empty_leaves = 0;
        uint16_t num_elements = 0;
        
        // Loop through the children.
        for (int j=0; j < 4; ++j)
        {
            const int child_index = node.first_child + j;
            const QuadAABB2Node& child = nodes[child_index];
            if (child.count == 0) {
                ++num_empty_leaves;
            } else if (child.count == NOT_LEAF) {
                to_process.push_back(child_index);
                num_elements = -1;
            } else if (num_elements != NOT_LEAF) {
							num_elements += child.count;
						}
        }

        // If all the children were empty leaves, remove them and
        // make this node the new empty leaf.
        if (num_empty_leaves == 4)
        {
            // Push all 4 children to the free list.
            nodes[node.first_child].first_child = free_node;
            free_node = node.first_child;

            // Make this node the new empty leaf.
            node.first_child = -1;
            node.count = 0;
            
            changed = true;
            
        }  else if (num_elements != NOT_LEAF && num_elements <= max_elements / 2) {
        	
        	SmallList<int> elements;
        	
					// Consolidate children
        	for (int j=0; j < 4; ++j) {
						const int child_index = node.first_child + j;
						QuadAABB2Node& child = nodes[child_index];
						
						while (child.first_child != -1) {
							int eltIdx = child.first_child;
							QuadAABB2EltNode& elt = elt_nodes[eltIdx];
							child.first_child = elt.next;
							
							// Insert element to the transfer list.
							elements.push_back(elt.element);
							
							// Pop off the element node from the leaf and remove it from the qt.
							elt_nodes.erase(eltIdx);
						}
					}
						
					assert(num_elements == elements.size());
					
					// Push all 4 children to the free list.
					nodes[node.first_child].first_child = free_node;
					free_node = node.first_child;
					
					node.first_child = -1;
					node.count = num_elements;
					
					// Transfer the elements
					for (int i = 0; i < num_elements; ++i) {
						// Insert the element node to the leaf.
						const QuadAABB2EltNode new_elt_node = {node.first_child, elements[i]};
						node.first_child = elt_nodes.insert(new_elt_node);
					}
					
					changed = true;
        }
    }
    
    return changed;
}

QuadAABB2NodeData QuadtreeAABB2::root_data() const
{
    QuadAABB2NodeData rd = { root_rect, 0, 0 };
    return rd;
}
