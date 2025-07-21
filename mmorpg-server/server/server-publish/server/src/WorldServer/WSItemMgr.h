#ifndef _WS_ITEM_MGR_H_
#define _WS_ITEM_MGR_H_

#include "WSPackage.h"
#include "CLObjectIndex.h"
#include <AvalonSimpleTimer.h>

class Tribe;
class WSPlayer;

/**
 *@brief ���߹�����
 */
class WSItemMgr
{
public:
	WSItemMgr();
	~WSItemMgr();

public:
	/**
	 *@brief ���û�ȡӵ����
	 */
	void SetOwner(Tribe* owner) { m_pOwner = owner; }
	Tribe* GetOwner() const { return m_pOwner; }

	/** 
	 *@brief ��ȡ����
	 */
	WSPackage* GetPackage(UInt8 pack) const;

	/**
	 *@brief ���ҵ���
	 */
	WSItem* FindItem(ObjID_t id);

	/**
	 *@brief ����һ������
	 */
	bool LoadItem(WSItem* item);

	/**
	 *@brief �Ƴ�����
	 */
	UInt16 RemoveItem(WSPlayer *pPlayer, WSItem* item, UInt16 uNum);
	void RemoveItemDirect(WSItem *item);

	/**
	 *@brief ������Ӻ�ɾ������
	 */
	bool CheckAddItem(UInt32 itemid, UInt16 num, UInt8 quality, UInt8 pack, UInt8 strengthen);
	UInt16 GetGridUsed(UInt32 itemid, UInt16 num, UInt8 quality, UInt8 pack, UInt8 strengthen);
	UInt16 AddItem(WSPlayer *pPlayer, WSItem* item, UInt8 pack, UInt32 price = 0);
	bool IsFull(UInt8 pack);

	/**
	*@brief ����
	*/
	void Lock() { m_uLock = 3; }
	void UnLock() { m_uLock = 0; }
	bool IsLocked() { return m_uLock != 0; }

public://ͬ�����
	/**
	 *@brief ͬ�������
	 */
	void SyncToPlayer(WSPlayer *pPlayer, UInt8 pack);

	/**
	 *@brief ͬ���������ߵ��ͻ���
	 */
	void SyncToPlayer(WSPlayer *pPlayer, WSItem* item);

	/**
	*@brief ���浥������
	*/
	void SaveItem(WSItem *pItem);
	
public:
	/**
	 *@brief �������е���
	 */
	void VisitItems(WSItemVisitor& visitor);

	/**
	 *@brief ����ָ����������
	 */
	void VisitItems(UInt8 pack, WSItemVisitor& visitor);
	
public://�¼�
	/**
	 *@brief ����
	 */
	void OnTick(const Avalon::Time& now);

private:
	/**
	 *@brief �ӵ��ߵ�ָ��λ��
	 */
	bool AddItem(WSPlayer *pPlayer, WSItem* item, const ItemPos pos, UInt32 uPrice = 0);

private:
	//ӵ����
	Tribe*		m_pOwner;
	//����
	WSPackage*	m_pPackages[TRIBE_PACK_MAX];
	//�����б�
	CLIDObjIndex<WSItem>	m_Items;
	// ������ʶ
	UInt8		m_uLock;
	// ������ʱ��
	Avalon::SimpleTimer		m_timer;

};

#endif

