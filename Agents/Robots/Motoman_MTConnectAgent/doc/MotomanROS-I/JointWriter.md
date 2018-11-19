CJointWriter  
=============
CJointWriter is a class that handles updating the joint value to be displayed by RVIZ, or if a real robot is running, update the robot position. 

    class CJointWriter {
    public:
        CJointWriter(ros::NodeHandle &nh);

        // Position only for now
        bool JointTrajectoryWrite(std::vector<sensor_msgs::JointState>);
        bool JointTrajectoryPositionWrite(sensor_msgs::JointState joint);
        ////////////////////////////////
        ros::Publisher traj_pub;
        std::vector<std::string> jointnames;
        static boost::mutex _writer_mutex;
    };	

CJointWriter constructor requires the ROS node handle, to advertise that it will be publishing  joint values. In order to publish joint values, it must have the names of the joints that it will be updating. Thus, the command geParam with the parameter name controller_joint_names  retrieves a list of the jointnames.  Then, the constructor advertises to ROS that it will be publishing to the topic “joint_path_command”.

    CJointWriter::CJointWriter(ros::NodeHandle &nh)
    {
         nh.getParam("controller_joint_names", jointnames);
         // Trajectory publisher
        traj_pub = nh.advertise<trajectory_msgs::JointTrajectory>("joint_path_command", 1);
    }

The  JointTrajectoryPositionWrite method publishes updated joint values that the ROS system will publish to all listeners (which of interest in our case is RVIZ). It accepts a sensor_msg  JointState structure containing the updated joint values. In theory, the joint_path_command  topic could accept many points to display, however, only 1 point at a time is written to the topic. 

    bool CJointWriter::JointTrajectoryPositionWrite(sensor_msgs::JointState joint) {

        ActionGoal traj_goal;
        trajectory_msgs::JointTrajectory traj;
        std::vector<trajectory_msgs::JointTrajectoryPoint> points;
        size_t n_joints=joint.position.size(); 
        
        // Where we are going
        trajectory_msgs::JointTrajectoryPoint point;
        point.positions.resize(n_joints);
        point.positions=joint.position;
        point.velocities.resize(n_joints, 0.0);
        point.accelerations.resize(n_joints, 0.0);
        
        traj.joint_names = jointnames;
        traj.points.resize(1, point);
        // Send trajectory
        traj.header.stamp = ros::Time(0); // Start immediately
        traj_pub.publish(traj);
        return true;
    }

The  JointTrajectoryWrite  method to updates a vector of joint values. (Unclear if this is useful.) It accepts a std vector of sensor_msg  JointState values,  and will update and then write each value (containing a vector of joint positions). 

    bool CJointWriter::JointTrajectoryWrite(std::vector<sensor_msgs::JointState> joints ) {
        
       for (size_t i = 0; i < joints.size(); i++) {
            JointTrajectoryPositionWrite(joints[i]);
        }
        return true;
    }

The package.xml manifest contains all the following  Moveit! entries, although it is unclear which ones are necessary.

        <build_depend>moveit_core</build_depend>
        <build_depend>moveit_ros_planning_interface</build_depend>
        <build_depend>moveit_ros_move_group</build_depend>
        <build_depend>moveit_ros_planning</build_depend>
        <build_depend>moveit_ros_manipulation</build_depend>

        <run_depend>moveit_core</run_depend>
        run_depend>moveit_ros_planning_interface</run_depend>
        <run_depend>moveit_ros_move_group</run_depend>
        <run_depend>moveit_ros_planning</run_depend>
        <run_depend>moveit_ros_manipulation</run_depend>	

Likewise the CMakeLists.txt contains the following moveit entries. Of note, the trajectory_msgs 

    ## Find catkin macros and libraries
    ## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
    ## is used, also find other catkin packages
    find_package(catkin REQUIRED COMPONENTS
      moveit_core
      roscpp
      cmake_modules
      trajectory_msgs
      sensor_msgs
        moveit_ros_planning_interface
        moveit_ros_move_group
        moveit_ros_planning
        moveit_ros_manipulation
    )

    catkin_package(
      INCLUDE_DIRS
        include
      LIBRARIES
      CATKIN_DEPENDS
        roscpp
        moveit_core
        sensor_msgs
        moveit_ros_planning_interface
        moveit_ros_move_group
        moveit_ros_planning
        moveit_ros_manipulation
      DEPENDS
        Boost
        Eigen



