/**
 *@author	 
 *@email	 
 *@date		2011-03-12	
 *@brief	处理单元，基于线程的封装
 */
#ifndef _AVALON_PROCESSUNIT_H_
#define _AVALON_PROCESSUNIT_H_

#include "AvalonTime.h"

namespace Avalon
{
	/**
	 *@brief 处理单元，定义了一个执行框架（也就是处理流程：BeginProcess，EndProcess）. 凡继承自该类的具体类要实现虚函数(比如：OnInit，OnTick，OnQuit）
	 */
	class ProcessUnit
	{
	public:
		ProcessUnit();
		virtual ~ProcessUnit();

	public:
		/**
		 *@brief 设置tick间隔
		 */
		void SetInterval(const Time& interval);

		/**
		 *@brief sleep一段时间
		 */
		static void Sleep(const Time& time);

		/**
		 *@brief 判断是否活动
		 */
		bool IsAlive() const{ return m_bRun; }

protected:

		/**
		 *@brief 开始执行  该函数会在线程的线程入口函数中被调用(主线程也会在Application::Main()调用，因为Application继承自ProcessUnit)
		 */
		void BeginProcess();

		/**
		 *@brief 停止
		 */
		void EndProcess();

		/**
		 *@brief 初始化
		 *@return 初始化成功返回true，否则返回false
		 */
		virtual bool OnInit() = 0;

		/**
		 *@brief 一次处理
		 */
		virtual void OnTick(const Time& now) = 0;

		/**
		 *@brief 退出
		 */
		virtual void OnQuit() = 0;

	private:
		//tick间隔
		Time m_Interval;
		//是否在运行
		volatile bool m_bRun;
	};
}

#endif
