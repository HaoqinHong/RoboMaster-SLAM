#include "geometry_msgs/PointStamped.h"
#include "robot_msgs/rm_game_info.h"
#include "ros.h"
void point_publish(geometry_msgs::PointStamped &point,ros::NodeHandle &node)
{
    ros::Publisher click_point_pub;
    click_point_pub = node.advertise<geometry_msgs::PointStamped>("/clicked_point",1);
    click_point_pub.publish(point);
}

void point_prepare(ros::NodeHandle &n)
{
    geometry_msgs::PointStamped prepare_point;
    prepare_point.header.frame_id = "map";
    prepare_point.header.stamp = ros::Time::now();
    prepare_point.point.x = 1.0;
    prepare_point.point.y = 2.0;
    prepare_point.point.z = 0.0;
    point_publish(prepare_point,n);
}

void point_invincible(ros::NodeHandle &n)
{
    geometry_msgs::PointStamped prepare_point;
    prepare_point.header.frame_id = "map";
    prepare_point.header.stamp = ros::Time::now();
    prepare_point.point.x = 1.0;
    prepare_point.point.y = 2.0;
    prepare_point.point.z = 0.0;
    point_publish(prepare_point,n);
}