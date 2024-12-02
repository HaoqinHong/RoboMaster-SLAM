#include <ros/ros.h>
#include <std_msgs/String.h>
#include <tf/transform_listener.h>
#include <tf/tf.h>
#include <tf/transform_broadcaster.h>
#include "std_msgs/String.h"
#include "geometry_msgs/PointStamped.h"
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Geometry>
geometry_msgs::PointStamped enemy_pos_imu;
geometry_msgs::PointStamped enemy_pos_world;
uint8_t callback_flag;
static double last_diagonal_line;
static double diagonal_line;
void position_callback(const geometry_msgs::PointStamped::ConstPtr &msg)
{
    static tf::TransformListener listener;
    try
    {
        callback_flag = 1;
        enemy_pos_imu.header = msg->header;
        enemy_pos_imu.point = msg->point;
        listener.waitForTransform("map","base_footprint",msg->header.stamp,ros::Duration(3.0));
        ROS_INFO("enemy in imu frame: (%f, %f, %f)",enemy_pos_imu.point.x,enemy_pos_imu.point.y, enemy_pos_imu.point.z);
        last_diagonal_line = diagonal_line;
        diagonal_line = sqrt(enemy_pos_imu.point.x*enemy_pos_imu.point.x+enemy_pos_imu.point.y*enemy_pos_imu.point.y);
        if(diagonal_line>4)
        {
            callback_flag = 0;//超过4m就不追了
        }
        else if(diagonal_line>1)
        {
            diagonal_line-=0.8;//敌人在1～4m内时追击至离敌人0.8m距离
        }
        else callback_flag = 0;//小于1m内的敌人不追

        double angle = atan(abs(enemy_pos_imu.point.x)/abs(enemy_pos_imu.point.y));
        if(enemy_pos_imu.point.x>=0)
            enemy_pos_imu.point.x = diagonal_line*sin(angle);
        else enemy_pos_imu.point.x = -diagonal_line*sin(angle);
        if(enemy_pos_imu.point.y>=0)
            enemy_pos_imu.point.y = diagonal_line*cos(angle);
        else enemy_pos_imu.point.y = -diagonal_line*cos(angle);
        enemy_pos_imu.point.z = 0;
        ROS_INFO("hit point in imu frame: (%f, %f, %f)",enemy_pos_imu.point.x,enemy_pos_imu.point.y, enemy_pos_imu.point.z);
        listener.transformPoint("map",enemy_pos_imu,enemy_pos_world);
        ROS_INFO("enemy in world frame: (%f, %f, %f)",enemy_pos_world.point.x,enemy_pos_world.point.y, enemy_pos_world.point.z);
    }
    catch(tf::TransformException &error)
    {
        ROS_WARN("%s",error.what());
    }
    
}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "follow_enemy");
    ros::NodeHandle nh;
    ros::Subscriber enemy_position_sub = nh.subscribe("aim_point", 10, &position_callback);
    ros::Publisher enemy_world_position_pub = nh.advertise<geometry_msgs::PointStamped>("enemy_position", 10);
    ros::Publisher enemy_goal_pub = nh.advertise<geometry_msgs::PoseStamped>("goal", 10);
	ros::Rate loop_rate(1);  
    while(nh.ok())
    {
        if(callback_flag)
        {
            enemy_world_position_pub.publish(enemy_pos_world);
            geometry_msgs::PoseStamped enemy_pose_pub;
            enemy_pose_pub.header = enemy_pos_imu.header;
            enemy_pose_pub.header.frame_id = "map";
            enemy_pose_pub.pose.position.x = enemy_pos_world.point.x;
            enemy_pose_pub.pose.position.y = enemy_pos_world.point.y;
            enemy_pose_pub.pose.position.z = enemy_pos_world.point.z;
            enemy_pose_pub.pose.orientation.w = 1;
            enemy_pose_pub.pose.orientation.x = 0;
            enemy_pose_pub.pose.orientation.y = 0;
            enemy_pose_pub.pose.orientation.z = 0;
            enemy_goal_pub.publish(enemy_pose_pub);
            callback_flag = 0;
        }
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}