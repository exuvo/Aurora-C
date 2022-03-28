/*
 * WeaponSystem.cpp
 *
 *  Created on: 1 Dec 2020
 *      Author: exuvo
 */

#include <iostream>

#include "starsystems/systems/Systems.hpp"
#include "utils/Utils.hpp"

void ColonySystem::init(void* data) {
	Systems* systems = (Systems*) data;
//	LOG4CXX_INFO(log, "init");
	
}

void ColonySystem::update(delta_type delta) {
	auto view = registry.view<ColonyComponent, PlanetComponent>();
		
		for (entt::entity entity : view) {
			ColonyComponent& colony = view.get<ColonyComponent>(entity);
			PlanetComponent& planet = view.get<PlanetComponent>(entity);
			
			
			
//			starSystem.changed<ColonyComponent>(entity);
		}
}
