/**
 *@author	 
 *@email	 
 *@date		2011-04-19		
 *@brief	��־�����
 */
#ifndef _AVALON_APPENDER_H_
#define _AVALON_APPENDER_H_

#include "AvalonLogEvent.h"
#include "AvalonMutex.h"

namespace Avalon
{
	/**
	 *@brief ��־�����
	 */
	class Appender
	{
	public:
		Appender(){}
		virtual ~Appender(){}

		/**
		 *@brief ��׷����
		 */
		bool Open();

		/**
		 *@brief �ر�
		 */
		void Close();

		/**
		 *����һ����־�¼�
		 */
		bool Append(const LogEvent& logEvent);

	public:
		virtual bool Init() { return true; }
		virtual bool Finish() { return true; }

	protected:
		virtual bool DoOpen() = 0;
		virtual bool DoAppend(const LogEvent& logEvent) = 0;
		virtual void DoClose() = 0;

	private:
		//������
		Avalon::Mutex m_Mutex;
	};
}

#endif
