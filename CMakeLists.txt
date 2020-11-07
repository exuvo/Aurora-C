cmake_minimum_required (VERSION 3.17)

project (AuroraC VERSION 0.1 LANGUAGES CXX)

#TODO speedups https://cmake.org/cmake/help/v3.16/prop_tgt/UNITY_BUILD.html https://cmake.org/cmake/help/latest/command/target_precompile_headers.html
# c++ reflection https://github.com/jsoysouvanh/Refureku

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "" FORCE)

option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." TRUE)
if (${FORCE_COLORED_OUTPUT})
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		add_compile_options (-fdiagnostics-color=always)
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		add_compile_options (-fcolor-diagnostics)
	endif ()
endif ()

add_subdirectory(lib/Artemis-Cpp)
#add_subdirectory(lib/Vulkan2DRenderer)

set(TRACY_ENABLE ON)
set(TRACY_ON_DEMAND ON)
set(TRACY_NO_EXIT OFF)
set(TRACY_NO_BROADCAST OFF)
add_subdirectory(lib/tracy)

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

#file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)
#file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

#add_executable(AuroraC ${SOURCE_FILES} lib/tracy/TracyClient.cpp)
add_executable(AuroraC src/AuroraC.cpp)
#add_executable(AuroraC src/AuroraC.cpp lib/tracy/TracyClient.cpp)
target_include_directories(AuroraC PUBLIC src)

#if (WIN32 OR CYGWIN)
#	target_link_libraries(AuroraC PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/asmlib/libacof64o.lib)
#elseif (UNIX)
#	target_link_libraries(AuroraC PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/asmlib/libaelf64o.a)
#else()
#	message(WARNING "No optimized library for this OS, using compiler included")
#endif()
#target_include_directories(AuroraC PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/asmlib)

target_link_libraries(AuroraC PUBLIC tracy_client)
target_link_libraries(AuroraC PUBLIC ArtemisCpp)
#target_link_libraries(AuroraC PUBLIC gdx-cpp)
#target_link_libraries(AuroraC PUBLIC Vulkan2DRenderer)
#target_link_libraries(AuroraC PUBLIC Boost:filesystem)

#target_precompile_headers(AuroraC PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/PreCompileHeader.h")

target_compile_features(AuroraC PUBLIC cxx_std_20)
set_target_properties(AuroraC PROPERTIES CXX_EXTENSIONS ON) # required on to set POSIX version correctly for wsock32 for inline tracy
set_target_properties(AuroraC PROPERTIES INTERPROCEDURAL_OPTIMIZATION off) #link time optimization
#target_link_options(AuroraC PRIVATE -)

if(CYGWIN)
	set_target_properties(AuroraC PROPERTIES CXX_EXTENSIONS ON) # required on to default POSIX version correctly for wsock32 for inline tracy
elseif(MINGW)
	#set_target_properties(AuroraC PROPERTIES LINK_SEARCH_START_STATIC ON) # manually specified libs
	#set_target_properties(AuroraC PROPERTIES LINK_SEARCH_END_STATIC ON) # automatic system libs
	set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
	set(CMAKE_EXE_LINKER_FLAGS "-static") # static all?
endif()

#target_compile_definitions(AuroraC PUBLIC TRACY_ENABLE TRACY_ONLY_LOCALHOST TRACY_ONLY_IPV4)
##target_compile_definitions(AuroraC PUBLIC TRACY_DELAYED_INIT TRACY_MANUAL_LIFETIME)
#target_compile_definitions(AuroraC PUBLIC TRACY_ON_DEMAND)
##target_compile_definitions(AuroraC PUBLIC TRACY_NO_EXIT)
#if (WIN32)
#	target_compile_definitions(AuroraC PUBLIC _WIN32_WINNT=0x0601) # API version win7
#	target_link_libraries(AuroraC PRIVATE imagehlp intl wsock32 Ws2_32)
#	target_compile_definitions(AuroraC PUBLIC TRACY_NO_SYSTEM_TRACING)
#	target_compile_definitions(AuroraC PUBLIC TRACY_NO_CALLSTACK) # does not work on win7
#endif()
target_include_directories(AuroraC PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/tracy>)
target_include_directories(AuroraC PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/tracy/client>)
target_include_directories(AuroraC PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/tracy/common>)

#if (UNIX)
#	target_link_libraries(AuroraC PUBLIC ${CMAKE_DL_LIBS})
#endif()

#set(THREADS_PREFER_PTHREAD_FLAG ON)
#find_package(Threads REQUIRED)
#target_link_libraries(AuroraC PRIVATE Threads::Threads)

#if (CYGWIN) 
#	target_compile_options(AuroraC PUBLIC -mcmodel=small)
#endif()

include(ProcessorCount)
ProcessorCount(cores)
if (cores EQUAL 0)
  message("Unable to determine amount of cpu cores, assuming 1")
  SET(cores 1)
endif()

LIST (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#FIND_PACKAGE (APR)
#IF (APR_FOUND)
##  message("Found Apache Runtime Environment")
## should not be needed but APRUTIL seems incapable at finding APR
#  SET (APR_WITH --with-apr=/usr/)
#ELSE()
#  message("APR not found locally, you should install it or let me download and compile during build (slow)")
#  message("dir ${APR_PREFIX}")
#  include(ExternalProject)
#  ExternalProject_Add(
#    APR
#    URL https://ftp.acc.umu.se/mirror/apache.org//apr/apr-1.7.0.tar.gz
#    URL_HASH SHA256=48e9dbf45ae3fdc7b491259ffb6ccf7d63049ffacbc1c0977cced095e4c2d5a2
#    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DMIN_WINDOWS_VER=Windows7 -DCMAKE_BUILD_TYPE=RelWithDebInfo
#    #BUILD_BYPRODUCTS <INSTALL_DIR>/lib/libapr-1.a
#  )
#  ExternalProject_Get_Property(APR install_dir)
#  SET (APR_DEPEND APR)
#  SET (APR_WITH --with-apr=${install_dir})
#  SET (APR_INCLUDE_DIRS ${install_dir}/include)
#  SET (APR_LIBRARY_DIRS ${install_dir}/lib)
#  SET (APR_LIBRARIES apr-1)
#ENDIF()
#target_link_libraries(AuroraC PRIVATE ${APR_LIBRARIES})
#
#IF (APRUTIL_FOUND)
##  message("Found Apache Runtime Environment Utilities")
#ELSE()
#  message("APRUTIL not found locally, you should install it or let me download and compile during build (slow)")
#  include(ExternalProject)
#  ExternalProject_Add(
#    APRUTIL
#    URL https://ftp.acc.umu.se/mirror/apache.org//apr/apr-util-1.6.1.tar.gz
#    URL_HASH SHA256=b65e40713da57d004123b6319828be7f1273fbc6490e145874ee1177e112c459
#    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_BUILD_TYPE=RelWithDebInfo
#    CONFIGURE_COMMAND SOURCE_DIR/configure --prefix=INSTALL_DIR
#    DEPENDS ${APR_DEPEND}
#    #BUILD_BYPRODUCTS <INSTALL_DIR>/lib/libaprutil-1.a
#  )
#  ExternalProject_Get_Property(APRUTIL install_dir)
#  SET (APRUTIL_DEPEND APRUTIL)
#  SET (APRUTIL_WITH --with-apr-util=${install_dir})
#  SET (APRUTIL_INCLUDE_DIRS ${install_dir}/include)
#  SET (APRUTIL_LIBRARY_DIRS ${install_dir}/lib)
#  SET (APRUTIL_LIBRARIES aprutil-1)
#ENDIF()
#target_link_libraries(AuroraC PRIVATE ${APRUTIL_LIBRARIES})
#
#FIND_PACKAGE (Log4Cxx)
#IF (Log4Cxx_FOUND)
##  message("Found Log4Cxx")
#	target_include_directories(AuroraC PRIVATE $<BUILD_INTERFACE:${Log4Cxx_INCLUDE_DIRS}>)
#ELSE()
#  message("Log4Cxx not found locally, you should install it or let me download and compile during build (slow)")
#  include(ExternalProject)
#  ExternalProject_Add(
#    Log4Cxx
#    URL https://ftp.acc.umu.se/mirror/apache.org/logging/log4cxx/0.11.0/apache-log4cxx-0.11.0.tar.gz
#    URL_HASH SHA512=f8aa37c9c094e7a4d6ca92dff13c032f69f1e078c51ea55e284fcb931c13256b08950af3ea6eaf7a12282240f6073e9acab19bfe217f88dbd62a5d2360f3fbdd
#    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DBUILD_SHARED_LIBS=no -DBUILD_TESTING=off
#    DEPENDS ${APR_DEPEND} ${APRUTIL_DEPEND}
#    #BUILD_BYPRODUCTS <INSTALL_DIR>/lib/liblog4cxx.a
#  )
#  ExternalProject_Get_Property(Log4Cxx install_dir)
##  SET (Log4Cxx_INCLUDE_DIRS ${install_dir}/include)
#  SET (Log4Cxx_LIBRARY_DIRS ${install_dir}/lib)
#  SET (Log4Cxx_LIBRARIES log4cxx)
#  target_link_directories(AuroraC PRIVATE ${Log4Cxx_LIBRARY_DIRS})
#  target_include_directories(AuroraC PRIVATE $<TARGET_PROPERTY:Log4Cxx,INTERFACE_INCLUDE_DIRECTORIES>)
#ENDIF()
#target_link_libraries(AuroraC PRIVATE ${Log4Cxx_LIBRARIES})
#add_dependencies(AuroraC Log4Cxx)

