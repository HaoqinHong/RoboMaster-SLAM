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

如果 cmake 编译报错，可以在 /home/Livox-SDK/sdk_core/src/base 的 io_thread.h 文件夹中加入头文件 #include <memory>


#### 3.编译livox_ros_driver2
官方地址：https://github.com/Livox-SDK/livox_ros_driver2

```
git clone https://github.com/Livox-SDK/livox_ros_driver2.git ws_livox/src/livox_ros_driver2
```

记得要克隆 Livox-SDK2 包，这里我们要把安装位置选在 “livox_ros_driver2“ 相同目录下，也就是要在工作空间ws_livox的src目录下打开终端
```
git clone https://github.com/Livox-SDK/Livox-SDK2.git
```



终端进入到/src/livox_ros_driver2目录下

```
source /opt/ros/humble/setup.sh
./build.sh humble
```
git clone https://github.com/Livox-SDK/Livox-SDK2.git
