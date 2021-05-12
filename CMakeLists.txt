cmake_minimum_required(VERSION 2.8)

project(NUMBER)
SET(CMAKE_BUILE_TYPE DEBUG)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g -std=c++14")
SET(BIN_NAME "run")

FIND_PACKAGE(OpenCV 4.4.0 REQUIRED)


include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/Armor/include)
include_directories(${PROJECT_SOURCE_DIR}/SVMTrain/include)
include_directories(/usr/include/eigen3)
include_directories(${realsense2_INCLUDE_DIR})

FILE(GLOB_RECURSE sourcefiles "Armor/src/*.cpp")

ADD_EXECUTABLE(${BIN_NAME} predict.cpp ${sourcefiles})

target_link_libraries(${BIN_NAME} ${OpenCV_LIBS})
