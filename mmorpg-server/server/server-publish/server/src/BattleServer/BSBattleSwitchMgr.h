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

	// 是否开启
	bool IsOpen();

private:
	// 初始化数据
	void _InitTime();
	// 插入开启时间
	void _InsertOpenTime();
	// 更新开启时间
	void _UpdateOpenTime();

private:
	// Tick定时器，1000ms
	Avalon::SimpleTimer				m_TickTimer1000ms;
	// Tick定时器，30s
	Avalon::SimpleTimer				m_TickTimer30s;

	// 开启时间
	UInt64 m_OpenTime;
	// 当前星期几
	Int32 m_CurWeek;
	// 开始时间
	UInt64 startTime;
	// 结束时间
	UInt64 endTime;
};

#endif