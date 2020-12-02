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

class StarSystem;

class Command {
	public:
		
		virtual bool isValid() = 0;
		virtual void apply() = 0;
		virtual StarSystem* getSystem() = 0;
		
		virtual void free() = 0;
		virtual ~Command() = default;
		
	private:
};

class EntityCommand : Command {
	public:
		virtual bool isValid();
		virtual StarSystem* getSystem();
		
		virtual ~EntityCommand() = default;
		
	private:
		EntityReference entityRef;
};

class EntityMoveToPositionCommand : EntityCommand {
	public:
		virtual void apply();
		
		virtual ~EntityMoveToPositionCommand() = default;
		
		static void* operator new(size_t);
		static void operator delete(void* ptr);
		
	private:
		EntityReference entityRef;
};

#endif /* SRC_GALAXY_COMMANDS_HPP_ */
