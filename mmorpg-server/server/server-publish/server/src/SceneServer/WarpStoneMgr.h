#ifndef _WARP_STONE_MGR_H_
#define _WARP_STONE_MGR_H_

#include <CLDefine.h>
#include <CLObject.h>
#include <CLObjectProperty.h>
#include <CLOwnerObjectProperty.h>
#include <CLRecordCallback.h>
#include "WarpStone.h"

/**
*@brief ��Ԫʯ������
*/
typedef CLVisitor<WarpStone>	WarpStoneVisitor;

/**
*@brief ��Ԫʯ������
*/
class WarpStoneMgr : public CLOwnerComplexProperty
{
public:
	typedef std::vector<WarpStone> WarpStoneVec;

public:
	WarpStoneMgr();
	~WarpStoneMgr();

	void SetOwner(Player* player);
	Player* GetOwner() const { return m_pOwner; }

public:

	/**
	*@brief ����һ����Ԫʯ
	*/
	WarpStone* FindWarpStone(UInt32 id);

	/**
	*@brief ��ȡ��Ԫʯ����
	*/
	UInt32 GetWarpStoneSize(WarpStoneType type = STONE_INVALID, UInt32 level = 0);

	/**
	*@brief ��ȡ��Ԫʯ��С�ȼ�
	*/
	UInt32 GetWarpStoneMinLevel(UInt32 level, UInt32 size, WarpStoneType type = STONE_INVALID);

	/**
	*@brief ���ô�Ԫʯ�ȼ�
	*/
	void WarpStoneUpLevel(UInt32 id, UInt32 level);

	/**
	*@brief �������д�Ԫʯ
	*/
	void VisitWarpStone(WarpStoneVisitor& visitor);

	/**
	*@brief ����
	*/
	UInt32 Unlock(UInt32 id);

	/**
	*@brief ����
	*/
	UInt32 Charge(UInt32 id, std::vector<ChargeInfo> chargeInfo);

public: //�¼�
	/**
	*@brief �������
	*/
	void OnLevelup();


public:
	void Input(Avalon::NetInputStream& stream);
	void Output(Avalon::NetOutputStream& stream);

	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

	void ClearInvalidData();

private:
	//ӵ����
	Player* m_pOwner;
	//��Ԫʯ
	WarpStoneVec m_WarpStones;

};



#endif //_WARP_STONE_MGR_H_
