
#include <iostream>
#include <ros/ros.h>
// PCL specific includes
#include <sensor_msgs/PointCloud2.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/crop_box.h>
using namespace std;
class clip_box
{
public:
    clip_box()
    {   pub = nh.advertise<sensor_msgs::PointCloud2>("/livox_pcl_filter", 10);
        sub = nh.subscribe("/livox_pcl0",10,&clip_box::call_back,this);
 
    }
 
    void call_back(const sensor_msgs::PointCloud2ConstPtr input)
    {
        cout<<"I RECEIVED INPUT !"<<endl;
        pcl::fromROSMsg(*input,*cloud_in);
 

        Eigen::Vector4f min_point (-0.6f, -0.3f, -1.0f, 1.0f);
        Eigen::Vector4f max_point (0.6f, 0.6f, 1.0f, 1.0f);
        clipper.setMin(min_point);
        clipper.setMax(max_point);
        clipper.setInputCloud(cloud_in);
        clipper.filter(*cloud_final);
        clipper.setNegative(negative);//默认为false
 
        pcl::toROSMsg(*cloud_final,cloud_out);
        cloud_out.header.stamp = ros::Time::now();
        cloud_out.header.frame_id = "livox_frame";
        pub.publish(cloud_out);
        cout<<"I PROCESSED IT !"<<endl;
    }
private:
    ros::NodeHandle nh;
    ros::Publisher pub;
    ros::Subscriber sub;
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_in {new pcl::PointCloud<pcl::PointXYZ>};//转换为pcl格式
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_final {new pcl::PointCloud<pcl::PointXYZ>};//加工后的pcl格式
    sensor_msgs::PointCloud2 cloud_out;//转换为ros格式
    //clip
    pcl::CropBox<pcl::PointXYZ> clipper;//要包含的头文件，在官方文档里
    Eigen::Vector4f min_point;
    Eigen::Vector4f max_point;
    float x1,y1,z1;
    float x2,y2,z2;
    bool negative;
};
int main(int argc,char **argv)
{
    ros::init(argc,argv,"clip_box");
    clip_box clip_box;
    ros::spin();
}