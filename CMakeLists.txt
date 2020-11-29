cmake_minimum_required (VERSION 3.16)

project (AuroraC VERSION 0.1 LANGUAGES CXX)

#TODO speedups https://cmake.org/cmake/help/v3.16/prop_tgt/UNITY_BUILD.html https://cmake.org/cmake/help/latest/command/target_precompile_headers.html
# reflection https://github.com/jsoysouvanh/Refureku
# SIMD https://github.com/VcDevel/Vc
# custom allocators https://github.com/mtrebi/memory-allocators
# stack vectors, strings, allocator https://chromium.googlesource.com/chromium/chromium/+/master/base/stack_container.h
# print stack trace https://stackoverflow.com/questions/38433701/print-log4cxx-exception-stacktrace
# command line parsing cxxopts
# zlib for packaging resources ?
# more links https://github.com/MFatihMAR/Game-Networking-Resources
# polynomial solvers
#  https://www.alglib.net/equations/polynomial.php
#  https://www.gnu.org/software/gsl/doc/html/poly.html#general-polynomial-equations
#  https://www.boost.org/doc/libs/1_53_0/libs/math/doc/sf_and_dist/html/math_toolkit/toolkit/internals1/roots2.html
#  https://eigen.tuxfamily.org/dox/unsupported/classEigen_1_1PolynomialSolver.html
# vulcan multi gpu https://www.ea.com/seed/news/khronos-munich-2018-halcyon-vulkan https://github.com/Glavnokoman/vuh

# log4cxx does not build with mingw (due to we allowed linking to libs built for cygwin, that use different std lib?) but tracy works. 

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "" FORCE)

option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." TRUE)
if (${FORCE_COLORED_OUTPUT})
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		add_compile_options (-fdiagnostics-color=always)
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		add_compile_options (-fcolor-diagnostics)
	endif ()
endif ()

# Prevents cmake from following symbolic links using GLOB
cmake_policy(SET CMP0009 NEW)

#add_subdirectory(lib/Artemis-Cpp)

#if(NOT CYGWIN)
#	set(GIT_SUBMODULE OFF)
#endif()
#add_subdirectory(lib/Vulkan2DRenderer)

set(TRACY_ENABLE ON)
set(TRACY_ON_DEMAND ON)
set(TRACY_NO_EXIT OFF)
set(TRACY_NO_BROADCAST OFF)
add_subdirectory(lib/tracy)

add_subdirectory(lib/entt)

set(IMGUI_EXAMPLES OFF)
set(IMGUI_DISABLE_FILE_FUNCTIONS OFF)
set(IMGUI_ENABLE_STDLIB_SUPPORT ON)

set(IMGUI_IMPL_SDL OFF)
set(IMGUI_IMPL_METAL OFF)
set(IMGUI_IMPL_OSX OFF)
set(IMGUI_IMPL_WIN32 OFF)
set(IMGUI_IMPL_GLFW ON)
set(IMGUI_IMPL_GLUT OFF)
set(IMGUI_IMPL_OPENGL3 OFF)
set(IMGUI_IMPL_OPENGL2 OFF)
set(IMGUI_IMPL_DX9 OFF)
set(IMGUI_IMPL_DX10 OFF)
set(IMGUI_IMPL_DX11 OFF)
set(IMGUI_IMPL_DX12 OFF)
set(IMGUI_IMPL_VULKAN ON)
add_subdirectory(lib/imgui)

message("========== Aurora-C ==========")

find_package(Boost 1.66 REQUIRED) # COMPONENTS
find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(OpenMP REQUIRED)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)
#file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_executable(AuroraC ${SOURCE_FILES})
#add_executable(AuroraC src/AuroraC.cpp)
#add_executable(AuroraC src/AuroraC.cpp lib/tracy/TracyClient.cpp)
target_include_directories(AuroraC PUBLIC src)

if (WIN32 OR CYGWIN)
	target_link_libraries(AuroraC PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/asmlib/libacof64o.lib)
elseif (UNIX)
	target_link_libraries(AuroraC PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/asmlib/libaelf64o.a)
else()
	message(WARNING "No optimized library for this OS, using compiler included")
endif()
target_include_directories(AuroraC PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/asmlib)

target_link_libraries(AuroraC PUBLIC tracy_client)
#target_link_libraries(AuroraC PUBLIC ArtemisCpp)
target_link_libraries(AuroraC PUBLIC EnTT)
#target_link_libraries(AuroraC PUBLIC gdx-cpp)
#target_link_libraries(AuroraC PUBLIC Vulkan2DRenderer)
target_link_libraries(AuroraC PUBLIC ${Boost_LIBRARIES})
target_include_directories(AuroraC PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(AuroraC PUBLIC Eigen3::Eigen)
target_link_libraries(AuroraC PUBLIC OpenMP::OpenMP_CXX)
target_link_libraries(AuroraC PUBLIC imgui)
#target_link_libraries(AuroraC PUBLIC imgui-glfw imgui-vulkan)  

# broken shit
#target_precompile_headers(AuroraC PRIVATE <string>)
#target_precompile_headers(AuroraC PRIVATE "src/PreCompileHeader.h")

set_target_properties(AuroraC PROPERTIES CXX_STANDARD 20)
set_target_properties(AuroraC PROPERTIES CXX_STANDARD_REQUIRED ON)
set_target_properties(AuroraC PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(AuroraC PROPERTIES INTERPROCEDURAL_OPTIMIZATION off) #link time optimization

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
	target_compile_options(AuroraC PRIVATE -march=native)
#	target_compile_options(AuroraC PRIVATE -msse4.2 -mavx2)
	target_compile_options(AuroraC PRIVATE $<$<CONFIG:RELEASE>: -O3>)
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	target_compile_options(AuroraC PRIVATE -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable)
	target_compile_options(AuroraC PRIVATE $<$<CONFIG:RELEASE>: -fno-trapping-math -fno-math-errno -fno-signed-zeros >)
	if (MINGW)
		target_compile_options(AuroraC PRIVATE --warn-no-builtin-declaration-mismatch)
	else()
		target_compile_options(AuroraC PRIVATE -Wno-builtin-declaration-mismatch)
	endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	target_compile_options(AuroraC PRIVATE /arch:SSE4.2 /arch:AVX2)
endif()

#target_link_options(AuroraC PRIVATE -)

if(CYGWIN)
#	set_target_properties(AuroraC PROPERTIES CXX_EXTENSIONS ON) # required on to default POSIX version correctly for wsock32 for inline tracy
#	set_target_properties(AuroraC PROPERTIES LINK_SEARCH_START_STATIC ON) # manually specified libs
#	set_target_properties(AuroraC PROPERTIES LINK_SEARCH_END_STATIC ON) # automatic system libs
elseif(MINGW)
	set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
	set(CMAKE_EXE_LINKER_FLAGS "-static") # static all?
endif()

target_include_directories(AuroraC PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/tracy)
target_include_directories(AuroraC PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/tracy/client)
target_include_directories(AuroraC PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/tracy/common)

#if (UNIX)
#	target_link_libraries(AuroraC PUBLIC ${CMAKE_DL_LIBS})
#endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(AuroraC PRIVATE Threads::Threads)

include(ProcessorCount)
ProcessorCount(cores)
if (cores EQUAL 0)
  message("Unable to determine amount of cpu cores, assuming 1")
  SET(cores 1)
endif()

LIST (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#macro(WinToCygwinPath in_path out_var)
#	if(CMAKE_HOST_SYSTEM_NAME MATCHES "CYGWIN" AND MINGW)
#    string(SUBSTRING ${in_path} 0 1 CYG_DRIVE)
#    string(TOLOWER ${CYG_DRIVE} CYG_DRIVE)
#    string(SUBSTRING ${in_path} 2 -1 TMP_END_OF_PATH)
#    set(${out_var} "/cygdrive/${CYG_DRIVE}${TMP_END_OF_PATH}")
#		message("in " ${in_path} " out " ${${out_var}})
#	endif()
#endmacro()

FIND_PACKAGE (APR)
IF (APR_FOUND)
#  message("Found Apache Runtime Environment")
# should not be needed but APRUTIL seems incapable at finding APR
  SET (APR_WITH --with-apr=/usr/)
ELSE()
  message("APR not found locally, you should install it or let me download and compile during build (slow)")
  include(ExternalProject)
  
  ExternalProject_Add(
    APR
    URL https://ftp.acc.umu.se/mirror/apache.org//apr/apr-1.7.0.tar.gz
    URL_HASH SHA256=48e9dbf45ae3fdc7b491259ffb6ccf7d63049ffacbc1c0977cced095e4c2d5a2
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
    BUILD_COMMAND make -j${cores}
    BUILD_BYPRODUCTS <INSTALL_DIR>/lib/libapr-1.a
  )
  ExternalProject_Get_Property(APR install_dir)
  SET (APR_DEPEND APR)
  SET (APR_WITH --with-apr=${install_dir})
  SET (APR_INCLUDE_DIRS ${install_dir}/include)
  SET (APR_LIBRARY_DIRS ${install_dir}/lib)
  SET (APR_LIBRARIES apr-1)
ENDIF()

IF (APRUTIL_FOUND)
#  message("Found Apache Runtime Environment Utilities")
ELSE()
  message("APRUTIL not found locally, you should install it or let me download and compile during build (slow)")
  include(ExternalProject)
  ExternalProject_Add(
    APRUTIL
    URL https://ftp.acc.umu.se/mirror/apache.org//apr/apr-util-1.6.1.tar.gz
    URL_HASH SHA256=b65e40713da57d004123b6319828be7f1273fbc6490e145874ee1177e112c459
#    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_BUILD_TYPE=Release # windows only
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
    BUILD_COMMAND make -j${cores}
    BUILD_BYPRODUCTS <INSTALL_DIR>/lib/libaprutil-1.a
    DEPENDS ${APR_DEPEND}
  )
  ExternalProject_Get_Property(APRUTIL install_dir)
  SET (APRUTIL_DEPEND APRUTIL)
  SET (APRUTIL_WITH --with-apr-util=${install_dir})
  SET (APRUTIL_INCLUDE_DIRS ${install_dir}/include)
  SET (APRUTIL_LIBRARY_DIRS ${install_dir}/lib)
  SET (APRUTIL_LIBRARIES aprutil-1)
ENDIF()

FIND_PACKAGE (Log4Cxx)
IF (Log4Cxx_FOUND)
#  message("Found Log4Cxx")
ELSE()
  message("Log4Cxx not found locally, you should install it or let me download and compile during build (slow)")
  include(ExternalProject)
  ExternalProject_Add(
    Log4Cxx
    URL https://ftp.acc.umu.se/mirror/apache.org/logging/log4cxx/0.11.0/apache-log4cxx-0.11.0.tar.gz
    URL_HASH SHA512=f8aa37c9c094e7a4d6ca92dff13c032f69f1e078c51ea55e284fcb931c13256b08950af3ea6eaf7a12282240f6073e9acab19bfe217f88dbd62a5d2360f3fbdd
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DBUILD_SHARED_LIBS=no -DBUILD_TESTING=off -DCMAKE_BUILD_TYPE=Release
    DEPENDS ${APR_DEPEND} ${APRUTIL_DEPEND}
  )
  ExternalProject_Get_Property(Log4Cxx install_dir)
  SET (Log4Cxx_INCLUDE_DIRS ${install_dir}/include)
  SET (Log4Cxx_LIBRARY_DIRS ${install_dir}/lib)
  SET (Log4Cxx_LIBRARIES log4cxx)
  target_link_directories(AuroraC PRIVATE ${Log4Cxx_LIBRARY_DIRS})
	add_dependencies(AuroraC Log4Cxx)
  
  FIND_PACKAGE (ODBC) # Log4Cxx automatically builds against this shit if it exists with no build option to skip
	IF (ODBC_FOUND)
  	SET (Log4Cxx_LIBRARIES ${Log4Cxx_LIBRARIES} ${ODBC_LIBRARIES})
	ENDIF()
ENDIF()
target_include_directories(AuroraC PRIVATE ${Log4Cxx_INCLUDE_DIRS})
target_link_libraries(AuroraC PRIVATE ${Log4Cxx_LIBRARIES} ${APRUTIL_LIBRARIES} ${APR_LIBRARIES})
