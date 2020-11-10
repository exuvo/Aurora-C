/*
 * Galaxy.cpp
 *
 *  Created on: 9 Nov 2020
 *      Author: exuvo
 */

#include "Galaxy.hpp"

void Galaxy::init() {

	for (int i = 0; i < 2; i++) {
		std::thread *workerThread = new std::thread(starsystemWorker, this);
		threads.push_back(workerThread);
	}

	galaxyThread = new std::thread(galaxyWorker, this);
}

void Galaxy::galaxyWorker() {
	try {


	} catch (const std::exception &e) {
		LOG4CXX_ERROR(log, "Exception in galaxy loop" << e.what());
		speed = 0s;
	}
}

void Galaxy::starsystemWorker() {
	while (!shutdown) {
		{
			std::unique_lock<std::mutex> lock(mutex);
			if (takenWorkCounter >= systems.size()) {
				condvar.wait(lock);
//				lock.unlock(); automatic when lock is destroyed
			}
		}

		if (shutdown) {
			return;
		}

		uint32_t systemIndex = takenWorkCounter++;

		//					println("index ${Thread.currentThread().name} = $systemIndex")

		while (systemIndex < systems.size()) {

			StarSystem& system = systems[systemIndex];

			try {
				nanoseconds systemUpdateStart = duration_cast<nanoseconds>(steady_clock::now().time_since_epoch());
				system.update(ticksize);
				system.updateTime = (duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()) - systemUpdateStart);

//				system.updateTimeAverage = exponentialAverage(system.updateTime.toDouble(), system.updateTimeAverage, FastMath.min(100.0, (Units.NANO_SECOND / FastMath.abs(galaxy.speed)).toDouble()))

			} catch (const std::exception& e) {
				LOG4CXX_ERROR(log, "Exception in system update for $system tick ${galaxy.time}" << e.what());
				speed = 0s;
			}

			if (completedWorkCounter++ == systems.size()) {
//								galaxy.thread!!.interrupt()
				break;
			}

			systemIndex = takenWorkCounter++;
		}
	}
}
