# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

option(CODE_COVERAGE "generates command for code coverage" OFF)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

project ("IPCLib" VERSION 1.0.0)

# Include sub-projects.
add_subdirectory ("IPCLib")
add_subdirectory ("IPCTests")

set(INCLUDE_INSTALL_DIR include/ CACHE PATH "")
set(LIB_INSTALL_DIR lib/ CACHE PATH "")
set(BIN_INSTALL_DIR bin/ CACHE PATH "")

include(CMakePackageConfigHelpers)
configure_package_config_file(ipclib-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/ipclib-config.cmake
        INSTALL_DESTINATION ${LIB_INSTALL_DIR}/cmake/IPCLib
        PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR BIN_INSTALL_DIR)
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/ipclib-config-version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion )
install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/ipclib-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/ipclib-config-version.cmake
        DESTINATION ${LIB_INSTALL_DIR}/cmake/IPCLib )


enable_testing()
