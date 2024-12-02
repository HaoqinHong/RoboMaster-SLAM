#ifndef ROBOMASTER_PROTOCOL_H
#define ROBOMASTER_PROTOCOL_H

//协方差矩阵，用于里程计话题数据，用于robt_pose_ekf功能包
const double odom_pose_covariance[36]   = { 1e-3,    0,    0,   0,   0,    0, 
										                        0, 1e-3,    0,   0,   0,    0,
										                        0,    0,  1e6,   0,   0,    0,
										                        0,    0,    0, 1e6,   0,    0,
										                        0,    0,    0,   0, 1e6,    0,
										                        0,    0,    0,   0,   0,  1e3 };

const double odom_pose_covariance2[36]  = {1e-9,    0,    0,   0,   0,    0, 
										                        0, 1e-3, 1e-9,   0,   0,    0,
										                        0,    0,  1e6,   0,   0,    0,
										                        0,    0,    0, 1e6,   0,    0,
										                        0,    0,    0,   0, 1e6,    0,
										                        0,    0,    0,   0,   0, 1e-9 };

const double odom_twist_covariance[36]  = {1e-3,    0,    0,   0,   0,    0, 
										                        0, 1e-3,    0,   0,   0,    0,
										                        0,    0,  1e6,   0,   0,    0,
										                        0,    0,    0, 1e6,   0,    0,
										                        0,    0,    0,   0, 1e6,    0,
										                        0,    0,    0,   0,   0,  1e3 };
										      
const double odom_twist_covariance2[36] = {1e-9,    0,    0,   0,   0,    0, 
										                        0, 1e-3, 1e-9,   0,   0,    0,
										                        0,    0,  1e6,   0,   0,    0,
										                        0,    0,    0, 1e6,   0,    0,
										                        0,    0,    0,   0, 1e6,    0,
										                        0,    0,    0,   0,   0, 1e-9} ;

#define HEADER_SOF 0xA5
#define END1_SOF 0x0D
#define END2_SOF 0x0A

#pragma pack(push, 1)
//ZXX
typedef enum
{
  CHASSIS_ODOM_CMD_ID = 0x0101,
  CHASSIS_CTRL_CMD_ID = 0x0102,
  VISION_CTRL_CMD_ID = 0x0107,
  BEHAVIOR_CTRL_CMD_ID = 0x0108,
  SEND_NAV_INFO_CMD_ID = 0x0103,
  RECEIVE_COMPETITION_INFO_CMD_ID = 0x0104,
  VISION_ID=0x0105
} referee_data_cmd_id_type;

typedef  struct
{
  uint8_t SOF;
  uint16_t data_length;
  uint8_t seq;
  uint8_t CRC8;
} frame_header_struct_t;

typedef struct
{
  float vx=0;
  float vy=0;
  float vw=0;
  float relative_yaw=0;
  float gyro_yaw=0;
}  chassis_odom_info_t;

typedef struct
{
  float x_pos=0;
  float y_pos=0;
  float z_pos=0;
}  chassis_odom_pose_t;

typedef struct
{
    float vx=0;
    float vy=0;
    float vw=0;
    float yaw=0;
    float pitch=0;
    int8_t target_lock=0;
    int8_t fire_command=0; 
    int8_t spin_command=0;
    int32_t left_patrol_angle=0;
    int32_t right_patrol_angle=0;
}robot_ctrl_info_t;

typedef struct
{
    uint8_t command_info=0;//key board info
    uint8_t game_state=0;//比赛阶段，4为开始比赛，0为还没有开始比赛
    uint16_t our_outpost_hp=0;//我方前哨站血量
    uint16_t enemy_outpost_hp=0;//敌方前哨站血量
    uint16_t remain_bullet=0;//剩余发弹量
    uint16_t radar_info=0;//雷达信息
    float goal_point_x=0;// m
    float goal_point_y=0;// m
    float goal_point_z=0;//m
} receive_competition_info;

typedef struct
{
    uint8_t purpose;
    uint16_t start_point_x;//dm
    uint16_t start_point_y;//dm
    int8_t path_delta_x[49];//dm
    int8_t path_delta_y[49];//dm
} send_nav_info;

typedef struct
{
  uint16_t id;
  uint16_t shoot_sta;
  float pitch=0;
  float yaw=0;
  float roll=0;
  float quaternion[4]={0};
  float shoot_spd=0;
} vision_t;

typedef struct
{

  int16_t ch[5];
  char s[2];

}  rc_info_t;


#pragma pack(pop)
#endif //ROBOMASTER_PROTOCOL_H
