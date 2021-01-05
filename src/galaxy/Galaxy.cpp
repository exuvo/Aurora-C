/*
 * Galaxy.cpp
 *
 *  Created on: 9 Nov 2020
 *      Author: exuvo
 */

#include <iostream>
#include <fmt/core.h>

#include <Tracy.hpp>

#include "Galaxy.hpp"
#include "utils/Math.hpp"

void Galaxy::init() {
	LOG4CXX_INFO(log, "initializing galaxy");

	for (StarSystem* system : systems) {
		system->init(this);
	}
	
	updateSpeed();
	
	takenWorkCounter = systems.size();
	completedWorkCounter = systems.size();
	
	uint32_t cores = std::thread::hardware_concurrency();
	if (cores < 1) {
		LOG4CXX_WARN(log, "Unable to determine core count!");
		cores = 1;
	}
	
	for (uint32_t i = 0; i < cores; i++) {
		std::thread *workerThread = new std::thread(&Galaxy::starsystemWorker, this);
		threads.push_back(workerThread);
	}

	galaxyThread = new std::thread(&Galaxy::galaxyWorker, this);
}

void Galaxy::galaxyWorker() {
	tracy::SetThreadName("galaxy-worker");
	
	while(galaxyThread == nullptr) { // We must wait for the 
		std::this_thread::yield();
	}
	
	try {
		nanoseconds accumulator = speed;
		nanoseconds oldSpeed = speed;
		nanoseconds lastSleep = duration_cast<nanoseconds>(steady_clock::now().time_since_epoch());
		nanoseconds lastProcess = duration_cast<nanoseconds>(steady_clock::now().time_since_epoch());
		
		while (!shutdown) {
			nanoseconds now = duration_cast<nanoseconds>(steady_clock::now().time_since_epoch());
			
			if (speed > 0ns) {
				
				if (speed != oldSpeed) {
					accumulator = 0ns;
					oldSpeed = speed;
				} else {
					accumulator += now - lastSleep;
				}
				
				if (accumulator >= speed) {

					//TODO automatically adjust based on computer speed
					tickSize = speed >= duration_cast<nanoseconds>(1ms) ? 1 : duration_cast<nanoseconds>(1ms) / speed;

					// max sensible tick size is 1 minute, unless there is combat..
//						if (tickSize > 60) {
//							tickSize = 60
//						}
					
					const nanoseconds tickSpeed = speed * tickSize;
					
					accumulator -= tickSpeed;

//						println("tickSize $tickSize, speed $speed, diff ${now - lastProcess}, accumulator $accumulator")

					time += tickSize;
					LOG4CXX_TRACE(log, "tick " << time);
					updateDay();
					
					ProfilerEvents& profilerEvents = workingShadow->profilerEvents;
					profilerEvents.clear();
					
					const nanoseconds systemUpdateStart = duration_cast<nanoseconds>(steady_clock::now().time_since_epoch());
					
					profilerEvents.start("commands");
					for (Empire& empire : empires) {
						for (Command* command : empire.commandQueue) {
							try {
								if (command->isValid()) {
									command->getSystem()->commandQueue.push_back(command);
								} else {
									LOG4CXX_WARN(log, "Invalid command " << command);
								}
							} catch (const std::exception &e) {
								LOG4CXX_ERROR(log, "Exception validating command" << command << e.what());
							}
						}
					}
					profilerEvents.end();
					
					completedWorkCounter = 0;
					takenWorkCounter = 0;
					
					profilerEvents.start("run threads");
					{
						std::unique_lock<LockableBase(std::mutex)> lock(workerMutex);
						workerCondvar.notify_all();
					}
					workingShadow->added.clear();
					workingShadow->changed.clear();
					workingShadow->deleted.clear();
					
					profilerEvents.start("process");
					scheduler.update(tickSize);
					profilerEvents.end();
					
					profilerEvents.start("shadow update");
					workingShadow->update();
					profilerEvents.end();
					
					setThreadPriority(*galaxyThread, ThreadPriority::LOW);
					while (completedWorkCounter.load(std::memory_order_relaxed) < systems.size() && !shutdown) {
						std::this_thread::yield();
					}
					profilerEvents.end();
					setThreadPriority(*galaxyThread, ThreadPriority::NORMAL);
					
					profilerEvents.start("shadows lock");
					{
						std::unique_lock<LockableBase(std::mutex)> lock(shadowLock);
						profilerEvents.start("promote shadows");
						for (StarSystem* system : systems) {
							auto oldShadowWorld = system->shadow;
							
							system->shadow = system->workingShadow;
							system->workingShadow = oldShadowWorld;
						}
						
						auto oldShadowWorld = shadow;
						
						shadow = workingShadow;
						workingShadow = oldShadowWorld;
						profilerEvents.end();
					}
					profilerEvents.end();
					
					nanoseconds systemUpdateDuration = duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()) - systemUpdateStart;
					speedLimited = systemUpdateDuration > speed;
					
					if (speedLimited) {
//							log.warn("Galaxy update took ${Units.nanoToString(systemUpdateDuration)} which is more than the requested speed delay ${Units.nanoToString(speed)}")
//							println("Galaxy update took ${Units.nanoToString(systemUpdateDuration)} which is more than the requested speed delay ${Units.nanoToString(speed)}")
					}

//						for (system in systems) {
//							print("${system.sid} ${Units.nanoToString(system.updateTime)}, ")
//						}
//						println()
					
					// If one system took a noticeable larger time to process than others, schedule it earlier
					profilerEvents.start("system sort");
					std::sort(systems.begin(), systems.end(), [s = tickSpeed / 10](const StarSystem* a, const StarSystem* b) -> bool {
						nanoseconds diff = a->updateTime - b->updateTime;
						if (abs(a->updateTime - b->updateTime) <= s) {
							return false;
						}
						
						return a->updateTime < b->updateTime;
					});
					profilerEvents.end();
					
					lastProcess = now;
				}

				lastSleep = now;
				
				// If we are more than 10 ticks behind limit counting
				if (accumulator >= speed * 10) {
					accumulator = speed * 10L;
					
				} else if (accumulator < speed) {

					nanoseconds sleepTime = speed - accumulator;

					if (sleepTime > 1ms) {
						std::unique_lock<std::mutex> lock(galaxyThreadMutex);
						galaxyThreadCondvar.wait_for(lock, sleepTime - 1ms);
						
					} else if ((speed - accumulator) / 1us > 10) {
						std::this_thread::yield();
					}
				}

			} else {
				oldSpeed = speed;
				std::unique_lock<std::mutex> lock(galaxyThreadMutex);
				galaxyThreadCondvar.wait_for(lock, 1s);
			}
		}
		
		{
			std::unique_lock<LockableBase(std::mutex)> lock(workerMutex);
			workerCondvar.notify_all();
		}
		
		for (std::thread* thread : threads) {
			thread->join();
			delete thread;
		}
		
	} catch (const std::exception &e) {
		std::string stackTrace = getLastExceptionStacktrace();
		LOG4CXX_ERROR(log, "Exception in galaxy loop" << e.what() << "\n" << stackTrace);
		speed = 0s;
	}
}

void Galaxy::starsystemWorker() {
	static std::atomic<uint32_t> workerID;
	tracy::SetThreadName(fmt::format("starsystem-worker-{}", workerID++).c_str());
	
	while (!shutdown) {
		{
			std::unique_lock<LockableBase(std::mutex)> lock(workerMutex);
			if (takenWorkCounter >= systems.size()) {
				workerCondvar.wait(lock);
			}
		}

		if (shutdown) {
			return;
		}

		uint32_t systemIndex = takenWorkCounter++;

		//					println("index ${Thread.currentThread().name} = $systemIndex")

		while (systemIndex < systems.size()) {

			StarSystem& system = *systems[systemIndex];

			try {
				nanoseconds systemUpdateStart = duration_cast<nanoseconds>(steady_clock::now().time_since_epoch());
				system.update(tickSize);
				system.updateTime = (duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()) - systemUpdateStart);

				system.updateTimeAverage = exponentialAverage(system.updateTime.count(), system.updateTimeAverage, std::min(100.0, (double)(Units::NANO_SECOND / std::abs(speed.count()))));

			} catch (const std::exception& e) {
				std::string stackTrace = getLastExceptionStacktrace();
				LOG4CXX_ERROR(log, "Exception in system update for $system tick ${galaxy.time}" << e.what() << "\n" << stackTrace);
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

void Galaxy::updateSpeed() {
	int32_t lowestRequestedSpeed = std::numeric_limits<int32_t>::max();
	
	for (Player& player : players) {
		lowestRequestedSpeed = std::min(lowestRequestedSpeed, player.requestedSpeed);
	}
	
	speed = nanoseconds(lowestRequestedSpeed);
	
	if (speed > 0ns) {
		std::unique_lock<std::mutex> lock(galaxyThreadMutex);
		galaxyThreadCondvar.notify_one();
	}
}

int Galaxy::updateDay() {
	day = time / (24L * 60L * 60L);
	return day;
}
