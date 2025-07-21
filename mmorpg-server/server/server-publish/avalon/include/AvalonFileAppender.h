/**
 *@author	 
 *@email	 
 *@date		2011-04-19		
 *@brief	�ļ���־�����
 */
#ifndef _AVALON_FILEAPPENDER_H_
#define _AVALON_FILEAPPENDER_H_

#include <string>
#include <fstream>
#include "AvalonAppender.h"
#include "AvalonDate.h"


namespace Avalon
{
	/**
	 *@brief �ļ���������
	 */
	enum FileRollType
	{
		FILEROLL_NONE,	//������
		FILEROLL_HOUR,	//��ʱ
		FILEROLL_DAY,	//����
		FILEROLL_MONTH,	//����
	};

	/**
	 *@brief �ļ������  noted by aprilliu, ���ļ���ʽ�����־��Ϣ
	 */
	class FileAppender : public Appender
	{
	public:
		explicit FileAppender(const std::string& prefix, FileRollType rollType = FILEROLL_NONE);
		~FileAppender();

		bool DoOpen();

		bool DoAppend(const LogEvent& logEvent);

		void DoClose();

	private:
		std::string GetFileName() const;
		bool CheckReopen(const Date& now) const;

	private:
		//�ļ���
		std::string		m_Prefix;
		//����ļ���
		std::ofstream	m_Stream;
		//��������
		FileRollType	m_RollType;
		//���ļ���־
		Date			m_OpenDate;
	};
}

#endif
