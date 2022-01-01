# Configure project
project(Game LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Configure engine
set(URHO3D_STATIC_RUNTIME ON)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(URHO3D_ENABLE_ALL OFF)
set(URHO3D_DOCS OFF)
set(URHO3D_CSHARP OFF)
set(URHO3D_EXTRAS OFF)
set(URHO3D_GLOW OFF)
set(URHO3D_HASH_DEBUG OFF)
set(URHO3D_IK OFF)
set(URHO3D_LEGACY_RENDERER OFF)
set(URHO3D_LOGGING ON)
set(URHO3D_MONOLITHIC_HEADER OFF)
set(URHO3D_NAVIGATION OFF)
set(URHO3D_NETWORK OFF)
set(URHO3D_NO_EDITOR_PLAYER_EXE ON)
set(URHO3D_PHYSICS ON)
set(URHO3D_PLAYER OFF)
set(URHO3D_PROFILING OFF)
set(URHO3D_RMLUI OFF)
set(URHO3D_SAMPLES OFF)
set(URHO3D_TOOLS OFF)
set(URHO3D_URHO2D OFF)
set(URHO3D_WEBP OFF)
add_subdirectory(rbfx)

# rbfx easyscript
add_subdirectory(easyscript)

# Game executable
add_executable(Game main.cpp LevelManager.hpp Useable.hpp)
target_link_libraries(Game PUBLIC Urho3D rbfxeasyscript)

# Copy the stuff we need
add_custom_command(TARGET Game PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/Project $<TARGET_FILE_DIR:Game>/Project)
add_custom_command(TARGET Game PRE_BUILD
                  COMMAND ${CMAKE_COMMAND} -E copy_directory
                      ${CMAKE_SOURCE_DIR}/rbfx/bin/CoreData $<TARGET_FILE_DIR:Game>/Project/CoreData)
