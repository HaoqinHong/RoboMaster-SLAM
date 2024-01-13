### 基于 Point-Lio 和 Mid360 的哨兵导航方案

#### 1. 编译Livox-SDK2
官方地址：https://github.com/Livox-SDK/Livox-SDK2

```
git clone https://github.com/Livox-SDK/Livox-SDK2.git
cd ./Livox-SDK2/
mkdir build
cd build
cmake .. && make
sudo make install
```

#### 2.安装Livox-SKD
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

#### 3.编译livox_ros_driver2
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

#### 4.运行Livox ROS Driver 2
```
source ../../install/setup.sh
ros2 launch livox_ros_driver2 [launch file]
```

A rviz launch example for HAP LiDAR would be:
```
ros2 launch livox_ros_driver2 rviz_HAP_launch.py
```

#### 5.安装必须的库
打开github库中的sentry_ros_3d或已经下载到本地的代码打开“rosenvironment.sh"文件
使用mid360和Point lio进行导航部署需安装以下库
```
sudo apt-get install ros-noetic-pcl-conversions
sudo apt-get install libeigen3-dev
sudo apt-get install ros-noetic-serial
sudo apt-get install ros-noetic-move-base-msgs
sudo apt-get install ros-noetic-move-base
sudo apt-get install ros-noetic-map-server
sudo apt-get install ros-noetic-global-planner
sudo apt-get install ros-noetic-laser-filters
sudo apt-get install ros-noetic-teb-local-planner
sudo apt-get install ros-noetic-dwa-local-planner
sudo apt-get install ros-noetic-tf2-sensor-msgs
sudo apt-get install ros-noetic-octomap-ros #安装octomap
sudo apt-get install ros-noetic-octomap-msgs
sudo apt-get install ros-noetic-octomap-server
sudo apt-get install ros-noetic-octomap-rviz-plugins
sudo apt-get install ros-noetic-move-base-flex
```



