#ifndef _BUFF_MGR_H_
#define _BUFF_MGR_H_

#include <CLDefine.h>
#include <map>
#include <AvalonTime.h>
#include <AvalonSimpleTimer.h>
#include <CLObjectProperty.h>
#include <CLOwnerObjectProperty.h>
#include <CLObject.h>
#include <CLBuffDataEntry.h>
#include <CLDropItemDefine.h>
#include <CLSkillDefine.h>
#include <CLMatchDefine.h>

class Creature;
class BuffMgr;
class DungeonDataEntry;

#define BUFF_OVERLAY_MAX	2000
/**
 *@brief 技能buff
 */
struct Buff
{
public:
	AVALON_DEFINE_OBJECTPOOL(Buff, Avalon::NullMutex)

	Buff();
	~Buff();

public:
	inline void SetUid(UInt64 uid) { m_uid = uid; }
	inline UInt64 GetUid() { return m_uid; }
	inline void SetId(UInt32 id) { m_id = id; }
	inline UInt32 GetId() { return m_id; }
	void SetOverlay(UInt32 num);
	inline UInt32 GetOverlay() { return m_overlayNums; }
	inline void SetDataEntry(BuffDataEntry* data) { m_dataEntry = data; }
	inline BuffDataEntry* GetDataEntry() { return m_dataEntry; }
	inline void SetStartTime() { m_dirty = true;  m_startTime = Avalon::Time::CurrentTime(); }
	inline Avalon::Time GetStartTime() { return m_startTime; }
	UInt32 GetStartTimeSec() const { return (UInt32)m_startTime.Sec(); }
	void SetDuration(UInt32 d) { m_dirty = true;  m_duration = d; }
	inline UInt32 GetDuration() { return m_duration; }

	/**
	*@brief 生效次数
	*/
	void SetEffectTimes(UInt32 num) { m_dirty = true; m_EffectTimes = num; }
	UInt32 GetEffectTimes() { return m_EffectTimes; }
	bool DecreaseEffectTimes(UInt32 num);

	inline void SetDirty(bool d) { m_dirty = d; }
	inline bool GetDirty() { return m_dirty; }

	void SetOpActDataId(UInt32 id) { m_OpActDataId = id; }
	inline UInt32 GetOpActDataId() { return m_OpActDataId; }

	/**
	*@brief 设置获取运营BUFF flag
	*/
	void SetOpActBuffFlag(bool flag) { m_OpActBuffFlag = flag; }
	bool GetOpActBuffFlag() const { return m_OpActBuffFlag; }

public://编解码
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_id;

		if (m_id != 0) stream & m_uid & m_overlayNums & m_duration;
	}
	//db编解码
	void DecodeString(std::istringstream& is);

	void EncodeString(std::ostringstream& os);

	const std::string LogStr() const;

private:
	//uid
	UInt64	m_uid;
	//id
	UInt32	m_id;
	//叠加层数
	UInt32	m_overlayNums;
	//data数据表
	BuffDataEntry* m_dataEntry;
	//开始时间
	Avalon::Time	m_startTime;
	//持续时间
	UInt32		m_duration;
	//生效次数
	UInt32		m_EffectTimes;

	//脏标识
	bool		m_dirty;

	//是否运营活动buff
	bool		m_OpActBuffFlag;

	UInt32      m_OpActDataId = 0;
};

//buff访问器
typedef CLVisitor<Buff>	BuffVisitor;

struct Skill;

/**
 *@brief buff管理器
 */
class BuffMgr : public CLOwnerComplexProperty
{
	//最大buff数
	const static UInt32 MAX_UUFF_NUM = 32;

public:
	BuffMgr();
	~BuffMgr();

public:
	void SetOwner(Creature* owner);
	Creature* GetOwner() const { return m_pOwner; }

public:
	
	Buff* FindBuff(UInt32 buffId);
	Buff* CreateBuff(UInt32 buffid);
	bool AddBuff(UInt32 buffid);
	void VisitBuffs(BuffVisitor& visitor);

	/**
	*@brief 根据类型获取buff
	*/
	void GetBuffs(std::vector<Buff*>& buffs, BuffType buffType);

public:	//事件
	void OnTick(const Avalon::Time& now);


	//bool GetDungeonBufferAddition(UInt8& expRate, UInt8& goldRate, UInt8& taskDropNumRate, UInt16& pinkDropRate);				// 获取地下城Buffer加成
	bool GetDungeonBufferAddition(DungeonAddition& addition, const DungeonDataEntry* dungeonData);

	void OnUseDungeonBuffers(const DungeonDataEntry* dungeonData);

	/**
	*@brief 获取地下城战斗buff信息
	*/
	void GetDungeonRaceBuffInfo(DungeonRacePlayerInfo& dungeonRaceInfo, const DungeonDataEntry* dungeonData);

	/**
	*@brief 添加运营活动buff
	*/
	void AddOpActBuff(UInt32 buffid, UInt32 duration, UInt32 OpActId = 0);

	/**
	*@brief 根据活动id删除运营活动buff
	*/
	bool RemoveOpActBuff(UInt32 OpActId, UInt32 buffid);


	/**
	*@brief 删除buff
	*/
	bool RemoveBuff(UInt32 buffid);

	/**
	*@brief 获取经验加成
	*/
	void GetExpBuffAddition(DungeonAddition& addition);

private:
	/**
	*@brief buff是否适用地下城
	*/
	bool _IsApplyDungeonForBuff(const DungeonDataEntry* dungeonData, const BuffDataEntry* buffData);

	/**
	*@brief 通知删除buff
	*/
	void _NotifyRemoveBuff(UInt32 buffId);

public: //编码
	void Input(Avalon::NetInputStream& stream);
	void Output(Avalon::NetOutputStream& stream);

	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

	void SyncClient();

private:
	//拥有者
	Creature*	m_pOwner;
	//buff
	std::vector<Buff*>		m_Buffs;

	//上次tick时间
	Avalon::SimpleTimer m_Timer;
};

#endif
