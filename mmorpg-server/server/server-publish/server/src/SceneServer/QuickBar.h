#ifndef _QUICKBAR_H_
#define _QUICKBAR_H_

#include <CLObjectProperty.h>
#include <CLOwnerObjectProperty.h>
#include <CLSettingDefine.h>
#include <CLErrorCode.h>
#include <CLRecordClient.h>
#include <CLSkillDefine.h>
#include <map>

class Player;
class SkillMgr;
class PlayerSkillMgr;

/**
 *@brief 快捷栏  by huchenhui
 */
class QuickBar : public CLOwnerComplexProperty
{
public:
	QuickBar() {};
	virtual ~QuickBar() {};

	void SetOwner(Player* player) { m_Player = player; }
	Player* GetOwner() { return m_Player; }

	virtual void Init() = 0;
	virtual void OnLevelup() = 0;


public: //编解码

	virtual void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty) = 0;

	virtual void Decode(Avalon::NetInputStream& stream) = 0;

	virtual void DecodeString(std::istringstream& is) = 0;

	virtual void EncodeString(std::ostringstream& os) = 0;

protected:
	//玩家对象
	Player* m_Player;
};


/**
*@brief 技能栏
*/
class SkillBar : public QuickBar
{
public:
	typedef std::vector<SkillBarInfo> SkillBarVec;
	typedef std::vector<UInt16> SkillIdsVec;

public:
	SkillBar();
	virtual ~SkillBar();

protected:
	/**
	*@brief 检查技能方案
	*/
	UInt32 CheckBars(SkillBarVec skillBars);

	/**
	*@brief 检查某页技能方案
	*/
	UInt32 CheckBars(SkillBarVec skillBars, UInt32 page);

	/**
	*@brief 检查技能条
	*/
	UInt32 CheckBar(SkillBarInfo skillBar);

	/**
	*@brief 获取实际存在的SkillBars
	*/
	SkillBarVec GetExistSkillBar();

	/**
	*@brief 设置实际存在的SkillBars
	*/
	void SetExistSkillBar(SkillBarVec skillBars);

	/**
	*@brief 获取空闲的格子
	*/
	UInt32 GetFreeGridSize();



public:
	/**
	*@brief 是否存在技能
	*/
	bool IsExist(UInt16 id);

	/**
	*@brief 获取技能的槽位
	*/
	UInt8 GetSlot(UInt16 id);

	/**
	*@brief 设置当前方案的槽位
	*/
	void SetGrid(UInt8 slot, UInt16 id);

	/**
	*@brief 设置方案
	*/
	void SetBarIndex(UInt32 barIndex);

	/**
	*@brief 获得方案当前页
	*/
	inline UInt32 GetBarIndex() { return m_BarIndex; }


	/**
	*@brief 设置技能条
	*/
	UInt32 PushGrid(UInt8 barIndex, SkillIdsVec ids);

	/**
	*@brief 设置技能条
	*/
	//UInt32 PushGrid(SkillIdsVec ids);

	/**
	*@brief 设置技能到固定的格子
	*/
	UInt32 PushFixGrid(UInt8 barIndex, SkillGridVec skillGirdVec);

	/**
	*@brief 检查推荐配置和现有技能格子
	*/
	bool CheckRecommendGrid(UInt8 barIndex, SkillGridVec skillGirdVec);

	/**
	*@brief 清除项
	*/
	void ClearEntry(UInt16 id);

	/**
	*@brief 清除所有项
	*/
	void ClearAllEntry(UInt32 page);

	/**
	*@brief 获取技能管理
	*/
	PlayerSkillMgr& GetSkillMgr();

	/**
	*@brief 最大bar数量
	*/
	UInt32 GetMaxBarSize() { return m_MaxBarSize; }

public:
	/**
	*@brief 设置
	*/
	UInt32 SetBars(UInt8 index, std::vector<SkillBarInfo> tbars, bool isContinue = false);


	/**
	*@brief 不带类型的拷贝
	*/
	void CopyWithOutConfig(const SkillBar & rsh);


	/**
	*@brief 清除非法格子
	*/
	void ClearInvaildPvpBar();
public:

	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);

	void Decode(Avalon::NetInputStream& stream);

	void DecodeString(std::istringstream& is);

	void EncodeString(std::ostringstream& os);

public:
	using CLOwnerComplexProperty::Init;
	virtual void Init();
	virtual void OnLevelup();
	virtual void ClearNotValidEntry(UInt32 page);

protected:

	//一个bar最大格子数
	UInt32 m_MaxGridSize;

	//最大bar数量
	UInt32 m_MaxBarSize;

	//当前方案
	UInt32	m_BarIndex;

	//技能方案
	SkillBarVec m_SkillBars;

	//技能类型
	SkillConfigType m_SkillConfigType;

};

class PvpSkillBar : public SkillBar
{
public:
	PvpSkillBar():SkillBar() { m_SkillConfigType = SkillConfigType::SKILL_CONFIG_PVP; }
	~PvpSkillBar() { }
};


class EqualPvpSkillBar : public SkillBar
{
public:
	EqualPvpSkillBar() :SkillBar() { m_SkillConfigType = SkillConfigType::SKILL_CONFIG_EQUAL_PVP; }
	~EqualPvpSkillBar() { }
};



class ItemBar
{
public:
	ItemBar()
	{
	};
	virtual ~ItemBar() {};

private:


public:


private:


};

class WeaponBar : public QuickBar
{
public:
	WeaponBar();

	virtual ~WeaponBar();

public:
	using CLOwnerComplexProperty::Init;
	virtual void Init() {}
	virtual void OnLevelup() {}

public:

	bool IsInvalid();

	bool IsExist(ObjID_t id);

	UInt32 SetBarById(ObjID_t oldId, ObjID_t newId);

	UInt32 SetBarByIndex(UInt32 index, ObjID_t newId);

	UInt64 GetBarByIndex(UInt32 index);

public:

	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);

	void Decode(Avalon::NetInputStream& stream);

	void DecodeString(std::istringstream& is);

	void EncodeString(std::ostringstream& os);

private:

	//最大bar数量
	UInt32 m_MaxGridSize;

	std::vector<ObjID_t> m_Weapons;
};

#endif
