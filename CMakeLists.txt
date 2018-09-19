cmake_minimum_required(VERSION 2.8)
project(ce30_driver C CXX)

set(CMAKE_CXX_STANDARD 11)

set(CE30_DRIVER_MAJOR_VERSION 1)
set(CE30_DRIVER_MINOR_VERSION 2)
set(CE30_DRIVER_PATCH_VERSION 1)
set(CE30_DRIVER_VERSION
  ${CE30_DRIVER_MAJOR_VERSION}.${CE30_DRIVER_MINOR_VERSION}.${CE30_DRIVER_PATCH_VERSION})

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/cmake/ce30_driver)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

# set up include-directories
include_directories(
  "${PROJECT_SOURCE_DIR}"   # to find ce30_driver/ce30_driver.h
  "${PROJECT_BINARY_DIR}")  # to find ce30_driver/config.h

# Add sub-directories
add_subdirectory(ce30_driver)

# The interesting stuff goes here
# ===============================

# Add all targets to the build-tree export set
export(TARGETS ce30_driver
  FILE "${PROJECT_BINARY_DIR}/ce30_driverTargets.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE ce30_driver)

# Create the ce30_driverConfig.cmake and ce30_driverConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
   "${INSTALL_INCLUDE_DIR}")
# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(ce30_driverConfig.cmake.in
  "${PROJECT_BINARY_DIR}/ce30_driverConfig.cmake" @ONLY)
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${CE30_DRIVER_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(ce30_driverConfig.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/ce30_driverConfig.cmake" @ONLY)
# ... for both
configure_file(ce30_driverConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/ce30_driverConfigVersion.cmake" @ONLY)

# Install the ce30_driverConfig.cmake and ce30_driverConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/ce30_driverConfig.cmake"
  "${PROJECT_BINARY_DIR}/ce30_driverConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT ce30_driverTargets DESTINATION
  "${INSTALL_CMAKE_DIR}" COMPONENT dev)
