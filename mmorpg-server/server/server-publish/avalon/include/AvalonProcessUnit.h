/**
 *@author	 
 *@email	 
 *@date		2011-03-12	
 *@brief	����Ԫ�������̵߳ķ�װ
 */
#ifndef _AVALON_PROCESSUNIT_H_
#define _AVALON_PROCESSUNIT_H_

#include "AvalonTime.h"

namespace Avalon
{
	/**
	 *@brief ����Ԫ��������һ��ִ�п�ܣ�Ҳ���Ǵ������̣�BeginProcess��EndProcess��. ���̳��Ը���ľ�����Ҫʵ���麯��(���磺OnInit��OnTick��OnQuit��
	 */
	class ProcessUnit
	{
	public:
		ProcessUnit();
		virtual ~ProcessUnit();

	public:
		/**
		 *@brief ����tick���
		 */
		void SetInterval(const Time& interval);

		/**
		 *@brief sleepһ��ʱ��
		 */
		static void Sleep(const Time& time);

		/**
		 *@brief �ж��Ƿ�
		 */
		bool IsAlive() const{ return m_bRun; }

protected:

		/**
		 *@brief ��ʼִ��  �ú��������̵߳��߳���ں����б�����(���߳�Ҳ����Application::Main()���ã���ΪApplication�̳���ProcessUnit)
		 */
		void BeginProcess();

		/**
		 *@brief ֹͣ
		 */
		void EndProcess();

		/**
		 *@brief ��ʼ��
		 *@return ��ʼ���ɹ�����true�����򷵻�false
		 */
		virtual bool OnInit() = 0;

		/**
		 *@brief һ�δ���
		 */
		virtual void OnTick(const Time& now) = 0;

		/**
		 *@brief �˳�
		 */
		virtual void OnQuit() = 0;

	private:
		//tick���
		Time m_Interval;
		//�Ƿ�������
		volatile bool m_bRun;
	};
}

#endif
