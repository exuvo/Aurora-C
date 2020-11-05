cmake_minimum_required (VERSION 3.17)

project (AuroraC VERSION 0.1 LANGUAGES CXX)

#TODO speedups https://cmake.org/cmake/help/v3.16/prop_tgt/UNITY_BUILD.html https://cmake.org/cmake/help/latest/command/target_precompile_headers.html

#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "" FORCE)

option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." TRUE)
if (${FORCE_COLORED_OUTPUT})
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		add_compile_options (-fdiagnostics-color=always)
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		add_compile_options (-fcolor-diagnostics)
	endif ()
endif ()

#add_subdirectory(lib/Artemis-Cpp)
#add_subdirectory(lib/Vulkan2DRenderer)

# does not work
#set(TRACY_ENABLE ON)
#set(TRACY_ON_DEMAND OFF)
#set(TRACY_NO_EXIT ON)
#set(TRACY_NO_BROADCAST OFF)
#add_subdirectory(lib/tracy)

#set(GdxCpp_USE_BOX2D FALSE)
#add_subdirectory(lib/libgdx-cpp)

#set(GDX_SOURCE ../libgdx-cpp)
#set(GDX_SOURCE C:/Users/exuvo/git/libgdx-cpp)
#find_package(GdxCpp REQUIRED TRUE)
#include_directories(${GDXCPP_INCLUDE_DIR})

#set(Boost_USE_STATIC_LIBS ON)
#set(Boost_USE_MULTITHREADED ON)
#set(Boost_USE_STATIC_RUNTIME ON)
#set(Boost_DEBUG ON)
#find_package(Boost 1.50 REQUIRED COMPONENTS filesystem)
#message(STATUS "Boost version: ${Boost_VERSION}")

# Prevents cmake from following symbolic links using GLOB
cmake_policy(SET CMP0009 NEW)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)
#file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

#add_executable(AuroraC ${SOURCE_FILES})
#add_executable(AuroraC src/AuroraC.cpp)
add_executable(AuroraC src/AuroraC.cpp lib/tracy/TracyClient.cpp)
target_include_directories(AuroraC PUBLIC src)

if (WIN32 OR CYGWIN)
	target_link_libraries(AuroraC PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/asmlib/libacof64o.lib)
elseif (UNIX)
	target_link_libraries(AuroraC PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/asmlib/libaelf64o.a)
else()
	message(WARNING "No optimized library for this OS, using compiler included")
endif()
target_include_directories(AuroraC PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/asmlib)

#target_link_libraries(AuroraC PUBLIC tracy_client)
#target_link_libraries(AuroraC PUBLIC ArtemisCpp)
#target_link_libraries(AuroraC PUBLIC gdx-cpp)
#target_link_libraries(AuroraC PUBLIC Vulkan2DRenderer)
#target_link_libraries(AuroraC PUBLIC Boost:filesystem)

#target_precompile_headers(AuroraC PRIVATE "${CMAKE_SOURCE_DIR}/src/PreCompileHeader.h")

target_compile_features(AuroraC PUBLIC cxx_std_20)
set_target_properties(AuroraC PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(AuroraC PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE) #link time optimization
set_target_properties(AuroraC PROPERTIES LINK_SEARCH_START_STATIC ON) # manually specified libs
#set_target_properties(AuroraC PROPERTIES LINK_SEARCH_END_STATIC ON) # automatic system libs
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
set(CMAKE_EXE_LINKER_FLAGS "-static")
#target_link_options(AuroraC PRIVATE -)

target_compile_definitions(AuroraC PUBLIC TRACY_ENABLE)
#target_compile_definitions(AuroraC PUBLIC TRACY_ON_DEMAND)
target_compile_definitions(AuroraC PUBLIC TRACY_NO_EXIT)
if (MINGW)
	target_compile_definitions(AuroraC PUBLIC _WIN32_WINNT=0x0601) # API version win7
	target_link_libraries(AuroraC PUBLIC imagehlp intl wsock32 Ws2_32)
	target_compile_definitions(AuroraC PUBLIC TRACY_NO_SYSTEM_TRACING TRACY_NO_CALLSTACK)
endif()

#if (UNIX)
#	target_link_libraries(AuroraC PUBLIC ${CMAKE_DL_LIBS})
#endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(AuroraC PRIVATE Threads::Threads)
