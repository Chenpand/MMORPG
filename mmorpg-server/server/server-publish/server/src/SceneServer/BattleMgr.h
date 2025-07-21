#ifndef _BATTLE_MGR_H_
#define _BATTLE_MGR_H_

#include <CLDefine.h>

/**
 *@brief 吃鸡管理
 */
class BattleMgr : public Avalon::Singleton<BattleMgr>
{
public:
	BattleMgr();
	~BattleMgr();

	// 吃鸡开放时间
	UInt32 GetOpenTime() { return m_OpenTime; }
	void SetOpenTime(UInt32 time) { m_OpenTime = time; }

private:

	// 开放时间
	UInt32 m_OpenTime;

};

#endif
