project(MARTA)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

#Using c++11 standard
set(CMAKE_CXX_STANDARD 11)

#set the relative paths for external libraries and headers

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/headers" CACHE FILEPATH "description")
    if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
        message(STATUS "Target is 64 bits")
        set(LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/win64" CACHE FILEPATH "description")
        find_library(SEH_LIB libgcc_s_seh-1)
    else("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
        message(STATUS "Target is 32 bits")
        set(LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/win32" CACHE FILEPATH "description")
        find_library(DW2_LIB gcc_s_dw2-1)
        find_library(SJLJ_LIB gcc_s_sjlj-1 HINTS ${LIB_PATH})
    endif("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")

    #find the included libraries to link against them explicitely
    find_library(CPP_LIB libstdc++-6)
    find_library(WINPTHREAD_LIB libwinpthread-1)
    find_library(SNDFILE_LIB sndfile sndfile-1 HINTS ${LIB_PATH})
    find_library(MPG_LIB mpg123 mpg123-0 HINTS ${LIB_PATH})
    find_library(PORTAUDIO_LIB portaudio_static portaudio HINTS ${LIB_PATH})
else(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    #find libraries on other systems
    find_library(SNDFILE_LIB sndfile sndfile-1)
    find_library(MPG_LIB mpg123 mpg123-0)
    find_library(PORTAUDIO_LIB portaudio portaudio0)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

message(STATUS "Found portaudio library ${PORTAUDIO_LIB}")
message(STATUS "Found mpg123 library ${MPG_LIB}")
message(STATUS "Found sndfile library ${SNDFILE_LIB}")

#include external headers
include_directories(${INCLUDE_PATH})

set(LANGUAGE CXX)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    #define MARTA library
    add_library(MARTA SHARED CppRTA.cpp MARTA.cpp)
    add_executable(CppRTAtst CppRTAtst.cpp CppRTA.cpp)
    target_link_libraries(MARTA ${PORTAUDIO_LIB} -lwinmm -lole32 -luuid -lsetupapi ${MPG_LIB} ${SNDFILE_LIB})
    target_link_libraries(CppRTAtst ${PORTAUDIO_LIB} -lwinmm -lole32 -luuid -lsetupapi ${MPG_LIB} ${SNDFILE_LIB})
else(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_library(MARTA SHARED CppRTA.cpp MARTA.cpp)
    add_executable(CppRTAtst CppRTAtst.cpp CppRTA.cpp)
    target_link_libraries(MARTA ${PORTAUDIO_LIB} ${MPG_LIB} ${SNDFILE_LIB})
    target_link_libraries(CppRTAtst ${PORTAUDIO_LIB} ${MPG_LIB} ${SNDFILE_LIB})
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

#define test executable which uses MARTA lib c interface
#add_executable(MARTAtst MARTAtst.c)
#target_link_libraries(MARTAtst MARTA)

#find the relevant headers to copy them to the binary directory
find_file(MARTA_HEADER MARTA.h ${CMAKE_CURRENT_SOURCE_DIR})

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    #copy right libraries (32 vs 64 bin exception handling) and headers to the binary directory
    if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
            file(COPY ${SEH_LIB} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    else("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
            file(COPY ${DW2_LIB} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
            file(COPY ${SJLJ_LIB} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    endif("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    file(COPY ${CPP_LIB} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${WINPTHREAD_LIB} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${MPG_LIB} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${PORTAUDIO_LIB} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${SNDFILE_LIB} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

file(COPY ${MARTA_HEADER} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
