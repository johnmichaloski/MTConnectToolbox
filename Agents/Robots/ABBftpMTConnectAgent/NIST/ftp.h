#ifndef FTP_H
#define FTP_H

#include <string>
#include <vector>
#include <wininet.h> //you will also need to link to wininet.lib

enum FTP_SIZE_TYPE { FTP_SIZE_BYTE, FTP_SIZE_KB, FTP_SIZE_MB, FTP_SIZE_GB };

class ftp
{
public:
    ftp(const std::string& agentName="ftp");
    ~ftp();

    bool Connect(const std::string& host, const std::string& username="", const std::string& password="", INTERNET_PORT port=INTERNET_DEFAULT_FTP_PORT, bool passive=false);
    bool Disconnect();

    bool GetConnectionState() const;
    bool List(std::vector<WIN32_FIND_DATA>& vec, const std::string& rootDir="") const;
    bool DownloadFile(const std::string& remoteFile, const std::string& localFile, bool overwrite = false) const;
    bool UploadFile(const std::string& localFile, const std::string& remoteFile) const;
    DWORD GetFileSize(const std::string& remoteFile, FTP_SIZE_TYPE type=FTP_SIZE_BYTE) const;
    bool CreateDirectory(const std::string& remoteDir);
    bool DeleteFile(const std::string& remoteFile);
    bool DeleteDirectory(const std::string& remoteDir);
    bool RenameFile(const std::string& remoteFile, const std::string& newFile); //both parameters must containt full path

private:
    HINTERNET m_hInternet;
    HINTERNET m_hConnection;
    bool m_bConnected;

    ftp(const ftp& other); // copy ctor
    ftp& operator=(const ftp& other); // asignment operator
};

ftp::ftp(const std::string& agentName/*="ftp"*/) : m_bConnected(false)
{
    m_hInternet = InternetOpen(agentName.c_str() , INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0);
}

ftp::~ftp()
{
    ftp::Disconnect();
    InternetCloseHandle(m_hConnection);
}

bool ftp::Connect(const std::string& host, const std::string& username/*=""*/, const std::string& password/*=""*/, INTERNET_PORT port/*=INTERNET_DEFAULT_FTP_PORT*/, bool passive/*=false*/)
{
    DWORD flag;
    if(passive)
        flag = INTERNET_FLAG_PASSIVE;
    else
        flag = 0;

    m_hConnection = InternetConnect(m_hInternet, host.c_str(), port, username.c_str(), password.c_str(), INTERNET_SERVICE_FTP, flag, 0);

    m_bConnected = (bool)m_hConnection;
    return m_bConnected;
}

bool ftp::Disconnect()
{
    BOOL bClose = InternetCloseHandle(m_hConnection);
    m_bConnected = (bool)!bClose;

    return bClose;
}

bool ftp::GetConnectionState() const
{
    return m_bConnected;
}

bool ftp::List(std::vector<WIN32_FIND_DATA>& vec, const std::string& rootDir/*=""*/) const
{
    if(!m_bConnected)
        return false;

    HINTERNET hFind;
    WIN32_FIND_DATA fileInfo;

    hFind = FtpFindFirstFile(m_hConnection, rootDir.c_str(), &fileInfo, 0, 0);
    if(hFind == NULL)
        return false;
    vec.push_back(fileInfo);

    while(InternetFindNextFile(hFind, &fileInfo) == TRUE)
    {
        vec.push_back(fileInfo);
    }

    InternetCloseHandle(hFind);

    return true;
}

bool ftp::DownloadFile(const std::string& remoteFile, const std::string& localFile, bool overwrite/*=false*/) const
{
    return (bool)FtpGetFile(m_hConnection, remoteFile.c_str(), localFile.c_str(), (BOOL)overwrite, FILE_ATTRIBUTE_NORMAL, FTP_TRANSFER_TYPE_UNKNOWN, 0);
}

bool ftp::UploadFile(const std::string& localFile, const std::string& remoteFile) const
{
    return (bool)FtpPutFile(m_hConnection, localFile.c_str(), remoteFile.c_str(), FTP_TRANSFER_TYPE_UNKNOWN, 0);
}

DWORD ftp::GetFileSize(const std::string& remoteFile, FTP_SIZE_TYPE type/*=FTP_SIZE_BYTE*/) const
{
    if(!m_bConnected)
        return 0;

    HINTERNET hFile;
    DWORD size;

    hFile = FtpOpenFile(m_hConnection, remoteFile.c_str(), GENERIC_READ, FTP_TRANSFER_TYPE_UNKNOWN, 0);
    size = FtpGetFileSize(hFile, 0);

    InternetCloseHandle(hFile);

    switch(type)
    {
        case FTP_SIZE_KB:
            return size / 1024;

        case FTP_SIZE_MB:
            return size / (1024*1024);

        case FTP_SIZE_GB:
            return size / (1024*1024*1024);

        default:
            return size;
    }
}

bool ftp::CreateDirectory(const std::string& remoteDir)
{
    return (bool)FtpCreateDirectory(m_hConnection, remoteDir.c_str());
}

bool ftp::DeleteFile(const std::string& remoteFile)
{
    return (bool)FtpDeleteFile(m_hConnection, remoteFile.c_str());
}

bool ftp::DeleteDirectory(const std::string& remoteDir)
{
    return (bool)FtpRemoveDirectory(m_hConnection, remoteDir.c_str());
}

bool ftp::RenameFile(const std::string& remoteFile, const std::string& newFile)
{
    return (bool)FtpRenameFile(m_hConnection, remoteFile.c_str(), newFile.c_str());
}

#endif // FTP_H