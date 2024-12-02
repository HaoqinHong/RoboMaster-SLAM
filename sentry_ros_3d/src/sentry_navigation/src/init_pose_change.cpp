#include <ros/ros.h>
#include <std_msgs/String.h>
#include <tf/transform_listener.h>
#include <tf/tf.h>
#include <tf/transform_broadcaster.h>
#include "std_msgs/String.h"
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "math.h"
#include "robot_msgs/vision.h"
int robot_id = -1; // 7 is red 107 is blue
void robot_id_callback(const robot_msgs::vision::ConstPtr &msg)
{
    robot_id = msg->id;
    ROS_INFO("get id = %d", robot_id);

}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "init_pose_change");
    ros::NodeHandle nh;
    ros::Subscriber robot_id_sub = nh.subscribe("vision_data", 1, &robot_id_callback);
    ros::Publisher init_pose_pub = nh.advertise<geometry_msgs::PoseWithCovarianceStamped>("initialpose", 1);
    ros::Rate loop_rate(3);
    if (nh.ok())
    {
        ros::Duration(20).sleep();
        do{
            if (robot_id == 7)
            {
                double alpha = 0.0; // radian value
                double x_pos = 0.0;
                double y_pos = 0.0;

                geometry_msgs::PoseWithCovarianceStamped pose_msg;

                pose_msg.header.stamp = ros::Time::now();
                pose_msg.header.frame_id = "map";
                pose_msg.pose.pose.position.x = x_pos;
                pose_msg.pose.pose.position.y = y_pos;
                pose_msg.pose.covariance[0] = 0;
                pose_msg.pose.covariance[6 * 1 + 1] = 0;
                pose_msg.pose.covariance[6 * 5 + 5] = 0.0;
                pose_msg.pose.pose.orientation.z = sin(alpha / 2);
                pose_msg.pose.pose.orientation.w = cos(alpha / 2);

                init_pose_pub.publish(pose_msg);
                ROS_INFO("Setting to :(%f,%f)", x_pos, y_pos);
                return 0;
            }
            else if (robot_id == 107)
            {
                double alpha = 3.14; // radian value
                double x_pos = 5.45;
                double y_pos = -2.28;

                geometry_msgs::PoseWithCovarianceStamped pose_msg;

                pose_msg.header.stamp = ros::Time::now();
                pose_msg.header.frame_id = "map";
                pose_msg.pose.pose.position.x = x_pos;
                pose_msg.pose.pose.position.y = y_pos;
                pose_msg.pose.covariance[0] = 0;
                pose_msg.pose.covariance[6 * 1 + 1] = 0;
                pose_msg.pose.covariance[6 * 5 + 5] = 0.0;
                pose_msg.pose.pose.orientation.z = sin(alpha / 2);
                pose_msg.pose.pose.orientation.w = cos(alpha / 2);
                init_pose_pub.publish(pose_msg);
                ROS_INFO("Setting to :(%f,%f)", x_pos, y_pos);
                return 0;
            }
            ROS_INFO("id is %d", robot_id);
            ros::spinOnce();
        }while (1);
        // ros::spinOnce();
        // loop_rate.sleep();
    }
    return 0;
}
