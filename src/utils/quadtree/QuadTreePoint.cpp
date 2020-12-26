
// *********************************************************************************
// QuadPointTree.cpp
// *********************************************************************************
#include <array>
#include <optional>
#include <utility>

#include "QuadTreePoint.hpp"

void QuadtreePoint::leaf_insert(QuadtreePoint& tree, const QuadPointNodeData& node_data, int32_t element)
{
    QuadPointNode* node = &tree.nodes[node_data.index];

    // Insert the element node to the leaf.
    const QuadPointEltNode new_elt_node = {node->first_child, element};
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

static QuadPointNodeData child_data(int32_t mx, int32_t my, int32_t sx, int32_t sy, int32_t index, uint8_t depth)
{
    const QuadPointNodeData cd = {{mx, my, sx, sy}, index, depth};
    return cd;
}

static QuadPointNodeList find_leaves(const QuadtreePoint& tree, const QuadPointNodeData& root, const std::array<int32_t, 4> rect)
{
    QuadPointNodeList leaves, to_process;
    to_process.push_back(root);
    while (to_process.size() > 0)
    {
        const QuadPointNodeData nd = to_process.pop_back();

        // If this node is a leaf, insert it to the list.
        if (tree.nodes[nd.index].count != QuadtreePoint::NOT_LEAF)
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

static std::optional<QuadPointNodeData> find_leaf(const QuadtreePoint& tree, const QuadPointNodeData& root, int x, int y)
{
    QuadPointNodeList leaves, to_process;
    to_process.push_back(root);
    while (to_process.size() > 0)
    {
        const QuadPointNodeData nd = to_process.pop_back();

        // If this node is a leaf, insert it to the list.
        if (tree.nodes[nd.index].count != QuadtreePoint::NOT_LEAF)
            leaves.push_back(nd);
        else
        {
            // Otherwise push the children that intersect the rectangle.
            const int mx = nd.rect.mid_x, my = nd.rect.mid_y;
            const int sx = nd.rect.size_x, sy = nd.rect.size_y;
            const int hx = sx >> 1, hy = sy >> 1;
            const int fc = tree.nodes[nd.index].first_child;
            const int dp = nd.depth + 1;

            if (y <= my) {
                if (x <= mx) {
                    to_process.push_back(child_data(mx-hx, my-hy, hx, hy, fc+0, dp));
                } else {
                    to_process.push_back(child_data(mx+hx, my-hy, hx, hy, fc+1, dp));
                }
            } else {
                if (x <= mx) {
                    to_process.push_back(child_data(mx-hx, my+hy, hx, hy, fc+2, dp));
                } else {
                    to_process.push_back(child_data(mx+hx, my+hy, hx, hy, fc+3, dp));
                }
            }
        }
    }
    
    assert(leaves.size() <= 1);
    
    if (leaves.size() == 0) {
    	return {};
    }
    
    return leaves[0];
}

void QuadtreePoint::node_insert(QuadtreePoint& tree, const QuadPointNodeData& node_data, int32_t element)
{
    // Find the leaves and insert the element to all the leaves found.
    const std::optional<QuadPointNodeData> leaf = find_leaf(tree, node_data, tree.elts[element].mx, tree.elts[element].my);
    leaf_insert(tree, *leaf, element);
}

QuadtreePoint::QuadtreePoint(int32_t width, int32_t height, uint16_t start_max_elements, uint8_t imax_depth): max_depth(imax_depth) {
	max_elements = start_max_elements;
    const QuadPointNode root_node = {-1, 0};
    nodes.push_back(root_node);

    root_rect.size_x = width >> 1;
    root_rect.size_y = height >> 1;
    root_rect.mid_x = 0;
    root_rect.mid_y = 0;
//    root_rect.mid_x = root_rect.size_x;
//    root_rect.mid_y = root_rect.size_y;
}

int32_t QuadtreePoint::insert(uint32_t id, int32_t x, int32_t y)
{
    const QuadPointElt new_elt = {id, x, y};
    const int element = elts.insert(new_elt);
    node_insert(*this, root_data(), element);
    return element;
}

void QuadtreePoint::remove(int32_t element)
{
    // Find the leaf.
    const std::optional<QuadPointNodeData> leaf = find_leaf(*this, root_data(), elts[element].mx, elts[element].my);

    // For each leaf node, remove the element node.
		QuadPointNode& node = nodes[leaf->index];

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
		
    // Remove the element.
    elts.erase(element);
}

SmallList<uint32_t> QuadtreePoint::query(const std::array<int32_t, 4> rect, int32_t omit_element)
{
    // Find the leaves.
    const QuadPointNodeList leaves = find_leaves(*this, root_data(), rect);

    SmallList<uint32_t> elementIDs;
    
    // For each leaf node, look for elements that intersect.
    for (uint32_t j=0; j < leaves.size(); ++j)
    {
        const QuadPointNodeData& nd = leaves[j];
        QuadPointNode& node = nodes[nd.index];

        // Walk the list and add elements that intersect.
        int elt_node_index = node.first_child;
        while (elt_node_index != -1)
        {
            const int element = elt_nodes[elt_node_index].element;
            if (element != omit_element)// Don't do intersection test here as tree data is not always up to date. && intersect(elts[element].mx, elts[element].my, rect)
            {
                elementIDs.push_back(elts[element].id);
            }
            elt_node_index = elt_nodes[elt_node_index].next;
        }
    }
    
    return elementIDs;
}

bool QuadtreePoint::cleanup()
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
        QuadPointNode& node = nodes[node_index];

        // Loop through the children.
        int num_empty_leaves = 0;
        for (int j=0; j < 4; ++j)
        {
            const int child_index = node.first_child + j;
            const QuadPointNode& child = nodes[child_index];
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

bool QuadtreePoint::cleanupFull()
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
        QuadPointNode& node = nodes[node_index];

        int num_empty_leaves = 0;
        uint16_t num_elements = 0;
        
        // Loop through the children.
        for (int j=0; j < 4; ++j)
        {
            const int child_index = node.first_child + j;
            const QuadPointNode& child = nodes[child_index];
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
						QuadPointNode& child = nodes[child_index];
						
						while (child.first_child != -1) {
							int eltIdx = child.first_child;
							QuadPointEltNode& elt = elt_nodes[eltIdx];
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
						const QuadPointEltNode new_elt_node = {node.first_child, elements[i]};
						node.first_child = elt_nodes.insert(new_elt_node);
					}
					
					changed = true;
        }
    }
    
    return changed;
}

QuadPointNodeData QuadtreePoint::root_data() const {
    QuadPointNodeData rd = { root_rect, 0, 0 };
    return rd;
}

void QuadtreePoint::traverse(void* user_data, QuadtreePointNodeFunc* branch, QuadtreePointNodeFunc* leaf) {
	QuadPointNodeList to_process;
  to_process.push_back(root_data());
  
	while (to_process.size() > 0) {
		const QuadPointNodeData nd = to_process.pop_back();
		
		// If this node is a leaf, insert it to the list.
		if (nodes[nd.index].count == QuadtreePoint::NOT_LEAF) {
			const int mx = nd.rect.mid_x, my = nd.rect.mid_y;
			const int sx = nd.rect.size_x, sy = nd.rect.size_y;
			const int hx = sx >> 1, hy = sy >> 1;
			const int fc = nodes[nd.index].first_child;
			const int dp = nd.depth + 1;
			
			to_process.push_back(child_data(mx - hx, my - hy, hx, hy, fc + 0, dp));
			to_process.push_back(child_data(mx + hx, my - hy, hx, hy, fc + 1, dp));
			to_process.push_back(child_data(mx - hx, my + hy, hx, hy, fc + 2, dp));
			to_process.push_back(child_data(mx + hx, my + hy, hx, hy, fc + 3, dp));
			
			if (branch) {
				branch(this, user_data, nd.index, nd.depth, nd.rect.mid_x, nd.rect.mid_y, nd.rect.size_x, nd.rect.size_y);
			}
			
		} else if (leaf) {
			leaf(this, user_data, nd.index, nd.depth, nd.rect.mid_x, nd.rect.mid_y, nd.rect.size_x, nd.rect.size_y);
		}
	}
}
