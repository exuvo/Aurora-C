#ifndef PROCESS_SCHEDULER_HPP
#define PROCESS_SCHEDULER_HPP

#include <algorithm>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "entt/config/config.h"
#include "entt/core/type_traits.hpp"
#include "log4cxx/logger.h"

#include "utils/Profiling.hpp"

template<typename Derived, typename Delta>
class process {
	public:
		using delta_type = Delta;
		
    /*! @brief Default destructor. */
    virtual ~process() {
        static_assert(std::is_base_of_v<process, Derived>, "Incorrect use of the class template");
    }
    
    void init(void* data = nullptr) const ENTT_NOEXCEPT {}
    bool checkProcessing(void) const ENTT_NOEXCEPT {
    	return true;
    }
    void preUpdate(void) const ENTT_NOEXCEPT {}
    void update(const Delta delta) const ENTT_NOEXCEPT {}
    void postUpdate(void) const ENTT_NOEXCEPT {}
    
	protected:
    
	private:
    
};

template<typename Delta>
class scheduler {
	public:
		struct process_handler;
	private:
		template<typename Proc>
		static void deleter(void* proc) {
			delete static_cast<Proc*>(proc);
		}
		
		template<typename Proc>
		static bool checkProcessing(process_handler& handler) {
			return static_cast<Proc*>(handler.instance.get())->checkProcessing();
		}
		
		template<typename Proc>
		static void preUpdate(process_handler& handler) {
			static_cast<Proc*>(handler.instance.get())->preUpdate();
		}
		
		template<typename Proc>
		static void update(process_handler& handler, Delta delta) {
			static_cast<Proc*>(handler.instance.get())->update(delta);
		}
		
		template<typename Proc>
		static void postUpdate(process_handler& handler) {
			static_cast<Proc*>(handler.instance.get())->postUpdate();
		}
		
		template<typename Proc>
		static void init(process_handler& handler, void* data) {
			static_cast<Proc*>(handler.instance.get())->init(data);
		}
		
		template<typename Proc>
		static const char* name() {
			return typeid(Proc).name();
		}
	
	public:
		struct process_handler {
				using instance_type = std::unique_ptr<void, void(*)(void*)>;
				using checkProcessing_fn_type = bool (process_handler&);
				using preUpdate_fn_type = void (process_handler&);
				using update_fn_type = void (process_handler&, Delta);
				using postUpdate_fn_type = void (process_handler&);
				using init_fn_type = void (process_handler&, void*);
				using name_fn_type = const char* (void);

				instance_type instance;
				checkProcessing_fn_type* checkProcessing;
				preUpdate_fn_type* preUpdate;
				update_fn_type* update;
				postUpdate_fn_type* postUpdate;
				init_fn_type* init;
				name_fn_type* name;
		};

		scheduler() = default;
		scheduler(scheduler&&) = default; // move constructor
		scheduler& operator=(scheduler&&) = default; // move assignment operator
		
		std::vector<process_handler> handlers { };
		bool profiling = false;
		ProfilerEvents profilerEvents;

		/**
		 * @brief Number of processes currently scheduled.
		 * @return Number of processes currently scheduled.
		 */
		[[nodiscard]] size_t size() const ENTT_NOEXCEPT {
			return handlers.size();
		}
		
		/**
		 * @brief Returns true if at least a process is currently scheduled.
		 * @return True if there are scheduled processes, false otherwise.
		 */
		[[nodiscard]] bool empty() const ENTT_NOEXCEPT {
			return handlers.empty();
		}
		
		/**
		 * @brief Discards all scheduled processes.
		 *
		 * Processes aren't aborted. They are discarded along with their children
		 * and never executed again.
		 */
		void clear() {
			handlers.clear();
		}
		
		/**
		 * @brief Schedules a process for the next tick.
		 *
		 * Example of use (pseudocode):
		 *
		 * @code{.cpp}
		 * // schedules a task in the form of a process class
		 * scheduler.attach<my_process>(arguments...)
		 * // appends a child in the form of a lambda function
		 * .then([](auto delta, void *, auto succeed, auto fail) {
		 *     // code
		 * })
		 * // appends a child in the form of another process class
		 * .then<my_other_process>();
		 * @endcode
		 *
		 * @tparam Proc Type of process to schedule.
		 * @tparam Args Types of arguments to use to initialize the process.
		 * @param args Parameters to use to initialize the process.
		 */
		template<typename Proc, typename ... Args>
		void attach(Args&& ... args) {
			static_assert(std::is_base_of_v<process<Proc, Delta>, Proc>, "Invalid process type");
			auto proc = typename process_handler::instance_type { new Proc { std::forward<Args>(args) ... }, &scheduler::deleter<Proc> };
			process_handler handler { std::move(proc), 
				&scheduler::checkProcessing<Proc>, 
				&scheduler::preUpdate<Proc>,
				&scheduler::update<Proc>, 
				&scheduler::postUpdate<Proc>, 
				&scheduler::init<Proc>,
				&scheduler::name<Proc>
			};
			
			handlers.emplace_back(std::move(handler));
		}
		
		void init(void* data = nullptr) {
			for (process_handler& handler : handlers) {
				handler.init(handler, data);
			}
		}
		
		/**
		 * @brief Updates all scheduled processes.
		 *
		 * All scheduled processes are executed in order.<br/>
		 *
		 * @param delta Elapsed time.
		 */
		void update(const Delta delta) {
			size_t size = handlers.size();
			bool active[size];
			
			for (size_t i = 0; i < size; i++) {
				process_handler& handler = handlers[i];
				active[i] = handler.checkProcessing(handler);
			}
			
			if (!profiling) {
				
				for (size_t i = 0; i < size; i++) {
					if (active[i]) {
						process_handler& handler = handlers[i];
						handler.preUpdate(handler);
					}
				}
				
				for (size_t i = 0; i < size; i++) {
					if (active[i]) {
						process_handler& handler = handlers[i];
						handler.update(handler, delta);
					}
				}
				
				for (size_t i = 0; i < size; i++) {
					if (active[i]) {
						process_handler& handler = handlers[i];
						handler.postUpdate(handler);
					}
				}
				
			} else {
				
				profilerEvents.clear();
				
				profilerEvents.start("preUpdate");
				for (size_t i = 0; i < size; i++) {
					if (active[i]) {
						process_handler& handler = handlers[i];
						profilerEvents.start(handler.name());
						handler.preUpdate(handler);
						profilerEvents.end();
					}
				}
				profilerEvents.end();
				
				profilerEvents.start("update");
				for (size_t i = 0; i < size; i++) {
					if (active[i]) {
						process_handler& handler = handlers[i];
						profilerEvents.start(handler.name());
						handler.update(handler, delta);
						profilerEvents.end();
					}
				}
				profilerEvents.end();
				
				profilerEvents.start("postUpdate");
				for (size_t i = 0; i < size; i++) {
					if (active[i]) {
						process_handler& handler = handlers[i];
						profilerEvents.start(handler.name());
						handler.postUpdate(handler);
						profilerEvents.end();
					}
				}
				profilerEvents.end();
				
			}
		}
		
	private:
};

#endif
