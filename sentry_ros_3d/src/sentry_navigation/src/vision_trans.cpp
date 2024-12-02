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
geometry_msgs::TransformStamped vision_odom;
tf::Quaternion RQ2;
int callback_flag=0;
float yaw_angle,roll_angle;
float trans_x=0,trans_y=0,trans_z=0;
float rot_w = 0.707,rot_x = 0,rot_y=0,rot_z=0.707;
void odom_callback(const nav_msgs::Odometry::ConstPtr &msg)
{
    static tf::TransformBroadcaster vision_broadcaster;
    tf::Transform transform_v;
    transform_v.setOrigin(tf::Vector3(msg->pose.pose.position.x, msg->pose.pose.position.y, msg->pose.pose.position.z));
    tf::Quaternion q(0,0,0,1);
    transform_v.setRotation(q);
    // // 将雷达里程转换为底盘里程
    tf::Transform laser_transform;
    laser_transform.setOrigin(tf::Vector3(trans_x, trans_y, trans_z));
    laser_transform.setRotation(tf::Quaternion(rot_x, rot_y, rot_z, rot_w));
    transform_v = transform_v * laser_transform;
    vision_broadcaster.sendTransform(tf::StampedTransform(transform_v, msg->header.stamp, "camera_init", "vision_axes"));

    vision_odom.header = msg->header;
    vision_odom.header.frame_id = "camera_init";
    vision_odom.header.stamp = ros::Time::now();
    vision_odom.child_frame_id = "vision_axes";
    vision_odom.transform.translation.x = transform_v.getOrigin().getX();
    vision_odom.transform.translation.y = transform_v.getOrigin().getY();
    vision_odom.transform.translation.z = 0;
    vision_odom.transform.rotation.w = 0;
    vision_odom.transform.rotation.x = 0;
    vision_odom.transform.rotation.y = 0;
    vision_odom.transform.rotation.z = 0;

    callback_flag = 1;
    ROS_INFO("get odome");
}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "odom_trans");
    ros::NodeHandle nh;
    ros::Subscriber lio_odom_sub = nh.subscribe("aft_mapped_to_init", 100, &odom_callback);
    ros::Publisher vision_odom_pub = nh.advertise<geometry_msgs::TransformStamped>("vision_odom", 100);
	ros::Rate loop_rate(100);  
    while(nh.ok())
    {
        if(callback_flag==1)
        {
            vision_odom_pub.publish(vision_odom);
            callback_flag=0;
        }
        ros::spinOnce();
        // loop_rate.sleep();
    }
    return 0;
}