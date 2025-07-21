#ifndef _VIP_H_
#define _VIP_H_

#include <CLDefine.h>
#include <CLObjectProperty.h>
#include <CLVipDataEntry.h>

class Player;

class Vip : public CLComplexProperty
{
public:
	Vip();
	~Vip();

	void Init(UInt8 level, UInt32 exp);

	Player* GetOwner() { return m_Owner; };
	void SetOwner(Player* owner) { m_Owner = owner; }

	//获取VIP等级
	UInt8 GetLevel() { return m_Level; }

	//获取VIP经验
	UInt32 GetExp() { return m_Exp; }


public:
	void AddExp(const char* reason, UInt32 exp);

	void CheckLevel();

	void LevelUp();

	void GetValue(VipPrivilegeType type, UInt32& value);

	void GetValue(VipPrivilegeType type, bool& value);

	UInt32 GetValueById(UInt32 id);

	UInt32 BuyGiftItem(UInt8 vipLevel);

	void SaveToDB();

public:
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_Level & m_Exp;
	}

	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

private:

	void OnLevelUp();

private:

	//玩家
	Player* m_Owner;

	//VIP等级
	UInt8 m_Level;

	//VIP经验
	UInt32 m_Exp;

	//是否脏数据
	bool m_IsDirty;

	//数据项
	VipDataEntry* m_pDataEntry;

};



#endif // _VIP_H_