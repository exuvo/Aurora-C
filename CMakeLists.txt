cmake_minimum_required (VERSION 3.16)

cmake_policy(SET CMP0048 NEW) # set version in project
project (AuroraC VERSION 0.1 LANGUAGES CXX)

# does not work with ninja
#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "" FORCE)

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

# Update library submodules with git.
if(EXISTS "${PROJECT_SOURCE_DIR}/.git" AND NOT EXISTS "${PROJECT_SOURCE_DIR}/lib/Vulkan2DRenderer/CMakeLists.txt")
	find_package(Git QUIET)
	if(GIT_FOUND)
			message(STATUS "Submodule update")
			execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
				WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
				RESULT_VARIABLE GIT_SUBMOD_RESULT)
			if(NOT GIT_SUBMOD_RESULT EQUAL "0")
				message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
			endif()
	else()
		message(FATAL_ERROR "Git not found, make sure submodules are initialized")
	endif()
endif()

set(GLFW_USE_WAYLAND OFF)
set(VK2D_BUILD_DOCS OFF)
set(VK2D_ENABLE_LIBRARY_DEBUG_FEATURES OFF)
set(VK2D_BUILD_TESTS OFF)
set(VK2D_BUILD_EXAMPLES OFF)
set(VK2D_BUILD_STATIC_LIBRARY ON)
add_subdirectory(lib/Vulkan2DRenderer)
 
set(BUILD_TESTING OFF)
add_subdirectory(lib/entt)

set(TRACY_ENABLE ON)
set(TRACY_ON_DEMAND ON)
set(TRACY_NO_EXIT OFF)
set(TRACY_NO_BROADCAST OFF)

set(IMGUI_EXAMPLES OFF)
set(IMGUI_DISABLE_FILE_FUNCTIONS OFF)
set(IMGUI_ENABLE_STDLIB_SUPPORT ON)
set(IMGUI_IMPL_SDL OFF)
set(IMGUI_IMPL_METAL OFF)
set(IMGUI_IMPL_OSX OFF)
set(IMGUI_IMPL_WIN32 OFF)
set(IMGUI_IMPL_GLFW OFF)
set(IMGUI_IMPL_VULKAN ON)

add_subdirectory(lib)

add_subdirectory(lib/Refureku/Refureku)

set(SLIKENET_ENABLE_SAMPLES OFF)
set(SLIKENET_ENABLE_DLL OFF)
set(SLIKENET_ENABLE_STATIC ON)
add_subdirectory(lib/SLikeNet)

message("========== Aurora-C ==========")

find_package(Boost 1.66 REQUIRED) # COMPONENTS
find_package(Eigen3 3.4 REQUIRED NO_MODULE)
find_package(OpenMP REQUIRED)
find_package(fmt REQUIRED)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)
#file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

add_executable(AuroraC ${SOURCE_FILES} "${CMAKE_CURRENT_SOURCE_DIR}/src/refureku/Refureku.cpp")
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
target_link_libraries(AuroraC PUBLIC EnTT)
target_include_directories(AuroraC PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/entt/src)
target_link_libraries(AuroraC PUBLIC VK2D)
target_link_libraries(AuroraC PUBLIC fmt)
target_link_libraries(AuroraC PUBLIC ${Boost_LIBRARIES})
target_include_directories(AuroraC PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(AuroraC PUBLIC Eigen3::Eigen)
target_link_libraries(AuroraC PUBLIC OpenMP::OpenMP_CXX)
target_link_libraries(AuroraC PUBLIC imgui-vulkan)
target_link_libraries(AuroraC PUBLIC glslang glslang-default-resource-limits OGLCompiler OSDependent SPIRV SPVRemapper) #HLSL

target_include_directories(AuroraC PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui_sugar)

target_include_directories(AuroraC PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/tracy)
target_include_directories(AuroraC PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/tracy/client)
target_include_directories(AuroraC PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/tracy/common)

target_include_directories(AuroraC PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/Refureku/Refureku/Library/Include/Public)
target_link_libraries(AuroraC PUBLIC Refureku)

target_include_directories(AuroraC PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/SLikeNet/Source/include)
target_link_libraries(AuroraC PUBLIC SLikeNetLibStatic)

target_precompile_headers(AuroraC PRIVATE "src/PreCompileHeader.h")

set_target_properties(AuroraC PROPERTIES CXX_STANDARD 20)
set_target_properties(AuroraC PROPERTIES CXX_STANDARD_REQUIRED ON)
set_target_properties(AuroraC PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(AuroraC PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE ON) #link time optimization

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
	target_compile_options(AuroraC PRIVATE -march=native)
	#target_compile_options(AuroraC PRIVATE -msse4.2 -mavx2)
#	target_compile_options(AuroraC PRIVATE $<$<CONFIG:DEBUG>: -O1>)
	target_compile_options(AuroraC PRIVATE $<$<CONFIG:RELEASE>: -O3>)
	
	find_library(backtrace_FOUND backtrace)
	if (backtrace_FOUND MATCHES "backtrace_FOUND-NOTFOUND")
		message(WARNING "library backtrace not found, stacktraces might not show filenames. ${backtrace_FOUND}")
		target_compile_options(AuroraC PRIVATE -rdynamic -DBOOST_STACKTRACE_USE_ADDR2LINE=/usr/bin/addr2line)
		target_link_libraries(AuroraC PUBLIC dl)
	else()
		target_compile_options(AuroraC PRIVATE -rdynamic -DBOOST_STACKTRACE_USE_BACKTRACE)
		target_link_libraries(AuroraC PUBLIC backtrace) 
	endif()
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
#	target_compile_options(AuroraC PRIVATE -ftime-report) # debug compile time
	target_compile_options(AuroraC PRIVATE -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-stringop-truncation -Wno-narrowing -Wno-frame-address -Wno-invalid-offsetof)
	target_compile_options(AuroraC PRIVATE -fno-extern-tls-init) # avoid thread_local support for dynamic init overhead, alternative compiler specific __thread
	target_compile_options(AuroraC PRIVATE $<$<CONFIG:DEBUG>: -fno-signed-zeros -funsafe-math-optimizations -ffinite-math-only -fno-rounding-math -fcx-limited-range >)
	target_compile_options(AuroraC PRIVATE $<$<CONFIG:RELEASE>: -ffast-math >)
	if (MINGW)
		target_compile_options(AuroraC PRIVATE --warn-no-builtin-declaration-mismatch)
	else()
		target_compile_options(AuroraC PRIVATE -Wno-builtin-declaration-mismatch)
	endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(_CMAKE_LTO_THIN TRUE)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	target_compile_options(AuroraC PRIVATE /arch:SSE4.2 /arch:AVX2)
endif()

# gprof profiling
#target_compile_options(AuroraC PRIVATE -pg)
#target_link_options(AuroraC PRIVATE -pg)

#	set_target_properties(AuroraC PROPERTIES LINK_SEARCH_START_STATIC ON) # manually specified libs
#	set_target_properties(AuroraC PROPERTIES LINK_SEARCH_END_STATIC ON) # automatic system libs

if(MINGW)
	set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
	set(CMAKE_EXE_LINKER_FLAGS "-static") # static all?
endif()

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

################
# Refureku
################

macro(fixPathsForRefureku list)
string(PREPEND ${list} "\"")
string(REPLACE ";" "\",\n\"" ${list} "${${list}}")
string(APPEND ${list} "\"")
endmacro()

#set(RefurekuSources ${HEADER_FILES})
#list(FILTER RefurekuSources EXCLUDE REGEX "src/refureku/.*")
file(GLOB_RECURSE RefurekuSources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/starsystems/components/*.hpp")
file(GLOB_RECURSE RefurekuGenSources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/refureku/*.rfks.h")

set(RefurekuInputFiles "${RefurekuSources}")
fixPathsForRefureku(RefurekuInputFiles)

#set(RefurekuInputDirs "${CMAKE_CURRENT_SOURCE_DIR}/src")
#fixPathsForRefureku(RefurekuInputDirs)

set(RefurekuIgnoreDirs "")
set(RefurekuIgnoreFiles "")
set(RefurekuOutputDir "${CMAKE_CURRENT_SOURCE_DIR}/src/refureku")

string(APPEND RefurekuIgnoreDirs "${RefurekuOutputDir}")
fixPathsForRefureku(RefurekuIgnoreDirs)

get_target_property(RefurekuIncludeDirs AuroraC INCLUDE_DIRECTORIES)
fixPathsForRefureku(RefurekuIncludeDirs)

list(TRANSFORM RefurekuGenSources PREPEND "#include \"")
list(TRANSFORM RefurekuGenSources APPEND "\"")
string(REPLACE ";" "\n" RefurekuGenSources "${RefurekuGenSources}")

configure_file("${PROJECT_SOURCE_DIR}/gensrc/RefurekuSettings.toml" "${PROJECT_BINARY_DIR}/RefurekuSettings.toml")
configure_file("${PROJECT_SOURCE_DIR}/gensrc/Refureku.cpp" "${RefurekuOutputDir}/Refureku.cpp")

add_custom_target(RunRefurekuGenerator
	COMMAND RefurekuGenerator "${PROJECT_BINARY_DIR}/RefurekuSettings.toml" > refureku.log
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/"
	DEPENDS "${PROJECT_BINARY_DIR}/RefurekuSettings.toml;${RefurekuSources}"
	BYPRODUCTS ${RefurekuOutputDir} "${RefurekuOutputDir}/Refureku.cpp"
)
add_dependencies(AuroraC RunRefurekuGenerator)

################
# Assets
################

add_custom_command(
	OUTPUT "${PROJECT_BINARY_DIR}/assets"
	COMMENT "Symlinking assets"
	COMMAND ${CMAKE_COMMAND} -E create_symlink "${PROJECT_SOURCE_DIR}/assets" "${PROJECT_BINARY_DIR}/assets"
)
add_custom_target(symlink_assets DEPENDS "${PROJECT_BINARY_DIR}/assets")
add_dependencies(AuroraC symlink_assets)
