#ifndef _ACTIVITY_ATTRIBUTE_MGR_H_
#define _ACTIVITY_ATTRIBUTE_MGR_H_

#include <CLActivityDefine.h>

class Player;
class CLRecordCallback;
class OpActivityAttribute;

typedef std::vector<OpActivityAttribute*> OpActivityAttributeVec;

struct OpActivityRegInfo;

class ActivityAttributeMgr
{
public:
	ActivityAttributeMgr();
	~ActivityAttributeMgr();

public:
	void SetOwner(Player* player) { m_Owner = player; }
	Player* GetOwner() { return m_Owner; }

public:
	void LoadOpActivityAttributeFromDB(CLRecordCallback* callback);

public:
	OpActivityAttribute* CreateAttribute(UInt32 opActId);

	OpActivityAttribute* FindOpActivityAttribute(UInt32 opActId);

	/*
	*@brief 同步活动属性
	*/
	void SyncActivityAttributes(const OpActivityRegInfo* opAct);

public:
	/*
	*@brief 抽罐子罐子
	*/
	void OnOpenJar(const OpActivityRegInfo* opAct, UInt32 comboBuyNum, UInt32& costMoney);

public:
	/*
	*@brief 罐子打折
	*/
	void DiscountJarCostMoneyByOpActivity(const OpActivityRegInfo* opAct, UInt32 comboBuyNum, UInt32& costMoney);

public:
	/*
	*@brief 同步神器罐子打折信息
	*/
	void SyncArtifactJarDiscountInfo(UInt32 opActId);

	/*
	*@brief 检查是否可以抽取神器罐子打折
	*/
	bool CanExtractArtifactJarDiscount(UInt32 opActId);

	/*
	*@brief 抽取神器罐子打折
	*/
	void OnExtractArtifactJarDiscount(UInt32 opActId, UInt32 discountRate, UInt32 effectTimes);

private:
	/*
	*@brief 神器罐子活动打折
	*/
	void _DiscountArtifactJarCostMoney(const OpActivityRegInfo* opAct, UInt32 comboBuyNum, UInt32& costMoney);

private:
	Player* m_Owner;
	OpActivityAttributeVec m_OpActAttrVec;
};

#endif