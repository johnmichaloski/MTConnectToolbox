
# README 
----

Wednesday, October 25, 2017
This document presents a brief background on the mechanics of an MTConnect agent implementation for Universal Robots.  This document concerns itself with how an embedded Adapter operates to communicate with a UR robot. One or more embedded UR adapters read a configuration file, and then read real-time data from a UR robot using the established UR TCP protocol.   It includes a brief background on MTConnect, UR robot background, system architecture, communication scheme, and installation notes. This document also describes the data gathering for a Universal Robot (UR) using a socket interface. Multiple adapters to UR robots are possible, each contained within one MTConnect Agent.
This document assumes the reader is familiar with MTConnect operation, and for deeper explanation of MTConnect, please refer to MTConnect URL: http://www.mtconnect.org/ for more information. 
# Background 

## MTConnect Overview
MTConnect is an evolving standard developed to facilitate the exchange of data on the manufacturing floor. The MTConnect open specification provides for cost effective data acquisition on the manufacturing floor for machine tools and related devices.  MTConnect is based upon prevalent Web technology including XML and HTTP.   Figure 1 shows the MT Connect architecture. An "MTConnect Device" is a piece of equipment – in this case a UR robot, which (optionally) includes an MTConnect Adapter so that we can get data from it. The "Agent" is a process that acts as a "bridge" between a device and a factory "Client Application".  To learn more about MTConnect visit: http://www.mtconnect.org/ 
Figure 1 shows a typical MTConnect UR Robot system architecture (with one or more UR robot devices). Communication between a Windows PCs and the UR robot is assumed, communicating over Ethernet to communicate the UR robot status.


![Figure1](./images/UR_AgentReadme_image1.gif)


<p align="center">
_Figure 1 MTConnect NIKON CMM Logging Agent system architecture_
</p>
This document describes the communication and data gathering software technology used to communicate with a UR robot to provide data for an MTConnect Agent to make available to clients. Multiple adapters to UR robots software are possible, each contained within one MTConnect Agent.
Universal Robots is a Danish manufacturer of smaller flexible industrial robot arms whose primary application are as collaborative robots. Collaborative robots differ from traditional robots by being able to work side-by-side with human workers as the robots are deemed inherently safe, and thus do not require a fence to safeguard workers from dangerous robot behavior. A collaborative robot differs from a traditional industrial robot due to the fact that a collaborative robot is designed to work alongside human employees, while an industrial robot replaces a worker (as it unsafe for an industrial robot to even be near a human). In theory, a collaborative robot can assist employees with work that may be too dangerous, strenuous, or tedious for them to accomplish on their own, creating a safer, more efficient workplace. By contrast, industrial robots are used to automate the manufacturing process almost entirely without human help on the manufacturing floor. 
UR robots also claim to be more easy to teach program than industrial robots because they can  be programmed by moving the arm to the desired teach position. Industrial robots cannot be so easily reprogrammed since they are dangerous and they require a joystick to move the robot to the desired position. In either case, it would be preferable if the robot could use sensing and background engineering to determine the desired goal position. Many robots use CAD models of the factory to understand the manufacturing environment and goal positions, but this is a non-trivial integration exercise, especially in a one-off production environment.
Figure 1 show the three different sizes of Universal Robots – UR3, UR5 and UR10, which are named for their payload weight in kilograms and vary in size, reach, weight and footprint. The UR robot's somewhat dated basic specification [1]:
 - Max load capacity of 11 lbs for the UR5 (UR5 AE3) and 22 lbs for the UR10
 - Max speed of 1 meter per sec under full load
 - Simple teaching style programming with the ability to create complex logic in a point and click environment
 - Full scripting language for complex programming required
 - I/O handshaking and fieldbus communications
 - Torsion sensing technology for use in collaborative situations
 - Compatible with industrial vision systems such as Cognex
 - Repeatability of +/- 0.004"
 - Environmental Rating of IP54
 - Weight without Pedestal: 40.6 lbs for the UR5 (UR5 AE3) and 63.7 for the UR10
 - Calculated Life of 36000 Hours of Operation at rated loads


![Figure2](./images/UR_AgentReadme_image2.gif)


<p align="center">
_Figure 2 Universal Robots - UR3, UR5, and UR10_
</p>

Of interest in this document is reporting status of UR robots. At a low level of control, UR robots have a controller called "URControl", which is the low-level robot controller running on the Mini-ITX PC in the controller cabinet. When the PC boots up URControl starts up as a daemon (like a service) and PolyScope User Interface connects as a client using a local TCP/IP connection and listens for socket connections. Monitoring status of a UR robot is done by writing a client application, in this case the MTConnect Agent, and connecting to URControl using a TCP/IP socket, with these parameters:
 - Hostname IP 
 - port: 30002

The UR MTConnect Agent contains a back end adapter that communicates with the UR controller via the TCP/IP socket. In general, UR supplies three TCP/IP interfaces: primary, secondary and real-time. The secondary communication channel was used to retrieve robot status at 15Hz. With the MTConnect agent running, a connection to each UR robot is established to receive status communication from the robot. Then, if you bring up a browser and enter the IP plus port 5000 and the command current, e.g., http://xxx.xxx.xxx.xxx:5000/current the agent will return the current state of the data items in the UR robot. Shown below is a formatted output table from a VB script that extracts the MTConnect agent XML from one UR robot .

![Figure3](./images/UR_AgentReadme_image3.gif)


<p align="center">
_Figure 3 Web-browser Display Showing Table of MTConnect Agent Data Items_
</p>
The UR TCP/IP communication streams the status of the robot over the socket connection. The MTConnect Agent back-end adapter reads the status, and decodes these UR robot message into a series of C++ structures. These UR status interfaces include:
struct ur_additional_info;
struct ur_cartesian_info;
struct ur_configuration_data;
struct ur_force_mode_data;
struct ur_joint_data ;
struct ur_joint_limits;
struct ur_joint_maxs;
struct ur_key_message;
struct ur_label_message;
struct ur_masterboard_data;
struct ur_message_header;
struct ur_popup_message;
struct ur_request_value_message;
struct ur_robot_comm_message;
struct ur_robot_message;
struct ur_robot_mode_data ;
struct ur_runtime_exception_message;
struct ur_security_message;
struct ur_text_message;
struct ur_tool_data;
struct ur_var_message;
struct ur_version_message;

Importantly the interface robot_mode_data, joint_data, cartesian_data, version_message, masterboard_data, and configuration_data are the status data buffers that are used by the MTConnect back-end Adapter. Within these data buffers the UR robot status is collected and then interpreted into MTConnect standard data items. Although many MTConnect items are CNC –centric and aren't intuitively reflected within the UR robot status buffers (e.g., MANUAL versus AUTOMATIC mode), it is possible to convert the UR robot status into an MTConnect device status representation.  In fact, the UR robot has more than sufficient status information, so that determining the data items for the MTConnect Agent to monitor when displaying a UR robot state can depend on the particular client application. For example, an OEE client application would require different data items than a client application to determine the prognostic and health of the robot.
<TABLE>
<TR>
<TD>MTConnect Data Item<BR></TD>
<TD>UR robot interface item<BR></TD>
</TR>
<TR>
<TD>power<BR><BR></TD>
<TD>ON if ur_robot_mode_data.isPowerOnRobot<BR><BR></TD>
</TR>
<TR>
<TD>mode<BR><BR></TD>
<TD>Became robotmode and controlmode tags<BR>ur_robot_mode_data.robotMode<BR>ur_robot_mode_data.controlMode<BR><BR></TD>
</TR>
<TR>
<TD>execution<BR></TD>
<TD>ACTIVE:ur_robot_mode_data.isProgramRunning INTERRUPTED:ur_robot_mode_data.isProgramPaused READY: state not above<BR></TD>
</TR>
<TR>
<TD>axes status<BR></TD>
<TD>ur_joint_data gives position, velocity and other low-level joint parameters as well as joint_mode<BR></TD>
</TR>
<TR>
<TD>estop<BR></TD>
<TD>TRIGGERED: if ur_robot_mode_data.isEmergencyStopped<BR>ARMED: otherwise<BR></TD>
</TR>
<TR>
<TD>fovr<BR></TD>
<TD>ur_robot_mode_data.targetSpeedFraction<BR></TD>
</TR>
<TR>
<TD>pose or PATH_POSITION<BR></TD>
<TD>Combination of r_cartesian_info x,y,z,rx,ry,rz values.<BR></TD>
</TR>
<TR>
<TD>message<BR></TD>
<TD>Unclear, many text messages returnd by UR<BR></TD>
</TR>
<TR>
<TD><BR></TD>
<TD><BR></TD>
</TR>
</TABLE>

The match between a CNC-centric MTConnect and a robot device is not perfect.  The description of axes (joints in robot terminology) is one apparent difference. The natural match, a  "rotary" axis (such as a CNC spindle), is considered a continuous axis in typical robot terminology. However, the concept of a rotary MTConnect axis was used, but was treated as a robot rotary joint (with units in radians not RPM). This was among some of the differences in coalescing the two application realm terminology, but did not cause any major programming issues, but may cause a misunderstanding of output.
Enterprise integration of UR factory robots using MTConnect would be beneficial for OEE, simulation, downtime and repair response, prognostics and health management, among other client applications. One aspect of robot's utility that is useful is a complete model of the robot. For example, the popular Robot Operating System (ROS) uses the Unified Robot Description Format (URDF) as a general-purpose robot model, which includes a wide range of robot specification information. 
Fortunately, MTConnect has the capability to seamlessly pass this URDF information to clients.  Besides the device data items, MTConnect standard addresses other auxiliary manufacturing equipment that are not a component of a machine [2]. To allow generating and managing such auxiliary data models at runtime, MTConnect assets were introduced. Examples of MTConnect assets are objects like cutting tools, workholding systems, parts and fixtures. MTConnect assets offer a good framework for future expansions so that the URDF robot specification information can be channeled through MTConnect agent as an asset.
# UR Robot URDF Information from MTConnect Agent
ROS uses the Unified Robot Description Format (URDF) as a general-purpose robot model. The URDF specification covers the kinematic and dynamic description of the robot, the visual representation of the robot, and the collision model of the robot. URDF assumes the robot consists of rigid links connected by joints; flexible elements are not supported. URDF can be used to model a kinematic chain or kinematic tree robot. Of note,  since URDF is an XML specification, URDF is limited to robots that exhibit only tree structures representations, ruling out all parallel robots like Stewart Platforms. 
Most Industrial Robots are kinematic chain robots and so can be modeled as a serial set of links and joints. Multi-arm robots can also be modeled in URDF as a kinematic tree of links connected to multiple joints, such as a headed robot with two arms. URDF is an XML specification to describe a robot. Because it is an XML specification, the information is modeled as a tree with elements (e.g., "<Name>'') with subelement branches under elements. Each robot is modeled in ROS by an URDF file, which contains 
**Link Information** Link elements describe a rigid body with an geometry, inertia, visual features, obstacle bounds and other robot properties. The visual element within the link element contains the specification for display of the link in visualization tool (either a mesh or a geometry element). The visual also contains the material element which sets the color of the link.

**Joint Information** Joint elements describe the kinematics and dynamics of the joint and also specify the safety limits of the joint. Joint elements contain elements for parent link, child link, axis of rotation, calibration, dynamics, limit, and safety controller information. Joints are also able to mimic other joints. URDF assumes each joint is either a revolute, prismatic, fixed, or continuous type joint which depending on the type can have position, velocity and effort limits specified.

The URDF is conveyed to MTConnect clients as "Assets". If you on the host machine of the UR MTConnect agent and type in the URL http://127.0.0.1:5000/assets, you will get a XML description with the URDF XML embedded inside the MTConnect XML. Below is a screen snapshot of  the assets query of the MTConnect agent for the asset named UR5.

![Figure4](./images/UR_AgentReadme_image4.gif)


<p align="center">
_Figure 4 URDF for UR5 Robot as MTConnect Asset_
</p>
In order to convey the asset you need to have an asset declared in the MTConnect devices data item specification. Thus the MTConnect probe must contain a data item to reflect the existence of the URDF asset.  

	<DataItem id="UR5_urdf_asset" type="ASSET_CHANGED" category="EVENT"/>

For the UR MTconnect agent to be reported via a query, C++ code transfered a URDF file that was read into a string and then passed into the core MTConnect agent using the addAsset function call. To alleviate XML confusion, the URDF header  <?xml version="1.0" ?> was removed, or a web browser would hang at the returned XML. To use  addAsset, the  device,  the id of the asset data item (UR5_urdf_asset), the textual body of the asset and the type (Robot) of the asset must be provided, as shown below:

	void AdapterT::addUrdfAsset (std::string urdfstr)
	{
	    std::string aId = this->mDevice + "_urdf_asset";
	
	    // remove the <?xml version="1.0" ?> as it messes up web browsers
	    size_t n = urdfstr.find("?>");
	
	    if ( n != std::string::npos )
	    {
	        urdfstr = urdfstr.substr(n + 2);
	    }
	
	    // <DataItem id="m1_urdf_asset" type="ASSET_CHANGED" category="EVENT"/>
	    std::string aBody = urdfstr;
	    std::string type  = "Robot";
	
	    ::Device *device = mAgentconfig->getAgent( )->getDeviceByName(mDevice);
	
	    mAgentconfig->getAgent( )->addAsset(device, aId, aBody, type);
	}

Once the asset has been registered with the agent, the MTConnect agent status for the UR5_urdf_asset  data item reflects that data is available.
# Version Information from MTConnect Agent
The version information of the various MTConnect components is available through web browser access. It is uses simple XSLT formatting of the XML. The open source core MTConnect agent is downloaded from github and "frozen". The 1.2 version is used but is rebuilt when the entire agent is compiled.  

	MTConnect Core Agent Version 1.2.0.0 - built on Wed Oct 18 17:37:49 2017
It is not perfect but the output shown below gives an indication of the software involved in the MTConnect Agent operation. It can be modified to include other version information, but requires a recompilation at this time.
Thus, to get the version information in a web page, one can log onto the host PC of the MTConnect Agent, start a web browser, and type in this URL:

	http://127.0.0.1:5000/version
If the UR MTConnect Agent is running you should receive a formatted Web Page as follows:

![Figure5](./images/UR_AgentReadme_image5.gif)


<p align="center">
_Figure 5 UR MTConnect Agent Version Table_
</p>
# Configuration
The UR MTConnect Agent uses the open-source MTconnect agent version 1.2 as the backbone to read http requests, generate XML responses, and update the underlying data.  There is a back-end communication and interpretation adapter addition to handle UR status information. The adapter can handle multiple device log file and update the core MTConnect agent. 
The configuration is done at installation time during the wizard screens.  The user must supply pairs of device names, model of UR robots, URDF name file, and the version the UR robot communication interface supports.

The user can modify the config.ini file in the C:/Program Files/MTConnect/UR_MTConnectAgent folder.  However, the devices.xml and the agent.cfg files necessary for the core MTConnect agent are generated at installation time, and thus a reinstallation would be required to modify these files. 
In the config.ini file, you can manually change GLOBAL variables ServiceName, Agent http port, ResetAtMidnight flag, and list of MTConnect devices (which is specified as a comma separated list (CSV)). These changes will take if you stop/restart the Agent service or reboot the machine.

	{GLOBALS]
	Config=UPDATED
	Version=3.2
	ResetAtMidnight=false
	Debug=0
	MTConnectDevice=UR5
	ServiceName=UR_Agent
	logging_level=FATAL
	QueryServer=10000
	ServerRate=5000
	HttpPort=5001
	
	[UR5]
	IP=129.6.33.123
	jointnames=shoulder_pan_joint,shoulder_lift_joint,elbow_joint,wrist_1_joint,wrist_2_joint,wrist_3_joint
	Version=3.4
	Model=UR5
	Urdf=Ur5.urdf 
Thus, the agent it reads the config.ini file for list of "MTConnectDevice"s under the Globals section. The configuration file also sets the Global flags: QueryServer, ServerRate, Debug, AgentPort(default 5000), and ResetAtMidnite.
For each MTConnectDevice lin the list, a section must be created under which the variables, IP, jointnames, Version, Model and Urdf are assigned.  The IP variable tells the agent the IP of the UR PC. The jointnames gives the names of the joints one which to use. The model tells the agent the robot type, and at this time is not used. And finally the variable Urdf, tells the agent the name of the urdf file to add as an MTConnect asset.
# UR Simulator
UR supplies  simulators for all UR robots that run on a Linux box. The simulator is packages as  'ursim-3.4.4-97.tar.gz ont the Universal Robotics' web site. The web site https://www.universal-robots.com/download/?option=26264 contains the different simulator versions matching different UR interfaces and further details on installing and running the simulator are given. Briefly, the UR web site explains how to install the Universal Robots software (GUI and controller) on Ubuntu Linux version 10.04.  You will need sudo privileges to install the simulation package.  
SHOW STARTING INSTRUCTIONS AND SCREEN CAPTURE
You can verity operations on the Linux laptop, that the UR simulator is working by using telnet to the local machine to port 30002 and seeing a binary stream of data. You should check if there is a firewall blocking access to port 30001, 30002 and 30003.
Interestingly, the UR web site has simulations for all the interface versions available corresponding to their actual robots.

## Struct Decoder
The struct decoder performs conversions between network byte arrays and C++ structs. This intended for use in handling binary data from network connections. First, all the structure variables are declared. Then, preprocessor macros are used to define the size and  order of the byte stream associated with this C++ struct.  It relies on sizeof to determine the size of the struct variable, and uses a printf specifier to format the variable for output. It assumes the struct is packed with no padding between variable elements. However, C++ compiler methods can be used to ensure that the variables are slotted with their native size and no padding is performed. (FIXME: show example.)
The struct_decoder struct is useful for reading/writing network byte streams. It uses the preprocessor definition to define a property table describing each  entry so they can easily be read/write/print including byte swapping if required. Uses the Curiously recurring template pattern to handle accessing the outermost property

<TABLE>
<TR>
<TD>Format<BR></TD>
<TD>C Type<BR></TD>
<TD>Standard size<BR>bytes<BR></TD>
<TD>Notes<BR></TD>
</TR>
<TR>
<TD>%c<BR></TD>
<TD>Signed char<BR></TD>
<TD>1<BR></TD>
<TD> <BR></TD>
</TR>
<TR>
<TD>%c<BR></TD>
<TD>unsigned char<BR></TD>
<TD>1<BR></TD>
<TD>(3)<BR></TD>
</TR>
<TR>
<TD>%uc<BR></TD>
<TD>unsigned char<BR></TD>
<TD>1<BR></TD>
<TD>(3)<BR></TD>
</TR>
<TR>
<TD>%c<BR></TD>
<TD>bool<BR></TD>
<TD>1<BR></TD>
<TD>(1)<BR></TD>
</TR>
<TR>
<TD>%u<BR></TD>
<TD>short<BR></TD>
<TD>2<BR></TD>
<TD>(3)<BR></TD>
</TR>
<TR>
<TD>%hu<BR></TD>
<TD>unsigned short<BR></TD>
<TD>2<BR></TD>
<TD>(3)<BR></TD>
</TR>
<TR>
<TD>%d<BR></TD>
<TD>int<BR></TD>
<TD>4<BR></TD>
<TD>(3)<BR></TD>
</TR>
<TR>
<TD>%ud<BR></TD>
<TD>unsigned int<BR></TD>
<TD>4<BR></TD>
<TD><BR></TD>
</TR>
<TR>
<TD>%ld<BR></TD>
<TD>long<BR></TD>
<TD>4<BR></TD>
<TD><BR></TD>
</TR>
<TR>
<TD>%uld<BR></TD>
<TD>unsigned long<BR></TD>
<TD>4<BR></TD>
<TD><BR></TD>
</TR>
<TR>
<TD>%lld<BR></TD>
<TD>long long<BR></TD>
<TD>8<BR></TD>
<TD><BR></TD>
</TR>
<TR>
<TD>%ulld<BR></TD>
<TD>unsigned long long<BR></TD>
<TD>8<BR></TD>
<TD><BR></TD>
</TR>
<TR>
<TD>%f<BR></TD>
<TD>float<BR></TD>
<TD>4<BR></TD>
<TD><BR></TD>
</TR>
<TR>
<TD>%lf<BR></TD>
<TD>double<BR></TD>
<TD>8<BR></TD>
<TD><BR></TD>
</TR>
<TR>
<TD>%Lf<BR></TD>
<TD>long double<BR></TD>
<TD>16<BR></TD>
<TD><BR></TD>
</TR>
<TR>
<TD>%s<BR></TD>
<TD>char[]<BR></TD>
<TD> <BR></TD>
<TD> <BR></TD>
</TR>
</TABLE>
Notes:


# UR Version of Interfaces Table Comparison
The UR communication interface has several incarnations for different versions. This can be confusing. There is a breakdown of each interface in Excel Spreadsheet format supplied by Universal Robots. Instad of scanning across worksheets, the following table attempts to collate each INPUT interface into a table with changes noted between versions.
Foremost is the robot mode interface which supplies the current operating state of the robot:
<TABLE>
<TR>
<TD>Interface<BR></TD>
<TD>1.6<BR></TD>
<TD>1.7<BR></TD>
<TD>1.8<BR></TD>
<TD>3.0<BR></TD>
<TD>3.1<BR></TD>
<TD>3.2<BR></TD>
<TD>3.3<BR></TD>
<TD>3.4<BR></TD>
</TR>
<TR>
<TD>Robot<BR>Mode<BR>Data<BR></TD>
<TD>int packageSize<BR>unsigned char packageType <BR>uint64_t timestamp<BR>bool isRobotConnected<BR>bool isRealRobotEnabled<BR>bool isPowerOnRobot<BR>bool isEmergencyStopped<BR>bool isSecurityStopped<BR>bool isProgramRunning<BR>bool isProgramPaused<BR>unsigned char robotMode<BR>double speedFraction<BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
<TD>int packageSize<BR>unsigned char packageType <BR>uint64_t timestamp<BR>bool isRobotConnected<BR>bool isRealRobotEnabled<BR>bool isPowerOnRobot<BR>bool isEmergencyStopped<BR>bool isProtectiveStopped<BR>bool isProgramRunning<BR>bool isProgramPaused<BR>unsigned char robotMode<BR>unsigned char controlMode<BR>double targetSpeedFraction<BR>double speedScaling<BR></TD>
<TD>ibid<BR></TD>
<TD>int packageSize<BR>unsigned char packageType <BR>uint64_t timestamp<BR>bool isRobotConnected<BR>bool isRealRobotEnabled<BR>bool isPowerOnRobot<BR>bool isEmergencyStopped<BR>bool isProtectiveStopped<BR>bool isProgramRunning<BR>bool isProgramPaused<BR>unsigned char robotMode<BR>unsigned char controlMode<BR>double targetSpeedFraction<BR>double speedScaling<BR>double targetSpeedFractionLimit<BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
</TR>
</TABLE>


<TABLE>
<TR>
<TD>Interface<BR></TD>
<TD>1.6<BR></TD>
<TD>1.7<BR></TD>
<TD>1.8<BR></TD>
<TD>3.0<BR></TD>
<TD>3.1<BR></TD>
<TD>3.2<BR></TD>
<TD>3.3<BR></TD>
<TD>3.4<BR></TD>
</TR>
<TR>
<TD>Joint <BR>data<BR></TD>
<TD>double q_actual<BR>double q_target<BR>double qd_actual<BR>float I_actual<BR>float V_actual<BR>float T_motor<BR>float T_micro<BR>unsigned char jointMode<BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
</TR>
</TABLE>

<TABLE>
<TR>
<TD>Interface<BR></TD>
<TD>1.6<BR></TD>
<TD>1.7<BR></TD>
<TD>1.8<BR></TD>
<TD>3.0<BR></TD>
<TD>3.1<BR></TD>
<TD>3.2<BR></TD>
<TD>3.3<BR></TD>
<TD>3.4<BR></TD>
</TR>
<TR>
<TD>Cartesian<BR>Info<BR></TD>
<TD>double X<BR>double Y<BR>double Z<BR>double Rx<BR>double Ry<BR>double Rz<BR></TD>
<TD><BR></TD>
<TD><BR></TD>
<TD><BR></TD>
<TD>double X <BR>double Y <BR>double Z <BR>double Rx <BR>double Ry <BR>double Rz <BR>double TCPOffsetX <BR>double TCPOffsetY <BR>double TCPOffsetZ <BR>double TCPOffsetRx <BR>double TCPOffsetRy <BR>double TCPOffsetRz <BR></TD>
<TD><BR></TD>
<TD><BR></TD>
<TD><BR></TD>
</TR>
</TABLE>


<TABLE>
<TR>
<TD>Interface<BR></TD>
<TD>1.6<BR></TD>
<TD>1.7<BR></TD>
<TD>1.8<BR></TD>
<TD>3.0<BR></TD>
<TD>3.1<BR></TD>
<TD>3.2<BR></TD>
<TD>3.3<BR></TD>
<TD>3.4<BR></TD>
</TR>
<TR>
<TD>Version Message<BR></TD>
<TD>uint64_t timestamp <BR>char source <BR>char robotMessageType<BR>char projectNameSize <BR>char array projectName <BR>unsigned char majorVersion<BR>unsigned char minorVersion<BR>int svnRevision <BR>char array buildDate <BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
<TD>uint64_t timestamp <BR>char source <BR>char robotMessageType <BR>char projectNameSize <BR>char array projectName <BR>unsigned char majorVersion <BR>unsigned char minorVersion <BR>int bugfixVersion <BR>int buildNumber <BR>char array buildDate <BR></TD>
<TD><BR></TD>
</TR>
</TABLE>

<TABLE>
<TR>
<TD>Interface<BR></TD>
<TD>1.6<BR></TD>
<TD>1.7<BR></TD>
<TD>1.8<BR></TD>
<TD>3.0<BR></TD>
<TD>3.1<BR></TD>
<TD>3.2<BR></TD>
<TD>3.3<BR></TD>
<TD>3.4<BR></TD>
</TR>
<TR>
<TD>Masterboard<BR>Data<BR></TD>
<TD>int packageSize<BR>unsigned char packageType <BR>short digitalInputBits<BR>short digitalOutputBits<BR>char analogInputRange0<BR>char analogInputRange1<BR>double analogInput0<BR>double analogInput1<BR>char analogOutputDomain0<BR>char analogOutputDomain1<BR>double analogOutput0<BR>double analogOutput1<BR>float masterBoardTemperature<BR>float robotVoltage48V<BR>float robotCurrent<BR>float masterIOCurrent<BR>unsigned char masterSaftyState<BR>unsigned char masterOnOffState<BR>char euromap67InterfaceInstalled<BR>int euromapInputBits<BR>int euromapOutputBits<BR>short euromapVoltage<BR>short euromapCurrent)<BR></TD>
<TD><BR></TD>
<TD><BR></TD>
<TD>int packageSize<BR>unsigned char packageType <BR>int digitalInputBits<BR>int digitalOutputBits<BR>char analogInputRange0<BR>char analogInputRange1<BR>double analogInput0<BR>double analogInput1<BR>char analogOutputDomain0<BR>char analogOutputDomain1<BR>double analogOutput0<BR>double analogOutput1<BR>float masterBoardTemperature<BR>float robotVoltage48V<BR>float robotCurrent<BR>float masterIOCurrent<BR>unsigned char safetyMode<BR>unsigned char InReducedMode<BR>char euromap67InterfaceInstalled<BR>int euromapInputBits<BR>int euromapOutputBits<BR>float euromapVoltage<BR>float euromapCurrent)<BR>uint32_t (Used by Universal<BR> Robots software only)<BR></TD>
<TD><BR></TD>
<TD>int packageSize<BR>unsigned char packageType <BR>int digitalInputBits<BR>int digitalOutputBits<BR>char analogInputRange0<BR>char analogInputRange1<BR>double analogInput0<BR>double analogInput1<BR>char analogOutputDomain0<BR>char analogOutputDomain1<BR>double analogOutput0<BR>double analogOutput1<BR>float masterBoardTemperature<BR>float robotVoltage48V<BR>float robotCurrent<BR>float masterIOCurrent<BR>unsigned char safetyMode<BR>unsigned char InReducedMode<BR>char euromap67InterfaceInstalled<BR>int euromapInputBits<BR>int euromapOutputBits<BR>float euromapVoltage<BR>float euromapCurrent)<BR>uint32_t (Used by Universal<BR> Robots software only)<BR>uint8_t operationalModeSelectorInput<BR>uint8_t threePositionEnablingDeviceInput<BR></TD>
<TD><BR></TD>
<TD><BR></TD>
</TR>
</TABLE>




<TABLE>
<TR>
<TD>Interface<BR></TD>
<TD>1.6<BR></TD>
<TD>1.7<BR></TD>
<TD>1.8<BR></TD>
<TD>3.0<BR></TD>
<TD>3.1<BR></TD>
<TD>3.2<BR></TD>
<TD>3.3<BR></TD>
<TD>3.4<BR></TD>
</TR>
<TR>
<TD>Tool<BR>Data<BR></TD>
<TD>NA<BR></TD>
<TD>char analogInputRange2  <BR>char analogInputRange3  <BR>double analogInput2  <BR>double analogInput3  <BR>float toolVoltage48V  <BR>unsigned char toolOutputVoltage  <BR>float toolCurrent  <BR>float toolTemperature  <BR>unsigned char toolMode  <BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
</TR>
</TABLE>


In the configuration data it is sometimes unclear how many fields a "for each joint:" pertains to.
<TABLE>
<TR>
<TD>Interface<BR></TD>
<TD>1.6<BR></TD>
<TD>1.7<BR></TD>
<TD>1.8<BR></TD>
<TD>3.0<BR></TD>
<TD>3.1<BR></TD>
<TD>3.2<BR></TD>
<TD>3.3<BR></TD>
<TD>3.4<BR></TD>
</TR>
<TR>
<TD>Configuration<BR>Data<BR></TD>
<TD>NA<BR></TD>
<TD>NA<BR></TD>
<TD>for each joint:<BR>double jointMinLimit	<BR>double jointMaxLimitt	<BR>for each joint:	<BR>double jointMaxSpeed	<BR>double jointMaxAcceleration	<BR>double vJointDefault	<BR>double aJointDefault	<BR>double vToolDefault	<BR>double aToolDefault	<BR>double eqRadius	<BR>for each joint:	<BR>double DHa	<BR>for each joint:	<BR>double DHd	<BR>for each joint:	<BR>double DHalpha	<BR>for each joint:	<BR>double DHtheta	<BR>int masterboardVersion	<BR>int controllerBoxType	<BR>int robotType	<BR>int robotSubType	<BR>for each joint:	<BR>int  motorType	<BR></TD>
<TD>for each joint:<BR>double jointMinLimit<BR>double jointMaxLimitt<BR>for each joint:<BR>double jointMaxSpeed<BR>double jointMaxAcceleration<BR>double vJointDefault<BR>double aJointDefault<BR>double vToolDefault<BR>double aToolDefault<BR>double eqRadius<BR>for each joint:<BR>double DHa<BR>for each joint:<BR>double DHd<BR>for each joint:<BR>double DHalpha<BR>for each joint:<BR>double DHtheta<BR>int masterboardVersion<BR>int controllerBoxType<BR>int robotType<BR>int robotSubType<BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
</TR>
</TABLE>


<TABLE>
<TR>
<TD>Interface<BR></TD>
<TD>1.6<BR></TD>
<TD>1.7<BR></TD>
<TD>1.8<BR></TD>
<TD>3.0<BR></TD>
<TD>3.1<BR></TD>
<TD>3.2<BR></TD>
<TD>3.3<BR></TD>
<TD>3.4<BR></TD>
</TR>
<TR>
<TD>Force<BR>Mode<BR>Data<BR></TD>
<TD>NA<BR></TD>
<TD>NA<BR></TD>
<TD>double X	<BR>double Y	<BR>double Z	<BR>double Rx	<BR>double Ry	<BR>double Rz	<BR>double robotDexterity<BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
<TD>ibid<BR></TD>
</TR>
</TABLE>

# Installation
To install the UR Agent double click the (for 64 bit machines only).

![Figure6](./images/UR_AgentReadme_image6.gif)



![Figure7](./images/UR_AgentReadme_image7.gif)




![Figure8](./images/UR_AgentReadme_image8.gif)





![Figure9](./images/UR_AgentReadme_image9.gif)






![Figure10](./images/UR_AgentReadme_image10.gif)







![Figure11](./images/UR_AgentReadme_image11.gif)



Acknowledge the installation permission challenge, then you should see the installation screen:

![Figure12](./images/UR_AgentReadme_image12.gif)




![Figure13](./images/UR_AgentReadme_image13.gif)



The Service installation is often precarious. So let's verify that the UR_Agent service was installed (although not started) in the Windows Service Control Manager (SCM)

![Figure14](./images/UR_AgentReadme_image14.gif)


The agent will NOT start unless you tell it to START (unless you reboot).


![Figure15](./images/UR_AgentReadme_image15.gif)



Manual installation of the UR MTC agent is of course possible. Change directory to the installation folder, and then right click on "Install.bat" and Run As Administrator. This assumes you have enough UAC privileges to do installation. If not you will need someone who will. If you are successful you should see the following DOS console screen flash before you eyes. (I added a pause to the end of the script to prevent the window from disappearing.) 


![Figure16](./images/UR_AgentReadme_image16.gif)


You should then see the UR_Agent service in the SCM. You can then start the service.
# Uninstall
First make sure the Nikon Agent is stopped in the Service Control Manager:
Right click on My Computer -> Select Manage -> Acknowledge UAC challenge
Select Services and Applications and then Services, scroll down to NikonAgent, and push Stop button.

![Figure17](./images/UR_AgentReadme_image17.gif)


After you have stopped the Nikon Agent service, go into the control panel and uninstall the program: MTConnectAgentNikonx64


![Figure18](./images/UR_AgentReadme_image18.gif)




![Figure19](./images/UR_AgentReadme_image19.gif)


Yes!

![Figure20](./images/UR_AgentReadme_image20.gif)


Please wait .. acknowledge UAC permission to uninstall challenge (you must be administrator or have administrator priviledges).  It will uninstall and you may see a black console screen popup in the background momemtarily.
And then in the Service Manager click Action->Refresh, and the Nikon Agent service should be removed. The agent code in C:\Program Files\MTConnect\MTConnectAgentNikonx64 should also be removed.
# Configuration
The installation wizard installs the log file Agent into the folder: C:\Program Files\MTConnect\MTConnectAgentNikonx64 where x64 means a 64 bit installation platform, e.g., windows 7. The MTConnect Agent executable must be paired with the correct platform (32 or 64 bit).

![Figure21](./images/UR_AgentReadme_image21.gif)


In the folder, the vb script MTConnectPage-1.4.vbs in the can be used to verify that the log file Agent is working. MTConnectPage-1.4.vbs reads the data from the agent via http://127.0.0.1:5000 and then formats the data. (assuming you have configure the agent port to 5000).

# DIRECTIONS TO MANUALLY CONFIGURING UR AGENT.

Modify Config.ini in C:\Program Files\MTConnect\MTConnectAgentXXXXx64directory
 1. Stop MTConnect agent, edit config.ini file, add new configuration:

	[GLOBALS]
	Config=NEW
This will cause a new devices.xml file to be generated.

 2. Add new devices under [GLOBALS] section tag "MTConnectDevice" (spaces are stripped out) using a comma separated list (e.g., a,b,c) WHICH IS CASE SENSITIVE:
 

	MTConnectDevice=UR5,UR3

 3. Make sure there is an ini file "section" for each device (in this case UR5, UR3) and that the section has the three tags IP, jointnames and urdf. IP gives the TCP/IP address of the PC that hosts the UR controller and the jointnames give the list of joints for the given robot, while urdf defines the file to transfer as an asset through the MTConnect agent:

	
	[UR5]
	IP=129.6.33.123
	jointnames= joint_1,joint_2,joint_3,joint_4,joint_5,joint_6 
	urdf=ur5.urdf
	[UR3]
	IP=129.6.33.123
	jointnames=shoulder_pan_joint,shoulder_lift_joint,elbow_joint,wrist_1_joint,wrist_2_joint,wrist_3_joint
	urdf=ur3.urdf
 4. Start UR agent, the agent will detect a new configuration, and then write a new Devices.xml file to add the new devices.
 5. If it works config.ini tag should say:

	"Config=UPDATED" if a problem tag will say: "Config=ERROR"



# Bibliography

<TABLE>
<TR>
<TD>[1] <BR></TD>
<TD>A. Larson, "Rethink's Baxter vs Universal Robots: Which Collaborative Robot is best for you?," 22 1 2013. [Online]. Available: cross-automation.com/blog/rethinks-baxter-vs-universal-robots-which-collaborative-robot-best-you. [Accessed 17 10 2017].<BR></TD>
</TR>
<TR>
<TD>[2] <BR></TD>
<TD>T. Trautner, F. Pauker and B. Kittl, "ADVANCED MTCONNECT ASSET MANAGEMENT (AMAM)," in International Conference on Innovative Technologies, IN‐TECH 2016, Prague, 2016. <BR></TD>
</TR>
</TABLE>


