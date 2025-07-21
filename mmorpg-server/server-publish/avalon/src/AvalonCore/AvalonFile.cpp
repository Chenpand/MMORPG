#include "AvalonFile.h"

#ifdef AVALON_OS_WINDOWS
#include <windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#endif


namespace Avalon
{
	File::File(const std::string& fullpath, FileType type)
		:m_FullPath(fullpath), m_Type(type)
	{
		std::string::size_type index = m_FullPath.rfind('/');
#ifdef AVALON_OS_WINDOWS
		std::string::size_type index1 = m_FullPath.rfind('\\');
		
		if(index == std::string::npos) index = index1;
		else if(index1 != std::string::npos) index = index > index1 ? index : index1;
#endif
		if(index == std::string::npos) m_Name = m_FullPath;
		else m_Name = m_FullPath.substr(index + 1);
	}

	Directory File::ToDirectory() const{ return Directory(m_FullPath); }

	bool File::Touch()
	{
#ifdef AVALON_OS_WINDOWS
		HANDLE handle = ::CreateFile(m_FullPath.c_str(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if(handle == INVALID_HANDLE_VALUE) return false;

		SYSTEMTIME sysnow;
		GetLocalTime(&sysnow);
		FILETIME filenow;
		BOOL ret = SystemTimeToFileTime(&sysnow, &filenow);
		LocalFileTimeToFileTime(&filenow, &filenow);
		if(ret == FALSE) return false;

		FILETIME createTime;
		FILETIME accessTime;
		FILETIME writeTime;
		if(GetFileTime(handle, &createTime, &accessTime, &writeTime) == FALSE) return false;

		LARGE_INTEGER now;
		now.HighPart = filenow.dwHighDateTime;
		now.LowPart = filenow.dwLowDateTime;

		if(CompareFileTime(&createTime, &filenow) > 0)
		{
			createTime = filenow;
		}
		SetFileTime(handle, &createTime, &filenow, &filenow);
#else
#endif
		return true;
	}


	Directory::Directory(const std::string& path)
		:m_Path(path)
	{
		if(!m_Path.empty())
		{
			char end = m_Path[m_Path.length() - 1];
#ifdef AVALON_OS_WINDOWS
			if(end != '\\' && end != '/')
#else
			if(end != '/')
#endif
			{
				m_Path.append("/");
			}
		}
	}

	Directory::iterator Directory::Begin()
	{
		if(!m_Files.empty()) return m_Files.begin();

#ifdef AVALON_OS_WINDOWS
		std::string path = m_Path;
		path.append("*");

		WIN32_FIND_DATA fileData;
		HANDLE file = ::FindFirstFileA(path.c_str(), &fileData);
		BOOL bNext = (file != INVALID_HANDLE_VALUE);

		while(bNext)
		{
			std::string fileName = fileData.cFileName;
			if(fileName.compare(".") != 0 && fileName.compare("..") != 0)
			{
				FileType type = FILE_OTHER;
				if( fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) type = FILE_DIRECTORY;
				else if( !(fileData.dwFileAttributes & FILE_ATTRIBUTE_DEVICE) ) type = FILE_REGULAR;
				std::string fullPath = m_Path;
				fullPath.append(fileName);
				m_Files.push_back(File(fullPath, type));
			}

			bNext = ::FindNextFileA(file, &fileData);
		}
		::FindClose(file);
#else
		DIR* pDir = opendir(m_Path.c_str());
		if(pDir == NULL) return m_Files.begin();

		struct dirent* pEntry = readdir(pDir);
		while( pEntry != NULL)
		{
			std::string fileName = pEntry->d_name;
			if(fileName.compare(".") != 0 && fileName.compare("..") != 0)
			{
				FileType type = FILE_OTHER;
				if(pEntry->d_type == DT_DIR) type = FILE_DIRECTORY;
				else if(pEntry->d_type == DT_REG ) type = FILE_REGULAR;

				std::string fullPath = m_Path;
				fullPath.append(fileName);
				m_Files.push_back(File(fullPath, type));
			}

			pEntry = readdir(pDir);
		}
		closedir(pDir);
#endif
		return m_Files.begin();
	}

	Directory::iterator Directory::End()
	{
		return m_Files.end();
	}
}

