# sentry_ros

参考深圳技术大学 "悍匠" 战队开源的哨兵 ROS 代码部分，并且集成了了 Fast-Lio 等 SLAM 建图方案。

使用Robomaster-C型开发板上的虚拟串口功能使用 simple-robot 功能包进行与嵌入式下位机的通讯

可能会需要的与底层的接口有： 

file&address：sentry_ros/src/simple_robot/msg/vision.msg  

topic： vision_data

data：      uint16 id （哨兵此时为红/蓝方）
            uint16 shoot_sta （发射控制位）
            float32 pitch （上正下负）
            float32 yaw （右手向上螺旋方向为正）
            float32 roll （右手拇指朝前螺旋方向为正）
            float32 shoot_spd （射速）

file&address：sentry_ros/src/simple_robot/msg/competition_info.msg  

topic:  competition_info

data:       uint8 command_info (云台手按下的键盘信息)
            uint8 game_state (比赛进程，包括比赛未开始，与比赛开始)
            uint16 our_outpost_hp (我方前哨战血量)
            uint16 enemy_outpost_hp (敌方前哨战血量,用于视觉判断是否需要攻击敌方哨兵)
            uint16 remain_bullet (哨兵剩余弹量)
            uint16 radar_info (雷达站信息，用于判断对方英雄位置)
            float32 goal_point_x (云台手发布的坐标位置)
            float32 goal_point_y
            float32 goal_point_z

file&address：sentry_ros/src/simple_robot/msg/robot_ctrl.msg  

topic:  robot_ctrl

data:       float32 yaw
            float32 pitch
            int8 target_lock（自瞄有效位）
            int8 fire_command (开火有效位)

topic:  behavior_ctrl

data:
            int8 spin_command(若开启行为树则发布陀螺指令)
            int32 left_patrol_angle(云台巡逻角度)
            int32 right_patrol_angle
