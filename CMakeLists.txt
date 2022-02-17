# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("IPCLib")

# Include sub-projects.
add_subdirectory ("IPCTests")
add_subdirectory ("IPCLib")

enable_testing()
