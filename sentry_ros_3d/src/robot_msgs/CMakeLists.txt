cmake_minimum_required(VERSION 3.1)
project(robot_msgs)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

find_package(catkin REQUIRED COMPONENTS
  std_msgs
  geometry_msgs
  message_generation
)

add_message_files(
  DIRECTORY msg/mv_cam_imu
  FILES
  EulerAngles.msg
  vision.msg
  vision_pts.msg
  control.msg
  sc_rc_msg.msg
  robot_ctrl.msg
  competition_info.msg
)

generate_messages(
  DEPENDENCIES
  std_msgs
  geometry_msgs
)

catkin_package(
  CATKIN_DEPENDS
  message_runtime
)