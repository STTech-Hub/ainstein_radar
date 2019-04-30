# ainstein_radar:

ROS metapackage containing functionality for interfacing with Ainstein radars.

## ainstein_radar_msgs

ROS message definitions for Ainstein radars.

## ainstein_radar_drivers

ROS drivers (interfaces) and nodes for Ainstein radars.  All development and testing has been done in **Ubuntu 16.04 with ROS Kinetic.***

### Supported radars:

- K79 (tested Feb. 2019)
- K77 (tested Mar. 2019)
- T79 with BSD firmware (tested Mar. 2019)

Both nodes and [nodelets](http://wiki.ros.org/nodelet) are provided.

### Additional resources:

- [radar_sensor_msgs/RadarData](https://github.com/AinsteinAI/radar_sensor_msgs/blob/master/msg/RadarData.msg) to [sensor_msgs/PointCloud2](http://docs.ros.org/melodic/api/sensor_msgs/html/msg/PointCloud2.html) tranformation node and nodelet, for use with eg. open-source point cloud library [PCL](http://pointclouds.org/)
- [radar_sensor_msgs/RadarData](https://github.com/AinsteinAI/radar_sensor_msgs/blob/master/msg/RadarData.msg) to [sensor_msgs/LaserScan](http://docs.ros.org/melodic/api/sensor_msgs/html/msg/LaserScan.html) tranformation node, for use with eg. open-source SLAM package [gmapping](http://wiki.ros.org/gmapping)

## System Requirements and Dependencies

- A means of creating SocketCAN interfaces (eg ```can0```, ```can1```) either natively (on embedded hardware such as the NVIDIA Jetson series) or through the use of an adapter (such as the [PEAK PCAN_USB adapter](https://www.peak-system.com/PCAN-USB.199.0.html?&L=1)
- (Optional) The linux [can-utils](https://github.com/linux-can/can-utils) SocketCAN utilities, installed on Ubuntu with ```sudo apt install can-utils```, for radar configuration and debugging

## ROS Package Dependencies

The following packages can be installed from apt using ```rosdep install --from-paths src --ignore-src --rosdistro=ROSDISTRO -y```, replacing ```ROSDISTRO``` with your ROS distribution name.

- [can_msgs](http://wiki.ros.org/can_msgs?distro=melodic) : A generic canopen implementation for ROS
- [socketcan_bridge](http://wiki.ros.org/can_msgs?distro=melodic) : A generic canopen implementation for ROS

The following packages are not available from [ros.org](ros.org) and must be cloned separately:

- [radar_sensor_msgs](https://github.com/AinsteinAI/radar_sensor_msgs) : Package defining custom radar message types. 

## Package Structure

```
radar_ros_interface/
├── CMakeLists.txt
├── include
│   └── radar_ros_interface
│       ├── config_t79_bsd.h
│       ├── radardata_to_laserscan.h
│       ├── radardata_to_pointcloud.h
│       ├── radar_interface.h
│       ├── radar_interface_k79.h
│       └── radar_interface_t79_bsd.h
├── launch
│   ├── k77_node.launch
│   ├── k79_node.launch
│   ├── k79_nodelet.launch
│   └── t79_node.launch
├── LICENSE
├── nodelet_k79.xml
├── nodelet_radardata_to_pointcloud.xml
├── nodelet_t79_bsd.xml
├── package.xml
├── readme.md
├── scripts
│   ├── k79_gui.py
│   └── t79_bsd_config.sh
└── src
    ├── k79_node.cpp
    ├── k79_nodelet.cpp
    ├── radardata_to_laserscan.cpp
    ├── radardata_to_laserscan_node.cpp
    ├── radardata_to_pointcloud.cpp
    ├── radardata_to_pointcloud_node.cpp
    ├── radardata_to_pointcloud_nodelet.cpp
    ├── radar_interface_k79.cpp
    ├── radar_interface_t79_bsd.cpp
    ├── t79_bsd_node.cpp
    └── t79_bsd_nodelet.cpp
```				

## Supported Radars and Usage

### T79+BSD and/or K77:

The file ```src/t79_bsd_node.cpp``` implements a ROS node for both K77 and T79 using the RadarInterfaceT79BSD interface class.  With the current 4+1 firmware, this node works for both K77 and T79 *by specifying the radar type*.

**Setup:**

To configure the radar type prior to testing, use the provided [t79_bsd_config.sh](https://github.com/AinsteinAI/radar_ros_interface/blob/master/scripts/t79_bsd_config.sh) script which depends on [can-utils](https://github.com/linux-can/can-utils). Using eg. ```candump can0``` is also useful debugging tool to monitor CAN traffic on the SocketCAN interface.

These CAN radars require a [socketcan_bridge](http://wiki.ros.org/socketcan_bridge) node publishing CAN frames to the ```/received_messages``` ROS topic (see the launch file below for an example).  This package can be installed with:

```bash
sudo apt install ros-kinetic-socketcan-bridge
```

**Command line usage:**	

The optional parameters are scoped private, eg host IP is read from /k77_node/radar_type in this case:

```bash
rosrun radar_ros_interface t79_bsd_node [_radar_type:=RADAR_TYPE] [_frame_id:=RADAR_FRAME_ID]
```

where ```RADAR_TYPE``` is to be specified according to the following enum:

```
enum RadarType
{
    KANZA = 0,
    TIPI_79_FL,
    TIPI_79_FR,
    TIPI_79_RL,
    TIPI_79_RR,
    N_RADARS
};
```

For example, K77 is type ```0``` (KANZA), T79 on the front-left corner of a vehicle is type ```1``` (TIPI_79_FL) and so on.  If unspecified, ```radar_type``` defaults to ```0``` and ```frame_id``` defaults to ```map```.

**Example launch file usage (from launch/k77_node.launch):**

```xml
<launch>
  <node name="socketcan_bridge" pkg="socketcan_bridge" type="socketcan_bridge_node"  required="true" >
    <param name="can_device" value="can0" />
  </node>
  <node name="k77_node" pkg="radar_ros_interface" type="t79_bsd_node" required="true" >
    <param name="radar_type" value="0" />
  </node>
</launch>
```

**Example launch file usage (from launch/t79_bsd_node.launch):**

```xml
<launch>
  <node name="socketcan_bridge" pkg="socketcan_bridge" type="socketcan_bridge_node"  required="true" >
    <param name="can_device" value="can0" />
  </node>
  <node name="t79_node" pkg="radar_ros_interface" type="t79_bsd_node" required="true" >
    <param name="radar_type" value="1" />
  </node>
</launch>
```

### K79:

The file k79_node.cpp implements a ROS node using the RadarInterfaceK79 interface class to create a UDP socket bound to the host IP address and port (which must match the radar's configuration), launch a thread to read and publish data to the RadarData message type.

**Setup:**

The python script [k79_gui.py](https://github.com/AinsteinAI/radar_ros_interface/blob/master/scripts/k79_gui.py) can be used to configure the network parameters and flash new firmware. More documentation on this to come.

**Command line usage:**	

The optional parameters are scoped private, eg host IP is read from /k79_node/host_ip in this case:

```bash
rosrun radar_ros_interface k79_node [_host_ip:=HOST_IP_ADDRESS] [_host_port:=HOST_UDP_PORT] [_radar_ip:=RADAR_IP_ADDRESS] [_radar_port:=RADAR_UDP_PORT] [_frame_id:=RADAR_FRAME_ID]
```

If unspecified, ```host_ip``` defaults to ```10.0.0.75```, ```radar_ip``` defaults to ```10.0.0.10```, ```radar_port``` defaults to ```7```, ```host_port``` defaults to ```1024``` and ```frame_id``` defaults to ```map```.

**Example launch file usage (from launch/k79_node.launch):**

```xml
<launch>
  <node name="k79_node" pkg="radar_ros_interface" type="k79_node" output="screen" required="true" >
    <param name="host_ip" value="10.0.0.75" />
    <param name="host_port" value="1024" />
    <param name="radar_ip" value="10.0.0.10" />
    <param name="radar_port" value="7" />
  </node>
</launch>
```

See the ```k79_nodelet.launch``` file for example nodelet usage.
