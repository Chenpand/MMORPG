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
	*@brief ͬ�������
	*/
	void SyncActivityAttributes(const OpActivityRegInfo* opAct);

public:
	/*
	*@brief ����ӹ���
	*/
	void OnOpenJar(const OpActivityRegInfo* opAct, UInt32 comboBuyNum, UInt32& costMoney);

public:
	/*
	*@brief ���Ӵ���
	*/
	void DiscountJarCostMoneyByOpActivity(const OpActivityRegInfo* opAct, UInt32 comboBuyNum, UInt32& costMoney);

public:
	/*
	*@brief ͬ���������Ӵ�����Ϣ
	*/
	void SyncArtifactJarDiscountInfo(UInt32 opActId);

	/*
	*@brief ����Ƿ���Գ�ȡ�������Ӵ���
	*/
	bool CanExtractArtifactJarDiscount(UInt32 opActId);

	/*
	*@brief ��ȡ�������Ӵ���
	*/
	void OnExtractArtifactJarDiscount(UInt32 opActId, UInt32 discountRate, UInt32 effectTimes);

private:
	/*
	*@brief �������ӻ����
	*/
	void _DiscountArtifactJarCostMoney(const OpActivityRegInfo* opAct, UInt32 comboBuyNum, UInt32& costMoney);

private:
	Player* m_Owner;
	OpActivityAttributeVec m_OpActAttrVec;
};

#endif