/**
 *@author	 
 *@email	 
 *@date		2011-04-12		
 *@brief	Ӧ�ó�����
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
	 *@brief Ӧ�ó�����࣬������ܶ��壬���ü��أ���־��ʼ��
	 *		����Ӧ�ó����������
	 *		-Debug ����ģʽ���У���ʱ��־���ȼ���ΪLOG_DEBUG
	 */
	class Application : public ProcessUnit
	{
	public:
		Application();
		virtual ~Application();

	public:
		/**
		 *@brief ��ʼ����Ӧ�ó�����������
		 */
		void Init(const std::string& appName,const std::string& configName);	
	
		/**
		 *@brief main���
		 */
		void Main(int argc,char* argv[]);

		/**
		 *@brief ��ֹ����
		 */
		void Exit();

		/**
		 *@brief ��ȡ���Լ�
		 */
		PropertySet* GetProperties(){ return &m_Properties; }

		/**
		 *@brief ��ȡӦ�ó�����
		 */
		const std::string& GetName() const{ return m_AppName; }

		/**
		 *@brief ��ȡ��־·��
		 */
		const std::string& GetLogDir() const { return m_LogDir; }

		/**
		 *@brief ��ȡϵͳ��־
		 */
		Logger* GetSysLog() const { return m_pSysLog; }

	protected://ProcessUnit
		bool OnInit();

		void OnTick(const Time& now);

		void OnQuit();

	private:
		//app����
		std::string m_AppName;
		//��־·��
		std::string m_LogDir;
		//��������
		std::string m_ConfigName;
		//���Լ�
		PropertySet m_Properties;
		//ϵͳ��־
		Logger*		m_pSysLog;
	};

}

#endif
