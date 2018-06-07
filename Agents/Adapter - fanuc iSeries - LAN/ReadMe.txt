
Thu 11/09/2017_14:02:45.31  
Removed code

	std::string getProgramNameFromComments();
std::string CiSeries::getProgramNameFromComments()
{
	GLogger.Status("getProgramNameFromComments Enter\n");
	short ret;
	int maxAxes = MAX_AXIS;

	ODBDY2 dyn;
	memset(&dyn, 0xEF, sizeof(dyn));
	GLogger.Status("cnc_rddynamic2 call\n");
	ret = cnc_rddynamic2(_adapter->mFlibhndl, ALL_AXES, sizeof(dyn), &dyn);
	if (ret != EW_OK)
	{
		GLogger.Status(StrFormat("Cannot get the rddynamic2 data %hd\n", ret ));
		return "";
	}
	// From: https://github.com/mtconnect/adapter/tree/master/fanuc
	std::string sProgram; // FIXME: get old program name
	if (dyn.prgnum != _adapter->mProgramNum)
	{
		GLogger.Status("dy.prgnum != _adapter->mProgramNum\n");

		char program[2048];
		short prognum = dyn.prgnum;
		short ret = cnc_upstart(_adapter->mFlibhndl, prognum);
		if (ret != EW_OK)
		{
			GLogger.Status(StrFormat("cnc_upstart failed program num=&d EW=%hd\n", prognum, ret ));
			return "";
		}
		do 
		{
			// One for the \0 terminator
			long len = sizeof(program) - 1;
			ret = cnc_upload3(_adapter->mFlibhndl, &len, program);
			if (ret != EW_OK)
			{
				GLogger.Status(StrFormat("cnc_upload3 failed program len=%d EW=%hd\n", len, ret ));
				return "";
			}
			if (ret == EW_OK)
			{

				bool nl = false;
				program[len] = '\0';
				std::string str(program, len);
				GLogger.Status(StrFormat("At cnc_upload3 %s\n" ,program));

				// First line: O1234(143A3120-7,2-1,010617,052646)
				//  number is 143A3120-7. 
				//Media number is 2-1.
				std::istringstream input(str);
				std::getline(input, sProgram,'\n');
				GLogger.Status(StrFormat("At Getline sProgram %s\n" ,sProgram.c_str()));

				//  look for O # which must be there
				//if(1 != sscanf(sProgram.c_str(), "O%d", &prognum))
				//	continue;

				// Now find string between comments (...)
				int n = sProgram.find("(");
				int m= sProgram.find(")");
				if(std::string::npos !=n && std::string::npos !=m)
					sProgram= sProgram.substr(n+1,m-n-1);
				else
					sProgram= "";
	          	GLogger.Status(StrFormat("Extracting comments: sProgram %s\n" ,sProgram.c_str()));
			}
		} while (ret == EW_BUFFER);
		GLogger.Status(StrFormat("Done extracting comments: sProgram %s\n" ,sProgram.c_str()));
		cnc_upend3(_adapter->mFlibhndl);
	}
	_adapter->mProgramNum = dyn.prgnum;
	return sProgram;

}


sc delete Fanuc32LanMTConnectAgent

REMINDERS:

TO clear out old code - new version will cause new GUID



Icacls * /T /C /grant system:F

OLD WAY TO GET PROGRAM O#

int CiSeries::getLine()
{
	GLogger.LogMessage("iSeries::getLine Enter\n", 3);
	std::string prognum,progname;

	short ret;
	if (!_adapter->mConnected)
		return -1;

	GLogger.Info("iSeries::getLine\n");
	ODBSEQ block;
	ret = cnc_rdseqnum(_adapter->mFlibhndl, &block ); // 15,16,18,21,0,powermate
	if (ret == EW_OK)
	{
		_adapter->SetMTCTagValue("line", StrFormat("%d", block.data));
	}
	else
	{
		GLogger.Info(StrFormat("Error iSeries::cnc_rdseqnum=%d\n", ret));
	}

	ODBPRO buf ;
	ret = cnc_rdprgnum( _adapter->mFlibhndl, &buf ) ; // 15,16,18,21,0,powermate
	if (ret == EW_OK)
	{
		//prognum=StrFormat("O%04hd", buf.data); // mdata -  Main program number. data- Running program number.
		prognum=StrFormat("O%04d", buf.data); // mdata -  Main program number. data- Running program number.
		GLogger.Info(StrFormat("iSeries::cnc_rdprgnum prognum = %d\n", buf.mdata));
		GLogger.Info(StrFormat("iSeries::cnc_rdprgnum prognum = %d\n",(int) (buf.mdata&0xFFFF)));
		GLogger.Info(StrFormat("iSeries::cnc_rdprgnum prognum data = %d\n",(int) (buf.data&0xFFFF)));
	}
	else
	{
		GLogger.Info(StrFormat("Error CiSeries::cnc_rdprgnum=%d", ret));
		LogErrorMessage(" CiSeries::cnc_rdprgnum FAILED\n", ret );
	}

	// Get first comment as program name
	//if(Globals.ProgramLogic == "FirstComment")
	progname=getProgramNameFromComments();

	// Decide best program name, if none use O#
	if(!progname.empty())
	{
		_adapter->SetMTCTagValue("program", progname);
	}
	else if(!prognum.empty())
	{
		_adapter->SetMTCTagValue("program", prognum);
	}
	else
	{
		_adapter->SetMTCTagValue("program", "O1");
	}
	GLogger.LogMessage("iSeries::getLine Leave\n", 3);
	return EW_OK;
}