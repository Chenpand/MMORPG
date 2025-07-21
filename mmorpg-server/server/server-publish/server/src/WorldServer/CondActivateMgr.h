#ifndef _COND_ACTIVATE__MGR_
#define _COND_ACTIVATE__MGR_

#include "CondActivateInfo.h"

#include <CLMallDefine.h>
#include <AvalonSimpleTimer.h>
#include <CLMallPersonalTailorDataEntry.h>
#include <CLMallGiftPackDataEntry.h>


// ˽�˶��ƴ���״̬
enum PersonalTailorActivateState
{
	// ��ʼ��
	PTAS_INIT = 0,
	// �ѿ���
	PTAS_OPEN,
	// �ѹر�
	PTAS_CLOSED,
	// ����
	PTAS_ONLINE,
};

// ������������
enum ActivateCondType
{
	// ������
	COND_TYPE_NONE = 0,
	// ���ô���
	COND_TYPE_SHARE,
};

/**
*@brief ��������key
*/
struct ActivateCountKey
{
	ActivateCountKey(UInt8 type, UInt8 cond) : m_ActivateType(type), m_ActivateCond(cond) {}

	// ��������
	UInt8 m_ActivateType;
	// ��������
	UInt8 m_ActivateCond;

	bool operator < (const ActivateCountKey& other) const
	{
		if (m_ActivateType < other.m_ActivateType)
		{
			return true;
		}
		if (other.m_ActivateType < m_ActivateType)
		{
			return false;
		}

		if (m_ActivateCond < other.m_ActivateCond)
		{
			return true;
		}
		if (other.m_ActivateCond < m_ActivateCond)
		{
			return false;
		}

		return false;
	}
};


/**
*@brief ��������������
*/
//typedef CLVisitor<CondActivateInfo>	CondActivateInfoVisitor;

/**
*@brief ��������������
*/
class CondActivateMgr
{

public:
	CondActivateMgr();
	~CondActivateMgr();

public:
	/**
	*@brief ���û�ȡӵ����
	*/
	void SetOwner(WSPlayer* player) { m_Owner = player; }
	WSPlayer* GetOwner() const { return m_Owner; }

	/**
	*@brief ����һ������
	*/
	void ActivateCond(UInt8 activateCond, UInt8 activateType);

	/**
	*@brief ������������
	*/
	bool AddCondActivateInfo(CondActivateInfo* info);

	/**
	*@brief ���ߴ���
	*/
	void OnOnline();

	/**
	*@brief ����
	*/
	void OnTick(const Avalon::Time& now);

	//void VisitCondActivateInfo(CondActivateInfoVisitor& visitor);

	/**
	*@brief ��ȡ��������
	*/
	CondActivateInfo* FindCondActivateInfoByCond(UInt8 activateType, UInt8 activateCond);

	/**
	*@brief ��ȡ�ѿ�������������
	*/
	CondActivateInfo* FindOpenCondActivateInfoByCond(UInt8 activateType, UInt8 activateCond);
	CondActivateInfo* FindOpenCondActivateInfoByGoodsId(UInt8 activateType, UInt32 mallGoodsId);

private:
	/* ˽�˶��� */
	/**
	*@brief ����˽�˶���
	*/
	void _ActivatePersonalTailor(UInt8 activateCond);

	/**
	*@brief ����Ƿ����ƴ���˽�˶���
	*/
	bool _IsLimitCreatePersonalTailor(MallPersonalTailorDataEntry* dataEntry);

	/**
	*@brief ͬ���ͻ���˽�˶���
	*/
	void _SyncMallPersonalTailorState(PersonalTailorActivateState state);

private:
	/* ��ʱ��� */
	/**
	*@brief ������ʱ���
	*/
	void _ActivateLimitTimeGiftPack(UInt8 activateCond);

	/**
	*@brief ����Ƿ����ƴ�����ʱ���
	*/
	bool _IsLimitCreateLimitTimeGiftPack(MallGiftPackDataEntry* dataEntry);

private:
	/* ͨ�õĽӿ� */
	/**
	*@brief ������������
	*/
	void _CreateCondActivate(UInt8 activateCond, UInt32 mallGoodsId, UInt16 limitInterval, ConditionActivateType activateType, UInt8 activateSubCond = 0);

private:
	// ���(һ����ɫ)
	WSPlayer*			m_Owner;

	// ���˺����Ѵ������������ļ���
	std::map<ActivateCountKey, UInt16>	m_ActivatedCondCountOnAccount;

	// һ����ɫ�µ�ȫ����������
	std::vector<CondActivateInfo*>	m_AllCondActivateInfo;

	// ˽�˶�������״̬
	PersonalTailorActivateState		m_PersonalTailorPushState;
};

#endif