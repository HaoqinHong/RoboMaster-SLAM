#!/bin/bash
catkin_make -DCATKIN_WHITELIST_PACKAGES="robot_msgs" #功能包里有自定义消息时用，先编译自定义消息 rosinrange_msg
catkin_make -DCATKIN_WHITELIST_PACKAGES="rosinrange_msg"
catkin_make -DCATKIN_WHITELIST_PACKAGES="livox_ros_driver"
catkin_make -DCATKIN_WHITELIST_PACKAGES="fast_lio"
catkin_make -DCATKIN_WHITELIST_PACKAGES="livox_ros_driver2"
catkin_make -DCATKIN_WHITELIST_PACKAGES=""


