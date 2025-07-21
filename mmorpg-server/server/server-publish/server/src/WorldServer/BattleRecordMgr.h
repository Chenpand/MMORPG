#ifndef __BATTLE_RECORD_MGR_H__
#define __BATTLE_RECORD_MGR_H__

#include <CLDefine.h>
#include <CLPremiumLeagueDefine.h>

class BattleRecordMgr
{
public:
	BattleRecordMgr();
	~BattleRecordMgr();
	
	/**
	*@brief �������
	*/
	void Clear();

public:
	/**
	*@brief ��Ӽ�¼
	*/
	void AddEntry(BattleRecordEntry* entry);

	/**
	*@brief ��ȡ��¼����
	*/
	UInt32 GetSize() const;

	/**
	*@brief ��ȡ��¼
	*/
	std::vector<BattleRecordEntry*> GetRecord(UInt32 startPos, UInt32 count) const;

	/**
	*@brief ��ȡ�Լ��ļ�¼
	*/
	std::vector<BattleRecordEntry*> GetSelfRecord(ObjID_t roleId) const;

protected:
	/**
	*@brief ���λ�ȡ���������
	*/
	virtual UInt32 GetPreTimeMaxCount() const { return 100; }

private:
	std::vector<BattleRecordEntry*>		m_Entrys;
};

#endif