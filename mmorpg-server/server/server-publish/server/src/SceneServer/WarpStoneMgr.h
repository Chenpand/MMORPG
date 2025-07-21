#ifndef _WARP_STONE_MGR_H_
#define _WARP_STONE_MGR_H_

#include <CLDefine.h>
#include <CLObject.h>
#include <CLObjectProperty.h>
#include <CLOwnerObjectProperty.h>
#include <CLRecordCallback.h>
#include "WarpStone.h"

/**
*@brief 次元石访问器
*/
typedef CLVisitor<WarpStone>	WarpStoneVisitor;

/**
*@brief 次元石管理器
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
	*@brief 查找一个次元石
	*/
	WarpStone* FindWarpStone(UInt32 id);

	/**
	*@brief 获取次元石个数
	*/
	UInt32 GetWarpStoneSize(WarpStoneType type = STONE_INVALID, UInt32 level = 0);

	/**
	*@brief 获取次元石最小等级
	*/
	UInt32 GetWarpStoneMinLevel(UInt32 level, UInt32 size, WarpStoneType type = STONE_INVALID);

	/**
	*@brief 设置次元石等级
	*/
	void WarpStoneUpLevel(UInt32 id, UInt32 level);

	/**
	*@brief 访问所有次元石
	*/
	void VisitWarpStone(WarpStoneVisitor& visitor);

	/**
	*@brief 解锁
	*/
	UInt32 Unlock(UInt32 id);

	/**
	*@brief 充能
	*/
	UInt32 Charge(UInt32 id, std::vector<ChargeInfo> chargeInfo);

public: //事件
	/**
	*@brief 玩家升级
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
	//拥有者
	Player* m_pOwner;
	//次元石
	WarpStoneVec m_WarpStones;

};



#endif //_WARP_STONE_MGR_H_
