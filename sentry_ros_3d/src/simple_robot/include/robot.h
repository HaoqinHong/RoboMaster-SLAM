#ifndef ROBOMASTER_ROBOT_H
#define ROBOMASTER_ROBOT_H

#include "serial_device.h"
#include "protocol.h"
#include "crc.h"

#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "std_msgs/String.h"
#include "std_msgs/Int8.h"
#include "std_msgs/Float32.h"
#include "nav_msgs/Path.h"
#include "robot_msgs/sc_rc_msg.h"
#include "robot_msgs/robot_ctrl.h"
#include "robot_msgs/vision.h"
#include "robot_msgs/competition_info.h"
#include <thread>
#include <tf/tf.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>

/**
 * @brief Robot Base Node
 *        Main Process is
 *        1. RECEIVING:
 *           Virtual Serial Comm -> Unpack and Get Protocol Data
 *           -> Convert to ROS Data -> ROS Publish
 *        2. SENDING:
 *           ROS Subscribe -> Get ROS Data -> Convert to Protocol Data
 *           -> Convert to Protocol Data and Pack -> Virtual Serial Comm
 */
double x = 0.0;
double y = 0.0;
double th = 0.0;
ros::Time current_time, last_time;

namespace robomaster
{
  class Robot
  {
  public:
    Robot(std::string device_path = "/dev/robomaster") : device_path_(device_path)
    {

      if (!(ROSInit() && CommInit()))
      {
        ros::shutdown();
      };
    }
    ~Robot()
    {
      if (recv_thread_.joinable())
      {
        recv_thread_.join();
      }
    }

    void navgation_ctrl_callback(const geometry_msgs::Twist &cmd_vel)
    {
      robot_ctrl.vx = cmd_vel.linear.x;
      robot_ctrl.vy = cmd_vel.linear.y;
      robot_ctrl.vw = cmd_vel.angular.z;
      uint16_t send_length = SenderPackSolve((uint8_t *)&robot_ctrl, sizeof(robot_ctrl_info_t),
                                             CHASSIS_CTRL_CMD_ID, send_buff_.get());
      device_ptr_->Write(send_buff_.get(), send_length);
      // ROS_INFO("Sending nav_ctrl msg");
    }

    void robot_ctrl_callback(const robot_msgs::robot_ctrl::ConstPtr &msg)
    {
      robot_ctrl.pitch = msg->pitch;
      robot_ctrl.yaw = msg->yaw;
      robot_ctrl.target_lock = msg->target_lock;
      robot_ctrl.fire_command = msg->fire_command;
      uint16_t send_length = SenderPackSolve((uint8_t *)&robot_ctrl, sizeof(robot_ctrl_info_t),
                                             VISION_CTRL_CMD_ID, send_buff_.get());
      device_ptr_->Write(send_buff_.get(), send_length);
      ROS_INFO("Sending robot_ctrl msg");
    }

    void behavior_ctrl_callback(const robot_msgs::robot_ctrl::ConstPtr &msg)
    {
      robot_ctrl.spin_command = msg->spin_command;
      robot_ctrl.left_patrol_angle = msg->left_patrol_angle;
      robot_ctrl.right_patrol_angle = msg->right_patrol_angle;
      uint16_t send_length = SenderPackSolve((uint8_t *)&robot_ctrl, sizeof(robot_ctrl_info_t),
                                             BEHAVIOR_CTRL_CMD_ID, send_buff_.get());
      device_ptr_->Write(send_buff_.get(), send_length);
      ROS_INFO("Sending behavior_ctrl msg");
    }

    void global_path_callback(const nav_msgs::Path::ConstPtr &msg)
    {
      int path_lenth = std::end(msg->poses) - std::begin(msg->poses);
      if(path_lenth>0)
      {
        if(start_point==true)
        {
          nav_info.purpose = 3;
          nav_info.start_point_x = msg->poses[0].pose.position.x*10;
          nav_info.start_point_y = msg->poses[0].pose.position.y*10;
          ROS_INFO("path lenth is :%d",path_lenth);
          for (int i = 0; i < path_lenth-1; i++)
          {
              nav_info.path_delta_x[i] = 0;//(int8_t)(msg->poses[i+1].pose.position.x*10 - msg->poses[i].pose.position.x*10);
              nav_info.path_delta_y[i] = 0;//(int8_t)(msg->poses[i+1].pose.position.y*10 - msg->poses[i].pose.position.y*10);
          }
          start_point = false;
        }
        else if(start_point==false)
        {
          nav_info.purpose = 1;
          nav_info.start_point_x = msg->poses[path_lenth-1].pose.position.x*10;
          nav_info.start_point_y = msg->poses[path_lenth-1].pose.position.y*10;
          ROS_INFO("path lenth is :%d",path_lenth);
          for (int i = 0; i < path_lenth-1; i++)
          {
              nav_info.path_delta_x[i] = 0;//(int8_t)(msg->poses[i+1].pose.position.x*10 - msg->poses[i].pose.position.x*10);
              nav_info.path_delta_y[i] = 0;//(int8_t)(msg->poses[i+1].pose.position.y*10 - msg->poses[i].pose.position.y*10);
          }
          start_point = true;
        }
      }
      else ROS_INFO("path get failed");
      for (int i = path_lenth; i < 49; i++)
      {
          nav_info.path_delta_x[i] = 0;
          nav_info.path_delta_y[i] = 0;
      }
      
      uint16_t send_length = SenderPackSolve((uint8_t *)&nav_info, sizeof(nav_info),
                                             SEND_NAV_INFO_CMD_ID, send_buff_.get());
      device_ptr_->Write(send_buff_.get(), send_length);
      // ROS_INFO("Sending nav_ctrl msg\nstart:x=%d,y=%d\ndelta:x0=%d,y0=%d",nav_info.start_point_x,nav_info.start_point_y,nav_info.path_delta_x[0],nav_info.path_delta_y[0]);
    }
  private:
    bool ROSInit()
    {
      ros::NodeHandle nh;

      robot_ctrl_sub_=nh.subscribe("robot_ctrl",1,&Robot::robot_ctrl_callback,this);
      behavior_ctrl_sub_=nh.subscribe("behavior_ctrl",1,&Robot::behavior_ctrl_callback,this);
      global_path_sub_ = nh.subscribe("move_base_flex/GlobalPlanner/plan",2000,&Robot::global_path_callback,this);
      rc_msg_pub_ = nh.advertise<robot_msgs::sc_rc_msg>("rc_message", 1);
      vision_pub_ = nh.advertise<robot_msgs::vision>("vision_data", 100);
      competition_info_pub_ = nh.advertise<robot_msgs::competition_info>("competition_info",100);
      nav_by_hand_pub_=nh.advertise<robot_msgs::competition_info>("nav_by_hand_info",100);
      cmd_vel_sub_ = nh.subscribe("cmd_vel", 10, &Robot::navgation_ctrl_callback, this);
      current_time = ros::Time::now();
      last_time = ros::Time::now();

      return true;
    }
    bool CommInit()
    {

      device_ptr_ = std::make_shared<SerialDevice>(device_path_, 115200); // 比特率115200

      if (!device_ptr_->Init())
      {
        return false;
      }

      recv_buff_ = std::unique_ptr<uint8_t[]>(new uint8_t[BUFF_LENGTH]);
      send_buff_ = std::unique_ptr<uint8_t[]>(new uint8_t[BUFF_LENGTH]);

      memset(&frame_receive_header_, 0, sizeof(frame_header_struct_t));
      memset(&frame_send_header_, 0, sizeof(frame_header_struct_t));

      /** Specific Protocol Data Initialize here**/
      // memset(&summer_camp_info_, 0, sizeof(summer_camp_info_t));

      // Start recv thread
      recv_thread_ = std::thread(&Robot::RecvThread, this);
      return true;
    }

    void RecvThread()
    {

      int a = 0;
      int flag = 0;

      uint8_t last_len = 0;

      while (ros::ok())
      {
        // uint16_t read_length = device_ptr_->Read(recv_buff_.get(),BUFF_LENGTH);

        last_len = device_ptr_->ReadUntil2(recv_buff_.get(), END1_SOF, END2_SOF, 128);

        while (flag == 0 && last_len == 1)
        {
          if ((recv_buff_[a] == END1_SOF) && (recv_buff_[a + 1] == END2_SOF))
          {
            flag = 1;
            // printf("%x  ",recv_buff_[a]);
            //  printf("%x  ",recv_buff_[a+1]);
            // printf("\n");
            // printf("------------------------------------------\n");
            SearchFrameSOF(recv_buff_.get(), a);
          }
          // printf("%x  ",recv_buff_[a]);
          a++;
        }
        flag = 0;
        a = 0;
        /*
        if (flag ==0 )
         {
           memcpy(p,p2,read_length);
           last_len = read_length;
           flag = 1;
         }
         else if(flag == 1)
         {
             memcpy(p+last_len,p2,read_length);
             last_last_len = read_length;
           flag = 2;
         }
         else
         {
           memcpy(p+last_len+last_last_len,p2,read_length);
           flag = 0;
         */
        // printf("len:%d\n",read_length+last_len+last_last_len);
        // printf("len1:%d\n",read_length);
        // for( a = 0;a<read_length+last_len+last_last_len;a++){
        // printf("%x  ",Recv_Buf[a]);
        //  }
        // printf("\n");
        // printf("------------------------------------------\n");
        ros::spinOnce(); 
        usleep(1);
      }
    }

    void SearchFrameSOF(uint8_t *frame, uint16_t total_len)
    {
      uint16_t i;
      uint16_t index = 0;
      int a = 0;

      for (i = 0; i < total_len;)
      {
        if (*frame == HEADER_SOF)
        {
          // for(a=0;a<21;a++)
          //  {
          //   printf("%x  ",*(frame+a));
          //}
          // printf("\n");
          ReceiveDataSolve(frame);
          i = total_len;
        }
        else
        {
          frame++;
          i++;
        }
      }
      /*
          for (i = 0; i < total_len;) {

            if (*frame == HEADER_SOF) {
              printf("%d\n ",total_len);
        for(int  a = 0; a<total_len; a++){
            printf("%x  ",*(frame+a));
             }
            printf("\n");
              index = ReceiveDataSolve(frame);
              i += index;
              frame += index;
            } else {
              i++;
              frame++;
            }
          }
      */
    }

    uint16_t ReceiveDataSolve(uint8_t *frame)
    {
      uint8_t index = 0;
      uint16_t cmd_id = 0;

      if (*frame != HEADER_SOF)
      {
        return 0;
      }

      memcpy(&frame_receive_header_, frame, sizeof(frame_header_struct_t));
      index += sizeof(frame_header_struct_t);

      // printf("CRC8: %d\n",Verify_CRC8_Check_Sum(frame, sizeof(frame_header_struct_t)));
      // printf("CRC16: %d\n",Verify_CRC16_Check_Sum(frame, frame_receive_header_.data_length + 9));
      // printf("data length : %d \n",frame_receive_header_.data_length);

      if ((!Verify_CRC8_Check_Sum(frame, sizeof(frame_header_struct_t))) || (!Verify_CRC16_Check_Sum(frame, frame_receive_header_.data_length + 9)))
      {
        ROS_ERROR("CRC  EEROR!");
        return 0;
      }
      else
      {
        memcpy(&cmd_id, frame + index, sizeof(uint16_t));
        index += sizeof(uint16_t);
        // printf("id:%x\n", cmd_id);
        switch (cmd_id)
        {

        /** Write Your code here to get different types of data and publish them using ROS interface
         *
         *  Example:
         *
         *   case XXXX_CMD_ID:{
         *
         *    memcpy(&xxxx_info, frame + index, sizeof(xxxx_info_t))
         *    break;
         *
         *   }
         *
         */
        // case CHASSIS_ODOM_CMD_ID:
        // {
          // ROS_INFO("ODOM info");
//           // printf("get chassis_odom_msg");
          // memcpy(&chassis_odom_info_, frame + index, sizeof(chassis_odom_info_t));
          // current_time = ros::Time::now();

//           float dt = (current_time - last_time).toSec();
//           chassis_odom_pose.x_pos += (chassis_odom_info_.vy * sin(chassis_odom_info_.relative_yaw*0.0174) + chassis_odom_info_.vx * cos(chassis_odom_info_.relative_yaw*0.0174)) * dt;
//           chassis_odom_pose.y_pos += (chassis_odom_info_.vy * cos(chassis_odom_info_.relative_yaw*0.0174) - chassis_odom_info_.vx * sin(chassis_odom_info_.relative_yaw*0.0174)) * dt;
//           chassis_odom_pose.z_pos = vision_msg_.yaw*0.0174;
//           //      因为里程计使用麦轮解算得到,无需里程计到base_footprint的tf变换,直接从融合算法ekf发布tf变换即可
//           geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(chassis_odom_pose.z_pos);

// // //给amcl3d用的，巨抽象，用的话题数据是geometry_msgs::TransformStamped，既不是tf变换也不是nav_msgs::Odometry，妈的傻逼 4/14/04：39留，搞了半个晚上
//     odom_trans.header.stamp = current_time;
//     odom_trans.header.frame_id = "odom";
//     odom_trans.child_frame_id = "base_footprint";

//     odom_trans.transform.translation.x += (chassis_odom_info_.vy * sin(chassis_odom_info_.relative_yaw*0.0174) + chassis_odom_info_.vx * cos(chassis_odom_info_.relative_yaw*0.0174)) * dt;
//     odom_trans.transform.translation.y += (chassis_odom_info_.vy * cos(chassis_odom_info_.relative_yaw*0.0174) - chassis_odom_info_.vx * sin(chassis_odom_info_.relative_yaw*0.0174)) * dt;
//     odom_trans.transform.translation.z = 0;
//     odom_trans.transform.rotation = odom_quat;

//     //send the transform
//     amcl_odom_pub_.publish(odom_trans);

//           odom_.header.stamp = current_time;
//           odom_.header.frame_id = "odom";

//           odom_.pose.pose.position.x = chassis_odom_pose.x_pos;
//           odom_.pose.pose.position.y = chassis_odom_pose.y_pos;
//           odom_.pose.pose.position.z = chassis_odom_pose.z_pos;
//           odom_.pose.pose.orientation = odom_quat;

//           odom_.child_frame_id = "base_footprint";
//           odom_.twist.twist.linear.x = (chassis_odom_info_.vy * sin(chassis_odom_info_.relative_yaw*0.0174) + chassis_odom_info_.vx * cos(chassis_odom_info_.relative_yaw*0.0174));
//           odom_.twist.twist.linear.y = (chassis_odom_info_.vy * sin(chassis_odom_info_.relative_yaw*0.0174) + chassis_odom_info_.vx * cos(chassis_odom_info_.relative_yaw*0.0174));
//           odom_.twist.twist.angular.z = chassis_odom_info_.gyro_yaw;
//           // if (chassis_odom_info_.vx ==0 && chassis_odom_info_.vy ==0 && chassis_odom_info_.vw ==0)
//           // {
//           //   // 如果velocity是零，说明编码器的误差会比较小，认为编码器数据更可靠
//           //   memcpy(&odom_.pose.covariance, odom_pose_covariance2, sizeof(odom_pose_covariance2)),
//           //   memcpy(&odom_.twist.covariance, odom_twist_covariance2, sizeof(odom_twist_covariance2));
//           // }
//           // else
//           // {
//           //   // 如果小车velocity非零，考虑到运动中编码器可能带来的滑动误差，认为imu的数据更可靠
//           //   memcpy(&odom_.pose.covariance, odom_pose_covariance, sizeof(odom_pose_covariance)),
//           //   memcpy(&odom_.twist.covariance, odom_twist_covariance, sizeof(odom_twist_covariance));

//           // }
          // chassis_odom_pub_.publish(odom_);
          // last_time = current_time;
        // }
        // break;

        case VISION_ID:
        {
          ROS_INFO("VISION info");
          memcpy(&vision_msg_, frame + index, sizeof(vision_t));
          vision_pubmsg.header.frame_id = "euler";
          vision_pubmsg.header.seq++;
          vision_pubmsg.header.stamp = ros::Time::now();
          vision_pubmsg.id = vision_msg_.id;
          vision_pubmsg.yaw = vision_msg_.yaw;
          vision_pubmsg.pitch = vision_msg_.pitch;
          vision_pubmsg.roll = vision_msg_.roll;
          vision_pubmsg.shoot_spd = vision_msg_.shoot_spd;
          vision_pubmsg.shoot_sta = vision_msg_.shoot_sta;
          vision_pubmsg.quaternion.resize(4);//设置自定义消息数组的长度
          for (int i = 0; i < 4; i++)
          {
            vision_pubmsg.quaternion[i] = vision_msg_.quaternion[i];
          }
          vision_pub_.publish(vision_pubmsg);
        }
        break;
        
        case RECEIVE_COMPETITION_INFO_CMD_ID:
        {
          ROS_INFO("COMPETITION info");
            memcpy(&competition_info_,frame + index,sizeof(receive_competition_info));
            competition_infomsg.command_info =competition_info_.command_info;
            competition_infomsg.game_state =competition_info_.game_state;
            competition_infomsg.our_outpost_hp = competition_info_.our_outpost_hp;
            competition_infomsg.enemy_outpost_hp = competition_info_.enemy_outpost_hp;
            competition_infomsg.remain_bullet = competition_info_.remain_bullet;
            competition_infomsg.radar_info = competition_info_.radar_info;
            competition_infomsg.goal_point_x =competition_info_.goal_point_x;
            competition_infomsg.goal_point_y =competition_info_.goal_point_y;
            competition_infomsg.goal_point_z =competition_info_.goal_point_z;
            if (last_competition_infomsg.goal_point_x!=competition_infomsg.goal_point_x ||last_competition_infomsg.goal_point_y!=competition_infomsg.goal_point_y){
                nav_by_hand_pub_.publish(competition_infomsg);
            }
            last_competition_infomsg=competition_infomsg;
            competition_info_pub_.publish(competition_infomsg);
        }
        break;

        default:
          break;
        }
        index += frame_receive_header_.data_length + 2;
        return index;
      }
    }

    uint16_t SenderPackSolve(uint8_t *data, uint16_t data_length,
                             uint16_t cmd_id, uint8_t *send_buf)
    {

      uint8_t index = 0;
      frame_send_header_.SOF = HEADER_SOF;
      frame_send_header_.data_length = data_length;
      frame_send_header_.seq++;

      Append_CRC8_Check_Sum((uint8_t *)&frame_send_header_, sizeof(frame_header_struct_t));

      memcpy(send_buf, &frame_send_header_, sizeof(frame_header_struct_t));

      index += sizeof(frame_header_struct_t);

      memcpy(send_buf + index, &cmd_id, sizeof(uint16_t));

      index += sizeof(uint16_t);

      memcpy(send_buf + index, data, data_length);

      Append_CRC16_Check_Sum(send_buf, data_length + 9);

      return data_length + 9;
    }

  private:
    //! VCOM Data Receiving Thread (Tips: VCOM Sending part is in Each ROS Data Callback)
    std::thread recv_thread_;

    //! Device Information and Buffer Allocation
    std::string device_path_;
    std::shared_ptr<SerialDevice> device_ptr_;
    std::unique_ptr<uint8_t[]> recv_buff_;
    std::unique_ptr<uint8_t[]> send_buff_;
    const unsigned int BUFF_LENGTH = 512;

    //! Frame Information
    frame_header_struct_t frame_receive_header_;
    frame_header_struct_t frame_send_header_;

    /** @brief specific protocol data are defined here
     *         xxxx_info_t is defined in protocol.h
     */

    //! Receive from VCOM
    chassis_odom_info_t chassis_odom_info_;
    chassis_odom_pose_t chassis_odom_pose;
    robot_ctrl_info_t robot_ctrl;
    send_nav_info nav_info;
    // geometry_msgs::TransformStamped odom_tf_;//! ros chassis odometry tf
    geometry_msgs::TransformStamped odom_trans;
    vision_t vision_msg_;
    receive_competition_info competition_info_;
    bool start_point=false;

    nav_msgs::Odometry odom_;
    robot_msgs::sc_rc_msg sc_rc_msg;
    robot_msgs::vision vision_pubmsg;
    robot_msgs::competition_info competition_infomsg;
    robot_msgs::competition_info last_competition_infomsg;
    //! Send to VCOM

    /** @brief ROS data corresponding to specific protocol data are defined here
     *         You can use ROS provided message data type or create your own one
     *         More information please refer to
     *               http://wiki.ros.org/ROS/Tutorials/CreatingMsgAndSrv
     */

    /** @brief ROS Subscription and Publication
     */

    ros::Subscriber message_sub_;
    ros::Subscriber robot_ctrl_sub_;
    ros::Subscriber behavior_ctrl_sub_;
    ros::Subscriber cmd_vel_sub_;
    ros::Subscriber global_path_sub_;
    ros::Publisher message_pub_;
    ros::Publisher competition_info_pub_;
    ros::Publisher nav_by_hand_pub_;
    ros::Publisher motor_message_pub_;
    ros::Publisher vision_pub_;
    ros::Publisher rc_msg_pub_;
  };
}

#endif // ROBOMASTER_ROBOT_H
