/**
 *@author	 
 *@email	 
 *@date		2011-04-12		
 *@brief	应用程序框架
 */
#ifndef _AVALON_APPLICATION_H_
#define _AVALON_APPLICATION_H_

#include <string>
#include "AvalonProcessUnit.h"
#include "AvalonPropertySet.h"
#include "AvalonLogger.h"

namespace Avalon
{
	/**
	 *@brief 应用程序基类，包含框架定义，配置加载，日志初始化
	 *		定义应用程序参数如下
	 *		-Debug 调试模式运行，此时日志优先级设为LOG_DEBUG
	 */
	class Application : public ProcessUnit
	{
	public:
		Application();
		virtual ~Application();

	public:
		/**
		 *@brief 初始设置应用程序名和配置
		 */
		void Init(const std::string& appName,const std::string& configName);	
	
		/**
		 *@brief main入口
		 */
		void Main(int argc,char* argv[]);

		/**
		 *@brief 终止程序
		 */
		void Exit();

		/**
		 *@brief 获取属性集
		 */
		PropertySet* GetProperties(){ return &m_Properties; }

		/**
		 *@brief 获取应用程序名
		 */
		const std::string& GetName() const{ return m_AppName; }

		/**
		 *@brief 获取日志路径
		 */
		const std::string& GetLogDir() const { return m_LogDir; }

		/**
		 *@brief 获取系统日志
		 */
		Logger* GetSysLog() const { return m_pSysLog; }

	protected://ProcessUnit
		bool OnInit();

		void OnTick(const Time& now);

		void OnQuit();

	private:
		//app名字
		std::string m_AppName;
		//日志路径
		std::string m_LogDir;
		//配置名字
		std::string m_ConfigName;
		//属性集
		PropertySet m_Properties;
		//系统日志
		Logger*		m_pSysLog;
	};

}

#endif
