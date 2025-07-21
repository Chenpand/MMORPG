#ifndef _TFS_APPLICATION_H_
#define _TFS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>

/**
 *@brief ��ת������Ӧ�ó�����
 */
class TFSApplication : public CLApplication, public Avalon::Singleton<TFSApplication>
{
public:
	TFSApplication();
	~TFSApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	virtual void OnSignal(int signal);

private:
	/**
	*@brief ���ظ��ֶ�̬����
	*/
	void LoadDynamicConfig();

private:

	//tickʱ��
	UInt64	m_TickTime;
	// ��������
	bool m_ReloadConfig;
};

#endif
