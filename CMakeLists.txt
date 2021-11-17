# Set project name
project(BalancePro LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_subdirectory(rbfx)

add_executable(BalancePro main.cpp)
target_link_libraries(BalancePro PUBLIC Urho3D)

# Copy the stuff we need
add_custom_command(TARGET BalancePro PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/Project $<TARGET_FILE_DIR:BalancePro>/Project)
add_custom_command(TARGET BalancePro PRE_BUILD
                  COMMAND ${CMAKE_COMMAND} -E copy_directory
                      ${CMAKE_SOURCE_DIR}/rbfx/bin/CoreData $<TARGET_FILE_DIR:BalancePro>/Project/CoreData)
