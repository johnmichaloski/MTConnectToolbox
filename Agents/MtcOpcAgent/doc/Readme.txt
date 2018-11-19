

			Globals.ResetAtMidnight	= config.GetSymbolValue("OPCSERVER.ResetAtMidnight", 0).toNumber<int>() ;
			Globals.HttpPort=config.GetSymbolValue("Agent.HttpPort", 5000).toNumber<int>() ;
