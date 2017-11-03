CJointReader 
=============== 
CJointReader  is a reader which listens to  sensor_msgs/JointState messages. The sensor_msgs/JointState describe the latest robot joint readers, either simulated or real joint values.   The CJointReader   class handles the subscription to the JointState message, the updating of the  latest joint readings, and any responding to any queries for the latest joint states.
 
    class CJointReader {
    public:
        CJointReader(ros::NodeHandle &nh);
        sensor_msgs::JointState GetCurrentReadings();
        void Start();
        std::vector<double> GetJointValues();
        ////////////////////////////////////////////////////////////////////
        void callback(const sensor_msgs::JointState::ConstPtr& msg);
        ros::NodeHandle &_nh;
        sensor_msgs::JointState _latestreading;
        sensor_msgs::JointState _lastreading;
        ros::Subscriber sub;
        // Not sure why you'd want to keep ring of last n readings ...
        std::vector<sensor_msgs::JointState> _readings;
        static boost::mutex _reader_mutex;
    };

CJointReader   class will listens for changes to JointState messages  by “subscribing” to the joint_states message and providing callback routine  (i.e., CJointReader::callback ) to handle updates. This subscription and callback is initiated with the “Start” method. It depends on the constructor providing a ROS node handle, so it can call the central ROS services to subscribe to the message and receive Joint reading updates. 

    void CJointReader::Start() {
        sub = _nh.subscribe("joint_states", 10, &CJointReader::callback, this);
    }

Per ROS subscribe interaction, there is a callback to receive the joint messages and record the latest position.
    void CJointReader::callback(const sensor_msgs::JointState::ConstPtr& msg) {
         boost::unique_lock<boost::mutex> scoped_lock (_reader_mutex);
        _lastreading=_latestreading;
        _latestreading.position.clear();
        _latestreading.position.insert(_latestreading.position.begin(), msg->position.begin(),
                msg->position.end());
        _latestreading.velocity = msg->velocity;
        _latestreading.effort = msg->effort;
    }	
To get the latest Joints value, a thread who has pointer to the joint reader, calls the method GetCurrentReadings() to retrieve the latest values.

    sensor_msgs::JointState CJointReader::GetCurrentReadings() {
        boost::unique_lock<boost::mutex> scoped_lock(_reader_mutex);
        return _latestreading;
    } 

In general, one instance of a joint reader is shared throughout a program. Also, use of the boost shared pointer construct will handle reference counting and deletion of the heap object , so one declares one instance of a  boost::shared_ptr<CJointReader>   and passes it to any listeners.

    boost::shared_ptr<CJointReader>jointReader = boost::shared_ptr<CJointReader>(new CJointReader(nh));

That’s all there is to joint sensor reading. ROS only seems to support joint reading, so if you wanted the latest robot pose (position and orientation) you would need to compute it with the forward kinematics.

In the package.xml manifest file, which lists all the dependencies for the ROS package (build, install, run, etc).  CJointReader does not really depend on Moveit!, (I think). However,  its does depend on reading sensor_msgs messages that contain the  “joint_states"
 so this the sensor_msg package is a build and runtime dependency.  In the package.xml manifest, you will need to add:


    <build_depend>sensor_msgs</build_depend>
    …

    <run_depend>sensor_msgs</run_depend>
    And in the CMakeLists.txt file, which describes how to build the package, you will need to add:
    find_package(catkin REQUIRED COMPONENTS
      moveit_core
      roscpp
      cmake_modules
      sensor_msgs
    )

In my package, the joint readings are read once cyclically and updated.

