#ifndef _MS_APPLICATION_H_
#define _MS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>

/**
*@brief �����Ӧ�ó�����
*/
class MSApplication : public CLApplication, public Avalon::Singleton<MSApplication>
{
public:
	MSApplication();
	~MSApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	/**
	*@brief �źŴ���
	*/
	virtual void OnSignal(int signal);

	/**
	*@brief ս�����
	*/
	UInt64 GetBattleFlag() const { return m_BattleFlag; }

private:
    /**
    *@brief �����������ݱ�
    */
    bool LoadDataTables();

	/**
	*@brief ���ض�̬������
	*/
	void LoadDynamicConfig();

private:
	//���¼�������
	bool			m_ReloadConfig;

	// ս�����
	UInt64			m_BattleFlag;
};

#endif
