

 note enumeration remappings done before key renames!




	//////////////////////////////
	// Change enumerations
	if(enummapping.find(key+"."+value)!= enummapping.end())
	{
		value=enummapping[key+"."+value];
	}

	// Map  shdr key (e.g., mode) into new key (controllermode) 
	if(keymapping.find(key)!= keymapping.end())
	{
		key=keymapping[key];
	}

