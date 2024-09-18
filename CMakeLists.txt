cmake_minimum_required(VERSION 3.10)
project(CppSimulation)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify the paths for headers and sources
include_directories(include)

# Find SFML package
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Gather all source files
file(GLOB SOURCES "src/*.cpp" "src/classes/*.cpp")

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link SFML libraries
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)