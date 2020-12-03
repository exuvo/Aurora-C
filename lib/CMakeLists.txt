cmake_minimum_required (VERSION 3.16)

project(tracy)

message("========== Tracy ==========")

set(CMAKE_CXX_STANDARD 11)

option(TRACY_ENABLE "Profiling is enabled" ON)
option(TRACY_ON_DEMAND "Profiling enabled on demand" OFF)
option(TRACY_NO_EXIT "Client executable does not exit until all profile data is sent to server" OFF)
option(TRACY_NO_BROADCAST "Disable client discovery by broadcast to local network" OFF)

add_library(tracy_client STATIC tracy/TracyClient.cpp)

macro(set_option OPTION)
	if(${OPTION})
		message(STATUS "${OPTION}: ON")
			target_compile_definitions(tracy_client PUBLIC ${OPTION})
	else()
		message(STATUS "${OPTION}: OFF")
	endif()
endmacro()

set_option(TRACY_ENABLE)
set_option(TRACY_ON_DEMAND)
set_option(TRACY_NO_EXIT)
set_option(TRACY_NO_BROADCAST)

#target_compile_definitions(tracy_client PUBLIC TRACY_ONLY_LOCALHOST) # broken
target_compile_definitions(tracy_client PUBLIC TRACY_ONLY_IPV4) 
target_compile_definitions(tracy_client PUBLIC TRACY_DELAYED_INIT TRACY_MANUAL_LIFETIME)

target_include_directories(tracy_client PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

#set_target_properties(tracy_client PROPERTIES LINK_SEARCH_START_STATIC ON) # manually specified libs
#set_target_properties(tracy_client PROPERTIES LINK_SEARCH_END_STATIC ON) # automatic system libs

if (CYGWIN OR MINGW)
	target_compile_definitions(tracy_client PRIVATE _WIN32_WINNT=0x0601) # API version win7
	target_compile_definitions(tracy_client PRIVATE TRACY_NO_SYSTEM_TRACING)
endif()

if (MINGW)
	target_link_libraries(tracy_client PRIVATE imagehlp intl wsock32 Ws2_32)
endif()

if (CYGWIN OR MINGW OR (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows" AND CMAKE_HOST_SYSTEM_VERSION MATCHES "6.1.7601"))
	target_compile_definitions(tracy_client PRIVATE TRACY_NO_CALLSTACK) # requires minimum win 8.1
endif()

if (UNIX)
	target_link_libraries(tracy_client PRIVATE ${CMAKE_DL_LIBS})
#	set(THREADS_PREFER_PTHREAD_FLAG ON)
#	find_package(Threads REQUIRED)
#	target_link_libraries(tracy_client PRIVATE Threads::Threads)
endif()
