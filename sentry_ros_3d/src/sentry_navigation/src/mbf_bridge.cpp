#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include "mbf_msgs/MoveBaseAction.h"
#include <std_srvs/Empty.h>
mbf_msgs::MoveBaseActionGoal action_goal;
int callback_flag=0;
void goal_callback(const geometry_msgs::PoseStamped::ConstPtr& goal){
    ROS_DEBUG_NAMED("move_base_flex","In ROS goal callback, wrapping the PoseStamped in the action message and re-sending to the server.");
    action_goal.header.stamp = ros::Time::now();
    action_goal.goal.target_pose = *goal;
    ROS_INFO("get_goal");
    callback_flag = 1;

  }


int main(int argc, char** argv) {
  ros::init(argc, argv, "mbf_bridge");
  ros::NodeHandle action_nh("move_base_flex/move_base");
  ros::NodeHandle simple_nh;
  ros::Publisher action_goal_pub_;
  ros::Subscriber goal_sub_;
  goal_sub_=simple_nh.subscribe<geometry_msgs::PoseStamped>("goal", 1,goal_callback);
  action_goal_pub_ = action_nh.advertise<mbf_msgs::MoveBaseActionGoal>("goal", 1);
  ros::ServiceClient clear_costmaps_client = action_nh.serviceClient<std_srvs::Empty>("/move_base_flex/clear_costmaps");
  while (action_nh.ok()&&simple_nh.ok())
  {
    if(callback_flag == 1)
    {
        std_srvs::Empty clear_costmaps_srv;
        clear_costmaps_client.call(clear_costmaps_srv);
        ros::Duration(0.5).sleep();
        action_goal_pub_.publish(action_goal);
        callback_flag = 0;
        ROS_INFO("send goal");
    } 
  ros::spinOnce();
  }
  
  return 0;
}
