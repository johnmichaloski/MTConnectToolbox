

<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<CRCLCommandInstance>
  <CRCLCommand xsi:type="GetStatusType">
    <CommandID>0</CommandID>
  </CRCLCommand>
</CRCLCommandInstance>


FIXES:
had to add buffer to curent first, since only read entire buffer once!
two status messages could be read at once 
regex match of <StartingTag0_9 was tricky

Rufous: 129.6.32.176 Port 64444
telnet, open 29.6.32.176  64444 - Connects!

Does init set config- units, etc. Just once? What are they?
	- wont run without init() cmd
get health report when error
get version information
Why no firewall?



<CRCLStatus
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
  xsi:noNamespaceSchemaLocation="../xmlSchemas/CRCLStatus.xsd">
  <CommandStatus>
  <CommandID>1</CommandID>
  <StatusID>36169</StatusID>
  <CommandState>Done</CommandState>
  </CommandStatus>
  <JointStatuses>
  <JointStatus><JointNumber>1</JointNumber><JointPosition>200.0</JointPosition></JointStatus><JointStatus><JointNumber>2</JointNumber><JointPosition>0.0</JointPosition></JointStatus><JointStatus><JointNumber>3</JointNumber><JointPosition>0.0</JointPosition></JointStatus><JointStatus><JointNumber>4</JointNumber><JointPosition>0.0</JointPosition></JointStatus><JointStatus><JointNumber>5</JointNumber><JointPosition>0.0</JointPosition></JointStatus><JointStatus><JointNumber>6</JointNumber><JointPosition>0.0</JointPosition></JointStatus></JointStatuses>
  <Pose><Point><X>1.25</X><Y>0.0</Y><Z>0.00000000000000006123233995736766</Z></Point>
  <XAxis><I>1.0</I><J>0.0</J><K>0.0</K></XAxis>
  <ZAxis><I>0.0</I><J>0.0</J><K>1.0</K></ZAxis>
  </Pose></CRCLStatus>

  
std::string XMLClient::SyncRead()
{
	boost::system::error_code ec; 
	boost::asio::streambuf buf; 
	char reply[40];
	bool bFlag=false;
	while(!bFlag)
	{
		//size_t reply_length = boost::asio::read(_socket,
		//	boost::asio::buffer(reply, 40));
		//bFlag=BufferHandler(std::string(reply, reply_length));
		size_t reply_length = boost::asio::read_until(_socket,	buf, 0, ec);
		if(ec)
		{
			return "";
		}
		//std::string line=makeString(buf, reply_length); 
		const char* line=boost::asio::buffer_cast<const char*>(buf.data());
		buf.consume(reply_length); 
		bFlag=BufferHandler(line);

	}
	return LatestMsgQueue();
}
