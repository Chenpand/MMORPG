#ifndef _OPEN_JAR_MGR_H_
#define _OPEN_JAR_MGR_H_
#include <vector>
#include "CLItemDataEntry.h"
#include "CLEventMgr.h"
#include "Item.h"

class Player;
class JarItem;
class FakeWeightBase;
struct OpActivityRegInfo;

#define MAGIC_BOX_GUILD_ID 51	//魔盒引导ID

//开罐类型&品质 组合key
struct JarBonusColorKey
{
	JarBonusColorKey(UInt32 type, UInt8 color) : m_type(type), m_color(color){}

	UInt32 m_type;
	UInt8 m_color;

	bool operator < (const JarBonusColorKey& other) const
	{
		if (m_type < other.m_type)
			return true;
		if (other.m_type < m_type)
			return false;

		if (m_color < other.m_color)
			return true;
		if (other.m_color < m_color)
			return false;

		return false;
	}
};

class OpenJarMgr
{
public:
	OpenJarMgr();
	~OpenJarMgr();

	typedef std::vector<FakeWeightBase*>				FakeWeightVec;
	typedef std::vector<JarItem*>						BonusVec;
	typedef	std::map<JarBonusColorKey, BonusVec>		BonusPoolColorMap;				//罐子类别&职业索引to品质索引
	typedef std::map<UInt32, BonusVec>				BonusPoolMap;					//罐子类别&职业索引to数据

	void SetOwner(Player* owner) { m_pOwner = owner; }
	Player* GetOwner() const { return m_pOwner; }

	bool	LoadJarItem(JarItem* jarItem);
	void	Init();

	//开罐子
	UInt32	OpenJar(UInt32 id, UInt8 combo, UInt32& getPointId, UInt32& getPoint, UInt32& getPointCrit, UInt32& baseItemId, UInt32& baseItemNum, UInt32 opActId);

	//开罐子测试接口
	void	OpenJarSample(UInt32 type, UInt32 combo, UInt32 num);

	//给予罐子奖励
	UInt32	GiveJarRewards(const std::string& reason, UInt32 type);

	//给予罐子奖励
	void	GiveJarRewards(const std::string& reason, JarBonusDataEntry* data, UInt32 comboBuyNum, bool bNotify, UInt32& baseItemId, UInt32& baseItemNum);

	//给予罐子奖励
	void	GiveJarRewards(const std::string& reason, JarBonusDataEntry* data, UInt32 comboBuyNum, bool bNotify, UInt32& baseItemId, UInt32& baseItemNum, RewardGroup& rewardGroup);

	//通用抽罐子接口, type:罐子类型
	void	ComRandJarItem(UInt32 type, std::string condCt);		

	//是否活动罐子
	bool IsActivityJar(JarType type);

	//充值积分
	void	ResetPoint(bool online);

	//获得开罐子结果
	void GetOpenResult(std::vector<Item*>& results, std::vector<OpenJarResult>& jarResult);

	//活动罐子折扣是否重置
	bool IsActiveJarSaleReset(UInt32 jarId);

	//开魔盒
	UInt32 OpenMagicBox(UInt64 uid, UInt8 isBatch);

	//开罐子规则的礼包
	UInt32 OpenJarGift(UInt32 jarid);

	//开罐子规则的礼包
	UInt32 OpenJarGift(UInt32 jarid, RewardGroup& rewardGroup);

	//是否装备回收奖励罐子
	bool IsEquipRewardJar(JarType type);

	// 是否正在开启魔罐
	void StartOpenJar() { m_opening = true; }
	void FinishOpenJar() { m_opening = false; }
	bool IsOpenJar() { return m_opening; }

	// 清除保存的道具
	void ClearHitItems();

	//往Global发送活动神器罐数据
	void SendArtifactJarDataToGlobal();

	// 开活动神器罐子次数
	void OpenActifactJarData();

	// 神器罐子派奖需要次数
	UInt32 GetActifactJarLotteryNeedCnt(UInt32 jarId);

	// 是否是活动的神器罐子
	bool IsActivityArtifactJar(UInt32 jarId);

	// 神器罐子活动计数
	void ArtifactJarActivity(UInt32 jarId, UInt32 cnt);

public:	//事件
	void OnOnline();
	void OnOffline();

private:
	//注册事件
	void RegistGameEvent();
	void UnRegistGameEvent();

	//检查开罐子免费次数,返回可以免费的次数
	UInt32 CheckFreeCD(JarBonusDataEntry* data);

	//记录开到指定品质的道具
	void OpenSpecifyItemRecord(UInt32 jarId, std::string jarName, JarType jarType);

	void RandJarItem(JarBonusDataEntry* data, bool isMagicBox = false);
	JarItem* HitProc(BonusVec& pool);

	void CreateMap(JarItem* jarItem);
	void DeleteMap(JarItem* jarItem);
	void DeletAllMap();

	void AddJarItem(JarItem* jarItem, UInt8 color);

	bool GetBonusPool(UInt32 key, std::string cond, BonusVec& datas);
	bool GetBonusPool(UInt32 key, JarBonusDataEntry* data, BonusVec& datas);
	bool GetBonusPool(JarBonusColorKey key, JarBonusDataEntry* data, BonusVec& datas);

	//罐子表转成对应的jaritem列表
	bool GetBonusPool(JarBonusDataEntry* data, BonusVec& datas);

	void RejectJarItem(JarBonusDataEntry* data, BonusVec& inDatas, BonusVec& outDatas);
	void RejectJarItem(std::string cond, BonusVec& inDatas, BonusVec& outDatas);

	void RareDropItemReq(BonusVec& inDatas);

	UInt32	CheckOpenJar(JarBonusDataEntry* data, UInt32 comboBuyNum, UInt32& cost, OpActivityRegInfo* opAct);
	void GetOccuDatas(BonusVec& datas);

	void SyncPoolToDB(BonusVec& pool);
	void GetFakeWeightBase(BonusVec& pool, FakeWeightVec& fwPool);

	//开罐获得积分
	void OnGetPoint(std::string reason, JarBonusDataEntry* data, UInt32 comboNum, UInt32& getPointId, UInt32& getPoint, UInt32& getPointCrit);
	UInt32 GetPointId(JarBonusDataEntry* data);
	UInt32	GetPointNum(JarBonusDataEntry* data);
	UInt32	GetPointCrit(JarBonusDataEntry* data);
	bool	IsResetPoint(bool isGold);
	
	//首次开魔罐处理
	void OnFirstOpenMagJar();

	//获取罐子装备随机强化等级
	UInt8 GetRandStrength(JarItemPoolData* data);

	//初始化魔罐计数
	void InitMagJarCounter();

	UInt32 CheckOpenMagBox(Item* item, JarBonusDataEntry* data, UInt8 isBatch, UInt32& costBoxNum, UInt32& costKeyNum);

	//是否首次开魔盒
	bool IsFirstOpenMagBox();

private: // 系统开关
	bool IsGoldJarPointOpen();
	bool IsMagicJarPointOpen();

	//发送获得道具公告
	void SendAnnounce(Item* item, Int32 num, std::string jarName);
	void SendAnnounce(UInt32 itemId, Int32 num, std::string jarName);

	void SendUdpLog(JarBonusDataEntry* data, UInt32 combo, ItemRewardVec itemRewards);

private: // 事件处理
	void OnGetNewItemEvent(IEventParam* param);
	void OnUpdateItemEvent(IEventParam* param);
	void OnLevelUp(IEventParam* param);

	//是否已经初始化
	bool					m_inited;

	//拥有者
	Player*					m_pOwner;

	//罐子奖池
	BonusPoolMap		bonusPool;				//罐子类别 索引的奖池
	BonusPoolColorMap	bonusPoolColor;			//罐子类别&品质 索引奖池		
	BonusVec			occuToDatas;			//data集合

	//一次开罐得结果
	std::vector<Item*> m_hitItems;
	std::vector<UInt32> m_hitJarItems;

	// 是否正在开启魔罐
	bool					m_opening;
};


#endif