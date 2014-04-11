//
// Path.h
//
#pragma once
#include <string>

// Summary:
//     Performs operations on System.std::string instances that contain file or directory
//     path information. These operations are performed in a cross-platform manner.
class CPath
{
public:
	CPath()
	{
		DirectorySeparatorChar=_T('\\');
		AltDirectorySeparatorChar=_T('/');
		PathSeparator=_T('\\');
		VolumeSeparatorChar= _T(':');
	}
	// Summary:
	//     Provides a platform-specific alternate character used to separate directory
	//     levels in a path std::string that reflects a hierarchical file system organization.
	TCHAR AltDirectorySeparatorChar;

	//
	// Summary:
	//     Provides a platform-specific character used to separate directory levels
	//     in a path std::string that reflects a hierarchical file system organization.
	TCHAR  DirectorySeparatorChar;

	//
	// Summary:
	//     A platform-specific separator character used to separate path std::strings in
	//     environment variables.
	TCHAR PathSeparator;
	//
	// Summary:
	//     Provides a platform-specific volume separator character.
	TCHAR VolumeSeparatorChar;

	// Summary:
	//     Changes the extension of a path std::string.
	//
	// Parameters:
	//   path:
	//     The path information to modify. The path cannot contain any of the characters
	//     defined in System.IO.Path.GetInvalidPathChars().
	//
	//   extension:
	//     The new extension (with or without a leading period). Specify null to remove
	//     an existing extension from path.
	//
	// Returns:
	//     A std::string containing the modified path information.On Windows-based desktop
	//     platforms, if path is null or an empty std::string (""), the path information
	//     is returned unmodified. If extension is null, the returned std::string contains
	//     the specified path with its extension removed. If path has no extension,
	//     and extension is not null, the returned path std::string contains extension appended
	//     to the end of path.
	//
	// Exceptions:
	//   System.ArgumentException:
	//     path contains one or more of the invalid characters defined in System.IO.Path.GetInvalidPathChars().
	std::string ChangeExtension(std::string path, std::string extension)
	{
		std::string::size_type idx; 
		idx = path.rfind(_T('.')); 

		if(idx != std::string::npos) 
		{ 
			return path.substr(0,idx)+extension; 
		}
		return _T("");
	}

	//
	// Summary:
	//     Returns the directory information for the specified path std::string.
	//
	// Parameters:
	//   path:
	//     The path of a file or directory.
	//
	// Returns:
	//     A System.std::string containing directory information for path, or null if path
	//     denotes a root directory, is the empty std::string (""), or is null. Returns System.std::string.Empty
	//     if path does not contain directory information.
	//
	// Exceptions:
	//   System.ArgumentException:
	//     The path parameter contains invalid characters, is empty, or contains only
	//     white spaces.
	//
	//   System.IO.PathTooLongException:
	//     The path parameter is longer than the system-defined maximum length.
	std::string GetDirectoryName(std::string path)
	{
		std::string::size_type idx; 
		idx = path.rfind(_T('\\')); 

		if(idx != std::string::npos) 
		{ 
			return path.substr(0,idx); 
		}
		return _T("");
	}
	//
	// Summary:
	//     Returns the extension of the specified path std::string.
	//
	// Parameters:
	//   path:
	//     The path std::string from which to get the extension.
	//
	// Returns:
	//     A System.std::string containing the extension of the specified path (including
	//     the "."), null, or System.std::string.Empty. If path is null, GetExtension returns
	//     null. If path does not have extension information, GetExtension returns Empty.
	//
	// Exceptions:
	//   System.ArgumentException:
	//     path contains one or more of the invalid characters defined in System.IO.Path.GetInvalidPathChars().
	std::string GetExtension(std::string path)
	{
		std::string::size_type idx; 
		idx = path.rfind(_T('.')); 

		if(idx != std::string::npos) 
		{ 
			return path.substr(idx+1); 
		}
		return _T("");

	}
	//
	// Summary:
	//     Returns the file name and extension of the specified path std::string.
	//
	// Parameters:
	//   path:
	//     The path std::string from which to obtain the file name and extension.
	//
	// Returns:
	//     A System.std::string consisting of the characters after the last directory character
	//     in path. If the last character of path is a directory or volume separator
	//     character, this method returns System.std::string.Empty. If path is null, this
	//     method returns null.
	//
	// Exceptions:
	//   System.ArgumentException:
	//     path contains one or more of the invalid characters defined in System.IO.Path.GetInvalidPathChars().
	std::string GetFileName(std::string path)
	{
		std::string::size_type idx; 
		idx = path.rfind(_T('\\')); 

		if(idx != std::string::npos) 
		{ 
			return path.substr(idx+1); 
		}
		return _T("");
	}
	//
	// Summary:
	//     Returns the file name of the specified path std::string without the extension.
	//
	// Parameters:
	//   path:
	//     The path of the file.
	//
	// Returns:
	//     A System.std::string containing the std::string returned by System.IO.Path.GetFileName(System.std::string),
	//     minus the last period (.) and all characters following it.
	//
	// Exceptions:
	//   System.ArgumentException:
	//     path contains one or more of the invalid characters defined in System.IO.Path.GetInvalidPathChars().
	std::string GetFileNameWithoutExtension(std::string path)
	{
		std::string filename = GetFileName(path);
		if(!filename.empty())
		{
			std::string::size_type idx;
			idx=filename.find(_T("."));
			if(idx!= std::string::npos)
			{
				filename = filename.substr(0,idx-1);
			}
		}
		return filename;
	}
	//
	// Summary:
	//     Gets an array containing the characters that are not allowed in file names.
	//
	// Returns:
	//     An array containing the characters that are not allowed in file names.
	TCHAR * GetInvalidFileNameChars(){ return _T("\"<>|\b\t"); }
	//
	// Summary:
	//     Gets an array containing the characters that are not allowed in path names.
	//
	// Returns:
	//     An array containing the characters that are not allowed in path names.
	TCHAR * GetInvalidPathChars() { return _T("\"<>|\b\t"); }
	//
	// Summary:
	//     Gets the root directory information of the specified path.
	//
	// Parameters:
	//   path:
	//     The path from which to obtain root directory information.
	//
	// Returns:
	//     A std::string containing the root directory of path, such as "C:\", or null if
	//     path is null, or an empty std::string if path does not contain root directory
	//     information.
	//
	// Exceptions:
	//   System.ArgumentException:
	//     path contains one or more of the invalid characters defined in System.IO.Path.GetInvalidPathChars().-or-
	//     System.std::string.Empty was passed to path.
	std::string GetPathRoot(std::string path);
	//
	// Summary:
	//     Returns a random folder name or file name.
	//
	// Returns:
	//     A random folder name or file name.
	std::string GetRandomFileName();
	//
	// Summary:
	//     Creates a uniquely named, zero-byte temporary file on disk and returns the
	//     full path of that file.
	//
	// Returns:
	//     A System.std::string containing the full path of the temporary file.
	//
	// Exceptions:
	//   System.IO.IOException:
	//     An I/O error occurs, such as no unique temporary file name is available.-
	//     or -This method was unable to create a temporary file.
	std::string GetTempFileName()
	{
		_TCHAR buf[MAX_PATH];
		::GetTempFileNameA(this->GetTempPath().c_str(),_T(""), 0, buf);
		return buf;
	}

	// Summary:
	//     Returns the path of the current system's temporary folder.
	//
	// Returns:
	//     A System.std::string containing the path information of a temporary directory.
	//
	// Exceptions:
	//   System.Security.SecurityException:
	//     The caller does not have the required permissions.
	std::string GetTempPath()
	{
		_TCHAR buf[MAX_PATH];
		::GetTempPath(MAX_PATH, buf);
		return buf;

	}
	//
	// Summary:
	//     Determines whether a path includes a file name extension.
	//
	// Parameters:
	//   path:
	//     The path to search for an extension.
	//
	// Returns:
	//     true if the characters that follow the last directory separator (\\ or /)
	//     or volume separator (:) in the path include a period (.) followed by one
	//     or more characters; otherwise, false.
	//
	// Exceptions:
	//   System.ArgumentException:
	//     path contains one or more of the invalid characters defined in System.IO.Path.GetInvalidPathChars().
	bool HasExtension(std::string path)
	{
		std::string::size_type idx1 = path.rfind(_T('.')); 
		std::string::size_type idx2 = path.rfind(_T('\\')); 
		if(idx1 == std::string::npos)
			return false;
		if(path.substr(idx1+1).find(_T("\\"))!= std::string::npos)
			return false;
		return true  ;
	}
	//
	// Summary:
	//     Gets a value indicating whether the specified path std::string contains absolute
	//     or relative path information.
	//
	// Parameters:
	//   path:
	//     The path to test.
	//
	// Returns:
	//     true if path contains an absolute path; otherwise, false.
	//
	// Exceptions:
	//   System.ArgumentException:
	//     path contains one or more of the invalid characters defined in System.IO.Path.GetInvalidPathChars().
	bool IsPathRooted(std::string path)
	{
		if(path[0]==_T('\\'))
			return true;
		if(path[1]==_T(':'))
			return true;
		return false;

	}
	//inline boolean Exists(LPCTSTR pszFilename)
	//{
	//	struct stat st;
	//	return stat(pszFilename, &st)==0;
	//}
};

__declspec(selectany)  CPath Path; 