cmake_minimum_required(VERSION 3.10)

# Set the build to debug mode
set(CMAKE_BUILD_TYPE "Debug")

# Set compiler
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# Executable directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ..)

# Set to true if compiling a C++ program only
set(COMPILING_CXX_PROJECT true)

# Name of the project
set(PROGRAM_NAME minecraft)
project(${PROGRAM_NAME})

# Set the C standard (C11 by default)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Set the C++ standard (C++20 by default)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# Add the executable name, and the source files
add_executable(${PROGRAM_NAME}
./src/init.cpp
./src/window.cpp
./src/logger.cpp
./src/device.cpp
./src/queue.cpp
./src/vkcomponents.cpp)


# Compiler flags
target_compile_options(${PROGRAM_NAME} PRIVATE 
-fdiagnostics-color
-fPIC
-Wall
-Werror
-Wpedantic
-Wextra
-ffunction-sections
-fdata-sections
-Wdouble-promotion
-Wimplicit-fallthrough
-Wmissing-include-dirs
-Wformat=2
-Wconversion
-Wduplicated-branches
-Wduplicated-cond
-Wshadow
-Wfloat-equal
-Wcast-qual
-Wcast-align
-Wnull-dereference
-Winvalid-pch
-Winline
-Wunsafe-loop-optimizations
-Wlogical-op
-Wdisabled-optimization
)

if (${COMPILING_CXX_PROJECT})
	# These are C++/ObjC++ exclusive options
	target_compile_options(${PROGRAM_NAME} PRIVATE
		-Wctor-dtor-privacy
		-Wnon-virtual-dtor
		-Wmismatched-tags
		-Wextra-semi
		-Wsuggest-override
		-Wsuggest-final-types
		-Wsuggest-final-methods
		-Wuseless-cast
		-Wvirtual-inheritance
		-fstrict-enums
		-Weffc++
		-fimplicit-constexpr
	)

endif()


# Options for release build
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Running release build")
    target_compile_options(${PROGRAM_NAME} PRIVATE 
    -DNDEBUG
    -O3
    -flto
    -fprofile-generate=./profile # This executes the binary a few times for sample analysis (Replace with "fprofile-use=PATH" once complete)
    -fprofile-partial-training
    -march=native)
    target_link_options(${PROGRAM_NAME} PUBLIC -flto -s)
endif()

# Include directories
target_include_directories(${PROGRAM_NAME} PUBLIC "./include")

# Libraries to link with (optional)
target_link_libraries(${PROGRAM_NAME} PUBLIC glfw vulkan) # gcov needed for profiling
