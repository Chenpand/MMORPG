/**
 *@author	 
 *@email	 
 *@date		2011-03-12	
 *@brief	文件
 */
#ifndef _AVALON_FILE_SYSTEM_H_
#define _AVALON_FILE_SYSTEM_H_

#include "AvalonDefine.h"
#include <string>
#include <vector>

namespace Avalon
{
	/**
	 *@brief 文件类型
	 */
	enum FileType
	{
		FILE_REGULAR	=	1 << 0,		//普通文件
		FILE_DIRECTORY	=	1 << 1,		//目录
		FILE_OTHER		=	1 << 2,		//其他
	};

	class Directory;

	/**
	 *@brief 文件
	 */
	class File
	{
		friend class Directory;
	
		File(const std::string& fullpath, FileType type);
	public:
		~File(){}

	public:
		/**
		 *@brief 获取完整路径
		 */
		std::string GetFullPath() const { return m_FullPath; }
		/**
		 *@brief 获取文件名
		 */
		std::string GetName() const { return m_Name; }

		bool IsFile() const {	return m_Type == FILE_REGULAR; }
		bool IsDir() const { return m_Type == FILE_DIRECTORY; }
		Directory ToDirectory() const;

		/**
		 *@brief touch这个文件                                                                     
		 */
		bool Touch();

	private:
		//完整路径
		std::string m_FullPath;
		//文件名
		std::string m_Name;
		//类型
		FileType	m_Type;
	};


	/**
	 *@brief 目录
	 */
	class Directory
	{
	public:
		typedef std::vector<File>::iterator iterator; 

	public:
		explicit Directory(const std::string& path);
		~Directory(){}

		/**
		 *@brief 获取路径
		 */
		std::string GetPath() const { return m_Path; }

		/**
		 *@brief 迭代目录文件
		 */
		iterator Begin();
		iterator End();

	private:
		//路径
		std::string		m_Path;
		//文件列表
		std::vector<File>	m_Files;
	};
}

#endif
