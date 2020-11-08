# the name of the target operating system
#SET(CMAKE_SYSTEM_NAME Windows)

LIST(APPEND CMAKE_PROGRAM_PATH  "D:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.12.25827/bin/Hostx64/x64" ...)

# start eclipse/cmake/make from "native tools command prompt for VS 2017"
# no parallel jobs from eclipse
# build tool nmake
# fails for external projects

# which compilers to use for C and C++
set(CMAKE_C_COMPILER "D:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.12.25827/bin/Hostx64/x64/cl.exe")
set(CMAKE_CXX_COMPILER "D:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.12.25827/bin/Hostx64/x64/cl.exe")
SET(CMAKE_RC_COMPILER "C:/Program Files (x86)/Windows Kits/10/bin/10.0.16299.0/x64/rc.exe")

# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH  "D:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.12.25827") 

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
#set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_MAKE_PROGRAM "D:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.12.25827/bin/Hostx64/x64/nmake.exe" CACHE FILEPATH "" FORCE)
