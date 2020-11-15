/*
 * Commands.cpp
 *
 *  Created on: 15 Nov 2020
 *      Author: exuvo
 */

#include "galaxy/Commands.hpp"

//TODO make generic for all commands, 
std::pmr::synchronized_pool_resource command_pool;
std::pmr::polymorphic_allocator<Command> command_allocator { &command_pool };

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
