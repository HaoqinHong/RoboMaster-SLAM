#include <ros/ros.h>
#include <std_msgs/String.h>
#include <tf/transform_listener.h>
#include <tf/tf.h>
#include <tf/transform_broadcaster.h>
#include "std_msgs/String.h"
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "sensor_msgs/Imu.h"
#include "math.h"
#include <nav_msgs/Odometry.h>
#include "robot_msgs/vision.h"
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Geometry>
geometry_msgs::TransformStamped odom;
nav_msgs::Odometry odom_move_base;
geometry_msgs::TransformStamped odom_last;
ros::Time current_time, last_time;
int callback_flag = 0;
double yaw_angle,pitch_angle,roll_angle,last_yaw_angle;
tf::Quaternion RQ2;
float trans_x = -0.185, trans_y = 0.0, trans_z = 0;
float rot_w = 0.6705, rot_x = 0.2243, rot_y = -0.2243, rot_z = 0.6705;
void odom_callback(const nav_msgs::Odometry::ConstPtr &msg)
{
    static tf::TransformBroadcaster odom_broadcaster;
    tf::Transform transform;
    tf::Quaternion q(msg->pose.pose.orientation.x, msg->pose.pose.orientation.y, msg->pose.pose.orientation.z, msg->pose.pose.orientation.w);
    transform.setOrigin(tf::Vector3(msg->pose.pose.position.x, msg->pose.pose.position.y, msg->pose.pose.position.z));
    transform.setRotation(q);
    // // 将雷达里程转换为底盘里程然后发布到tf关系中
    tf::Transform laser_transform;
    laser_transform.setOrigin(tf::Vector3(trans_x, trans_y, trans_z));
    laser_transform.setRotation(tf::Quaternion(rot_x, rot_y, rot_z, rot_w));
    transform = transform * laser_transform;
    odom_broadcaster.sendTransform(tf::StampedTransform(transform, msg->header.stamp, "camera_init", "base_footprint"));
    //计算里程计给movebase flex
    odom_last = odom;
    last_time = current_time;
    current_time = ros::Time::now();
    double dt = (current_time - last_time).toSec();
    tf::Quaternion q_odom(
        msg->pose.pose.orientation.x,
        msg->pose.pose.orientation.y,
        msg->pose.pose.orientation.z,
        msg->pose.pose.orientation.w);
    tf::Vector3 p_odom(
        msg->pose.pose.position.x,
        msg->pose.pose.position.y,
        msg->pose.pose.position.z);
    tf::Transform odom_transform(q_odom, p_odom);
    tf::Transform base_transform = transform.inverse() * odom_transform * transform;
    //填入里程计信息
    odom.transform.translation.x = base_transform.getOrigin().getX();
    odom.transform.translation.y = base_transform.getOrigin().getY();
    last_yaw_angle = yaw_angle;
    geometry_msgs::Quaternion q_angular;
    q_angular.w = transform.getRotation().getW();
    q_angular.x = transform.getRotation().getX();
    q_angular.y = transform.getRotation().getY();
    q_angular.z = transform.getRotation().getZ();
    tf::quaternionMsgToTF(q_angular,RQ2);
    tf::Matrix3x3(RQ2).getRPY(roll_angle,pitch_angle,yaw_angle);

    odom_move_base.header = msg->header;
    odom_move_base.child_frame_id = "base_footprint";
    odom_move_base.header.frame_id = "map";
    odom_move_base.pose.pose.position.x = base_transform.getOrigin().getX();
    odom_move_base.pose.pose.position.y = base_transform.getOrigin().getY();
    odom_move_base.pose.pose.position.z = 0;
    odom_move_base.pose.pose.orientation.w = transform.getRotation().getW();
    odom_move_base.pose.pose.orientation.x = transform.getRotation().getX();
    odom_move_base.pose.pose.orientation.y = transform.getRotation().getY();
    odom_move_base.pose.pose.orientation.z = transform.getRotation().getZ();

    odom_move_base.twist.twist.linear.x = (odom.transform.translation.x - odom_last.transform.translation.x) / dt;
    odom_move_base.twist.twist.linear.y = (odom.transform.translation.y - odom_last.transform.translation.y) / dt;
    odom_move_base.twist.twist.angular.z = (yaw_angle - last_yaw_angle) / dt;
    callback_flag = 1;
    ROS_INFO("get odome");
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "odom_trans");
    ros::NodeHandle nh;
    ros::Subscriber lio_odom_sub = nh.subscribe("aft_mapped_to_init", 1000, &odom_callback);
    ros::Publisher odom_pub = nh.advertise<nav_msgs::Odometry>("odom", 1000);
	// ros::Rate loop_rate(1000);  
    while (nh.ok())
    {
        if (callback_flag == 1)
        {
            odom_pub.publish(odom_move_base);
            callback_flag = 0;
        }
        ros::spinOnce();
        // loop_rate.sleep();
    }
    return 0;
}