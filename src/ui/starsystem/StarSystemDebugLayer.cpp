/*
 * StarSystemDebugLayer.cpp
 *
 *  Created on: Dec 17, 2020
 *      Author: exuvo
 */
#include <GLFW/glfw3.h>
#include <fmt/core.h>

#include "Aurora.hpp"
#include "StarSystemDebugLayer.hpp"
#include "starsystems/StarSystem.hpp"
#include "starsystems/ShadowStarSystem.hpp"
#include "starsystems/components/Components.hpp"
#include "galaxy/Galaxy.hpp"

StarSystemDebugLayer::StarSystemDebugLayer(AuroraWindow& parentWindow): StarSystemLayer(parentWindow) {
}

StarSystemDebugLayer::~StarSystemDebugLayer() {
}

void StarSystemDebugLayer::drawSpatialPartitioning() {
	QuadtreePoint* tree;
	
	if (Aurora.settings.render.useShadow) {
		tree = &starSystem->shadow->quadtreeShips;
	} else {
		tree = &starSystem->systems->spatialPartitioningSystem->tree;
	}
	
	const auto treeScale = SpatialPartitioningSystem::SCALE;
	
	struct traverseData {
		StarSystemDebugLayer* layer;
	} traverseData { this };
	
	QuadtreePointNodeFunc* leaf = [](QuadtreePoint* tree, void* user_data, int32_t node, uint8_t depth, int32_t mx, int32_t my, int32_t sx, int32_t sy) {
//		std::cout << "leaf node " << node << ", depth " << (uint32_t) depth << ", " << mx << " " << my << " " << sx << " " << sy << std::endl;
		struct traverseData* data = (struct traverseData*) user_data;
		StarSystemDebugLayer* layer = data->layer;
		
		int64_t x1 = mx - sx;
		int64_t y1 = my - sy;
		int64_t x2 = mx + sx;
		int64_t y2 = my + sy;
		
		Matrix2l pos{};
		pos << x1, y1, x2, y2;
		pos *= treeScale;
		layer->window.window->DrawRectangle(matrixToVK2D(layer->toScreenCoordinates(pos)), false, vk2d::Colorf::YELLOW());
		
		auto& enodes = tree->elt_nodes;
		auto& elts = tree->elts;
		
		int32_t enodeIdx = tree->nodes[node].first_child;
		
		while (enodeIdx != -1) {
			auto elementIdx = enodes[enodeIdx].element;
			enodeIdx = enodes[enodeIdx].next;
			
			auto entityID = elts[elementIdx].id;
			auto x = elts[elementIdx].mx;
			auto y = elts[elementIdx].my;

//					println("entity $entityID $l $t $r $b")
			
			pos << x, y, x, y;
			pos *= treeScale;
			auto screenPos = layer->toScreenCoordinates(pos);
			screenPos.row(0).array() -= 1;
			screenPos.row(1).array() += 1;
			layer->window.window->DrawRectangle(matrixToVK2D(screenPos), false, vk2d::Colorf::TEAL());
		}
	};
	
	QuadtreePointNodeFunc* branch = [](QuadtreePoint* tree, void* user_data, int32_t node, uint8_t depth, int32_t mx, int32_t my, int32_t sx, int32_t sy) {
		struct traverseData* traverseData = (struct traverseData*) user_data;
//				println("branch node $node, depth $depth, $mx $my $sx $sy")
//				shapeRenderer.color = sRGBtoLinearRGB(Color.TEAL)
//				shapeRenderer.rect((scale * (mx - sx - max2) - cameraOffset.x).toFloat(),
//				                   (scale * (my - sy - max2) - cameraOffset.y).toFloat(),
//				                   (2 * scale * sx).toFloat(),
//				                   (2 * scale * sy).toFloat())
	};
	
	tree->traverse(&traverseData, branch, leaf);
}

void StarSystemDebugLayer::drawSpatialPartitioningPlanetoids() {
	QuadtreeAABB* tree;
	
	if (Aurora.settings.render.useShadow) {
		tree = &starSystem->shadow->quadtreePlanetoids;
	} else {
		tree = &starSystem->systems->spatialPartitioningPlanetoidsSystem->tree;
	}
	
	const auto treeScale = SpatialPartitioningPlanetoidsSystem::SCALE;
	
	struct traverseData {
			StarSystemDebugLayer* layer;
	} traverseData { this };
	
	QuadtreeAABBNodeFunc* leaf = [](QuadtreeAABB* tree, void* user_data, int32_t node, uint8_t depth, int32_t mx, int32_t my, int32_t sx, int32_t sy) {
//		std::cout << "leaf node " << node << ", depth " << (uint32_t) depth << ", " << mx << " " << my << " " << sx << " " << sy << std::endl;
		struct traverseData* data = (struct traverseData*) user_data;
		StarSystemDebugLayer* layer = data->layer;
		
		int64_t x1 = mx - sx;
		int64_t y1 = my - sy;
		int64_t x2 = mx + sx;
		int64_t y2 = my + sy;
		
		Matrix2l pos{};
		pos << x1, y1, x2, y2;
		pos *= treeScale;
		layer->window.window->DrawRectangle(matrixToVK2D(layer->toScreenCoordinates(pos)), false, vk2d::Colorf::YELLOW());
		
		auto& enodes = tree->elt_nodes;
		auto& elts = tree->elts;
		
		int32_t enodeIdx = tree->nodes[node].first_child;
		
		while (enodeIdx != -1) {
			auto elementIdx = enodes[enodeIdx].element;
			enodeIdx = enodes[enodeIdx].next;
			
			auto entityID = elts[elementIdx].id;
			auto l = elts[elementIdx].ltrb[0];
			auto t = elts[elementIdx].ltrb[1];
			auto r = elts[elementIdx].ltrb[2];
			auto b = elts[elementIdx].ltrb[3];

//					println("entity $entityID $l $t $r $b")
			
			pos << l, t, r, b;
			pos *= treeScale;
			layer->window.window->DrawRectangle(matrixToVK2D(layer->toScreenCoordinates(pos)), false, vk2d::Colorf::TEAL());
		}
	};
	
	QuadtreeAABBNodeFunc* branch = [](QuadtreeAABB* tree, void* user_data, int32_t node, uint8_t depth, int32_t mx, int32_t my, int32_t sx, int32_t sy) {
		struct traverseData* traverseData = (struct traverseData*) user_data;
//				println("branch node $node, depth $depth, $mx $my $sx $sy")
//				shapeRenderer.color = sRGBtoLinearRGB(Color.TEAL)
//				shapeRenderer.rect((scale * (mx - sx - max2) - cameraOffset.x).toFloat(),
//				                   (scale * (my - sy - max2) - cameraOffset.y).toFloat(),
//				                   (2 * scale * sx).toFloat(),
//				                   (2 * scale * sy).toFloat())
	};
	
	tree->traverse(&traverseData, branch, leaf);
}

void StarSystemDebugLayer::render() {
	
	if (Aurora.settings.render.debugSpatialPartitioning) {
		profilerEvents.start("drawSpatialPartitioning");
		drawSpatialPartitioning();
		profilerEvents.end();
	}
	
	if (Aurora.settings.render.debugSpatialPartitioningPlanetoids) {
		profilerEvents.start("drawSpatialPartitioningPlanetoids");
		drawSpatialPartitioningPlanetoids();
		profilerEvents.end();
	}
}

