/*
 * Commands.cpp
 *
 *  Created on: 15 Nov 2020
 *      Author: exuvo
 */

#include "galaxy/Commands.hpp"

//TODO make generic for all commands, 
std::pmr::synchronized_pool_resource command_pool;

bool EntityCommand::isValid() {
	return false;
	
//	val entityRef2 = entityRef.system.galaxy.resolveEntityReference(entityRef)
//	
//	if (entityRef2 != null) {
//		entityRef = entityRef2
//		return true
//	}
//	
//	return false
}

StarSystem* EntityCommand::getSystem() {
	return entityRef.system;
}

static std::pmr::polymorphic_allocator<EntityMoveToPositionCommand> allocatorEntityMoveToPositionCommand { &command_pool };


void EntityMoveToPositionCommand::apply() {
	
}

static void* EntityMoveToPositionCommand::operator new(size_t n) {
	return allocatorEntityMoveToPositionCommand.allocate(n);
}

static void EntityMoveToPositionCommand::operator delete(void* ptr) {
	allocatorEntityMoveToPositionCommand.deallocate_object(ptr);
}
