//
// CrclInterface.cpp
//

#include "stdafx.h"
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
#define _USE_MATH_DEFINES 
#include "math.h"

#include <algorithm>
#include <vector>
#include <boost/numeric/ublas/storage.hpp>
//http://docs.ros.org/jade/api/tf/html/c++/Scalar_8h.html
#define 	TFSIMD_PI   M_PI_2
#define TFSIMD_2_PI   M_PI
 
#include "WinGlobals.h"
#include "NIST/File.h"

using namespace xsd;
using namespace xml_schema;
using namespace boost::numeric::ublas;


#define CLEANSTORE(Y,X,Z) try{ Y=X; } catch(...) { Y=Z; }
/**
	xercesc::XMLPlatformUtils::Initialize();
	CrclInterface crcl;
	std::string text = crcl.CRCLGetStatusCmd();
	xercesc::XMLPlatformUtils::Terminate ();
*/
std::string CrclInterface::CRCLGetStatusCmd()
{
#if 0
	return 
		"<?xml version=\"1.0\" encoding=\"UTF-8\"  ?> \n"
		"<CRCLCommandInstance 	xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
		"<CRCLCommand xsi:type=\"GetStatusType\">\n"
		"<CommandID>1</CommandID>\n"
		"</CRCLCommand>\n"
		"</CRCLCommandInstance>";
#else
	GetStatusType cmd(++_commandnum);
	xml_schema::namespace_infomap map;

	//map[""].name = "http://www.w3.org/2001/XMLSchema";
	//map[""].schema = "http://www.w3.org/2001/XMLSchema ../CRCLStatus.xsd";
	 std::ostringstream  strfs;
	CRCLCommandInstance (strfs, //std::cout,
		(CRCLCommandType &) cmd,
		map,
		"UTF-8",
		xml_schema::flags::dont_initialize);
	return strfs.str();
#endif
}
// http://docs.ros.org/api/tf/html/c++/Matrix3x3_8h_source.html
void getEulerYPR(matrix<double> m, double& yaw, double& pitch, double& roll, unsigned int solution_number = 1) 
{
	struct Euler
	{
		double yaw;
		double pitch;
		double roll;
	};

	Euler euler_out;
	Euler euler_out2; //second solution

	// Check that pitch is not at a singularity
	// Check that pitch is not at a singularity
	if (fabs(m(2,0) ) >= 1)
	{
		euler_out.yaw = 0;
		euler_out2.yaw = 0;

		// From difference of angles formula
		if (m(2,0) < 0)  //gimbal locked down
		{
			double delta = atan2(m(0,1), m(0,2)); 
			euler_out.pitch = TFSIMD_PI / double(2.0);
			euler_out2.pitch = TFSIMD_PI / double(2.0);
			euler_out.roll = delta;
			euler_out2.roll = delta;
		}
		{
			double delta = atan2(-m(0,1), -m(0,2)); 
			euler_out.pitch = -TFSIMD_PI / double(2.0);
			euler_out2.pitch = -TFSIMD_PI / double(2.0);
			euler_out.roll = delta;
			euler_out2.roll = delta;
		}
	}
	else
	{
		euler_out.pitch = - sin(m(2,0)); 
		euler_out2.pitch = TFSIMD_PI - euler_out.pitch;

		euler_out.roll = atan2(m(2,1)  / cos(euler_out.pitch), 
			m(2,2) / cos(euler_out.pitch));
		euler_out2.roll = atan2(m(2,1)  / cos(euler_out2.pitch), 
			m(2,2) / cos(euler_out2.pitch));

		euler_out.yaw = atan2(m(1,0)  / cos(euler_out.pitch), 
			m(0,0)  / cos(euler_out.pitch));
		euler_out2.yaw = atan2(m(1,0)  / cos(euler_out2.pitch), 
			m(0,0) / cos(euler_out2.pitch));
	}

	if (solution_number == 1)
	{ 
		yaw = euler_out.yaw; 
		pitch = euler_out.pitch;
		roll = euler_out.roll;
	}
	else
	{ 
		yaw = euler_out2.yaw; 
		pitch = euler_out2.pitch;
		roll = euler_out2.roll;
	}
}

void getRPY(matrix<double> m, double& roll, double& pitch, double& yaw, unsigned int solution_number = 1) 
{
	getEulerYPR(m, yaw, pitch, roll, solution_number);
}

matrix<double> GetQvmRotMatrix( VectorType Xrot,  VectorType Zrot) 
{

	// From Will Shackleford code:
	// https://github.com/usnistgov/crcl/blob/master/tools/java/crcl4java/crcl4java-utils/src/main/java/crcl/utils/CRCLPosemath.java
	// VectorType yAxis = cross(poseIn.getZAxis(), poseIn.getXAxis());
	VectorType YRot( Zrot.J()*Xrot.K() - Zrot.K()*Xrot.J(),
		Zrot.K()*Xrot.I() - Zrot.I()*Xrot.K(),
		Zrot.I()*Xrot.J() - Zrot.J()*Xrot.I() );


	double tmp[9]= {Xrot.I(), YRot.I(), Zrot.I(),
		Xrot.J(), YRot.J(), Zrot.J(),
		Xrot.K(), YRot.K(), Zrot.K()};

	std::vector<double> v( tmp, tmp+9 );

	//https://stackoverflow.com/questions/8109443/initializing-boost-matrix-with-a-stdvector-or-array
    unbounded_array<double> storage(3*3);
    std::copy(v.begin(), v.end(), storage.begin());

	matrix<double> m(3,3,storage);
	return m;
}

bool GetRPY(VectorType Xrot, VectorType Zrot, double & roll, double & pitch, double & yaw)
{
	try {
		matrix<double> m = GetQvmRotMatrix(Xrot, Zrot);
		getRPY(m, roll, pitch, yaw);
	}
	catch(...)
	{
	}
	return true;
}
std::string CrclInterface::CRCLInitCanonCmd()
{
#if 0
	return 
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<CRCLCommandInstance\n"
		"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" >\n"
		"  <CRCLCommand xsi:type=\"InitCanonType\">\n"
		"    <CommandID>1</CommandID>\n"
		"  </CRCLCommand>\n"
		"</CRCLCommandInstance>\n";
#else
	InitCanonType cmd(++_commandnum);
	xml_schema::namespace_infomap map;

	//map[""].name = "http://www.w3.org/2001/XMLSchema";
	//map[""].schema = "http://www.w3.org/2001/XMLSchema ../CRCLStatus.xsd";
	 std::ostringstream  strfs;
	CRCLCommandInstance (strfs, //std::cout,
		(CRCLCommandType &) cmd,
		map,
		"UTF-8",
		xml_schema::flags::dont_initialize);
	return strfs.str();
#endif
}

int CrclInterface::ParseCRCLStatus(std::string filename)
{
	std::string contents;
	Globals::ReadFile(filename,  contents);
	return ParseCRCLStatusString(contents);
}
int CrclInterface::ParseCRCLStatusString(std::string str)
{
	try {
		std::istringstream istr (str);
		Globals::AppendFile(File.ExeDirectory()+"Trace.log", str);
		std::auto_ptr<CRCLStatusType> crclstat (
			CRCLStatus (istr, xml_schema::flags::dont_initialize|xml_schema::flags::dont_validate|xml_schema::flags::keep_dom)
			);
		//xercesc::DOMElement* e = static_cast<xercesc::DOMElement*> ((*crclstat)._node ());
		CLEANSTORE(_status.CommandID, crclstat->CommandStatus().CommandID (), 0) ;
		CLEANSTORE(_status.StatusID, crclstat->CommandStatus().StatusID (), 0) ;
		CLEANSTORE(_status.CommandState, crclstat->CommandStatus().CommandState (), "") ;
		CLEANSTORE(_status.Point[0], crclstat->PoseStatus().get().Pose().Point().X(), 0)  ;
		CLEANSTORE(_status.Point[1], crclstat->PoseStatus().get().Pose().Point().Y(), 0)  ;
		CLEANSTORE(_status.Point[2], crclstat->PoseStatus().get().Pose().Point().Z(), 0)  ;
		CLEANSTORE(_status.XAxis[0], crclstat->PoseStatus().get().Pose().XAxis().I(), 0)  ;
		CLEANSTORE(_status.XAxis[1], crclstat->PoseStatus().get().Pose().XAxis().J(), 0)  ;
		CLEANSTORE(_status.XAxis[2], crclstat->PoseStatus().get().Pose().XAxis().K(), 0)  ;
		CLEANSTORE(_status.ZAxis[0], crclstat->PoseStatus().get().Pose().ZAxis().I(), 0)  ;
		CLEANSTORE(_status.ZAxis[1], crclstat->PoseStatus().get().Pose().ZAxis().J(), 0)  ;
		CLEANSTORE(_status.ZAxis[2], crclstat->PoseStatus().get().Pose().ZAxis().K(), 0)  ;
		
		_status.Alarm.empty();
		if(_status.CommandState == "CRCL_Error")
		{   
			if(crclstat->CommandStatus().StateDescription ().present())
				_status.Alarm =  crclstat->CommandStatus().StateDescription ().get(); 
		}

		CRCLStatusType::JointStatuses_optional & jntstatus = crclstat->JointStatuses ();
		//::JointStatusesType jnts;
		JointStatusesType::JointStatus_sequence jnts;
		if (jntstatus.present())
		{
			jnts = jntstatus.get().JointStatus ();

			for (unsigned int i = 0; i < jnts.size(); i++) {
				//::JointStatusType  jointstatus(jout[i ].JointNumber());
				double pos = jnts[i].JointPosition().present() ? jnts[i].JointPosition().get(): 0.0;
				_status.joint_pos.push_back(pos);
			}
		}

		_status.gripperpos=0.0;
		if(crclstat->GripperStatus().present())
		{
			ParallelGripperStatusType & gripper((ParallelGripperStatusType &)crclstat->GripperStatus().get());
			_status.gripperpos=gripper.Separation() ;
		}

		// Fails we're screwed.
		GetRPY(crclstat->PoseStatus().get().Pose().XAxis(), 
			crclstat->PoseStatus().get().Pose().ZAxis(), 
			_status.roll, 
			_status.pitch, 
			_status.yaw);

		//CLEANSTORE(_status.Alarm, crclstat->CommandStatus().CommandState (), "") ;
		_status.Dump();
	}
	catch (const xml_schema::exception& e)
	{
		Globals::ErrorMessage(e.what()); 
		return -1;
	}
	catch(...)
	{
		Globals::ErrorMessage("ParseCRCLStatusString error\n");
		return -1;
	}
	return 0;
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