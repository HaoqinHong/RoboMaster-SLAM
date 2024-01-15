### 基于 Point-Lio 和 Mid360 的哨兵导航方案

#### Ubuntu 22.04

#### 1.Docker 部署 ROS Noetic
因为 ROS 是面向操作系统的语言，所以 ROS1 只适配 Ubuntu 20.04 及以下的版本， Ubuntu 22.04 只适配 ROS2 （ROS Humble）的版本，而 ROS 依赖的版本向下兼容比较糟糕，而目前的 SLAM 算法大部分都是基于 ROS1 的项目，所以建议采用 Docker 的方法来在 Ubuntu 22.04 上部署 ROS1。


##### Docker 官方安装文档：https://docs.docker.com/engine/install/ubuntu/
使用阿里云一键安装：

```
curl -fsSL https://get.docker.com | bash -s docker --mirror Aliyun
```

##### 拉取 Docker + ROS Noetic 的虚拟环境
```
docker pull ros:noetic
```


#### 2. 编译Livox-SDK2
官方地址：https://github.com/Livox-SDK/Livox-SDK2

```
git clone https://github.com/Livox-SDK/Livox-SDK2.git
cd ./Livox-SDK2/
mkdir build
cd build
cmake .. && make
sudo make install
```

#### 3.安装Livox-SKD
官方地址：https://github.com/Livox-SDK/Livox-SDK.git

退出到 home 目录再输入以下命令：

```
git clone https://github.com/Livox-SDK/Livox-SDK.git
cd Livox-SDK
cd build && cmake ..
make
sudo make install
```

如果 cmake 编译报错，可以在 /home/Livox-SDK/sdk_core/src/base 的 io_thread.h 文件夹中加入头文件 

```
#include <memory>
```

#### 4.编译livox_ros_driver2
官方地址：https://github.com/Livox-SDK/livox_ros_driver2

```
git clone https://github.com/Livox-SDK/livox_ros_driver2.git ws_livox/src/livox_ros_driver2
```

记得要克隆 Livox-SDK2 包，这里我们要把安装位置选在 livox_ros_driver2 相同目录下，也就是要在工作空间ws_livox的src目录下打开终端
```
git clone https://github.com/Livox-SDK/Livox-SDK2.git
cd ./Livox-SDK2/
mkdir build
cd build
cmake .. && make -j
sudo make install
```

然后终端进入到/src/livox_ros_driver2目录下

```
source /opt/ros/humble/setup.sh
./build.sh humble
```

血的教训，一定要退出 Anaconda，debug 了很久才发现问题！

```
conda deactivate
```

#### 5.运行Livox ROS Driver 2
```
source ../../install/setup.sh
ros2 launch livox_ros_driver2 [launch file]
```

A rviz launch example for HAP LiDAR would be:
```
ros2 launch livox_ros_driver2 rviz_HAP_launch.py
```

#### 6.安装必须的库
打开github库中的sentry_ros_3d或已经下载到本地的代码打开“rosenvironment.sh"文件

使用mid360和Point lio进行导航部署需安装以下库
```
sudo apt-get install ros-humble-pcl-conversions
sudo apt-get install libeigen3-dev
sudo apt-get install ros-humble-serial
sudo apt-get install ros-humble-move-base-msgs
sudo apt-get install ros-humble-move-base
sudo apt-get install ros-humble-map-server
sudo apt-get install ros-humble-global-planner
sudo apt-get install ros-humble-laser-filters
sudo apt-get install ros-humble-teb-local-planner
sudo apt-get install ros-humble-dwa-local-planner
sudo apt-get install ros-humble-tf2-sensor-msgs
sudo apt-get install ros-humble-octomap-ros #安装octomap
sudo apt-get install ros-humble-octomap-msgs
sudo apt-get install ros-humble-octomap-server
sudo apt-get install ros-humble-octomap-rviz-plugins
sudo apt-get install ros-humble-move-base-flex
```

#### 7.创建用于C板虚拟串口的udev别名
打开代码目录sentry_ros_3d/src/simple_robot/udev输入以下命令
```
sudo sh setup.sh
```

#### 8. 编译构建
colcon是ROS构建工具catkin_make、catkin_make_isolated、catkin_tools和ament_tools的迭代。colcon目标是做一个通用构建工具（universal build tool），能够构建ROS1和ROS2的包，同时也能够构建一些非ROS包。ROS中，catkin_make、catkin_make_isolated、catkin_tools、ament_tools将逐步被colcon取代
按顺序执行以下命令
```
catkin_make -DCATKIN_WHITELIST_PACKAGES="robot_msgs"
catkin_make -DCATKIN_WHITELIST_PACKAGES="livox_ros_driver2"
catkin_make -DCATKIN_WHITELIST_PACKAGES="livox_ros_driver"
catkin_make -DCATKIN_WHITELIST_PACKAGES="fast_lio"
catkin_make -DCATKIN_WHITELIST_PACKAGES=""
```

如果 catkin 报错没有安装依赖，可能不是 catkin 的问题，而是虚拟环境没有配置
```
echo "source /opt/ros/humble/setup.bash" >> ~/.bashrc
source ~/.bashrc
catkin_init_workspace

```


