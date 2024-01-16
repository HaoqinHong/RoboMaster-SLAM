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
sudo docker pull osrf/ros:noetic-desktop-full
```

查看是否安装成功
```
sudo docker images
```

##### 新建 ROS Noetic 的 Docker 容器

由于第一步安装的ROS是没有实际项目所需要的环境依赖，所以我们需要编写一个Dockerfile文件，用这个文件重新创建一个带依赖的ROS镜像。 

该文件随便放一个文件夹：
```
touch Dockerfile
```

dockerfile文件内容如下：
```
FROM osrf/ros:noetic-desktop-full
 
# 如果机器带显卡，可以放开这里，我没用到，所以注释了
# ENV NVIDIA_VISIBLE_DEVICES \
# ${NVIDIA_VISIBLE_DEVICES:-all}
 
# ENV NVIDIA_DRIVER_CAPABILITIES \
# ${NVIDIA_DRIVER_CAPABILITIES:+$NVIDIA_DRIVER_CAPABILITIES,}graphics
 
# 自己需要什么依赖在这里加即可
RUN apt-get update && \
apt-get install -y \
libeigen3-dev \
libgoogle-glog-dev \
vim
```

在该文件路径下，打开终端，执行命令：（其中 ros1 为镜像的名称，可自定义）
```
sudo docker build -f Dockerfile -t ros1 .
```

执行完后用命令查看是否成功创建 ros1 这个镜像，有这个镜像则成功
```
sudo docker images
```

执行命令利用上一步创建的镜像来创建容器：（其中 -v$(pwd):/data参数是指将当前目录挂载到ROS容器根目录data文件夹下，可以用来和宿主机进行文件交换）
```
sudo docker run --privileged --name my_ros_container -it -v /dev:/dev -v /tmp/.X11-unix:/tmp/.X11-unix -v /home/phoenixkite-nuc/桌面:/ws_ros -e DISPLAY=$DISPLAY --network host ros1 bash
```

##### 新开一个终端，进入容器然后开始操作
查看容器ID及状态：
```
sudo docker ps -a
```

```
sudo docker exec -it [CONTAINER ID] bash
source ros_entrypoint.sh
roscore &
rviz
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

如果 cmake 编译报错，可以在 /home/Livox-SDK/sdk_core/src/base 的 thread_base.h 文件夹中加入头文件 

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
source /opt/ros/noetic/setup.sh
./build.sh ROS1
```

血的教训，一定要退出 Anaconda，debug 了很久才发现问题！

```
conda deactivate
```

#### 5.运行Livox ROS Driver 2
```
source ../../devel/setup.sh
roslaunch livox_ros_driver2 [launch file]
```

A rviz launch example for HAP LiDAR would be:
```
roslaunch livox_ros_driver2 rviz_HAP.launch
```

#### 6.安装必须的库
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

#### 7.创建用于C板虚拟串口的udev别名
打开代码目录sentry_ros_3d/src/simple_robot/udev输入以下命令
```
sudo sh setup.sh
```

#### 8. 编译构建
colcon是ROS构建工具catkin_make、catkin_make_isolated、catkin_tools和ament_tools的迭代。colcon目标是做一个通用构建工具（universal build tool），能够构建ROS1和ROS2的包，同时也能够构建一些非ROS包。ROS中，catkin_make、catkin_make_isolated、catkin_tools、ament_tools将逐步被colcon取代

在 sentry_ros_3d 打开终端按顺序执行以下命令
```
catkin_make -DCATKIN_WHITELIST_PACKAGES="robot_msgs"
catkin_make -DCATKIN_WHITELIST_PACKAGES="livox_ros_driver2"
catkin_make -DCATKIN_WHITELIST_PACKAGES="livox_ros_driver"
catkin_make -DCATKIN_WHITELIST_PACKAGES="fast_lio"
catkin_make -DCATKIN_WHITELIST_PACKAGES=""
```

如果报错
```
CMake Error at CMakeLists.txt:1:
  Parse error.  Expected a command name, got unquoted argument with text
  "/opt/ros/noetic/share/catkin/cmake/toplevel.cmake".
```

进入 src 中删除 CMakeList.txt,然后打开终端执行：

```
catkin_init_workspace

```


