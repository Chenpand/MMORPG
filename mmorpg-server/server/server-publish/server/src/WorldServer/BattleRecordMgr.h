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
	*@brief 清空数据
	*/
	void Clear();

public:
	/**
	*@brief 添加记录
	*/
	void AddEntry(BattleRecordEntry* entry);

	/**
	*@brief 获取记录数量
	*/
	UInt32 GetSize() const;

	/**
	*@brief 获取记录
	*/
	std::vector<BattleRecordEntry*> GetRecord(UInt32 startPos, UInt32 count) const;

	/**
	*@brief 获取自己的记录
	*/
	std::vector<BattleRecordEntry*> GetSelfRecord(ObjID_t roleId) const;

protected:
	/**
	*@brief 单次获取的最大数量
	*/
	virtual UInt32 GetPreTimeMaxCount() const { return 100; }

private:
	std::vector<BattleRecordEntry*>		m_Entrys;
};

#endif