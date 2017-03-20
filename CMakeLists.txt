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
    find_library(CPP_LIB libstdc++-6)
    find_library(WINPTHREAD_LIB libwinpthread-1)
    find_library(SNDFILE_LIB sndfile sndfile-1 HINTS ${LIB_PATH})
    find_library(MPG_LIB mpg123 mpg123-0 HINTS ${LIB_PATH})
    find_library(PORTAUDIO_LIB portaudio portaudio0 HINTS ${LIB_PATH})
else(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    #find libraries on other systems
    find_library(SNDFILE_LIB sndfile sndfile-1)
    find_library(MPG_LIB mpg123 mpg123-0)
    find_library(PORTAUDIO_LIB portaudio portaudio0)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

message(STATUS "Found portaudio library: ${PORTAUDIO_LIB}")
message(STATUS "Found mpg123 library: ${MPG_LIB}")
message(STATUS "Found sndfile library: ${SNDFILE_LIB}")

#include external headers
include_directories(${INCLUDE_PATH})

set(LANGUAGE CXX)

##### Qt5RTA Project tester generation (disabled by default)
#if(POLICY CMP0020)
#cmake_policy(SET CMP0020 NEW)
#endif()
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)
#find_package(Qt5Core)
#find_package(Qt5Gui)
#find_package(Qt5Widgets)
#find_package(Qt5PrintSupport)
#include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5PrintSupport_INCLUDE_PATH})
#add_executable(QtRTAtst QtRTAtst.cpp mainWindow.cpp CppRTA.cpp)
#target_link_libraries(QtRTAtst ${PORTAUDIO_LIB} ${MPG_LIB} ${SNDFILE_LIB}
#	${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5PrintSupport_LIBRARIES})
#find_library(QT5CORE_LIB Qt5Core.dll)
#find_library(QT5GUI_LIB Qt5Gui.dll)
#find_library(QT5WIDGETS_LIB Qt5Widgets.dll)
#find_library(QT5PRINT_LIB Qt5PrintSupport.dll)
#file(COPY ${QT5CORE_LIB} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#file(COPY ${QT5GUI_LIB} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#file(COPY ${QT5WIDGETS_LIB} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#file(COPY ${QT5PRINT_LIB} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

##### MARTA library generation (enabled by default) #####
add_library(MARTA SHARED CppRTA.cpp MARTA.cpp)
target_link_libraries(MARTA ${PORTAUDIO_LIB} ${MPG_LIB} ${SNDFILE_LIB})

##### C++-Project tester generation (enabled by default) #####
add_executable(CppRTAtst CppRTAtst.cpp CppRTA.cpp)
target_link_libraries(CppRTAtst ${PORTAUDIO_LIB} ${MPG_LIB} ${SNDFILE_LIB})

##### C-Project tester generation (disabled by default) #####
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
