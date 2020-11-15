/*
 * Galaxy.cpp
 *
 *  Created on: 9 Nov 2020
 *      Author: exuvo
 */

#include <iostream>

#include "Tracy.hpp"

#include "Galaxy.hpp"

void Galaxy::init() {

	for (int i = 0; i < 2; i++) {
		std::thread *workerThread = new std::thread(starsystemWorker, this);
		threads.push_back(workerThread);
	}

	galaxyThread = new std::thread(galaxyWorker, this);
}

void Galaxy::galaxyWorker() {
	tracy::SetThreadName("galaxy-worker");
	
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

					time += seconds(tickSize);
					LOG4CXX_INFO(log, "tick " << time.count());
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
						std::unique_lock<std::mutex> lock(mutex);
						condvar.notify_all();
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
						std::unique_lock<std::recursive_mutex> lock(shadowLock);
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
						sleeping = true;
						std::this_thread::sleep_for(sleepTime - 1ms);
						sleeping = false;
						
					} else if ((speed - accumulator) / 1us > 10) {
						sleeping = true;
						std::this_thread::yield();
						sleeping = false;
					}
				}

			} else {
				oldSpeed = speed;
				sleeping = true;
				std::this_thread::sleep_for(1s);
				sleeping = false;
			}
		}
		
		for (std::thread* thread : threads) {
			thread->join();
		}
		
	} catch (const std::exception &e) {
		LOG4CXX_ERROR(log, "Exception in galaxy loop" << e.what());
		speed = 0s;
	}
}

void Galaxy::starsystemWorker() {
	tracy::SetThreadName("starsystem-worker");
	
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

			StarSystem& system = *systems[systemIndex];

			try {
				nanoseconds systemUpdateStart = duration_cast<nanoseconds>(steady_clock::now().time_since_epoch());
				system.update(tickSize);
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

int Galaxy::updateDay() {
	day = time.count() / (24L * 60L * 60L);
	return day;
}
