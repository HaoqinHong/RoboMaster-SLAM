# simple_robot

## 文件结构

```
└── simple_robot
    ├── CMakeLists.txt
    ├── include
    │   ├── crc.h
    │   ├── hardware_interface.h
    │   ├── protocol.h
    │   ├── robot.h
    │   └── serial_device.h
    ├── launch
    │   └── robot.launch
    ├── msg
    │   ├── robot_ctrl.msg
    │   ├── sc_rc_msg.msg
    │   └── vision.msg
    ├── package.xml
    ├── readme.md
    ├── src
    │   ├── crc.cpp
    │   ├── robot_node.cpp
    │   └── serial_device.cpp
    ├── srv
    │   ├── sc_motor_srv.srv
    │   └── sc_servo_srv.srv
    └── udev
        ├── dbus-udev
        │   ├── dbus.rules
        │   └── setup.sh
        ├── robomaster_usb.rules
        └── setup.sh

```

## 功能说明
实现ROS与Robomaster C型开发板/A型开发板进行数据交互，通过micro USB连接，传输双方设定好的数据包

## 使用说明

1. 打开udev文件夹
2. 运行命令 `sudo sh setup.sh `设置端口权限
3. 启动节点 `roslaunch simple_robot robot.launch `

## 节点说明
以有示例程序
* 发布话题
/odom
/rc_message
/robot_ctrl
/vision_data
sc_rc_msg.msg
robot_ctrl.msg
vision.msg

* 自定义消息：
sc_rc_msg.msg
robot_ctrl.msg
vision.msg

* 修改方法

在 protocol.h内增加数据包结构体和数据包ID，在robot.h中增加话题或服务的回调函数和case cmd_ID设置发送和接收的数据包。

如果需要增加或修改msg只需要修改文件夹中的文件以及相应的头文件
其他节点要需要使用该msg则需要修改其他节点的引用头文件
修改数据包和增加功能，需要修改/include文件夹内robot.h和protocol.h

## Robomaster C板工程修改
* stm32工程修改方法

下位机使用C型开发板，代码包为standard_robot，已屏蔽部分无关任务。
烧录入C板后用microusb数据线将C板与上位机USB口连接即可通讯
通讯主要实现：
Usb_task 与decode_task 分别完成发送数据包和解码数据包。
CDC_send_queue 发送数据包传输队列

用户自定义修改：
Protocol_camp.h (数据包ID和数据包结构体)
Decode.c （接收数据包处理时间case）

* 主要逻辑
将需要发送的数据写入对应数据包结构体中，调用rm_queue_data函数入将数据包入队列等待发送。Usb_task将检测队列数据并将数据包序列化后发送。

当需要接收特定数据包的时候在Protocol_shaob.h增加数据包ID和数据包结构体，在decode.c文件中增加case事件，并在相应事件中将数值提取到对应全局结构体变量再由对应的执行控制任务执行完成处理，或者进行某些时间较短的IO操作（不能进行延时操作）







