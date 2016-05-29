//
// CrclInterface.cpp
//

#include <iostream>
#include <fstream>
#include <sstream>  

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>

#include "CRCLStatus.hxx"
#include "DataPrimitives.hxx"
#include "CRCLCommands.hxx"
#include "CRCLCommandInstance.hxx"

#include "CrclInterface.h"

#ifdef __linux__
#include "LinuxGlobals.h"
#endif

using namespace xsd;
using namespace xml_schema;

#define CLEANSTORE(Y,X,Z) try{ Y=X; } catch(...) { Y=Z; }

std::string CrclInterface::CRCLGetStatusCmd()
{
	return 
		"<?xml version=\"1.0\" encoding=\"UTF-8\"  ?> \n"
		"<CRCLCommandInstance 	xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
		"<CRCLCommand xsi:type=\"GetStatusType\">\n"
		"<CommandID>1</CommandID>\n"
		"</CRCLCommand>\n"
		"</CRCLCommandInstance>";
}

std::string CrclInterface::CRCLInitCanonCmd()
{
	return 
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<CRCLCommandInstance\n"
		"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" >\n"
		"  <CRCLCommand xsi:type=\"InitCanonType\">\n"
		"    <CommandID>1</CommandID>\n"
		"  </CRCLCommand>\n"
		"</CRCLCommandInstance>\n";
}

void CrclInterface::ParseCRCLStatus(std::string filename)
{
	std::string contents;
	Globals::ReadFile(filename,  contents);
	ParseCRCLStatusString(contents);
}
void CrclInterface::ParseCRCLStatusString(std::string str)
{
	try {
		std::istringstream istr (str);
		std::auto_ptr<CRCLStatusType> crclstat (
			CRCLStatus (istr, xml_schema::flags::dont_initialize|xml_schema::flags::dont_validate|xml_schema::flags::keep_dom)
			);
		//xercesc::DOMElement* e = static_cast<xercesc::DOMElement*> ((*crclstat)._node ());
		CLEANSTORE(_status.CommandID, crclstat->CommandStatus().CommandID (), 0) ;
		CLEANSTORE(_status.StatusID, crclstat->CommandStatus().StatusID (), 0) ;
		CLEANSTORE(_status.CommandState, crclstat->CommandStatus().CommandState (), "") ;
		CLEANSTORE(_status.Point[0], crclstat->Pose()->Point().X(), 0)  ;
		CLEANSTORE(_status.Point[1], crclstat->Pose()->Point().Y(), 0)  ;
		CLEANSTORE(_status.Point[2], crclstat->Pose()->Point().Z(), 0)  ;
		CLEANSTORE(_status.XAxis[0], crclstat->Pose()->XAxis().I(), 0)  ;
		CLEANSTORE(_status.XAxis[1], crclstat->Pose()->XAxis().J(), 0)  ;
		CLEANSTORE(_status.XAxis[2], crclstat->Pose()->XAxis().K(), 0)  ;
		CLEANSTORE(_status.ZAxis[0], crclstat->Pose()->ZAxis().I(), 0)  ;
		CLEANSTORE(_status.ZAxis[1], crclstat->Pose()->ZAxis().J(), 0)  ;
		CLEANSTORE(_status.ZAxis[2], crclstat->Pose()->ZAxis().K(), 0)  ;
		//CLEANSTORE(_status.Alarm, crclstat->CommandStatus().CommandState (), "") ;
		_status.Dump();
	}
	catch (const xml_schema::exception& e)
	{
		Globals::ErrorMessage(e.what()); 
	}
	catch(...)
	{
		Globals::ErrorMessage("ParseCRCLStatusString error\n");
	}

}
void CrclInterface::SerializeCRCLStatus(std::string filename)
{
	CRCLStatusType stat(CommandStatusType(  2,1,"Working"));

	//cmd->JointStatuses(::xsd::qif2::JointStatuses_type()); //
	stat.Pose (::PoseType(::PointType(1.2,0.0, 0.2),VectorType(1.0,0.0,0.0), VectorType(1.0,0.0,0.0) )); //XAxis_type,  ZAxis_type
	//cmd->GripperStatus (::xsd::qif2::GripperStatus_type());

	/**
	<QIFDocument xmlns="http://qifstandards.org/xsd/qif2" idMax="86" versionQIF="2.0.0" 
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
	xsi:schemaLocation="http://qifstandards.org/xsd/qif2 ../QIFApplications/QIFDocument.xsd">
	*/
	xml_schema::namespace_infomap map;

	//map[""].name = "http://www.w3.org/2001/XMLSchema";
	//map[""].schema = "http://www.w3.org/2001/XMLSchema ../CRCLStatus.xsd";
	std::ofstream ofs(filename.c_str(), std::ofstream::out);
	CRCLStatus (ofs, //std::cout,
		stat,
		map,
		"UTF-8",
		xml_schema::flags::dont_initialize);
	ofs.close();
}
//http://wiki.codesynthesis.com/Tree/Customization_guide
std::string CrclInterface::SerializeCRCLGetStatusCmd(int cmdnum)
{
	std::stringstream ss;
	::std::auto_ptr<GetStatusType> getstat(new GetStatusType(cmdnum));
	CRCLCommandInstanceType  cmd((::std::auto_ptr< CRCLCommandType >&) getstat);

	xml_schema::namespace_infomap map;
	CRCLCommandInstance(ss,
		cmd,
		map,
		"UTF-8",
		xml_schema::flags::dont_initialize);

	return ss.str();
}

/**
	asio read socket
	http://stackoverflow.com/questions/4933610/boostasio-async-read-guarantee-all-bytes-are-read
	http://www.tagwith.com/question_285175_when-do-i-call-boostasiostreambufconsume-and-boostasiostreambufcom
	http://www.dahuatu.com/jdyjaPxKWQ.html
	https://issues.apache.org/jira/browse/THRIFT-311
	http://pastebin.com/Li3wbpvu

	http://geekswithblogs.net/JoshReuben/archive/2014/11/17/c-boost-in-a-nutshell.aspx
	http://pastebin.com/YXk9stVA
	http://thisthread.blogspot.com/2013/09/simple-asio-tcp-clientserver-example.html
	http://dolinked.com/questions/4792923/uninitialized-read-error-on-asio-tcp-socket
	http://onnerby.se/~daniel/mC2/trunk/docs/core/html/_socket_8h-source.html
	http://en.pudn.com/downloads245/sourcecode/internet/proxy/detail1143109_en.html
	http://www.gamedev.net/topic/566670-c-implementing-ssl/
	http://cpp.knowcoding.com/view/20682-tcp-socket.html
	Buffering:

	http://stackoverflow.com/questions/4294651/sending-an-xml-message-in-parts-through-a-tcp-socket-using-qt
	http://www.codesynthesis.com/pipermail/xsde-users/2014-January/000631.html

	Raw:
	http://www.bogotobogo.com/cplusplus/sockets_server_client.php

*/