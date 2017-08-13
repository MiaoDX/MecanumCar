cmake_minimum_required(VERSION 2.8)
project(MecanumCar)

if( WIN32 )
    include(C:/tools/cmake_install_libs/serial/share/serial/cmake/serialConfig.cmake)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    find_package(serial REQUIRED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -pthread")
endif()

include_directories(${serial_INCLUDE_DIRS})

add_executable(platform platform.cpp SerialMultiSteppers.cpp SerialMultiSteppers.h)
add_executable(SerialMultiSteppersTest SerialMultiSteppersTest.cpp SerialMultiSteppers.cpp SerialMultiSteppers.h)
 
target_link_libraries(platform serial)
target_link_libraries(SerialMultiSteppersTest serial)
