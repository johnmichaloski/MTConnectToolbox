


http://www.bing.com/search?q=character+count&go=Submit&qs=n&form=QBLH&pq=character+count&sc=8-15&sp=-1&sk=&ghc=1&cvid=62F52C47D40E44CA8C6D0C30853A5D5A

#if 0
	COleDateTime							_realtimetime; /**< now + elapsed SHDR time */
	COleDateTime							_currenttime; /**< shdr current timestamps */
	COleDateTime							_lasttime; /**< last shdr current timestamps */
	COleDateTimeSpan                        _duration; /**< duration between last and current shdr timestamps */
#endif
//#include <ATLComTime.h>
				std::cout << "Duration " << _duration << std::endl;
			// // GetOleDateTimeString(_realtimetime)+_buffer.substr(delim);
		std::cout << "Millsecond Duration " << _duration.total_milliseconds() << std::endl;
		//return _duration.GetTotalSeconds();
			//_currenttime=GetOleDateTime(timestamp);
#if 0
	/**
	* \brief Get date and time in MS format from string.
	*/
	static COleDateTime GetOleDateTime(std::string s)
	{
		// parse 2012-02-03T17:31:51.0968Z 
		int Year, Month, Day, Hour, Minute, Second, Millisecond;
		if(sscanf(s.c_str(), "%d-%d-%dT%d:%d:%d.%d", &Year, &Month, &Day, &Hour, &Minute,&Second, &Millisecond)==7){}
		//12/2/2009 2:42:25 PM
		else if(sscanf(s.c_str(), "%d/%d/%4d%d:%d:%d", &Month, &Day, &Year,  &Hour, &Minute,&Second)==6){}
		else throw std::runtime_error(StrFormat("Unrecognized date-time format -%s\n", s.c_str()));
		return COleDateTime( Year, Month, Day, Hour, Minute, Second ); 
	}
	/**
	* \brief Print string using MS date and time in MS format .        
	*/
	static std::string GetOleDateTimeString(COleDateTime d)
	{
		//return StrFormat( "%d/%d/%d %d:%d:%d.%d", d.GetYear(), d.GetMonth(), d.GetDay(), d.GetHour(), d.GetMinute(),d.GetSecond(),0);
		return StrFormat( "%d-%d-%dT%d:%d:%d.%d", d.GetYear(), d.GetMonth(), d.GetDay(), d.GetHour(), d.GetMinute(),d.GetSecond(),0);
	}
#endif