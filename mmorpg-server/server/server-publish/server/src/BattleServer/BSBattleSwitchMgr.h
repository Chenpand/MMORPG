#ifndef _BS_BATTLE_SWITCH_MGR_H_
#define _BS_BATTLE_SWITCH_MGR_H_

#include <CLDefine.h>
#include <AvalonSimpleTimer.h>

class BSBattleSwitchMgr : public Avalon::Singleton<BSBattleSwitchMgr>
{
public:
	BSBattleSwitchMgr();
	~BSBattleSwitchMgr();

public:

	bool Init();
	
	void OnTick(const Avalon::Time& now);

	// �Ƿ���
	bool IsOpen();

private:
	// ��ʼ������
	void _InitTime();
	// ���뿪��ʱ��
	void _InsertOpenTime();
	// ���¿���ʱ��
	void _UpdateOpenTime();

private:
	// Tick��ʱ����1000ms
	Avalon::SimpleTimer				m_TickTimer1000ms;
	// Tick��ʱ����30s
	Avalon::SimpleTimer				m_TickTimer30s;

	// ����ʱ��
	UInt64 m_OpenTime;
	// ��ǰ���ڼ�
	Int32 m_CurWeek;
	// ��ʼʱ��
	UInt64 startTime;
	// ����ʱ��
	UInt64 endTime;
};

#endif