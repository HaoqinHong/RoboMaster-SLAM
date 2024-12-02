cmake_minimum_required(VERSION 3.1)
project(simple_robot)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17 -Os -pthread -g")

#############################
#Dependencies
#############################
find_package(Threads)
find_package(catkin REQUIRED COMPONENTS
    roscpp
    tf
    nav_msgs
    geometry_msgs
    std_msgs
    message_filters
    std_msgs
    rosconsole
    robot_msgs
    )

add_service_files(
FILES 

)

catkin_package(

# INCLUDE_DIRS include
CATKIN_DEPENDS roscpp std_msgs rosconsole message_runtime robot_msgs message_filters
)



include_directories(
  include 
  ${catkin_INCLUDE_DIRS}
)

add_executable(simple_robot src/robot_node.cpp src/crc.cpp src/serial_device.cpp)
target_link_libraries(simple_robot
    ${CMAKE_THREAD_LIBS_INIT}
    ${catkin_LIBRARIES})
