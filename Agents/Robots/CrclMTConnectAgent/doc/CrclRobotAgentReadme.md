
# README 
----

Friday, December 01, 2017
The goal of this report is to describe the integration of MTConnect and robots deploying the canonical robot control language (CRCL). MTConnect is a standard to solve the "Island of Automation" problem in the discrete manufacturing industry. So why is the "Island of Automation" such a problem? Although factory integration standards have and are routinely used elsewhere in manufacturing, acceptance has been slow within the discrete parts industry. The problem stems from the perceived, if not actual, lack of return on investment from integration. It is at times too hard, too costly and/or too complicated. MTConnect addresses these issues from the onset bringing an approach that ensures affordability as well as performance. MTConnect is an open, royalty-free standard that uses prevalent commercial off-the-shelf technology - Extensible Markup Language (XML) and Hypertext Transfer Protocol (HTTP).
CRCL is a robot communication standard that bridges the gap between robot tasks as defined in an ontology and low level robot motion planning and control. Both MTConnect and CRCL use XML for communication. Both use XML Schema (XSD) to define the XML interface between communicating parties. Yet, the XML communicated and method of communication are not the same. The major difference is that MTConnect is polling based and relies on HTTP and the Representational State Transfer (REST) architecture to communicate, while CRCL uses a stream based Transmission Control Protocol (TCP) socket to communicate the XML. In spite of the differences, MTConnect can monitor CRCL robot communication, translate the CRCL meaning into MTConnect intent, and then provide Internet access to resultant XML robot status data, potentially available world wide. 
The document includes a brief background on MTConnect, CRCL robot background, system architecture, communication scheme, and installation notes. This document assumes the reader is familiar with MTConnect operation, and for deeper explanation of MTConnect, please refer to MTConnect URL: http://www.mtconnect.org/ for more information. 
# <a name="Background"></a>Background 
## <a name="CRLC Background"></a>CRLC Background 
CRCL is part of the robot research at NIST for Kit building. Kit building or "kitting" is a process in which individually separate but related items are grouped, packaged, and supplied together as one unit (kit). To pick and place parts and components during kitting, the kitting workcell relies on a sensor system to retrieve the six-degree of freedom (6DOF) pose estimation of each of these objects. While the use of a sensor system allows objects' poses to be obtained, it also helps detecting failures during the execution of a kitting plan when some of these objects are missing or are not at the expected locations. 
Ontologies are a formal way to describe taxonomies and classification networks, essentially defining the structure of knowledge for various domains: the nouns representing classes of objects and the verbs representing relations between the objects [5].  The kitting ontology has been fully defined in OWL. However, the ontology was also fully defined in the XML schema language.  Tools to transform XML Schema file in Ontology definition files were developed [6] [7].
This kitting ontology models commands expressed in the canonical robot command language (CRCL). CRCL is a messaging language for controlling a robot. CRCL commands are executed by a low-level device robot controller. The usual source of CRCL commands is a plan/program execution system. CRCL is intended for use with devices typically described as industrial robots and for other automated positioning devices such as automated guided vehicles (AGVs). An AGV with a robotic arm attached may be regarded as a single robot responding to a single stream of CRCL commands or as two robots responding to two separate streams of CRCL commands. Although CRCL is not a programming language, the commands are in the context of a session consisting of getting ready for activity, performing activities, and becoming quiescent. CRCL commands may be collected in files for testing purposes, but executing such files (by giving the commands in the order they occur in the file) is not be the normal operating mode of a robot. Because robots operate in uncertain and changing environment, the reliance on sensors to adjust for such disturbances makes canned scripts ineffective under real conditions.
MTConnect is primarily a status reporting mechanism, so that the bulk of the work is fetching and reporting the status of CRCL robots. CRCL models a status message from a low-level robot controller Status includes the position and orientation (Poses) that are the subject of CRCL commands. If any joint status reporting is done, it is assumed that the system sending canonical commands and the system executing them both know the kinematics of the robot and have the same numbering system for the joints, starting with 1. The two systems also have the same understanding of where the zero point is and which direction is positive for each joint. 
## <a name="CRCL Status API"></a>CRCL Status API
The kitting ontology models a status message from a low-level robot controller to a controller sending commands to the low-level controller. The messages are designed to work with the canonical robot command language (CRCL). If any joint status reporting is done, it is assumed that the system sending canonical commands and the system executing them both know the kinematics of the robot and have the same numbering system for the joints, starting with 1. The two systems also have the same understanding of where the zero point is and which direction is positive for each joint. Status items for joints must be configured using a ConfigureJointReports command.  For each joint for which anything is to be reported, ConfigureJointReports specifies:
 - whether joint position should be reported
 - whether joint torque or force should be reported
 - whether joint velocity should be reported
During a CRCL session, until a ConfigureJointReports command has been executed that sets the reporting status for a joint, no joint status should be reported for that joint. The ConfigureJointReports command may be used more than once during a session to change joint status reporting. 
The units used in a status report are the following, where linear units are set by a SetLinearUnits command, and angular units are set by a SetAngularUnits command.
 - distance - linear units
 - angle - angular units
 - linear speed - linear units per second
 - angular speed - angular units per second
 - linear acceleration - linear units per second per second
 - angular acceleration - angular units per second per second
 - force - as set by a SetForceUnits command
 - torque - as set by a SetTorqueUnits command
If reporting a joint status requires a unit, the units are as set by the most recent SetXXXUnits command. If no SetXXXUnits command for a given unit type has been executed since the last InitCanonType command was executed, the following default units apply:
 - length -- meter
 - angle -- radian
 - force -- newton
 - torque -- newtonMeter
In order that this file may be translatable into OWL, all complexTypes defined in the CRCL XML Schema Definition (XSD) file include the (optional) Name element, inherited from DataThingType. 
### <a name="CommandStatusType"></a>CommandStatusType
CommandStatusType is derived from DataThingType. An instance of CommandStatusType has the following elements:
 - Name (inherited, optional)
 - CommandID echoes the command id from the received command to which the status message applies
 - StatusID ID associated with this status message.
 - CommandState relates the execution status of the currently executing command (or the most recently executed command, if there is no current command).
 - StateDescription (optional) is an optional brief description of the state such as "Joint 3 at -171.0 less than limit -170.0" or  "Waiting for Operator".
 - ProgramFile (optional) provides an optional reference if the currently executing command is known to have come from a file.
 - ProgramIndex (optional) provides an optional reference to the element within a program. If the currently executing command is known to have come from a particular file. The InitCanon command will have index 0, and first MiddleCommand will have index 1.
 - ProgramLength (optional) number of commands in the current program if  known.
The combination of StatusID and CommandID must be unique  within a session.
### <a name="CRCLStatusType"></a>CRCLStatusType
Status is returned periodically by the controller. See notes at the beginning of this file regarding configuring CRCL status messages.
 - Name (inherited, optional)
 - CommandStatus
 - JointStatuses (optional)
 - PoseStatus (optional)
 - GripperStatus (optional) should not be reported when there is no gripper and should be reported when there is a gripper.
 - SettingsStatus (optional).
The coordinate system in which the Pose is reported is always robot coordinates.
### <a name="GripperStatusType"></a>GripperStatusType
 - Name (inherited, optional)
 - GripperName
 - HoldingObject (optional) is true if the gripper is expected to be holding an object given its position and/or pressure sensors on the finger tips.
### <a name="JointStatusesType"></a>JointStatusesType
An instance of JointStatusesType has the following elements:
 - Name (inherited, optional)
 - JointStatus (multiple).
 
Each JointStatus element gives the status of one joint. No joint may be reported more than once in an instance of JointStatusesType. See notes at the beginning of this file regarding configuring joint status.
### <a name="JointStatusType"></a>JointStatusType
Reports the status of one joint.
 - Name (inherited, optional)
 - JointNumber
 - JointPosition (optional)
 - JointTorqueOrForce (optional)
 - JointVelocity (optional).

### <a name="JointLimitType"></a>JointLimitType
Reports the limits of one joint.
 - Name (inherited, optional)
 - JointNumber
 - JointMinPosition (optional)
 - JointMaxPosition (optional)
 - JointMaxTorqueOrForce (optional)
 - JointMaxVelocity (optional).
### <a name="ParallelGripperStatusType"></a>ParallelGripperStatusType
ParallelGripperStatusType gives gripper status for a parallel jaw gripper. 
 - Name (inherited, optional)
 - GripperName (inherited)
 - Separation gives the distance between the jaws in length units.
### <a name="PoseStatusType"></a>PoseStatusType
PoseStatusType provides a Cartesian counterpart to JointStatusesType, representing the generalized position/orientation, velocities, and forces.
of a reference frame.
 - Name (inherited, optional)
 - Pose
 - Twist (optional)
 - Wrench (optional)
 - Configuration (optional) provides a robot specific description of the current  configuration flags and turns.
### <a name="SettingsStatusType"></a>SettingsStatusType
Provides the values echoed back from the appropriate command to set that parameter. It might also provide the initial default value read from a configuration file or from a lower level controller on startup if no command has yet been  given.
        
 - Name (inherited, optional)
 - AngleUnitName (optional) is a string that can be only the literals 'radian' or 'degree'. This tells the robot that all further commands giving angle values will implicitly use the named unit.
 - EndEffectorParameterSetting (optional) is for setting parameters of end effectors that have parameters. The meaning of the parameter settings is not part of CRCL. It is expected that this command will be used only to send parameter values that can be used by the end effector currently in use.
 - EndEffectorSetting (optional) is for setting the effectivity of end effectors. If an end effector has multiple control modes, the control mode must be set using a SetEndEffectorParameters command, so that the meaning of SetEndEffector commands is unambiguous. For end effectors that have a continuously variable setting, the Setting means a  fraction of maximum openness, force, torque, power, etc. For end  effectors that have only two choices (powered or unpowered, open or  closed, on or off), a positive Setting value means powered, open,  or on, while a zero Setting value means unpowered, closed, or off.
 - ForceUnitName (optional) is a string that can be only the literals 'newton', 'pound', or 'ounce'. This tells the robot that all further commands giving force values will implicitly use the named unit.
 - JointLimits (optional) represents a list of different possible limits associated with each joint. These limits can not be directly set through CRCL.
 - IntermediatePoseTolerance indicates to the robot the precision withwhich it must reach each intermediate waypoint.
 - LengthUnitName (optional) is a string that can be only the literals 'meter', 'millimeter', or 'inch'. This tells the robot that all further commands giving position or length values will implicitly use the named unit. 
 - MaxCartesianLimit(optional) is the point with greatest X, Y, and Z values that can be reached without violating a configured cartesian limit. It cannot be directly changed through CRCL.
 - MinCartesianLimit (optional) is the point with lowest X, Y, and Z values that can be reached without violating a configured cartesian limit. It cannot be directly changed through CRCL.
 - MotionCoordinated (optional) is a boolean. If the value is true, rotational and translational motion must finish simultaneously in motion commands (including each segment in a multiple segment motion command), except as possibly temporarily overridden in the the motion command. If the value is false, there is no such requirement.
 - PoseTolerance (optional) indicates to the robot the precision with which it must reach its end location.
 - RobotParameterSetting (optional) is for setting robot parameters that cannot be set by any other CRCL command. The meaning of the parameter settings is not part of CRCL.
 - RotAccelAbsolute (optional) represents the target single axis rotational acceleration for the robot, in current angle units per second per second.
 - RotAccelRelative (optional) represents the fraction of the robot's maximum rotational acceleration that it should use.
 - RotSpeedAbsolute (optional) represents the target single axis rotational speed for the robot, in current angle units per second.
 - RotSpeedRelative(optional) represents the fraction of the robot's maximum rotational speed that it should use.
 - TorqueUnitName (optional) is a string that can be only the literals 'newtonMeter' or 'footPound'. This tells the robot that all further commands giving torque values will implicitly use the named unit.
 - TransAccelAbsolute (optional) represents the translational acceleration for the controlled point, in current length units per second per second.
 - TransAccelRelative (optional) represents the fraction of the robot's maximum translational acceleration that it should use.
 - TransSpeedAbsolute (optional) represents the translational speed for the controlled point, in current length units per second.
 - TransSpeedRelative (optional) represents the fraction of the robot's maximum translational speed that it should use.
### <a name="ThreeFingerGripperStatusType"></a>ThreeFingerGripperStatusType
ThreeFingerGripperStatusType gives gripper status for a three-finger gripper. The fingers are assumed to be non-articulated.  Finger position is 0.0 at fully closed and 1.0 at fully open and linear in either angle or distance for rotating fingers or  sliding fingers, respectively. All elements are optional, but typically all three positions will be used if any one of  them is used, and similarly for the three forces.

Force units are as set by the most recent SetForceUnits command. The system sending CRCL commands and the system executing them must agree on which fingers are Finger1, Finger2, and Finger3. An instance of ThreeFingerGripperStatusType has the following elements:
 - Name (inherited, optional)
 - GripperName (inherited)
 - Finger1Position (optional)
 - Finger2Position (optional)
 - Finger3Position (optional)
 - Finger1Force (optional)
 - Finger2Force (optional)
 - Finger3Force (optional).
### <a name="VacuumGripperStatusType"></a>VacuumGripperStatusType 
VacuumGripperStatusType is derived from GripperStatusType. VacuumGripperStatusType gives gripper status for a vacuum gripper. 
 - Name (inherited, optional)
 - IsPowered. IsPowered element is true if a vacuum is being applied and is false if not.

        
         
          

        

        


      

          

## <a name="MTConnect Overview"></a>MTConnect Overview
MTConnect is an evolving standard developed to facilitate the exchange of data on the manufacturing floor. The MTConnect open specification provides for cost effective data acquisition on the manufacturing floor for machine tools and related devices.  MTConnect is based upon prevalent Web technology including XML and HTTP.   Figure 1 shows the MT Connect architecture. An "MTConnect Device" is a piece of equipment – in this case a CRCL robot, which (optionally) includes an MTConnect Adapter so that we can get data from it. The "Agent" is a process that acts as a "bridge" between a device and a factory "Client Application".  To learn more about MTConnect visit: http://www.mtconnect.org/ 
Figure 1 shows a typical MTConnect CRCL Robot system architecture (with one or more CRCL robot devices). Communication between a Windows PCs and the UR robot is assumed, communicating over Ethernet to communicate the CRCL robot status.


<div style="text-align: center;" markdown="1">

![Figure1](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image1.gif?format=raw)



</div>
<p align="center">
_Figure 1 MTConnect NIKON CMM Logging Agent system architecture_
</p>
The CRCL MTConnect Agent contains back end adapters that periodically read CRCL status generated from the robot controller. From this status, the MTConnect reports time stamped events in the standard MTConnect XML format to the World Wide Web. The MTConnect Agent has the capability to launch a back-end for each CRCL robot that will connect, read status, and translate data into the MTConnect XML format.
The MTConnect Agent model uses the core C++ MTConnect Agent open source code as the backbone to do service or daemon handling, HTTP communication, streaming of samples, asset management, etc. This core C++ MTConnect Agent source code is modular and has been customized to allow any backend to read device data. In this case, the back end uses the TCP socket stream to read the robot CRCL XML. In core C++ MTConnect Agent source code, the agent reads a configuration file (i.e., Agent.cfg) to determine where Adapters will be producing device data. If no adapters are defined in the configuration file, then this part of the core code is unused. Instead, the back end reads the device data, and uses an Agent Device Data API method to store the data into the Agent device data model, which then can stream the data as required. Of note, the corresponding device model must match the specification defined in the file Devices.xml.
The open source C++ Agent found on the MTConnect github web site has two files to control configuration of the agent: "Agent.cfg" and Devices.xml.  The Agent.cfg file is structured as a YAML file, and defines:
 - the name of the XML file, which describes the devices xml – typically "Devices.xml";
 - the TCP port that the Agent will utilize for http communication; 
 - each IP address and port for all Adapters that provides data to the agent; 
 - logging level and file name if required
 - Service name of the agent
 - Etc.
Note, for each device the MTConnect Agent data is updated internally by the CRCL Backend Adapters, not with the usual  MTConnect SHDR Adapters, so the Adapters field is blank within the Agent.cfg file. The following show the relevant part in Agent.cfg:
Adapters
{
  
}
The Devices.xml will be provided to clients if they perform a http "probe".  The filename "Devices.xml" is specified in the Agent.cfg file, and in theory could be any XML file name, e.g., CNC1.xml. However, for the CRCL robots, the "Devices.xml" is created once and then never modified. The data items are for each CRCL robot are currently statically defined (minimally matching the status provided by the CRCL robot).  Advanced MTConnect functionality, e.g., conditions, assets, etc., are not handled. Conditions are not handled since the CRCL robot does not provide any health status. The current list of MTConnect CRCL device data includes:
 - avail- AVAILABLE, UNAVAILABLE
 - controllermode – MANUAL - always
 - execution – ACTIVE, PAUSED
 - heartbeat – agent update count
 - last_update - time stamp
 - position - 3 numeric values
 - power – ON/OFF/UNAVAILABLE
 - program – blank text
 - xorient- 3 numeric values
 - zorient - 3 numeric values
 
Some MTConnect data items, such as M1id_asset_chg, are hard coded into the MTConnect data streams, and cannot be removed. Note, "Error" is a status enumeration from CRCL but is currently a dead end, once you enter "ERROR" you cannot get out of the state – since there is no reset.
The CRCL MTConnect Agent contains a back-end adapter that communicates with the CRCL controller via the TCP/IP socket. In general, CRCL supplies three TCP/IP interfaces: primary, secondary and real-time. The secondary communication channel was used to retrieve robot status at 15Hz. With the MTConnect agent running, a connection to each CRCL robot is established to receive status communication from the robot. Then, if you bring up a browser and enter the IP plus port 5000 and the command current, e.g., http://xxx.xxx.xxx.xxx:5000/current the agent will return the current state of the data items in the CRCL robot. Shown below is a formatted output table from a VB script that extracts the MTConnect agent XML from one CRCL robot.

<div style="text-align: center;" markdown="1">

![Figure2](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image2.gif?format=raw)



</div>
<p align="center">
_Figure 2 Web-browser Display Showing Table of MTConnect Agent Data Items_
</p>
Within this XML data buffer, the CRCL robot status is collected and then interpreted into MTConnect standard data items. Although many MTConnect items are CNC –centric and aren't intuitively reflected within the CRCL robot status buffers (e.g., MANUAL versus AUTOMATIC mode), it is possible to convert the CRCL robot status into an MTConnect device status representation.  In fact, the CRCL robot has more than sufficient status information, so that determining the data items for the MTConnect Agent to monitor when displaying a CRCL robot state can depend on the particular client application. For example, an OEE client application would require different data items than a client application to determine the prognostic and health of the robot.
<table>
<tr>
<td>MTConnect Data Item<br></td>
<td>CRCL robot interface item<br></td>
</tr>
<tr>
<td>power<br><br></td>
<td>ON if connected to socket<br></td>
</tr>
<tr>
<td>mode<br><br></td>
<td>MANUAL (not supposed to run program)<br></td>
</tr>
<tr>
<td>execution<br></td>
<td>if(crcl._status.CommandState == "Done")<br>  execution="idle";<br>else if(crcl._status.CommandState == "Working")<br>  execution="running";<br>else if(crcl._status.CommandState == "Error")<br>  execution="idle";<br></td>
</tr>
<tr>
<td>axes status<br></td>
<td>???<br></td>
</tr>
<tr>
<td>estop<br></td>
<td>???<br></td>
</tr>
<tr>
<td>fovr<br></td>
<td>NA<br></td>
</tr>
<tr>
<td>pose or PATH_POSITION<br></td>
<td>crclstat->Pose()->Point().X()<br>crclstat->Pose()->Point().Y()<br>crclstat->Pose()->Point().Z()<br></td>
</tr>
<tr>
<td>XOrient<br></td>
<td>crclstat->Pose()->XAxis().I()<br>crclstat->Pose()->XAxis().J()<br>crclstat->Pose()->XAxis().K()<br></td>
</tr>
<tr>
<td>ZOrient<br></td>
<td>crclstat->Pose()->ZAxis().I()<br>crclstat->Pose()->ZAxis().J()<br>crclstat->Pose()->ZAxis().K()<br></td>
</tr>
<tr>
<td>heartbeat<br></td>
<td>Automatically increments every cycle to show alive<br></td>
</tr>
<tr>
<td>last_update<br></td>
<td>Timestamp of last CRCL reading<br></td>
</tr>
</table>

The match between CRCL and MTConnect is not perfect. For now, there is no IO to understand ESTOP. Likewise , joint status values have not been interpreted either.
# <a name="MTConnect CRCL Software Components"></a>MTConnect CRCL Software Components
MTConnect support for a CRCL robot required integration of several software components. Since MTConnect only needed to provide status and did not require a command interface to the CRCL robot (except for a couple of behind the scenes commands to synchronize data retrieval), the focus was on reading status from the streaming socket interface of the CRCL simulator.
<p align="center">
/
</p>
<p align="center">
_Figure 3 Software Components in MTConnect and CRCL Integration_
</p>
Figure 3 shows the basic components that were involved in the integration of MTConnect and CRCL.
Java CRLC Simulator – is a graphical user interface (GUI) simulator that accepts joint, Cartesian and other CRCL commands and then simulates robot action. Clients that desire robot status can use the XML interface to communicate and get simulated results as would be expected from an actual robot controller. See  Appendix I Java CRCL Simulator.
XMLReader – C++ class to handle all socket communication with the CRCL simulator. Relies on Boost Asio for low level IO handling. The class XMLReader handles the intracies of connecting, initiating, reading, buffering and queing messages within the Boost Asio framework.
Boost.Asio – Boost.Asio is a cross-platform C++ library that was used for network I/O programming because it abstracts the low level socket handling functionality [13]. Boost Asio is used for communication over sockets to the CRCL controller. Boost Asio is very robust with examples and issue solutions pervasively found on the Internet. 
CRCL interface – relied on CodeSynthesis and the Xerces XML DOM parser. Xerces was used to parse the CRCL XML. CodeSynthesis provided support for translating XML into a C++ representation. Given the C++, it was then simple matter to translate the intent of CRCL status message into a corresponding MTConnect intent. Code Synthesis "XSD" tool was used to generate the corresponding C++ classes from the CRCL XSD.
Backend – coordinates the interface to MTConnect core code. MTConnect streams based on device data updates, so the backend retrieves and decodes the CRCL status message into MTConnect data, and updates the MTConnect data collection.
MTConnect – Numerous open source C++ solutions exist for a variety of devices and MTConnect architectures. The code can be found on the MTConnect open source github site: https://github.com/mtconnect. Two MTConnect architectures have been developed to communicate with the robot CRCL: 1) Special backend with Core Open source MTConnect Agent Model and 2) Adapter model with COTS MTConnect Agent. 

## <a name="CRCL Socket Communication"></a>CRCL Socket Communication
CRCL communicates commands and status using XML. There is no framing mechanism – such as a trailing zero (which is useful since it is illegal in XML) – so the end of a CRCL XML message relies on the detection of a matching XML tag. This is universal to all CRCL communication, message are framed by a pair of opening/closing XML tags. For CRCL status, this means detection of <CRCLStatus> and </CRCLStatus> in the XML message is required to understand when the CRCL status message has completed. This section will further discuss these challenges to reading CRCL XML messages.
As discussed, there is also no terminating character (such as zero) in a CRCL message. Also, CRCL messages can also be of different buffer sizes. So framing the message requires buffering each message, such that the end of a status message is detected with a closing XML tag, and this message may be divided with some of the buffer belonging to the previous or next message. Since there is no CRCL message termination condition, a deadline timer was used to stop asynchronous reading and cancel the read since the last write of the message need not satisfy an asynchronous condition - such as buffer full or matching character. Likewise, often two CRCL will be combined into one aynchronous read operation, so that these two message must be separated by the CRCL streaming reader. 
The method to retrieve CRCL status from a robot controller is to first send a CRCL GetStatus command (that could be combined with other command to configure the status response, but is out of scope) as shown in the (a) portion of Figure 4. Upon receipt of a status command, then the CRCL controller responds with the XML status message as shown in the (b) portion of Figure 4. 
<p align="center">
/
</p>
<p align="center">
_Figure 4 Status Communication to CRCL Controller_
</p>
We will assume that the MTConnect has connected to the CRCL simulator socket. Figure 5 shows the components involved in communication.The XMLClient class initiates communication using Boost Asio to first send a "GetStatus" command to the CRCL robot simulator. Using Boost Asio, the class XMLClient then asynchronously reads the reponse from the CRCL robot simulator. When the XMLClient has read a complete message it queues this message onto the message queue. Since the message queue is a shared resource, and multiple threads share this resource, a mutex is used to lock the contents for one thread at a time access. The latest message is retrieved and then the CRCL data expressed in XML can be reinterpreted before storing into the appropriate MTConnect data manager – either device data streams or adapter updates. The translation uses the CRCL interface to parse the CRCL XML message and translate into C++. Once in C++, it is trivial exercise to reinterpret CRCL status into MTConnect lexicon, e.g., mapping enumerations.

<div style="text-align: center;" markdown="1">

![Figure3](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image3.gif?format=raw)



</div>
<p align="center">
_Figure 5 Communication Sequence with Robot CRCL Simulator_
</p>
The message queue handles potential timing differences between the XMLClient interface to the CRCL simulator and the MTConnect streaming interface – either Agent Web server or Adapter stream.  At this time, there is only one type of status message, but in the future, there may be numerous types of status messages. Thus, for the time being, the latest message on the queue is read, and then the message queue is cleared. 
A major portion of the XMLClient was the handling of the socket stream interface to CRCL simulator. The Boost Asio library was used. Boost Asio uses the Proactor design pattern for event handling in which long running activities are running in an asynchronous part [14]. The major complaint with Boost Asio is that it is more difficult to due to the separation in time and space between operation initiation and completion. Applications may also be harder to debug due to the inverted flow of control and difficulty in understanding when a problem occurred. In the Proactor model, a completion handler is called after the asynchronous event has triggered. In this software pattern, the connect asynchronous event is registered in Boost Asio, and when the connect event happens a "callback" handler method is invoked. Within this connection "callback" handler, the asynchronous read event is setup. This asynchronous read event setup function is used to specify how a stream will be asynchronously read a certain number of bytes of data from a stream. The asynchronous setup itself returns immediately. But, the asynchronous event detection will continue until one of the following conditions is true:
 - The supplied buffer is full. That is, the bytes transferred is equal to the sum of the buffer size.
 - An error occurred. Errors include socket disconnection, or asynchronous read cancelation by the deadline timer. Disconnects cause a discontinuation of reading and the asynchronous connection trigger is then restarted.
Then the asynchronous read event handler will be called. Within the asynchronous read event handler, the asynchronous read event setup must be done again, or no more bytes will be read from the stream. Of note, often the condition of completely filling the buffer is impossible as receiving the exact amount of buffer size is impractical. Because of this, deadline timers are incorporated into Boost Asio to terminate asynchronous reads even if the triggering event (full buffer) has not occurred. So a deadline timer is used to cancel a socket asynchronous read when it expires.
Boost Asio provides io_service, which is a singleton class for servicing I/O. Every program based on Boost.Asio uses an object of type io_service. There is one Asio "io_service" per application program and from our effort it helps if all the asynchronous operations are run in the same thread as this io_service object. Each asynchronous call in Boost Asio is enabled by the io_service methods run, run_one or poll. After all the asynchronous operations were placed in the same thread, the io_service communication responded better. However, Boost Asio was too efficient so an io_service run_one method was combined with a sleep and yield to allow other threads to run and to slow down the Boost Asio operation. 
Figure 6 shows the call sequence involved with the CRCL status reading. Of note, are the synchronous write to send the "Init" during the connection handling and the "GetStatus" command messages sent at the start of every new read of the CRCL simulator status. These are the only Boost Asio operations that were synchronous. Overall, the Boost Asio asynchronous connect operation could wait indefinitely upon startup of the CRCL simulator listener. This is as intended. It is unclear how often Boost Asio tests the socket for the CRCL listener. Upon connection of the socket, the handler for the async connect event is called and it calls the async read and async periodic timer (2 seconds). Either 1) the socket stream is read and the periodic timer is canceled, or 2) the periodic timer expires and the socket async read is canceled, which calls the read handler to see if it has read any bytes or is just waiting for a termination condition. If bytes have been read, there are buffered. In either case, the asynchronous read is called again.

<div style="text-align: center;" markdown="1">

![Figure4](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image4.gif?format=raw)



</div>
<p align="center">
_Figure 6 Asio Communication Sequence_
</p>
# <a name="CRCL Robot URDF Information from MTConnect Agent"></a>CRCL Robot URDF Information from MTConnect Agent
Enterprise integration of CRCL factory robots using MTConnect would be beneficial for OEE, simulation, downtime and repair response, prognostics and health management, among other client applications. One aspect of robot's utility that is useful is a complete model of the robot. For example, the popular Robot Operating System (ROS) uses the Unified Robot Description Format (URDF) as a general-purpose robot model, which includes a wide range of robot specification information. 
ROS uses the Unified Robot Description Format (URDF) as a general-purpose robot specification model. URDF covers the kinematic and dynamic description of the robot, the visual representation of the robot, and the collision model of the robot. Of note, since URDF is an XML specification, URDF is limited to robots that exhibit only tree structures representations, ruling out all parallel robots like Stewart Platforms. However, most Industrial Robots are kinematic chain robots and so can be modeled as a serial set of links and joints. Multi-arm robots can also be modeled in URDF as a kinematic tree of links connected to multiple joints, such as a headed robot with two arms. 
URDF is an XML specification to describe a robot. Because it is an XML specification, the information is modeled as a tree with elements (e.g., "<Name>'') with subelement branches under elements. Each robot is modeled in ROS by an URDF file, which contains 
**Link Information** Link elements describe a rigid body with an geometry, inertia, visual features, obstacle bounds and other robot properties. The visual element within the link element contains the specification for display of the link in visualization tool (either a mesh or a geometry element). The visual also contains the material element which sets the color of the link.

**Joint Information** Joint elements describe the kinematics and dynamics of the joint and also specify the safety limits of the joint. Joint elements contain elements for parent link, child link, axis of rotation, calibration, dynamics, limit, and safety controller information. Joints are also able to mimic other joints. URDF assumes each joint is either a revolute, prismatic, fixed, or continuous type joint which depending on the type can have position, velocity and effort limits specified.

Fortunately, MTConnect has the capability to seamlessly pass this URDF information to clients.  Besides the device data items, MTConnect standard addresses other auxiliary manufacturing equipment that are not a component of a machine[2]. To allow generating and managing such auxiliary data models at runtime, MTConnect assets were introduced. Examples of MTConnect assets are objects like cutting tools, workholding systems, parts and fixtures. MTConnect assets offer a good framework for future expansions so that the URDF robot specification information can be channeled through MTConnect agent as an asset.
Since MTConnect can incorporate and transport standardized XML data developed independently from the core MTConnect information models, passing URDF XML as an asset is straightforward. MTConnect defines "assets as an associative array of key/value stores in which to store the XML. This allows the ability to collect and report entire XML documents as they change within applications. Below, the XML shows how an AssetChanged tag with an asset type  URDF and  MODEL value that would be updated within the MTConnect XML query to indicate new quality results from an inspection.

	<AssetChanged dataItemId="dev_asset_chg" 
	timestamp="2017-09-08T19:42:16.855924Z" sequence="46" 
	assetType="URDF">MODEL</AssetChanged>
For our implementation, the ROS URDF XML was used that is then accessible via the Internet with the following query to an MTConnect agent:


	http://xxx.xxx.xxx.xxx/asset/MODEL?type=URDF
where xxx.xxx.xxx.xxx is the IP address of the MTConnect server and which returns an URDF XML "web page". So, the URDF is conveyed to MTConnect clients as "Assets". If you on the host machine of the CRCL MTConnect agent and type in the URL http://127.0.0.1:5000/assets, you will get a XML description with the URDF XML embedded inside the MTConnect XML. Below is a screen snapshot of the assets query of the MTConnect agent for the asset named UR5.


<div style="text-align: center;" markdown="1">

![Figure5](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image5.gif?format=raw)



</div>
<p align="center">
_Figure 7 URDF for CRCL5 Robot as MTConnect Asset_
</p>
Within the UR Agent, in order to convey the asset, you need to have an asset declared in the MTConnect devices data item specification. Thus, the MTConnect probe must contain a data item to reflect the existence of the URDF asset.  

	<DataItem id="Crcl_urdf_asset" type="ASSET_CHANGED" category="EVENT"/>

For the CRCL MTConnect agent to be reported via a query, C++ code transferred a URDF file that was read into a string and then passed into the core MTConnect agent using the addAsset function call. To alleviate XML confusion, the URDF header <?xml version="1.0" ?> was removed, or a web browser would hang at the returned XML. To use addAsset, the device, the id of the asset data item (Crcl_urdf_asset), the textual body of the asset and the type (Robot) of the asset must be provided, as shown below:

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

Once the asset has been registered with the agent, the MTConnect agent status for the Crcl_urdf_asset  data item reflects that data is available.
# <a name="Version Information from MTConnect Agent"></a>Version Information from MTConnect Agent
The version information of the various MTConnect components is available through web browser access. It is uses simple XSLT formatting of the XML. The open source core MTConnect agent is downloaded from github and "frozen". The 1.2 version is used but is rebuilt when the entire agent is compiled.  

	MTConnect Core Agent Version 1.2.0.0 - built on Wed Oct 18 17:37:49 2017
It is not perfect but the output shown below gives an indication of the software involved in the MTConnect Agent operation. It can be modified to include other version information, but requires a recompilation at this time.
Thus, to get the version information in a web page, one can log onto the host PC of the MTConnect Agent, start a web browser, and type in this URL:

	http://127.0.0.1:5000/version
If the CRCL MTConnect Agent is running you should receive a formatted Web Page as follows:
# <a name="Installation"></a>Installation
To install the UR Agent double click the (for 64 bit machines only). This installation is only for 64-bit Windows architecture – e.g., Window 7 and above. IT WILL NOT RUN ON 32-bit PLATFORMS.

<div style="text-align: center;" markdown="1">

![Figure6](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image6.gif?format=raw)



</div>

<div style="text-align: center;" markdown="1">

![Figure7](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image7.gif?format=raw)



</div>


<div style="text-align: center;" markdown="1">

![Figure8](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image8.gif?format=raw)



</div>



<div style="text-align: center;" markdown="1">

![Figure9](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image9.gif?format=raw)



</div>




<div style="text-align: center;" markdown="1">

![Figure10](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image10.gif?format=raw)



</div>





<div style="text-align: center;" markdown="1">

![Figure11](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image11.gif?format=raw)



</div>
Acknowledge the installation permission challenge, then you should see the installation screen:


<div style="text-align: center;" markdown="1">

![Figure13](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image13.gif?format=raw)



</div>
And it will eventuall install the application executable and all the dependencies (Microsoft C Runtime library, etc.)

<div style="text-align: center;" markdown="1">

![Figure14](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image14.gif?format=raw)



</div>

The Service installation is often precarious. So, let's verify that the Crcl_Agentservice was installed (although not started) in the Windows Service Control Manager (SCM)

<div style="text-align: center;" markdown="1">

![Figure15](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image15.gif?format=raw)



</div>
The agent will NOT start unless you tell it to START (unless you reboot).
However, the Crcl Agent might not have installed as a service, so you will have to manually install the agent.  Navigate a windows explorer to C:\Program Files\MTConnect\CrclMTConnectAgentX64 and run the InstallAgent.bat file as Administrator (assuming you have UAC privilidges).

<div style="text-align: center;" markdown="1">

![Figure16](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image16.gif?format=raw)



</div>
Manual installation of the UR MTC agent is often required. Change directory to the installation folder, and then right click on "Install.bat" and Run As Administrator. This assumes you have enough UAC privileges to do installation. If not you will need someone who will. If you are successful you should see the following DOS console screen flash before you eyes. (I added a pause to the end of the script to prevent the window from disappearing.) 


<div style="text-align: center;" markdown="1">

![Figure17](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image17.gif?format=raw)



</div>
You should then see the  Crcl_Agentservice in the SCM. You can then start the service.
Then you should see the Service in the Computer Management console either installed manually or during the installation script:

<div style="text-align: center;" markdown="1">

![Figure18](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image18.gif?format=raw)



</div>
If you double click it you can see the 


<div style="text-align: center;" markdown="1">

![Figure19](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image19.gif?format=raw)



</div>

In the folder where the  Crcl_Agentwas installed (e.g., C:\Program Files\MTConnect\CrclMTConnectAgentX64), there is a vb script MTConnectPage-1.4.vbs, which can be used to verify that the log file Agent is working. MTConnectPage-1.4.vbs reads the data from the agent via http://127.0.0.1:5000 and then formats the data. (assuming you have configured the agent port to 5000).

<div style="text-align: center;" markdown="1">

![Figure20](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image20.gif?format=raw)



</div>

And once you enter the correct IP (local host) and the port number you specified for the MTConnect Agent (5000 in this case) you should see an Internet Explorer window popup containing the device(s) specified in your configuration. Shown below is the popup window for the Agent and the data items specified for this CRCL device.

<div style="text-align: center;" markdown="1">

![Figure21](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image21.gif?format=raw)



</div>
# <a name="Uninstall"></a>Uninstall
First make sure the  Crcl_Agent  is stopped in the Service Control Manager:
Right click on My Computer -> Select Manage -> Acknowledge UAC challenge
Select Services and Applications and then Services, scroll down to UR_Agent, and push Stop button.

<div style="text-align: center;" markdown="1">

![Figure22](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image22.gif?format=raw)



</div>
After you have stopped the Crcl_Agent service, go into the control panel and uninstall the program: Crcl Agent


<div style="text-align: center;" markdown="1">

![Figure23](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image23.gif?format=raw)



</div>
Click Uninstall Crcl_Agent 


<div style="text-align: center;" markdown="1">

![Figure24](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image24.gif?format=raw)



</div>

<div style="text-align: center;" markdown="1">

![Figure25](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image25.gif?format=raw)



</div>
Please wait .. acknowledge UAC permission to uninstall challenge (you must be administrator or have administrator priviledges).  It will uninstall and you may see a black console screen popup in the background momentarily.
And then in the Service Manager click Action->Refresh, and the Crcl_Agent service should be removed. The agent code in C:\Program Files\MTConnect\CrclMTConnectAgentX64should also be removed.
# <a name="Configuration"></a>Configuration
The UR MTConnect Agent uses the open-source MTConnect agent version 1.2 as the backbone to read http requests, generate XML responses, and update the underlying device data.  There is a back-end communication and interpretation adapter addition to handle UR status information. The adapter can handle multiple device log file and update the core MTConnect agent.  Typically, the configuration is done at installation time during the wizard screens.  The user must supply pairs of device names, model of UR robots, URDF name file, and the version the UR robot communication interface supports.
However, the user can modify the config.ini file in the C:\Program Files\MTConnect\CrclMTConnectAgentX64 folder.  However, the devices.xml and the agent.cfg files necessary for the core MTConnect agent are generated at installation time, and thus a reinstallation would be required to modify these files. 
In the config.ini file, you can manually change GLOBAL variables ServiceName, Agent http port, ResetAtMidnight flag, and list of MTConnect devices (which is specified as a comma separated list (CSV)). These changes will take if you stop/restart the Agent service or reboot the machine.

	GLOBALS]
	HttpPort=5000
	sleep=2000
	ResetAtMidnite=false
	Debug=0
	MTConnectDevice=Moto1
	ServiceName=CrclAgent
	logging_level=FATAL
	QueryServer=10000
	ServerRate=5000
	
	
Thus, the agent reads the config.ini file for list of "MTConnectDevice"s under the Globals section. The configuration file also sets the Global flags: QueryServer, ServerRate, Debug, AgentPort(default 5000), and ResetAtMidnite.
For each MTConnectDevice in the list, a section must be created under which the variables, Ip, and Port are assigned – be careful INI file Key names can be case sensitive so IP is not the same as Ip.  The Ip variable tells the agent the IP of the UR PC. The Port tells the agent where the Crcl Server is located to retrieve XMl status.

	[Moto1]
	Ip=129.6.32.176
	Port=64444

## <a name="Manually Configuring Crcl Agent Directions"></a>Manually Configuring Crcl Agent Directions
The installation wizard installs the log file Agent into the folder: C:/Program Files/MTConnect/UR_MTConnectAgent which is a 64-bit installation platform, e.g., windows 7. The MTConnect Agent executable must be paired with the correct platform (32 or 64 bit).

<div style="text-align: center;" markdown="1">

![Figure26](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image26.gif?format=raw)



</div>

Modify Config.ini in C:/Program Files/MTConnect/UR_MTConnectAgent
 1. Stop MTConnect agent, edit config.ini file, add new configuration:

	[GLOBALS]
	Config=NEW
This will cause a new devices.xml file to be generated.

 2. Add new devices under [GLOBALS] section tag "MTConnectDevice" (spaces are stripped out) using a comma separated list (e.g., a,b,c) WHICH IS CASE SENSITIVE:
 

	MTConnectDevice=CRCL5,CRCL3

 3. Make sure there is an ini file "section" for each device (in this case CRCL5, CRCL3) and that the section has the three tags IP, jointnames and urdf. IP gives the TCP/IP address of the PC that hosts the CRCL controller and the jointnames give the list of joints for the given robot, while urdf defines the file to transfer as an asset through the MTConnect agent:

	
	[CRCL5]
	Ip=129.6.33.123
	Port=64444
	
	
	[UR3]
	Ip=129.6.33.123
	Port=64444
	
 4. Start UR agent, the agent will detect a new configuration, and then write a new Devices.xml file to add the new devices.
 5. If it works config.ini tag should say:

	"Config=UPDATED" if a problem tag will say: "Config=ERROR"


## <a name="Use of NIST Information"></a>Use of NIST Information
This document is provided as a public service by the National Institute of Standards and Technology (NIST). With the exception of material marked as copyrighted, information presented on these pages is considered public information and may be distributed or copied. Use of appropriate byline/photo/image credits is requested.
## <a name="Software Disclaimer"></a>Software Disclaimer
NIST-developed software is provided by NIST as a public service. You may use, copy and distribute copies of the software in any medium, provided that you keep intact this entire notice. You may improve, modify and create derivative works of the software or any portion of the software, and you may copy and distribute such modifications or works. Modified works should carry a notice stating that you changed the software and should note the date and nature of any such change. Please explicitly acknowledge the National Institute of Standards and Technology as the source of the software.
NIST-developed software is expressly provided "AS IS." NIST MAKES NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
You are solely responsible for determining the appropriateness of using and distributing the software and you assume all risks associated with its use, including but not limited to the risks and costs of program errors, compliance with applicable laws, damage to or loss of data, programs or equipment, and the unavailability or interruption of operation. This software is not intended to be used in any situation where a failure could cause risk of injury or damage to property. The software developed by NIST employees is not subject to copyright protection within the United States.
# <a name="Bibliography"></a>Bibliography
<table>
<tr>
<td>[1] <br></td>
<td>Wikipedia, "Web Ontology Language," [Online]. Available: https://en.wikipedia.org/wiki/Web_Ontology_Language. [Accessed 28 9 2015].<br></td>
</tr>
<tr>
<td>[2] <br></td>
<td>T. Kramer, B. Marks, C. Schlenoff, S. Balakirsky, Z. Kootbally and A. Pietromartir, "Software Tools for XML to OWL Translation," NISTIR, June 2015. [Online]. Available: http://nvlpubs.nist.gov/nistpubs/ir/2015/NIST.IR.8068.pdf. [Accessed 23 9 2015].<br></td>
</tr>
<tr>
<td>[3] <br></td>
<td>S. Balakirsky, T. Kramer, Z. Kootbally and A. Pietromartire, "Metrics and Test Methods for Industrial Kit Building," NIST Interagency/Internal Report (NISTIR) - 7942, Gaithersburg, MD, 2013.<br></td>
</tr>
<tr>
<td>[4] <br></td>
<td>B. Schäling, "The Boost C++ Libraries - I/O Services and I/O Objects," [Online]. Available: http://theboostcpplibraries.com/boost.asio-io-services-and-io-objects. [Accessed 24 9 2015].<br></td>
</tr>
<tr>
<td>[5] <br></td>
<td>I. Pyarali, T. Harrison, D. C. Schmidt and T. D. Jordan, "Proactor - An Object Behavioral Pattern for Demultiplexing and Dispatching Handlers for Asynchronous Events," in 4th Annual Pattern Languages of Programming, Allerton Park, Illinois, 1997. <br></td>
</tr>
<tr>
<td>[6] <br></td>
<td>T. Trautner, F. Pauker and B. Kittl, "ADVANCED MTCONNECT ASSET MANAGEMENT (AMAM)," in International Conference on Innovative Technologies, IN‐TECH 2016, Prague, 2016. <br></td>
</tr>
<tr>
<td>[7] <br></td>
<td>A. Larson, "Rethink's Baxter vs Universal Robots: Which Collaborative Robot is best for you?," 22 1 2013. [Online]. Available: cross-automation.com/blog/rethinks-baxter-vs-universal-robots-which-collaborative-robot-best-you. [Accessed 17 10 2017].<br></td>
</tr>
<tr>
<td>[8] <br></td>
<td>The World Wide Web Consortium, "XPath," [Online]. Available: http://www.w3.org/TR/xpath. [Accessed 25 9 2015].<br></td>
</tr>
<tr>
<td>[9] <br></td>
<td>Microsoft, "Microsoft XML Core Services (MSXML)," [Online]. Available: https://msdn.microsoft.com/en-us/library/ms763742(v=vs.85).aspx. [Accessed 25 9 2015].<br></td>
</tr>
<tr>
<td>[10] <br></td>
<td>Apache Software Foundation, "Apache Xerces C++," [Online]. Available: http://xerces.apache.org/. [Accessed 25 9 2015].<br></td>
</tr>
</table>



# <a name="Appendix I Java CRCL Simulator"></a>Appendix I Java CRCL Simulator
The GitHub project https://github.com/ros-industrial/crcl includes CRCL documentation, XML schema files and example instance files.
The following instructions are intended for a Linux PC and have been tested on Ubuntu 12.04 platform. The Java CRCL simulator project can be found on the master branch at github.com/wshackle/crcl4java, which if you clone into your home directory will create the folder crcl4java with all the GitHub files downloaded. If you change into the "crcl4java" folder, and execute "run.sh" you will start the simulator. At his point a simple GUI will pop up on your screen:

<div style="text-align: center;" markdown="1">

![Figure27](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image27.gif?format=raw)



</div>
<p align="center">
_Figure 9 CRCL Simulator into Screen_
</p>
At this point, click "Launch Simulated CRCL Server" and after a short pause you should see:

<div style="text-align: center;" markdown="1">

![Figure28](./CrclRobotAgentReadme_images/CrclRobotAgentReadme_image28.gif?format=raw)



</div>
<p align="center">
_Figure 10 CRCL Simulator_
</p>
Once you see this screen, you able to communicate to the TCP socket interface to the Java CRCL simulator.
As a prerequisite to running the CRCL Java simulator, you will need the latest Oracle Java distribution and not the latest Ubuntu Java distribution – since they differ. The version of Netbeans in the Ubuntu repositories will not work. Instead, use the instructions from website explaining how to install Oracle Java in Ubuntu:
http://www.webupd8.org/2012/09/install-oracle-java-8-in-ubuntu-via-ppa.html
**Oracle Jav** : Oracle Java is the proprietary, reference implementation for Java. This is no longer currently available in a supported Ubuntu repository. For more on this, please see here. However, one may install a supported version of Java (7 or 8) via Oracle's website, or via the WebUpd8 PPA.
To install the latest Java open a web browser and navigate to:
http://www.oracle.com/technetwork/java/javase/downloads/index.html 
Select JDK download (and saved) and then changed extension to bash and run as bash script. (Note: apt-get on ubuntu will get a version of the Java JDK that is too old unless you modify the sources that apt uses.)
