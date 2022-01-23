cmake_minimum_required(VERSION 3.14)

if (NOT CMAKE_ASAN_INCD STREQUAL "YES")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_SANS "undefined" CACHE STRING "Sanitizers to enable")
    else()
        set(CMAKE_SANS "" CACHE STRING "Sanitizers to enable (none by default)")
    endif()

    if (CMAKE_SANS STREQUAL "")
        message(STATUS "Not using sanitizers")
    else()
        message(STATUS "Using sanitizers: ${CMAKE_SANS}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-omit-frame-pointer -fsanitize=${CMAKE_SANS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=${CMAKE_SANS}")
        set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fno-omit-frame-pointer -fsanitize=${CMAKE_SANS}")
    endif()
    set(CMAKE_ASAN_INCD YES)
endif()
