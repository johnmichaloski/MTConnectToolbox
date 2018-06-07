
Fanuc LRMate 200iD Robot Operation Instructions
=========================================

Powerup:
--------------------------
1. Turn on power on front of controller (keyed)
2. If auto mode, make sure teach pendant upper left corner knob is **OFF**
3. If in fault- hold deadman switch halfway, Hold [**Shift**], press [**Reset**] key
4. reset to local mode
	`Menu -> [32] Remote/Local/... [F4] Local [Enter]`
5. Start ROS programs
	`[Teach][Select] => scroll down to ROS, number 39 hit [Enter]`
   (starts 2 programs)
6. Cycle start 
   `Green Auto button on front controller panel, press/release, green Light should go on`.

Powerdown:
--------------------------
1. Kill ROS programs - DO TWICE - 2 programs running
   `[FCNT] -> 1 -> [ENTER]`  
   `[FCNT] -> 1 -> [ENTER] ` 

Fault Recovery
--------------
If fanuc controller faulted, you have to manually reset  the joint(s) to a "safe" position:

2. Turn controller box to teach pendant from auto
3. hold deadman switch half-on, [**SHIFT**] Hold, hit [**Reset**]
4. Now move robot - +/- joint key or xyz key
5. Note to increase traversal- feedoverride in green xx% field in upper right corner 

Run ROS Fanuc demo
--------------------------

	roslaunch fanuc_lrmate200id_moveit_config  moveit_planning_execution.launch 
		sim:=false   robot_ip:=129.6.78.111


Run RVIZ roslaunch with Fanuc LRMate 200id 
--------------------------

**FIXME**

	#!/bin/bash
    source .../catkin_ws/devel/setup.bash
    roslaunch nist_fanuc lrmate200id_sim.launch

    sleep 100


Launch file:
--------------------------

    <?xml version="1.0"?>
    <launch>
      
      <arg name="sim" default="true" />
      
      <include file="$(find fanuc_lrmate200id_moveit_config)/launch/moveit_planning_execution.launch">
        <arg name="sim" value="$(arg sim)"/>
      </include>

    </launch>



Fanuc ROS installation tutorial - I
------
Web stop #1:

                http://wiki.ros.org/fanuc?distro=indigo
 
Fanuc ROS installation tutorial - II
------
Web stop #2:

                http://wiki.ros.org/fanuc/Tutorials/hydro/Installation
 
The “Next” link at the bottom takes you to the next in the series.


