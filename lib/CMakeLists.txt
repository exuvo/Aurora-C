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
	target_link_libraries(tracy_client PRIVATE imagehlp)
endif()

if (MINGW)
	target_link_libraries(tracy_client PRIVATE intl wsock32 Ws2_32)
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

project (imgui)
message("========== ImGUI ==========")

# Example of using Dear ImGui in your CMake project:
#
#   # Set various library options:
#   set(IMGUI_SDL_TARGET sdl CACHE STRING "" FORCE)
#   # Include Dear ImGui into build.
#   add_subdirectory(path/to/imgui)
#   <..>
#   # Link Dear ImGui to your application:
#   #  imgui - a main library target
#   #  imgui-sdl - a platform backend target
#   #  imgui-dx11 - a graphics backend target
#   target_link_libraries(YourApplication PUBLIC imgui imgui-sdl imgui-dx11)
#
# A list of defined targets:
#   imgui-sdl: platform backend for Windows/Linux/MacOS/etc using SDL2.
#   imgui-glfw: platform backend for Windows/Linux/MacOS/etc using GLFW.
#   imgui-osx: platform backend for MacOS.
#   imgui-win32: platform backend for Windows.
#   imgui-glut: platform backend for Windows/Linux/MacOS using glut/freeglut with OpenGL.
#   imgui-metal: graphics backend for MacOS using Metal graphics API.
#   imgui-opengl3: graphics backend for Windows/Linux/MacOS/etc using OpenGL3 graphics API.
#   imgui-opengl2: graphics backend for Windows/Linux/MacOS/etc using OpenGL2 graphics API.
#   imgui-vulkan: graphics backend for Windows/Linux/MacOS/etc using Vulkan graphics API.
#   imgui-dx9: graphics backend for Windows using DirectX 9 graphics API.
#   imgui-dx10: graphics backend for Windows using DirectX 10 graphics API.
#   imgui-dx11: graphics backend for Windows using DirectX 11 graphics API.
#   imgui-dx12: graphics backend for Windows using DirectX 12 graphics API.
#
# from https://gist.github.com/rokups

# CMake: 3.13: option() honors normal variables. Sufficiently new CMake and this policy allow user to configure library
# by setting plain variables as opposed to cache variables as demonstrated in example.
cmake_policy(SET CMP0077 NEW)

option(IMGUI_EXAMPLES "Build ImGui examples" ON)
option(IMGUI_DEMO "Include the ImGui demo window implementation in library" ON)

option(IMGUI_DISABLE_OBSOLETE_FUNCTIONS "Disable deprecated functions" ON)
option(IMGUI_DISABLE_FILE_FUNCTIONS "Disable use of file functions" OFF)
option(IMGUI_ENABLE_STDLIB_SUPPORT "Enable extra functions taking stdlib types" OFF)
option(IMGUI_ENABLE_COVERAGE "Enable coverage testing for supported compilers" OFF)

set(IMGUI_SDL_TARGET "" CACHE STRING "A custom SDL target name that will be used for linking to examples.")
set(IMGUI_GLFW_TARGET "" CACHE STRING "A custom GLFW target name that will be used for linking to examples.")
set(IMGUI_GLUT_TARGET "" CACHE STRING "A custom GLFW target name that will be used for linking to examples.")

option(IMGUI_IMPL_SDL "Build the SDL implementation (only if supported)" ON)
option(IMGUI_IMPL_METAL "Build the Metal implementation (only if supported)" ${APPLE})
option(IMGUI_IMPL_OSX "Build the OSX implementation (only if supported)" ${APPLE})
option(IMGUI_IMPL_WIN32 "Build the Win32 (native winapi) implementation (only if supported)" ${WIN32})
option(IMGUI_IMPL_GLFW "Build the GLFW implementation (only if supported)" ON)
option(IMGUI_IMPL_VULKAN "Build the Vulkan implementation (only if supported)" ON)

if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    set (IMGUI_IMPL_GLFW OFF)
endif ()

# Tidy up build dir, but only if user has not configured it yet.
if (NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif ()
if (NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif ()
if (NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif ()

# Use fresh C++ standard, but only if user has not configured it yet.
if (NOT DEFINED CMAKE_CXX_STANDARD)
    set (CMAKE_CXX_STANDARD 11)
endif ()

# Warnings.
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Werror -Wunused-parameter -Wformat $<$<COMPILE_LANGUAGE:CXX>:-pedantic>)
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # These warnings are overzealous in GCC and only practical to use with Clang.
    add_compile_options(-Wsign-conversion -Wshadow)
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-class-memaccess>)
endif ()

set (IMGUI_ROOT_DIR ${PROJECT_SOURCE_DIR}/imgui)
set (IMGUI_EXAMPLES_DIR ${IMGUI_ROOT_DIR}/examples)
set (IMGUI_BACKENDS_DIR ${IMGUI_ROOT_DIR}/backends)

# Set up main library
add_library(imgui ${IMGUI_ROOT_DIR}/imgui.h ${IMGUI_ROOT_DIR}/imgui_internal.h ${IMGUI_ROOT_DIR}/imgui.cpp
    ${IMGUI_ROOT_DIR}/imgui_draw.cpp ${IMGUI_ROOT_DIR}/imgui_widgets.cpp ${IMGUI_ROOT_DIR}/imgui_tables.cpp
)
if (IMGUI_DEMO)
    target_sources(imgui PRIVATE ${IMGUI_ROOT_DIR}/imgui_demo.cpp)
endif ()
if (IMGUI_DISABLE_OBSOLETE_FUNCTIONS)
    target_compile_definitions(imgui PUBLIC -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1)
endif ()
if (IMGUI_DISABLE_FILE_FUNCTIONS)
    target_compile_definitions(imgui PUBLIC -DIMGUI_DISABLE_FILE_FUNCTIONS=1)
endif ()
if (IMGUI_USER_CONFIG)
    target_compile_definitions(imgui PUBLIC "-DIMGUI_USER_CONFIG=<${IMGUI_USER_CONFIG}>")
endif ()
target_include_directories(imgui PUBLIC ${IMGUI_ROOT_DIR})
if (NOT IMGUI_DEMO)
    target_compile_definitions(imgui PUBLIC -DIMGUI_DISABLE_DEMO_WINDOWS=1)
endif ()
if (IMGUI_ENABLE_STDLIB_SUPPORT)
    target_include_directories(imgui PUBLIC ${IMGUI_ROOT_DIR}/misc/cpp)
    target_sources(imgui PRIVATE ${IMGUI_ROOT_DIR}/misc/cpp/imgui_stdlib.h ${IMGUI_ROOT_DIR}/misc/cpp/imgui_stdlib.cpp)
endif ()
if (IMGUI_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(imgui PUBLIC -fprofile-arcs -ftest-coverage)
    target_link_libraries(imgui PUBLIC -fprofile-arcs -ftest-coverage)
    target_link_libraries(imgui PUBLIC gcov)
endif ()

# Ignore silently when required headers are missing.
set(CMAKE_REQUIRED_QUIET ON)
include(CheckIncludeFile)

# PkgConfig will be needed for SDL and glfw on some platforms.
find_package(PkgConfig QUIET)

# Backends
if (IMGUI_IMPL_SDL)
    find_package(SDL2 QUIET)
    if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
        # Nothing to do for emscripten builds. SDL is bunled with the compiler.
    elseif (IMGUI_SDL_TARGET)
        # Custom user target was set. No auto-detection required.
    elseif (TARGET SDL2::SDL2)
        # Some platforms (Linux) have SDL target properly exported as CMake target.
    elseif (SDL2_FOUND)
        # Platforms that do not export target but use old CMake conventions can be handled this way.
        add_library(SDL2::SDL2 INTERFACE IMPORTED)
        target_link_libraries(SDL2::SDL2 INTERFACE ${SDL2_LIBRARIES})
        target_include_directories(SDL2::SDL2 INTERFACE ${SDL2_INCLUDE_DIRS})
    elseif (NOT "$ENV{SDL2_DIR}" STREQUAL "")
        # On windows we may set SDL2_DIR environment variable and link to binary SDL distribution.
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(IMGUI_SDL_PLATFORM_ARCH x64)
        elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
            set(IMGUI_SDL_PLATFORM_ARCH x86)
        else ()
            message(FATAL_ERROR "Unsupported platform.")
        endif()
        add_library(SDL2::SDL2 SHARED IMPORTED)
        target_include_directories(SDL2::SDL2 INTERFACE "$ENV{SDL2_DIR}/include")
        target_link_libraries(SDL2::SDL2 INTERFACE $ENV{SDL2_DIR}/lib/${IMGUI_SDL_PLATFORM_ARCH}/SDL2main.lib)
        set_target_properties(SDL2::SDL2 PROPERTIES
            IMPORTED_LOCATION "$ENV{SDL2_DIR}/lib/${IMGUI_SDL_PLATFORM_ARCH}/SDL2.dll"
            IMPORTED_IMPLIB "$ENV{SDL2_DIR}/lib/${IMGUI_SDL_PLATFORM_ARCH}/SDL2.lib"
        )
    elseif (PkgConfig_FOUND)
        # Rest of the platforms (like MacOS) can consume SDL via pkg-config.
        # CMake 3.6 supports IMPORTED_TARGET parameter which creates PkgConfig::sdl2 target that we can easily link to.
        # TODO: Switch to using IMPORTED_TARGET when CMake minimal version is increased.
        pkg_check_modules(SDL2 QUIET sdl2)
        if (SDL2_FOUND)
            add_library(SDL2::SDL2 INTERFACE IMPORTED)
            target_link_libraries(SDL2::SDL2 INTERFACE ${SDL2_LDFLAGS})
            target_compile_options(SDL2::SDL2 INTERFACE ${SDL2_CFLAGS})
        endif ()
    endif ()
    if (NOT IMGUI_SDL_TARGET)
        set(IMGUI_SDL_TARGET SDL2::SDL2)
    endif ()
    if (TARGET ${IMGUI_SDL_TARGET} OR CMAKE_SYSTEM_NAME STREQUAL Emscripten)
        add_library (imgui-sdl INTERFACE)
        target_sources(imgui-sdl INTERFACE
            ${IMGUI_BACKENDS_DIR}/imgui_impl_sdl.h
            ${IMGUI_BACKENDS_DIR}/imgui_impl_sdl.cpp
        )
        target_include_directories(imgui-sdl INTERFACE ${IMGUI_BACKENDS_DIR})
        target_link_libraries(imgui-sdl INTERFACE imgui)
        if (TARGET ${IMGUI_SDL_TARGET})
            target_link_libraries(imgui-sdl INTERFACE ${IMGUI_SDL_TARGET})
        endif ()
    else ()
        message(STATUS "IMGUI_IMPL_SDL set to ON but SDL2 could not be found.")
    endif ()
endif ()
if (IMGUI_IMPL_METAL AND APPLE)
    add_library(imgui-metal INTERFACE)
    target_sources(imgui-metal INTERFACE
        ${IMGUI_BACKENDS_DIR}/imgui_impl_metal.h
        ${IMGUI_BACKENDS_DIR}/imgui_impl_metal.mm
    )
    target_link_libraries(imgui-metal INTERFACE imgui "-framework Cocoa" "-framework Metal" "-framework QuartzCore")
    target_compile_options(imgui-metal INTERFACE -fobjc-arc)
    target_include_directories(imgui-metal INTERFACE ${IMGUI_BACKENDS_DIR})
endif ()
if (IMGUI_IMPL_OSX AND APPLE)
    add_library(imgui-osx INTERFACE)
    target_sources(imgui-osx INTERFACE
        ${IMGUI_BACKENDS_DIR}/imgui_impl_osx.h
        ${IMGUI_BACKENDS_DIR}/imgui_impl_osx.mm
    )
    target_link_libraries(imgui-osx INTERFACE imgui "-framework Cocoa" "-framework AppKit")
    target_compile_options(imgui-osx INTERFACE -fobjc-arc)
    target_include_directories(imgui-osx INTERFACE ${IMGUI_BACKENDS_DIR})
endif ()
if (IMGUI_IMPL_WIN32 AND WIN32)
    add_library(imgui-win32 INTERFACE)
    target_sources(imgui-win32 INTERFACE
        ${IMGUI_BACKENDS_DIR}/imgui_impl_win32.h
        ${IMGUI_BACKENDS_DIR}/imgui_impl_win32.cpp
    )
    target_include_directories(imgui-win32 INTERFACE ${IMGUI_BACKENDS_DIR})
endif ()
if (IMGUI_IMPL_GLFW)
    if (IMGUI_GLFW_TARGET)
        # Custom user target was set. No auto-detection required.
    elseif (TARGET glfw)
        # GLFW exists already. Nothing to do.
    elseif (EXISTS ${IMGUI_ROOT_DIR}/../glfw/CMakeLists.txt)
        # When parent directory contains GLFW source code - we can build it directly.
        set (GLFW_STANDALONE OFF)
        set (GLFW_INSTALL OFF)
        set (GLFW_BUILD_DOCS OFF)
        add_subdirectory (${IMGUI_ROOT_DIR}/../glfw ${CMAKE_CURRENT_BINARY_DIR}/glfw)
    elseif (MSVC)
        # We ship compiled libraries in our repository for Visual Studio.
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(IMGUI_GLFW_PLATFORM_BITS 64)
        elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
            set(IMGUI_GLFW_PLATFORM_BITS 32)
        else ()
            message(FATAL_ERROR "Unsupported platform.")
        endif()
        add_library(glfw INTERFACE IMPORTED)
        target_link_libraries(glfw INTERFACE libs/glfw/lib-vc2010-${IMGUI_GLFW_PLATFORM_BITS}/glfw3.lib)
        target_include_directories(glfw INTERFACE libs/glfw/include)
    elseif (PkgConfig_FOUND)
        # CMake 3.6 supports IMPORTED_TARGET parameter which creates PkgConfig::glfw target that we can easily link to.
        # TODO: Switch to using IMPORTED_TARGET when CMake minimal version is increased.
        pkg_check_modules(GLFW QUIET glfw3)
        if (GLFW_FOUND)
            add_library(glfw INTERFACE IMPORTED)
            target_link_libraries(glfw INTERFACE ${GLFW_LDFLAGS})
            target_compile_options(glfw INTERFACE ${GLFW_CFLAGS})
        endif ()
    endif ()
    if (NOT IMGUI_GLFW_TARGET)
        set(IMGUI_GLFW_TARGET glfw)
    endif ()
    if (TARGET ${IMGUI_GLFW_TARGET})
        add_library(imgui-glfw INTERFACE)
        target_sources(imgui-glfw INTERFACE
            ${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.h
            ${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.cpp
        )
        target_link_libraries(imgui-glfw INTERFACE ${CMAKE_DL_LIBS} ${IMGUI_GLFW_TARGET})
        target_include_directories(imgui-glfw INTERFACE ${IMGUI_BACKENDS_DIR})
    else ()
        message(STATUS "IMGUI_IMPL_GLFW set to ON but GLFW could not be found.")
    endif ()
endif ()
if (IMGUI_IMPL_VULKAN)
    find_package(Vulkan QUIET)
    if (Vulkan_FOUND)
        add_library(imgui-vulkan INTERFACE)
        target_sources(imgui-vulkan INTERFACE ${IMGUI_BACKENDS_DIR}/imgui_impl_vulkan.cpp)
        target_link_libraries (imgui-vulkan INTERFACE imgui ${Vulkan_LIBRARIES} Vulkan::Vulkan)
        target_include_directories(imgui-vulkan INTERFACE ${Vulkan_INCLUDE_DIRS} ${IMGUI_BACKENDS_DIR})
    else ()
        message(STATUS "IMGUI_IMPL_VULKAN set to ON but Vulkan could not be found.")
    endif ()
endif ()

if (IMGUI_EXAMPLES)
    function (IMGUI_EXAMPLE)
        cmake_parse_arguments(IMGUI_EXAMPLE "" "TARGET;SOURCE" "LINKS" ${ARGN})
        if (NOT IMGUI_EXAMPLE_SOURCE)
            set (IMGUI_EXAMPLE_SOURCE main.cpp)
        endif ()
        # Check whether dependency libraries are available.
        foreach (dep ${IMGUI_EXAMPLE_LINKS})
            if (NOT TARGET ${dep})
                return ()
            endif ()
        endforeach ()
        # Create a target.
        add_executable(${IMGUI_EXAMPLE_TARGET} ${IMGUI_EXAMPLES_DIR}/${IMGUI_EXAMPLE_TARGET}/${IMGUI_EXAMPLE_SOURCE})
        target_link_libraries(${IMGUI_EXAMPLE_TARGET} PRIVATE imgui ${IMGUI_EXAMPLE_LINKS})
        list(FIND IMGUI_EXAMPLE_LINKS imgui-sdl LINKS_SDL)
        if (NOT LINKS_SDL EQUAL -1 AND WIN32)
            get_target_property(SDL_IS_IMPORTED ${IMGUI_SDL_TARGET} IMPORTED)
            if (SDL_IS_IMPORTED)
                # When program links to imgui-sdl and imported target ${IMGUI_SDL_TARGET} exists - copy SDL dll.
                add_custom_command(TARGET ${IMGUI_EXAMPLE_TARGET} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                            $<TARGET_FILE:${IMGUI_SDL_TARGET}> $<TARGET_FILE_DIR:${IMGUI_EXAMPLE_TARGET}>)
            endif ()
        endif ()
    endfunction()
    if (APPLE)
        # MacOS Metal example
        # FIXME: This sample is still broken.
        add_executable(example_apple_metal MACOSX_BUNDLE
            ${IMGUI_EXAMPLES_DIR}/example_apple_metal/Shared/main.m
            ${IMGUI_EXAMPLES_DIR}/example_apple_metal/Shared/AppDelegate.h
            ${IMGUI_EXAMPLES_DIR}/example_apple_metal/Shared/AppDelegate.m
            ${IMGUI_EXAMPLES_DIR}/example_apple_metal/Shared/Renderer.h
            ${IMGUI_EXAMPLES_DIR}/example_apple_metal/Shared/Renderer.mm
            ${IMGUI_EXAMPLES_DIR}/example_apple_metal/Shared/ViewController.h
            ${IMGUI_EXAMPLES_DIR}/example_apple_metal/Shared/ViewController.mm
            ${IMGUI_EXAMPLES_DIR}/example_apple_metal/macOS/Base.lproj/Main.storyboard
        )
        target_link_libraries(example_apple_metal imgui imgui-osx imgui-metal)
        # Generate new plist
        file(READ ${IMGUI_EXAMPLES_DIR}/example_apple_metal/macOS/Info-macOS.plist INFO_PLIST)
        string(REPLACE "$(EXECUTABLE_NAME)" example_apple_metal INFO_PLIST "${INFO_PLIST}")
        string(REPLACE "$(PRODUCT_BUNDLE_IDENTIFIER)" org.imgui.example.apple-metal-macos INFO_PLIST "${INFO_PLIST}")
        string(REPLACE "$(MACOSX_DEPLOYMENT_TARGET)" 10.12 INFO_PLIST "${INFO_PLIST}")
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/example_apple_metal-info.plist "${INFO_PLIST}")
        # Consume new plist
        set_property(TARGET example_apple_metal PROPERTY
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/example_apple_metal-info.plist)
        # Compile storyboard
        if(NOT "${CMAKE_GENERATOR}" MATCHES "^Xcode.*")
            # Xcode compiles storyboard automatically. We must do it manually when using other build systems.
            find_program(IBTOOL ibtool HINTS /usr/bin ${OSX_DEVELOPER_ROOT}/usr/bin)
            add_custom_command(TARGET example_apple_metal POST_BUILD COMMAND ${IBTOOL} --errors --warnings --notices
                --output-format human-readable-text --compile
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/example_apple_metal.app/Contents/Resources/Base.lproj/Main.storyboardc
                ${IMGUI_EXAMPLES_DIR}/example_apple_metal/macOS/Base.lproj/Main.storyboard
                COMMENT "Compiling storyboard"
            )
        endif()
        IMGUI_EXAMPLE(TARGET example_apple_opengl2 LINKS imgui-osx imgui-opengl2 SOURCE main.mm)
        IMGUI_EXAMPLE(TARGET example_glfw_metal LINKS imgui-glfw imgui-metal SOURCE main.mm)
        IMGUI_EXAMPLE(TARGET example_sdl_metal LINKS imgui-sdl imgui-metal SOURCE main.mm)
    endif ()
    if (WIN32)
        IMGUI_EXAMPLE(TARGET example_sdl_directx11 LINKS imgui-sdl imgui-dx11)
        IMGUI_EXAMPLE(TARGET example_win32_directx9 LINKS imgui-win32 imgui-dx9)
        IMGUI_EXAMPLE(TARGET example_win32_directx10 LINKS imgui-win32 imgui-dx10)
        IMGUI_EXAMPLE(TARGET example_win32_directx11 LINKS imgui-win32 imgui-dx11)
        IMGUI_EXAMPLE(TARGET example_win32_directx12 LINKS imgui-win32 imgui-dx12)
        IMGUI_EXAMPLE(TARGET example_null)
    endif ()
    if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
        IMGUI_EXAMPLE(TARGET example_emscripten LINKS imgui-sdl imgui-opengl3)
        target_compile_options(example_emscripten PUBLIC -s USE_SDL=2)
        target_link_libraries(example_emscripten PUBLIC "-s WASM=1")
        target_link_libraries(example_emscripten PUBLIC "-s ALLOW_MEMORY_GROWTH=1")
        target_link_libraries(example_emscripten PUBLIC "-s DISABLE_EXCEPTION_CATCHING=1" "-s NO_EXIT_RUNTIME=0")
        target_link_libraries(example_emscripten PUBLIC "-s ASSERTIONS=1")
        target_link_libraries(example_emscripten PUBLIC "--shell-file ${IMGUI_EXAMPLES_DIR}/example_emscripten/shell_minimal.html")
        if (IMGUI_DISABLE_FILE_FUNCTIONS)
            target_link_libraries(example_emscripten PUBLIC "-s NO_FILESYSTEM=1")
        else ()
            target_link_libraries(example_emscripten PUBLIC --no-heap-copy "--preload-file ${IMGUI_ROOT_DIR}/misc/fonts@/fonts")
        endif ()
        # Uncomment next line to fix possible rendering bugs with Emscripten version older then 1.39.0 (https://github.com/ocornut/imgui/issues/2877)
        #target_link_libraries(example_emscripten PUBLIC "-s BINARYEN_TRAP_MODE=clamp")
        #target_link_libraries(example_emscripten PUBLIC "-s SAFE_HEAP=1")    ## Adds overhead
    else ()
        IMGUI_EXAMPLE(TARGET example_glfw_opengl2 LINKS imgui-glfw imgui-opengl2)
        IMGUI_EXAMPLE(TARGET example_glfw_opengl3 LINKS imgui-glfw imgui-opengl3)
        IMGUI_EXAMPLE(TARGET example_glfw_vulkan LINKS imgui-glfw imgui-vulkan)
        IMGUI_EXAMPLE(TARGET example_glut_opengl2 LINKS imgui-glut imgui-opengl2)
        IMGUI_EXAMPLE(TARGET example_sdl_opengl2 LINKS imgui-sdl imgui-opengl2)
        IMGUI_EXAMPLE(TARGET example_sdl_opengl3 LINKS imgui-sdl imgui-opengl3)
        IMGUI_EXAMPLE(TARGET example_sdl_vulkan LINKS imgui-sdl imgui-vulkan)
    endif ()
endif ()
