/**
 *@author	 
 *@email	 
 *@date		2011-03-12	
 *@brief	�ļ�
 */
#ifndef _AVALON_FILE_SYSTEM_H_
#define _AVALON_FILE_SYSTEM_H_

#include "AvalonDefine.h"
#include <string>
#include <vector>

namespace Avalon
{
	/**
	 *@brief �ļ�����
	 */
	enum FileType
	{
		FILE_REGULAR	=	1 << 0,		//��ͨ�ļ�
		FILE_DIRECTORY	=	1 << 1,		//Ŀ¼
		FILE_OTHER		=	1 << 2,		//����
	};

	class Directory;

	/**
	 *@brief �ļ�
	 */
	class File
	{
		friend class Directory;
	
		File(const std::string& fullpath, FileType type);
	public:
		~File(){}

	public:
		/**
		 *@brief ��ȡ����·��
		 */
		std::string GetFullPath() const { return m_FullPath; }
		/**
		 *@brief ��ȡ�ļ���
		 */
		std::string GetName() const { return m_Name; }

		bool IsFile() const {	return m_Type == FILE_REGULAR; }
		bool IsDir() const { return m_Type == FILE_DIRECTORY; }
		Directory ToDirectory() const;

		/**
		 *@brief touch����ļ�                                                                     
		 */
		bool Touch();

	private:
		//����·��
		std::string m_FullPath;
		//�ļ���
		std::string m_Name;
		//����
		FileType	m_Type;
	};


	/**
	 *@brief Ŀ¼
	 */
	class Directory
	{
	public:
		typedef std::vector<File>::iterator iterator; 

	public:
		explicit Directory(const std::string& path);
		~Directory(){}

		/**
		 *@brief ��ȡ·��
		 */
		std::string GetPath() const { return m_Path; }

		/**
		 *@brief ����Ŀ¼�ļ�
		 */
		iterator Begin();
		iterator End();

	private:
		//·��
		std::string		m_Path;
		//�ļ��б�
		std::vector<File>	m_Files;
	};
}

#endif
