/*
 * Commands.hpp
 *
 *  Created on: 15 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_GALAXY_COMMANDS_HPP_
#define SRC_GALAXY_COMMANDS_HPP_

#include <memory_resource>

#include "starsystems/StarSystem.hpp"
#include "starsystems/components/IDComponents.hpp"

class Command {
	public:
		
		virtual bool isValid() = 0;
		virtual void apply() = 0;
		virtual StarSystem* getSystem() = 0;
		
		virtual ~Command() = default;
		
	private:
};

class EntityCommand : Command {
	public:
		virtual bool isValid();
		virtual StarSystem* getSystem();
		
	private:
		EntityReference entityRef;
};

extern std::pmr::polymorphic_allocator<Command> command_allocator;

#endif /* SRC_GALAXY_COMMANDS_HPP_ */
