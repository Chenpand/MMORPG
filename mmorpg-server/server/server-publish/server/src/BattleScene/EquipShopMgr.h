#ifndef _SCENE_EQUIP_SHOP_H_
#define _SCENE_EQUIP_SHOP_H_

#include <CLDefine.h>
#include <CLObjectProperty.h>
#include <CLItemProtocol.h>

class Player;

/**
*@brief ���װ���̵�
*/
class EquipShopMgr : public CLComplexProperty
{
	const static UInt32 NORMAL_SHOP_ITEM_NUM = 10;

public:
	EquipShopMgr();
	~EquipShopMgr();

	/**
	*@brief ���á����������
	*/
	void SetOwner(Player *pPlayer) { m_pOwner = pPlayer; }
	Player* GetOwner() const { return m_pOwner; }

	/**
	*@brief ����װ��
	*/
	void Buy(UInt8 uType, UInt8 uIndex);

public:	// �¼�
	/**
	*@brief ����
	*/
	void OnOnline();

	/**
	*@brief ����
	*/
	void OnTick(const Avalon::Time &now, int tickType);

	/**
	*@brief ����
	*/
	void OnLevelUp();

public: //�����
	void Input(Avalon::NetInputStream& stream);
	void Output(Avalon::NetOutputStream& stream);
	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

private:
	/**
	*@brief ˢ��װ���б�
	*/
	void RefreshEquipList();

	/**
	*@brief ͬ��װ���б�
	*/
	void SyncEquipList();

private:
	// ������
	Player		*m_pOwner;
	// �ϴ�ˢ��ʱ��
	UInt32		m_uLastRefreshTime;
	// ��ͨ������Ʒ
	std::list<CLProtocol::RandEquipEntry>	m_listNormal;
	// vip������Ʒ
	std::list<CLProtocol::RandEquipEntry>	m_listVip;
};

#endif

