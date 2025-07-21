/**
 *@author	 
 *@email	 
 *@date		2011-04-19		
 *@brief	文件日志输出器
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
	 *@brief 文件滚动类型
	 */
	enum FileRollType
	{
		FILEROLL_NONE,	//不滚动
		FILEROLL_HOUR,	//按时
		FILEROLL_DAY,	//按日
		FILEROLL_MONTH,	//按月
	};

	/**
	 *@brief 文件输出器  noted by aprilliu, 以文件形式输出日志信息
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
		//文件名
		std::string		m_Prefix;
		//输出文件流
		std::ofstream	m_Stream;
		//滚动类型
		FileRollType	m_RollType;
		//打开文件日志
		Date			m_OpenDate;
	};
}

#endif
