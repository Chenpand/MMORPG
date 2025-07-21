#ifndef _MALL_GIFT_PACK_MGR_H_
#define _MALL_GIFT_PACK_MGR_H_


#include <CLMallDefine.h>


class MallGiftPack;
class WSPlayer;

//���id-->�������
typedef HashMap<UInt32, MallGiftPack*> RoleMallGiftPackMap;

//�������-->����
typedef HashMap<UInt8, UInt16>	CondCounterMap;

//���߼�
//typedef std::vector<MallItemInfo> MallItemInfosVec;


//�Ѽ�����ʱ���������
class MallGiftPackMgr
{
public:
	MallGiftPackMgr();
	~MallGiftPackMgr();

public:
	//����������
	void SetOwner(WSPlayer* player) { m_Owner = player; }
	WSPlayer* GetOwner() const { return m_Owner; }
	
	//�����̳���ʱ���
	void ActivateLimitGiftPack(UInt8 activateCond);

	//���û�ȡ����ɫ�µ���ʱ���
	bool SetRoleMallGiftPack(MallGiftPack* giftPack);
	MallGiftPack* GetMallGiftPack(UInt32 mallGoodsId);

	//���ñ���ɫ�� ����-->����
	void SetRoleCondCounter(UInt8 cond, UInt16 counter);

	//���ӱ��˺��� ����-->����
	void AddAccountCondCounter(UInt8 cond, UInt16 counter);

private:
	//�������
	UInt32 _CreateGiftPack(MallItemInfo* itemInfo, MallItemInfosVec& itemInfos, UInt8 activateCond);

	//����ɫ�¸������Ƿ���Լ������
	bool _IsRoleLimitActivateGiftPack(UInt8 activateCond, UInt16 limitTimes);

	//���˺��¸������Ƿ���Լ������
	bool _IsAccLimitActivateGiftPack(UInt8 activateCond, UInt16 limitTimes);

	//�����ѡһ���
	UInt32 _ActivateChooseOneGiftPack(UInt8 activateCond, MallItemInfosVec& itemInfos);

	//�������ͻ���
	void _SendToClient(MallItemInfosVec items, UInt32 errorCode);

	//������ʱ���
	void _DestroyGiftPacks();

private:
	//���(һ����ɫ)
	WSPlayer*			m_Owner;

	//����ɫ�µ��Ѽ�����ʱ���
	RoleMallGiftPackMap m_RoleGiftPacks;

	//����ɫ���Ѵ�����������
	CondCounterMap m_RoleCondCounter;

	//���˺����Ѵ�����������
	CondCounterMap m_AccCondCounter;
};

#endif