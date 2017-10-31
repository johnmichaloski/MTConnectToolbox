
# Source Code Documentation 
----

Tuesday, October 31, 2017
This document describes some of the in-house developed source code used in deploying the UR MTConnect agent. The code was developed and compiled using Microsoft Visual C++ 2010. Since MSVC 2010 predates the C11 standard, many useful programming primitives (bind, mutex, thread) relied on the boost 1.54 library instead of the std library in C11.  The rationale for keeping MSVC 2010 is its support of the MSI deployment project, so that after the UR Agent was coded and tested, it could be packaged and deployed as a MSI install script. An MSI script is helpful in that you can download and install any runtime libraries that may be required by the application (for example, the C++ runtime for each version of MSVC).
Some caveats are in order. The release build did not operate correctly, although the Debug build operated correctly. In order to fix this, ALL optimization flags were turned off in the Release build and this fixed the problems. These optimization flags appear in the C++/C Optimization properties and include: 
 - /GL  Turn off whole program optimization
 - /O2 /Oy- which optimized the code for speed, size etc were changed to none.

Table of Contents
** 

# Struct Decoder
A common approach for real-time data communication is to use read/write of a TCP/IP socket which will be streaming raw data. Raw binary data is most efficient as it typically is a 1:1 representation.  Unfortunately handling binary data from network connections requires an understanding of the layout and elements of the destination buffer. Each element within the buffer need to be handled individually, as the element may require "byte swapping" to realize its true value.  There are many approaches to handling this serialization and deserialization of stream buffers. What is required for communication with a robot across a network is a socket streaming communication interface, a buffering technique, a deserialization from a stream into buffer elements and vice versa, where the deserialization may require byte swapping in order to work. In addition, handling of element arrays, dynamic sizing of the arrays, version control of the buffer element definitions, are among other requirements that would increase the utility of the streaming process.
The struct decoder performs streaming conversions between network byte arrays and C++ structs. The approach is straightforward as it relies on declaration macros in each buffer class and uses an inherited class to handle the decoding and other functions. First, all the structure variables are declared. Then, several preprocessor macros are used to define the size and order of the byte stream associated with this C++ struct so they can easily be read/write/print including byte swapping if required. It assumes the struct is packed with no padding between variable elements.   Implicit in this encoding process is the determination of the byte order of the host machine versus the prevailing network byte order (Big Endian. If the host byte order is not the same as network byte order, bytes are swapped to match network byte order.
The struct_decoder struct is useful for reading/writing network byte streams. It uses the preprocessor definition to define a property table describing order, printable name, size, printing format, array, array size pointer and minimum and maximum version compliance. The struct_decoder relies a printf specifier to format the variable for output, which conforms to the following table:
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
<TD><BR></TD>
</TR>
<TR>
<TD>%uc<BR></TD>
<TD>unsigned char<BR></TD>
<TD>1<BR></TD>
<TD><BR></TD>
</TR>
<TR>
<TD>%c<BR></TD>
<TD>bool<BR></TD>
<TD>1<BR></TD>
<TD><BR></TD>
</TR>
<TR>
<TD>%u<BR></TD>
<TD>short<BR></TD>
<TD>2<BR></TD>
<TD><BR></TD>
</TR>
<TR>
<TD>%hu<BR></TD>
<TD>unsigned short<BR></TD>
<TD>2<BR></TD>
<TD><BR></TD>
</TR>
<TR>
<TD>%d<BR></TD>
<TD>int<BR></TD>
<TD>4<BR></TD>
<TD><BR></TD>
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

Array serial properties use these printf to format each element in the array.
The "Curiously Recurring Template" pattern is used to assist in decoding by allowing access to the outermost serial property map. Thus, each structure that uses the serialization macro mechanism, inherits from "struct_decoder" template class, but provides itself as the template type. This is useful if the structure inherits from a structure that itself has a serial property map.

	struct ur_cartesian_info : public struct_decoder<ur_cartesian_info>

The "Curiously Recurring Template" is useful when you need to access the outer class variables or methods. This is used to get the outer serial property map defined in the class, as shown in the struct_decoder base class:For ou

	template <class T> struct struct_decoder 
	{
	. . .
	
	T * t = static_cast<T *>(this;
For the streaming utilities, access to the outer definition of the property map relies on the "Curiously Recurring Template". 

	PropertyMap *props = static_cast<T *>(this)->GetPropertyMap();
	

There are a collection of C++ macros that handle the building of the property map that are used by the  "Curiously Recurring Template"  struct_decoder. These include:
 - BEGIN_SERIAL_PROP_MAP (X) marks the beginning of the classes' serial property map. It creates a msgname class to return ASCII string containing the message  name  (denoted by the preprocessor X). It also declares a the beginning of a static method  GetPropertyMap(), which gives a reference pointer to the embedded static table of serialization variables, as defined below:
 	- property type - type of element definition
 	- Name -  name of element 
 	- offset of element in class hierarchy
 	- positive size of element either single or array
 	- flag to indicate array, where 1 is yes.
 	- size of one element in array
 	- string representing format of printing element
 	- pointer to int containing size of array
 	- min version where this element is defined
 	- max version where this element is defined

 - PROP_SERIAL_ENTRY (mName, pformat) defines a preprocessor macro that inserts an entry into the serial property map table. It expects an existing class member variable name, and uses the mName to save the name, sizeof, and offset of the variable from beginning of the class definition (including any inherited classes definitions). Pformat defines a printf like formatting string for use in the print struct_decoder method.
 - PROP_SERIAL_ENTRY_VERSION(mName, pformat, min, max)    defines a preprocessor macro that inserts an entry into the serial property map table, much like   PROP_SERIAL_ENTRY. However, this time it also enter a min and max version number in floating point (e.g., 3.4) into the serial property table. VMAX and VMIN are macros to indicate the limits of the versioning in either direction.
 - PROP_SERIAL_ENTRY_VARARRAY(mName, mSize, pformat, var) defines a preprocessor macro that inserts an array entry into the serial property map table. mName and pformat are used as int PROP_SERIAL_ENTRY. mSize defines the size of each element in the array, will the var entry defining a pointer to the variable defining the actual size of the array. For example, below DHa is the name of the variable, and element size is the sizeof(double), the format is "%lf" for a double, and the pointer to the defining array size is given by &numjoints. 

	PROP_SERIAL_ENTRY_VARARRAY(DHa, sizeof( double ), "%lf", &numjoints)
 Note, the max size of the array is given by the sizeof(DHa) so it must be allocated as a declared array size, not as a std::vector. Using this information, we can insure that the size given by numjoints does not exceed the maximum declared array size.

			double                       DHa[8];

 
 - END_SERIAL_PROP_MAP()  marks the end of the serial property map. The end is signaled by having a null name pointer.
 - PROP_SERIAL_ENTRY_BASE(mName) allows the current structure to reference an inherited structure with a serial property map of its variables. It is not used in the UR Agent implementation.
 
For example, the class ur_cartesian_info illustrates the use of the  inheritance, range of applicability using minimum and maximum verion numbers, use of pragma to insure variables are packed,  

	struct ur_cartesian_info : public struct_decoder<ur_cartesian_info>
	{
		float mVersion;
	
	#pragma pack(push, 1)
		double   X;
		double   Y;
		double   Z;
		double   Rx;
		double   Ry;
		double   Rz;
	
		double TCPOffsetX;
		double TCPOffsetY;
		double TCPOffsetZ;
		double TCPOffsetRx;
		double TCPOffsetRy;
		double TCPOffsetRz;
	#pragma pack(pop)
		BEGIN_SERIAL_PROP_MAP(ur_cartesian_info)
			PROP_SERIAL_ENTRY(X, "%lf")
			PROP_SERIAL_ENTRY(Y, "%lf")
			PROP_SERIAL_ENTRY(Z, "%lf")
			PROP_SERIAL_ENTRY(Rx, "%lf")
			PROP_SERIAL_ENTRY(Ry, "%lf")
			PROP_SERIAL_ENTRY(Rz, "%lf")
			PROP_SERIAL_ENTRY_VERSION(TCPOffsetX, "%lf",3.1,VMAX)
			PROP_SERIAL_ENTRY_VERSION(TCPOffsetY, "%lf",3.1,VMAX)
			PROP_SERIAL_ENTRY_VERSION(TCPOffsetZ, "%lf",3.1,VMAX)
			PROP_SERIAL_ENTRY_VERSION(TCPOffsetRx, "%lf",3.1,VMAX)
			PROP_SERIAL_ENTRY_VERSION(TCPOffsetRy, "%lf",3.1,VMAX)
			PROP_SERIAL_ENTRY_VERSION(TCPOffsetRz, "%lf",3.1,VMAX)
		END_SERIAL_PROP_MAP( )
	} ;

The defines the ur_cartesian_info structure and uses the stream buffering elements and related serialization information.  As mentioned, the macros to define the streaming information rely on the struct_decoder class to understand and interpret the definition. This association is done in C++ at the class declaration time by inheriting from the struct_decoder class and providing the current class as a template parameter:

	struct ur_cartesian_info : public struct_decoder<ur_cartesian_info>
	{
The mVersion variable must be inserted into every outer class so that multiple instances of the ur_cartesian_info struct can support multiple versions of the interface.  It is manually inserted so that programmers are aware that versioning is being done, and the version must be specified. Thus a 3.0 UR robot and a 3.4 UR robot can each use the ur_cartesian_info definition, and by putting in either 3.0 or 3.4, different interfaces are supported during decoding.

		float mVersion;

In the definition of ur_cartesian_info, the Microsoft C++ compiler pragma pack is used to ensure that the variables are slotted with their native size and no padding is performed. Thus, before the start of the variables to be decodes from a binary representation, the pragma is used:

	#pragma pack(push, 1)
		double   X;
	. . .
	
	#pragma pack(pop)
The serial property map for each structure must be defined to allow the decoding (read) and the encoding (write) of raw binary message. Implicit in this transcribing process is the determination of the byte order of the host machine, to determine whether it is the same as network byte order or not. If the byte order is not the same as network byte order, bytes are swapped to accommodate network byte order.

		BEGIN_SERIAL_PROP_MAP(ur_cartesian_info)
			PROP_SERIAL_ENTRY(X, "%lf")
	. . .
		END_SERIAL_PROP_MAP( )

Changes due to versioning in the ur_cartesian_info struct occur in version 3.1 So, in the property serial map specification, the version ranges from VMIN to 3.1 to VMAX. So, the version 3.0 would not support the TCPOffsetX variable, while the version 3.1 would. Below the X variable is declared in the preprocessor macro PROP_SERIAL_ENTRY where the version implicitly ranges from VMIN to VMAX:

	PROP_SERIAL_ENTRY(X, "%lf")

Below the TCPOffsetX variable is declared in the preprocessor macro PROP_SERIAL_ENTRY_VERSION to be viable from version 3.1 on:

	PROP_SERIAL_ENTRY_VERSION(TCPOffsetX, "%lf",3.1,VMAX)

Once the structure has been setup to be decoded (and encoded), raw messages from the UR robot can be unpacked into the structure. Assuming a socket reader has been coded, we assume a variable "buf" contains a raw binary message, with a given offset (generally including or not including header) and the variable len describing the length of the message.  The ur_robot_state::unpackRobotState accepts these parameters on the stack to decode the UR message, in the following code:

	void ur_robot_state::unpackRobotState (uint8_t *buf, unsigned int offset,
	                                   uint32_t len)
	{
	    // package_types
	    offset += 5;
	
	    while ( offset < len )
	    {
	        ur_message_header hdr;
	        hdr.read((char *) &buf[offset]);
	        switch ( hdr.packageType )
	        {
	        case package_types::CARTESIAN_INFO:
	            {
	                _ur_cartesian_info.read((char *) &buf[offset + hdr.msglength( )]);
	            }
	            break;. . .
	         }
	        offset += hdr.packageSize;
	    }

The header offset describing the outer message type and length (of length 5 bytes) is added to the offset within the message.

	    // package_types
	    offset += 5;
Then the header message describing the type and length of the message is decoded using the ur_message_header decoding "read" routine:

	        ur_message_header hdr;
	        hdr.read((char *) &buf[offset]);
Using the hdr packageType  can be used in a C++ switch statement to determine the type of the inner message:. 

	        switch ( hdr.packageType )
	        {
We will assume that a  ur_cartesian_info message is to be decoded.  Then the package_types::CARTESIAN_INFO case will be matched:

	        case package_types::CARTESIAN_INFO:
	            {
	                _ur_cartesian_info.read((char *) &buf[offset + hdr.msglength( )]);
	            }
	            break;

And then the ur_cartesian_info.read routine will read (i.e.,decode) the buffer starting at position buf+offset with message length equal to hdr.msglength( ) that is determined dynamically each time in consideration of a potential version and resulting message length change. 

# Logging 
There is limited logging capability within the UR Agent.  The logging facility is based on ROS Console open source code, which can be found at https://github.com/ros/console_bridge/blob/master/include/console_bridge/console.h. ROS provides interesting C++ Logging filtering macros (once, periodically, etc.) here: https://github.com/ros/ros_comm/blob/4383f8fad9550836137077ed1a7120e5d3e745de/tools/rosconsole/include/ros/console.h Although  the logging facility is very robust and can be extracted out of the ROS framework with a little effort, the code base was too extensive to justify the work. 
The logging is global in scope in that the all diagnostic log messages are output to the class GLogger. GLogger is defined as an instance of class CLogger. Within the instance, there are several flags which can be used to change the logging functionality. One flag is accessed by the method DebugConsole() , with 0 giving no "OuputDebugConsole" for windows, and 1 echoing the logging message to output. There is a level determined by the method DebugLevel() which determines the threshold of diagnostic activity, which is explained a bit later.  The flag Timestamping( ) determines whether a timestamp will be prepended to every diagnostic message. If Timestamping( )  is true,  timestamps will be appended. For example, the logStatus message with a timestamp is shown below:

	2017-10-24 09:23:57.0259 Start UR_ Agent
The   CLogger variable DebugFile() determines the filename in which log messages are appended.  By default the filename is debug.txt and the location of the file is in the execution application directory.  
Users of applications can indeed create different log file using CLogger and DebugFile(), however, one will be limited to the logging method: logmessage (const char *file, int line, LogLevel level, const char *fmt, ...) to log message. Plus, the macros described below only output to GLogger, which take advantage of the logmessage method.
The logging level [-1..5] determines the amount of logging that will be done. Each numeric value corresponds to a threshold where logging will be done.  0 corresponds to Fatal, 1 corresponds to Error, 2 corresponds to Warning, 3 corresponds to Inform, and 4 corresponds to Debug.
System logging threshold omit logging with a diagnostic level above the system level. For example a system logging level of 3 will omit debug message (logging level 5) 
 - logAbort(fmt, ...)  Something unrecoverable has happened. Application will terminate imeediately, and not gracefully. 
 - logFatal(fmt, ...)     Something catastrophic has happened, attempt to shut the application down.                                    
 - logStatus(fmt, ...)  Information that is provided regardless of logging level.
 - logError(fmt, ...)  Something serious (but recoverable) has gone wrong.
 - logWarn(fmt, ...) Information that the user may find alarming, and may affect the output of the application, but is part of the expected working of the system.
 - logInform(fmt, ...) Small amounts of information that may be useful to a user.
 - logDebug(fmt, ...)   Information that you never need to see if the system is working properly.

There is logging level of None but it is unclear if this is obeyed.
Below is an example of the use of the logDebug macro. It accepts a character string as format, and a variable length  of arguments follow determined by the format statement. It uses stdargs to handle the variable arguments. 


	#include "NIST/Logger.h"
	. . .
			logDebug("\tAdapter %s Server Rate=%d\n", mDevice.c_str(), mServerRate);
	. . .

There is a C++ macro LOG_ONCE(X) to limit the logging output to only once. For example, the previous code can be output once by the following code modification:

	#include "NIST/Logger.h"
	. . .
			LOG_ONCE (
	                         logDebug("\tAdapter %s Server Rate=%d\n", mDevice.c_str(), mServerRate);
	                       )
	. . .
There is also a  LOG_THROTTLE(secs, X) based on the ROS filtering strategies, this macro will limit the output to a log on a periodic basis, with the rate determined in seconds. Thus, in the example below, the diagnostic message output will be throttled to once per minute.  If the time between logging outputs is greater than 60 seconds, then this log message will be output, and the logging timer will be reset.

	#include "NIST/Logger.h"
	. . .
			LOG_ THROTTLE (60,
	                         logDebug("\tAdapter %s Server Rate=%d\n", mDevice.c_str(), mServerRate);
	                       )
	. . .

In order to access the logging facility, at the beginning of the program a small preamble of code is necessary to setup and configure the logger. Below is a minimal example:

			GLogger.Open(File.ExeDirectory() + "debug.txt");
			GLogger.DebugLevel()=5;
			GLogger.Timestamping()=true;

In this case, the output file name is found in the exe folder and named debug.txt. The default logging level is 5, although it was overridden  later by a config file value. Likewise timestamping is enabled. Note, the use of method access to modify the flags (ie.e., ()). This is due to the fact that all these accessors pass a reference to the actual logger flag. Historically, this was done because it is easier to override a method than a variable in C++.
# Ini File Software
The UR Agent relies on some in-house INI configuration file management that is not typical of normal MTConnect installations, which rely on YAML.  As an aside, if MTConnect had relied on JSON to provide configuration details, boost has a nice library. However, JSON is a compliant with YAML but not vice versa.
The INI file format is an informal standard for configuration based historically on the Microsoft. INI files are simple text files with a basic structure composed of sections, key/properties/tags, and values.
The basic element contained in an INI file is the key or property. Every key has a name and a value, delimited by an equals sign (=). The name appears to the left of the equals sign.

	name=value
For the UR Agent configuration, keys are be grouped into named sections. The section name appears on a line by itself, in square brackets ([ and ]). All keys after the section declaration are associated with that section. There is no explicit "end of section" delimiter; sections end at the next section declaration, or the end of the file. Sections may not be nested. The term key and property and tag are used interchangeably and describe the same INI file element.

	[section]
	key=value
	tag=value
The ini file software is managed by the class Config, and uses a simplist variant class (StringVariant) to manage the different configuration types and conversion. Note, for this implementation the ability to determine wheter string case in keys and section names is important was added. For this implementation, Config assumes section and key names are CASE INSENSITIVE.  This was found to be useful as Microsoft ini file functions  (e.g., WritePrivateProfileString) are case insensitive as well. 

The software operation of the ini file management is done in the agent on a global level and in each adapter on a local device level. They both use the same Config.ini file for reading configuration data.  The Config.ini filename is held in the variable Globals.mInifile and is tested for existence. A fatal error occurs if the file is not found, and the UR Agent logs an fatal message and terminates. If the file exists, each important key is read from a section. For the agent, this section name is "GLOBALS" and for each adapter, it is the name of the robot device (e.g., [marvin]). Then each key is read into the corresponding global or device variable. 

	void AgentConfigurationEx::initialize (int aArgc, const char *aArgv[])
	{
	    std::string cfgfile = Globals.mInifile;
	
	    if (File.Exists(cfgfile.c_str( ))
	    {
	        mConfig.load(cfgfile);
	
	        Globals.mServerName
	            = mConfig.GetSymbolValue<std::string>("GLOBALS.ServiceName", Globals.mServerName);
	         MTConnectService::setName(Globals.mServerName);
	. . .
	
	        mDevices = mConfig.GetTokens("GLOBALS.MTConnectDevice", ",");
	
First, the ini file must be read:

	        mConfig.load(cfgfile);
Once loaded, any key or section can be parsed. Each key/value pair is read, and then the method GetSymbolValue is a template method which uses the template type in order to convert the underlying Config variant type into the given type (in this example the template typename is std::string). Importantly for MTConnect, one can name the UR Agent service and then store this name into the MTConnect Institute core agent:

	        Globals.mServerName
	            = mConfig.GetSymbolValue<std::string>("GLOBALS.ServiceName", Globals.mServerName);
	         MTConnectService::setName(Globals.mServerName);
The Config file also has the method to parse a token list (e.g., comma separated values) and return a standard vector of string. Thus GetTokens reads the section "[GLOBALS]" and the key "MTConnectDevice" first as a key/value pair, and then parses the value using the "," comma separator. This mDevices  variable ultimately contains the list of devices for which an adapter must be created.

	        mDevices = mConfig.GetTokens("GLOBALS.MTConnectDevice", ",");
The Config class is simplistic way of dealing with configuration data, but only requires a modest amount of code, and most every programmer is familiar with ini files.
# Reset at Midnight 
MTConnect Agents are expected to operate continually 24/7 without fault. Because some underlying software technology may be problematics, e.g., memory leaks or consume memory with garbage collection so that performance degrades, it became apparent that the capability to reset the agent at midnight requiring a few seconds of down time, every night, cleared the system of the potential software downsides and was helpful in enhancing reliability. 
The CResetAtMidnightThread class was coded to handle the reset at midnight behavior. It uses the Microsoft CWorkerThread template from the ATL, which is a  class that creates a worker thread, waits on one or more kernel object handles, and executes a specified client function when one of the handles is signaled. The condition that the CWorkerThread waits on is a timer which expires at midnight. Upon expiring, the CWorkerThread runs and contains code to spawn a Process which reset the service (as given by the service name in the configuration file).

The heart of the WorkerThread declaration is given below:

		CWorkerThread<> _resetthread;
		struct CResetThread : public IWorkerThreadClient
		{
			HRESULT Execute(DWORD_PTR dwParam, HANDLE hObject);
			HRESULT CloseHandle(HANDLE){ ::CloseHandle(_hTimer); return S_OK; }
			HANDLE _hTimer;
		} _ResetThread;
The variable _resetthread is declared as a CWorkerThread with no template parameter to define further thread characteristics:

		CWorkerThread<> _resetthread;
Microsoft uses a leading I on a class definition to indicate that he class defines an interface, which is in fact similar to an abstract base class in C++. Then a CResetThread is declared which implements the required IWorkerThreadClient abstract methods defined in the interface. These include Execute and CloseHandle.  CloseHandle closes the thread handle upon exit.
The Execute function does the actual service restart. In order to run the reset thread, it must be initialzed and a timer is added to the implementation that waits until midnight before running.  Below is the complete code to start the _resetthread. We use the flag bResetAtMidnight  to insure that the reset at midnight operation has been enabled.

	inline bool CResetAtMidnightThread<T>::Start()
	{
		if(_bResetAtMidnight)
		{
			COleDateTime now = COleDateTime::GetCurrentTime();
			COleDateTime date2 =  COleDateTime(now.GetYear(), now.GetMonth(), now.GetDay(), 0, 0, 0) +  COleDateTimeSpan(1, 0, 0, 1);
	
			COleDateTimeSpan tilmidnight = date2-now;
			_resetthread.Initialize();
			_resetthread.AddTimer(
				(long) tilmidnight.GetTotalSeconds() * 1000,
				&_ResetThread,
				(DWORD_PTR) this,
				&_ResetThread._hTimer  // stored newly created timer handle
				) ;
			return true;
		}
		return false;
	}
The start code must compute the difference in time between now and midnight in order for the timer to fire correctly. To do this the classes COleDateTime and COleDateTimeSpan were used. COleDateTime has a static variable  GetCurrentTime() to retrieve the current time

			COleDateTime now = COleDateTime::GetCurrentTime();
Then the next day at one minute past midnight is computed. It takes the now timestamp, zeros out the hours, minutes and seconds, and adds one day and one mintue to the date-time. If we subtract now from the future midnight into a  COleDateTimeSpan, we get the amount of time until then.

			COleDateTime date2 =  COleDateTime(now.GetYear(), now.GetMonth(), now.GetDay(), 0, 0, 0) +  COleDateTimeSpan(1, 0, 0, 1);
	
			COleDateTimeSpan tilmidnight = date2-now;
After initializing the CWorkerThread in the  suspended state (by default). We use this COleDateTimeSpan time  tilmidnight as a timer (using addTimer method) to indicate when the thread will execute, and we pass in this CWorkerThread class definition and the this pointer to the instance of the CWorkerThread.

			_resetthread.Initialize();
			_resetthread.AddTimer(
				(long) tilmidnight.GetTotalSeconds() * 1000,
				&_ResetThread,
				(DWORD_PTR) this,
				&_ResetThread._hTimer  // stored newly created timer handle
				) ;
When the timer expires, the Execute method of the CWorkerThread will be executed. The "this" pointer is passed in as the calling parameter dwParam. We first save the "this" pointer, and then cancel the existing timer using its handle given in hObject. Then we use boilerplate windows process code to create a process that will cause the Service Control Manager (SCM) to reset the named service.

	template<typename T>
	inline  HRESULT CResetAtMidnightThread<T>::CResetThread::Execute(DWORD_PTR dwParam, HANDLE hObject)
	{
	
		T * service = (T*) dwParam;
		CancelWaitableTimer(hObject);
	
		try {
	
			PROCESS_INFORMATION pi;
			ZeroMemory( &pi, sizeof(pi) );
	
			STARTUPINFO si;
			ZeroMemory( &si, sizeof(si) );
			si.cb = sizeof(si);
			si.dwFlags = STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_HIDE;	 // set the window display to HIDE	
	
			// SCM reset command of this service 
			std::string cmd = StdStringFormat("cmd /c net stop \"%s\" & net start \"%s\"", service->name(), service->name());        // Command line
	
			if(!::CreateProcess( NULL,   // No module name (use command line)
				const_cast<char *>(cmd.c_str()),
				NULL,           // Process handle not inheritable
				NULL,           // Thread handle not inheritable
				FALSE,          // Set handle inheritance to FALSE
				0,              // No creation flags
				NULL,           // Use parent's environment block
				NULL,           // Use parent's starting directory 
				&si,            // Pointer to STARTUPINFO structure
				&pi ))           // Pointer to PROCESS_INFORMATION structure
				AtlTrace("CreateProcess FAIL ") ;
	
			::Sleep(5000); // make sure process has spawned before killing thread
		}
		catch(...)
		{
			service->Fatal("Exception  - ResetAtMidnightThread(void *oObject");
		}
		return S_OK;
	}
A string is built to execute in the process which starts with the cmd or DOS shell that call net stop of the service, and then uses net start of the same service.

	std::string cmd = StdStringFormat("cmd /c net stop \"%s\" & net start \"%s\"", service->name(), service->name()); 
This brief stop and start takes only seconds, however, seems to rid the system of software memory leaks, insistent memory consumption,  as well as general software malaise  at no real cost to the agent operation. This coding strategy can be used on Windows for exe application as well. Of note, a similar construct was not found in Linux. 

If you manually enter the reset at midnight command for agent.exe alone you get this exchange in a DOS command window:

	Microsoft Windows [Version 6.1.7601]
	Copyright (c) 2009 Microsoft Corporation.  All rights reserved.
	
	C:\Users\michalos>cmd /c taskkill /F /IM "agent.exe" & "C:\Users\michalos\Documents
	\GitHub\Agents\MTConnectAgentFromShdr\MTConnectAgentFromShdr\x64\Debug\agent.exe
	" debug
	SUCCESS: Sent termination signal to the process "agent.exe" with PID 11528.
	
	C:\Users\michalos>
Be careful, the agent says success and restarts a new agent. Note, the use of the /F force kill option. The use of the /T option kills the child processes which include the reset process – so don't use!
Please note that if you have enabled reset at midnight for an app exe agemt, after one reset you will need to manually kill the task (or you could change the ResetAtMidnight in the Config.ini but this is not suggested).
# MTConnect Software Versioning
There is a class CProductVersion to handle version identification for archival purposes. It is rather primitive.  CProductVersion  is class used by UR Agent that generates a string based on all constituent  components used in this agent.  The version information is useful for archiving what components were used to generated the code, if the source code is not available, or compatibilities issues arise.  The output of the version information is displayed as an extension in the MTConnect core agent. You would access the version information with the following sample URL (assuming you are on the agent machine and the agent port is listening to port 5000): http://127.0.0.1:5000/version. The following non-XML but HTML table would be returned:
<TABLE>
<TR>
<TD>Version<BR></TD>
</TR>
<TR>
<TD>MTConnect Core Agent Version 1.2.0.0 - built on Tue Oct 24 08:30:22 2017<BR></TD>
</TR>
<TR>
<TD>UR_ Agent Extensions 64 bit Platform Version 1.3.0.60 - built on Tue Oct 24 08:30:22 2017<BR></TD>
</TR>
<TR>
<TD>UR_ Agent MSI Install Version 1.0.3<BR></TD>
</TR>
<TR>
<TD>Windows Version : Microsoft Windows 7 Enterprise Edition Service Pack 1 (build 7601), 64-bit<BR></TD>
</TR>
<TR>
<TD>Microsoft Visual C++ Version Visual Studio 2010 - MSVC++ 10.0<BR></TD>
</TR>
<TR>
<TD>XML Lib version 2.7.7<BR></TD>
</TR>
<TR>
<TD>Boost libraries 1.54.0<BR></TD>
</TR>
<TR>
<TD>UR_ Devices = Woodsy<BR></TD>
</TR>
</TABLE>

Incorporating the version information into the MTConnect agent requires overloading the virtual functions handleCall  and handleExtensionCall of the core Agent. (Did this require some modification to the Agent code also?)

	 class AgentConfiguration : public MTConnectService {
	. . .
	  Agent *getAgent() { return mAgent; }
	  void setAgent( Agent * agent) { mAgent=agent; }



	class AgentEx : public Agent
	{
	public:
	    AgentEx( ) { }
	protected:
	    CProductVersion version;
	#ifdef HANDLECALL
	    virtual std::string handleCall (ostream & out, const string & path,
	                                    const key_value_map & queries,
	                                    const string & call, const string & device)
	#else
	    virtual std::string handleExtensionCall (const std::string & call,
	                                             const std::string & device)
	#endif
	    {
	        if ( call == "version" )
	        {
	            std::string html;
	            std::string v = "<versions><version>" + getVersionString( );
	            ReplaceAll(v, "\n", "</version>\n<version>");
	            v = v.substr(0, v.size( ) - std::string("<version>").size( ));
	            v = v + "</versions>";
	            logTrace(v.c_str( ));
	            html = version.GenerateVersionTable(v);
	
	            return html;
	        }
	        else if ( call == "documentation" )
	        {
	            std::string html = version.GenerateXSLTVersion(
	                "<versions><version>" + version.GenerateVersionDocument( )
	                + "</version></versions>");
	            return html;
	        }
	#ifdef HANDLECALL
	        return Agent::handleCall(out, path, queries, call, device);
	#else
	        return "";
	#endif
	    }
	
	    std::string getVersionString ( )
	    {
	        std::string str;
	
	        str += StdStringFormat(
	            "MTConnect Core Agent Version %d.%d.%d.%d - built on " __TIMESTAMP__
	            "\n",
	            AGENT_VERSION_MAJOR, AGENT_VERSION_MINOR, AGENT_VERSION_PATCH,
	            AGENT_VERSION_BUILD);
	#ifdef WIN64
	        std::string compilearch = "64 bit";
	#else
	        std::string compilearch = "32 bit";
	#endif
	        str += StdStringFormat("UR_ Agent Extensions  %s Platform Version %s - "
	                               "built on  " __TIMESTAMP__ "\n",
	                               compilearch.c_str( ), STRPRODUCTVER);
	        str += StdStringFormat(
	            "UR_ Agent MSI Install Version %s  \n",
	            version
	                .GetUR_InstallVersion(
	                "C:\\Users\\michalos\\Documents\\GitHub\\Agents\\UR_"
	                "MTConnectAgent\\UR_SetupX64\\UR_SetupX64.vdproj")
	                .c_str( ));
	        str += StdStringFormat("Windows Version : %s\n",
	                               version.GetOSDisplayString( ).c_str( ));
	        str += StdStringFormat("Microsoft Visual C++ Version %s\n",
	                               version.GetMSVCVersion(_MSC_VER));
	
	        //		str+=StdStringFormat("Dlib version %d.%d dated %s \n",
	        // DLIB_MAJOR_VERSION , DLIB_MINOR_VERSION,DLIB_DATE);
	        // str+=StdStringFormat("Dlib version %d.%d\n", 17,47);
	        str += StdStringFormat("XML Lib version %s \n", LIBXML_DOTTED_VERSION);
			str += StdStringFormat("Boost libraries %d.%d.%d \n",
				BOOST_VERSION / 100000    ,
				BOOST_VERSION / 100 % 1000 ,  // min. version
				BOOST_VERSION % 100   );        // patch version
	        str += StdStringFormat("UR_ Devices = %s\n", getAllDevices( ).c_str( ));
	        return str;
	    }
	
	    // This is here becuase the mDeviceMap variable member of Agent is protected,
	    // and its easier to just access here than in CProductVersion
		std::string getAllDevices ( )
		{
			std::string d;
			for(size_t i=0; i < mDevices.size() ; i++)
			{
				if (i>0)
				{
					d += ",";
				}
				d += mDevices[i]->getName();
			}
			return d;
		}
	};
	

# Visual Studio Versioning
Unfortunately, Microsoft Visual C++ (among other IDEs) does not have an option to auto increment the build version of each compile. So an in-house solution was coded up based on many suggestions, hints, insights found on the Internet. So this section describes a build  macro that is integrated into Visual Studio  2010 that automatically increments the build integer of a version.  The solution is not very slick, as there are better, larger, more complex solutions on the internet. So,  it is still incumbent on the programmer to manually edit the major, minor and release version numbers.
In this approach the version information which must dovetail into MSVC rc file which contains version information that is embedded into exe, dll or lib. The version information includes the variables: FILEVERSION, PRODUCTVERSION, STRFILEVER, and STRPRODUCTVER. The first two variables are comma separated ascii numbers while the last two are period separated combination of version variables into a string. Thus, the file MTConnectAgentFromShdr.rc contains:

	VS_VERSION_INFO VERSIONINFO
	 FILEVERSION 1, 3, 0, 60
	 PRODUCTVERSION 1, 3, 0, 60
	 . . .
	            VALUE "FileVersion", "1.3.0.60\0"
	 . . . 
	            VALUE "ProductVersion", "1.3.0.60\0"


The first step is to replace the hard-coded version numbers and string with macro variables:

	VS_VERSION_INFO VERSIONINFO
	 FILEVERSION FILEVER
	 PRODUCTVERSION PRODUCTVER
	 . . .
	            VALUE "FileVersion", STRFILEVER
	 . . . 
	            VALUE "ProductVersion", STRPRODUCTVER
Then we could use a file with #define of these macros to modify the version numbers.

	#define FILEVER          1, 3, 0, 60
	#define PRODUCTVER       1, 3, 0, 60
	#define STRFILEVER       "1.3.0.60\0"
	#define STRPRODUCTVER    "1.3.0.60\0"

Instead, a file Version_info.h is created that contains more macros and the ability to generate all the version information dynamically when loaded into the rc file.

	
	#define VERSION_MAJOR 1
	#define VERSION_MINOR 3
	#define VERSION_REVISION 0
	#define VERSION_BUILD 151
	#ifdef _UNICODE
	#define _T(x)      L ## x
	#else
	#define _T(x)      x
	#endif 
	#define STRINGIZE2(s) _T(#s)
	#define STRINGIZE(s) STRINGIZE2(s)
	#define PRODUCTNAME     "MTConnect Agent\0"
	#define PRODUCTVER      VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, VERSION_BUILD
	#define FILEVER         PRODUCTVER
	#define STRPRODUCTVER        STRINGIZE(VERSION_MAJOR)            \
	                      _T(".") STRINGIZE(VERSION_MINOR)    \
	                       _T(".") STRINGIZE(VERSION_REVISION) \
	                        _T(".") STRINGIZE(VERSION_BUILD)    
	#define STRFILEVER      STRPRODUCTVER

At the same time, the rc file (e.g., MTConnectAgentFromShdr.rc) now has the following include file at the beginning of the file, that uses the dynamically created variables: FILEVERSION, PRODUCTVERSION, STRFILEVER, and STRPRODUCTVER.


	// Microsoft Visual C++ generated resource script.
	//
	#include "version_info.h"  // Generated above
	#define APSTUDIO_READONLY_SYMBOLS
	/////////////////////////////////////////////////////////////////////////////
	//
	// Generated from the TEXTINCLUDE 2 resource.
	//
	#include "afxres.h"
	
	/////////////////////////////////////////////////////////////////////////////
	#undef APSTUDIO_READONLY_SYMBOLS
	
	/////////////////////////////////////////////////////////////////////////////
	// English (United States) resources
	
	#if !defined(AFX_RESOURCE_DLL) || defined(AFX_TARG_ENU)
	LANGUAGE LANG_ENGLISH, SUBLANG_ENGLISH_US
	
	/////////////////////////////////////////////////////////////////////////////
	//
	// Version
	//
	
	VS_VERSION_INFO VERSIONINFO
	 FILEVERSION FILEVER
	 PRODUCTVERSION PRODUCTVER
	 FILEFLAGSMASK 0x3fL
	#ifdef _DEBUG
	 FILEFLAGS 0x1L
	#else
	 FILEFLAGS 0x0L
	#endif
	 FILEOS 0x40004L
	 FILETYPE 0x1L
	 FILESUBTYPE 0x0L
	BEGIN
	    BLOCK "StringFileInfo"
	    BEGIN
	        BLOCK "040904b0"
	        BEGIN
	            VALUE "CompanyName", "NIST Adaptation of MTConnect Institute Open Source Agent"
	            VALUE "FileDescription", "Agent Resource File with Version Information"
	            VALUE "FileVersion", STRFILEVER
	            VALUE "InternalName", "Agent.exe\0"
	            VALUE "\0"
	            VALUE "MTConnectAgent", "agent.exe\0"
	            VALUE "ProductName", PRODUCTNAME
	            VALUE "ProductVersion", STRPRODUCTVER
	        END
	    END
	    BLOCK "VarFileInfo"
	    BEGIN
	        VALUE "Translation", 0x409, 1200
	    END
	END
	
	#endif    // English (United States) resources

There is a batch file that will read and modify the version_info.h file, called BuildUpdate.vbs. It is a vb script and can be run as a command line without elevated privileges.  The script starts a vbs scripting file object to read the version_info.h file (supplied as a command line argument or must be hard-coded as shown below in italic font to a physical location on the disk). The script reads the file and searches for the "define VERSION_BUILD" string and then reads the following number as an integer, increments the number and writes the new number as a string. At the end, the every line is appended to the filestring variable. When done reading the file, filestring is written to the file version_info.h for future use in the rc file.

	dim objFS, objArgs, strFile, objFile, line, buildno, filestring
	
	Set objFS = CreateObject("Scripting.FileSystemObject")
	
	if WScript.Arguments.Count > 0 then
	               Set objArgs = WScript.Arguments
	               strFile= objArgs(0)
	else
	               strFile = "C:\Users\michalos\Documents\GitHub\MTConnectSolutions\MTConnectAgentFromShdr\MTConnectAgentFromShdr\version_info.h"
	end if
	
	Set objFile = objFS.OpenTextFile(strFile)
	
	do while not objFile.AtEndOfStream 
	    line =  objFile.ReadLine()
	
	   if instr(1, line, "define VERSION_BUILD") then
	       line = Trim(Mid(line, Len("#define VERSION_BUILD") + 1))
	       buildno = CInt(line) +1 
	       line = "#define VERSION_BUILD " & CStr(buildno)
	    end if 
	
	      filestring = filestring & line & vbCRLF
	loop
	
	objFile.Close
	
	'' Output file with updated version number
	
	set objFile = objFS.CreateTextFile(strFile, 2)
	objFile.Write filestring 
	objFile.Close
Then you integrate the vbs script into MSVC IDE in the Build Event Configuration Properties. This is done by done by modifying the Configuration Property: Configuration->Build Event-> Pre-build Event to now run the following command line:

	$(ProjectDire)\Buildupdate.vbs Version_info.h 
The following screen shot shows in more detail the inclusion of the BuildUpdate.vbs  (found in the same directory as the vcxproj file) as:

![Figure1](./images/UR_AgentSourceCodeDoc_image1.gif)


Admittedly, this is not a very good solution. This is a standalone kludgy solution,  and not even integrated into the install script. Other solutions exist on the internet, for example, an SVN solution can be found here: https://stackoverflow.com/questions/638053/how-to-increment-visual-studio-build-number-using-c#638100 .But SVN wasn't used in my development process.
The documentation is useful only for the fact that it shows where the various components of an automated versioning system would be required to modify.

# Agent Singleton
One of the more pernious problems that can happen to a MTConnect deployer, is the occasion where two agents are running at the same time. This circumstance can happen easily especially if one uses the RunAgent.bat command file and click X to terminate the command window, and expect the agent running inside the shell to also be terminated. However, often this is not the case. So two agents can end of up running and the symptoms are often hard to detect, especially as integration of a remote adapter, through a firewall may mislead a user into thinking the root cause of the error might be the device adapter, as opposed to multiple running agents.
The symptoms of multiple agents manisfest similar to an unconnected agent to an adapter – all data items read UNAVAILABLE. There are a few tricks you can do to determine if the cause of the all UNAVAILABLES are from the adapter if it's a SHDR. You can open ta telnet session and open 127.0.0.1 7878 (assuming you installed telnet on your windows PC) and see if SHDR data updates are streaming to the telnet window. If you do see data, and still get all UNAVAILABLES, you most likely have multiple agents running with one agent handling the Http requests, but unable to read the SHDR. You would think multiple agents could read the SHDR, and in fact should, but often problems occur.

If you add the MTConnectSingleton class to the derived AgentConfigurationEx declaration, you can easily test for and remove other agents with the same process name and agent port (you could have multiple processes running with different HTTP ports that should not collide in any regard).  Below illustrates a simple inclusion of the  MTConnectSingleton  to the inheritance tree:
class AgentConfigurationEx :
public AgentConfiguration
, public MTConnectSingleton<AgentConfigurationEx>
{
Then adding a configuration flag in the Config.ini file allows  toggling of the singleton detection capability:

	config.bSingleton= cfg.GetSymbolValue("GLOBALS.Singleton", "0").toNumber<int>();
	if(config.bSingleton)
	  config.KillAllOtherInstances();
If you indeed wish to detect multiple agents colliding, you can set the flag to one and the method  KillAllOtherInstances will terminate any colliding agents using the same Http.
The KillAllOtherInstances  handles the collation of the pids for matching process names to the agnet (but not to agents with different names as this would require further configuration information to detect). The  routine first gets the pid of the current agent, its port number from the agent.cfg using a Yaml parser, and then checks each matching running  process for a module name match and the same http  If so, the pid is used to terminate the offending agent process. A very engaging message is logged as to the termination activity, but no other notification is provided. It is a bit like trading evils.

	void KillAllOtherInstances()
	{
		if(!bSingleton)
			return;
		std::vector<std::string>  filepaths;
		DWORD  pid = GetPid();
		std::string modulename = File.ExeDirectory();  // f
		std::string modulepath = File.ExePath();;
		std::string processname  = File.Filename(modulepath);
		std::vector<DWORD> pids = GetModules(processname,  filepaths);
		int portnum = GetPortNum(modulename);
		for(size_t i=0; i<pids.size(); i++)
		{
			if(pid == pids[i])
				continue;
			bool b = IsSameAgent(filepaths[i], portnum);
			if(b)
			{
	                // Extreme log file notice.
			   logStatus("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");                        
	                logStatus("Conflict with another MTConnect Agent ... same http port nmber\n");
		         logStatus("Terminated process %s\n", filepaths[i].c_str());
	                logStatus("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	
			   TerminateAgentProcess(pids[i], 0);
			}
		}
	}

# YAML Reader
A YAML reader was coded based on boost property tree to read MTConnect agent configuration files. This was required to read the port number so that an executing agent could be determined to be a singleton if its process name (exe) and port number were unique. If not, in the singleton model, the duplicated agent process was killed.
YAML (Yet another markup language) is a human-readable data serialization language. It is commonly used for configuration files, and in fact,  agent configuration file within the MTConnect Institute open source agent uses YAML format. A sample Agent yaml configuration file is:

	Devices = Devices.xml 
	ServiceName = UR_Agent
	Port = 5000
	CheckpointFrequency=10000
	AllowPut=true
	Adapters 
	{
	 Mazak_1 
	 { 
	    Device = Mazak101
	    Host =  192.168.0.11
	    Port = 7878 
	  } 
	  
	Mazak_2 
	 { 
	    Device = Mazak102
	    Host =  192.168.0.12
	    Port = 7878
	  } 
	  
	}
	logger_config
	{
		logging_level = debug
		output = cout
	}
The YAML configuration file is a tree that has numerous implementation, but is not quite JSON, so it requires its own library or other mechanism to parse and retrieve settings. (No writing of YAML files was considered.) Dlib has the a Yaml parsing mechanism, but requires all of dlib be included, while YamlReader is standalone – if you consider boost standalone!  So, the class YamlRead was developed that reads and parses an MTConnect agent configuration file (typically agent.cfg) using Boost spirit classic.  If you have a syntax tree (which was found), translating  the syntax tree into boost spirit is not difficult. 
Users will parse an MTConnect agent cfg file into an object oriented (OO) naming scheme (e.g., GLOBAL.section1.key). The uppermost section (assumes no section has been declared) is called GLOBAL. Each section under the GLOBAL is GLOBAL.section1, GLOBAL.section2, etc. Embedded sections within a section are OO concatenated (e.g., GLOBAL.section1.section11). 
Parsing an agent.cfg file is straightforward. One declares a YamlRead, and then loads the yaml configuration from file:

	YamlReader  yaml;
	
	std::string filename = "Agent.cfg";
	yaml.LoadFromFile(filename);
After parsing the tree, access to elements uses the object oriented tree naming strategy described above, so that to find the agent port number one uses the name  "GLOBAL.Port" 

	std::string sportnum = yaml.Find("GLOBAL.Port");
For convenience, YamlReader also has a conversion utility to help in string conversions. For example, below the port number string is converted into an integer, with a default value supplied as zero:

	Int portnum= yaml.Convert<int>( sportnum,0);
	
This source has not been thoroughly tested.
# DISCLAIMER
## Use of NIST Information
This document is provided as a public service by the National Institute of Standards and Technology (NIST). With the exception of material marked as copyrighted, information presented on these pages is considered public information and may be distributed or copied. Use of appropriate byline/photo/image credits is requested.
## Software Disclaimer
NIST-developed software is provided by NIST as a public service. You may use, copy and distribute copies of the software in any medium, provided that you keep intact this entire notice. You may improve, modify and create derivative works of the software or any portion of the software, and you may copy and distribute such modifications or works. Modified works should carry a notice stating that you changed the software and should note the date and nature of any such change. Please explicitly acknowledge the National Institute of Standards and Technology as the source of the software.
NIST-developed software is expressly provided "AS IS." NIST MAKES NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
You are solely responsible for determining the appropriateness of using and distributing the software and you assume all risks associated with its use, including but not limited to the risks and costs of program errors, compliance with applicable laws, damage to or loss of data, programs or equipment, and the unavailability or interruption of operation. This software is not intended to be used in any situation where a failure could cause risk of injury or damage to property. The software developed by NIST employees is not subject to copyright protection within the United States.

