#ifndef _CL_RETINUE_MGR_H_
#define _CL_RETINUE_MGR_H_

#include <CLRetinueDefine.h>
#include <CLRetinueDataEntry.h>
#include <CLObject.h>
#include <CLObjectProperty.h>
#include <CLRecordCallback.h>
#include <CLSyncObject.h>
#include "Retinue.h"

class OffRetinueList : public CLComplexProperty
{
public:
	OffRetinueList();
	~OffRetinueList();

public:
	std::vector<UInt64>& GetRetinueIds() { return m_OffRetinueIds; };

	void SetOffRetinueId(UInt32 index, UInt64 value);

	UInt64 GetOffRetinueId(UInt32 index);

	void RemoveOffRetinueId(UInt64 id);

	bool IsUsing(UInt64 id, UInt32& index);

public:
	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

private:
	std::vector<UInt64> m_OffRetinueIds;

};

/**
*@brief 随从访问器
*/
typedef CLVisitor<Retinue>	RetinueVisitor;

/**
*@brief 随从管理器
*/
class RetinueMgr
{
public:
	typedef std::vector<Retinue*> RetinueVec;
	typedef std::map<ObjID_t, Retinue*> RetinueMap;

public:
	RetinueMgr();
	~RetinueMgr();

public:
	static bool Init();

public:
	void SetOwner(Player* player) { m_pOwner = player; }
	Player* GetOwner() const { return m_pOwner; }

	/**
	*@brief 访问所有随从
	*/
	void VisitorRetinue(RetinueVisitor& visitor);

	/**
	*@brief 根据数据表ID查找一个随从
	*/
	Retinue* FindRetinue(UInt32 dataId);

	/**
	*@brief 获取随从个数
	*/
	UInt32 GetRetinueSize();

	/**
	*@brief 根据ID查找一个随从
	*/
	Retinue* FindRetinue(ObjID_t id);

	/**
	*@brief 正在使用的主随从
	*/
	ObjUInt64& GetRetinueFighting() { return m_FightingRetinue; }

	/**
	*@brief 正在使用的副随从
	*/
	OffRetinueList& GetOffRetinueList() { return m_OffRetinueList; }

	/**
	*@brief 正在使用的副随从ID
	*/
	std::vector<UInt64>& GetOffRetinueIds() { return m_OffRetinueList.GetRetinueIds(); }

	/**
	*@brief 正在使用的随从DataId
	*/
	ObjUInt32& GetFightingDataId();

	/**
	*@brief 在场景中使用的随从
	*/
	ObjSceneRetinue& GetSceneRetinueInfo();

	/**
	*@brief 设置使用的随从
	*/
	void SetRetinueFighting(UInt64 id, UInt32 dataId, UInt8 level = 0);

	/**
	*@brief 单局随从
	*/
	void GetRaceRetinues(std::vector<RaceRetinue>& raceRetinues);

	/**
	*@brief 获取历史洗练次数
	*/
	UInt32 GetChangeSkillTotal();

	/**
	*@brief 获取技能个数
	*/
	UInt32 GetSkillCount(UInt32 id, UInt8 level);

	/**
	*@brief 获取随从最大等级
	*/
	UInt32 GetMaxRetinueLevel();

	/**
	*@brief 获取随从最大星级
	*/
	UInt32 GetMaxRetinueStar();

public:

	/**
	*@brief 增加随从
	*/
	UInt32 AddRetinue(UInt32 dataId, bool isConsume = false);

	/**
	*@brief 增加随从
	*/
	UInt32 AddRetinue(Retinue* retinue);

	/**
	*@brief 升级随从
	*/
	UInt32 UpLevelRetinue(ObjID_t id);

	/**
	*@brief 升星随从
	*/
	UInt32 UpStarRetinue(ObjID_t id);

	/**
	*@brief 洗练随从技能
	*/
	UInt32 ChangeSkill(ObjID_t id, std::vector<UInt32> skillIds);

	/**
	*@brief 洗练指定随从技能
	*/
	UInt32 TestChangeSkill(ObjID_t id, std::vector<UInt32> skillIds);

	/**
	*@brief 设置使用的随从
	*/
	UInt32 SetRetinue(ObjID_t id, UInt32 index = MAIN_RETINUE_INDEX);

	/**
	*@brief 检查添加随从
	*/
	UInt32 CheckAddRetinueByItemId(UInt32 itemId);

	/**
	*@brief 获取指定品阶以上的数量
	*/
	UInt32 GetRetinueSizeByQuality(RetinueQuality quality);

	/**
	*@brief 重置洗练次数
	*/
	void ResetChangeSkillCount();

	/**
	*@brief 保存到数据库
	*/
	void SaveToDB();

public: //事件
	/**
	*@brief 上线事件
	*/
	void OnOnline();

	/**
	*@brief 查询回调
	*/
	void OnSelectDataRet(CLRecordCallback* callback);

private:

	/**
	*@brief 检查添加随从
	*/
	UInt32 CheckAddRetinue(UInt32 dataId);

	/**
	*@brief 根据ID卸下副随从
	*/
	void RemoveOffRetinue(UInt64 id);

	/**
	*@brief 获取协议使用的结构
	*/
	std::vector<RetinueInfo> GetRetinueList();

	/**
	*@brief 下发随从列表
	*/
	void SyncRetinueList();

	/**
	*@brief 下发随从信息
	*/
	void SyncRetinue(Retinue* retinue);

private:
	//拥有者
	Player*	m_pOwner;

	//正在使用的主随从ID
	ObjUInt64	m_FightingRetinue;

	//正在使用的主随从DataId
	ObjUInt32	m_FightingDataId;

	//正在使用的主随从
	ObjSceneRetinue m_SceneRetinue;

	//副随从
	OffRetinueList m_OffRetinueList;

	//随从列表	对象Retinue*的列表
	RetinueVec	m_RetinueVec;

	//随从map, 以随从的guid为key
	RetinueMap	m_RetinueMap;

	//消耗点卷的系数
	UInt32		m_PointRatio;

};

#endif //_CL_RETINUE_MGR_H_
