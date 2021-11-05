# Set project name
project(BalancePro LANGUAGES CXX)
set(TARGET_NAME BalancePro)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

######################################

# Set CMake minimum version and CMake policy required by UrhoCommon module
cmake_minimum_required(VERSION 3.10.2)
if(COMMAND cmake_policy)
    # Disallow use of the LOCATION target property - so we set to OLD as we still need it
    cmake_policy(SET CMP0026 OLD)
    # Honor the visibility properties for SHARED target types only
    cmake_policy(SET CMP0063 OLD)
endif()
# Set CMake modules search path
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)
# Include Urho3D Cmake common module
include(UrhoCommon)
# Define source files
define_source_files()
# Setup target with resource copying
setup_main_executable()
