#ifndef _BATTLE_MGR_H_
#define _BATTLE_MGR_H_

#include <CLDefine.h>

/**
 *@brief �Լ�����
 */
class BattleMgr : public Avalon::Singleton<BattleMgr>
{
public:
	BattleMgr();
	~BattleMgr();

	// �Լ�����ʱ��
	UInt32 GetOpenTime() { return m_OpenTime; }
	void SetOpenTime(UInt32 time) { m_OpenTime = time; }

private:

	// ����ʱ��
	UInt32 m_OpenTime;

};

#endif
