#ifndef __GUILD_MGR_H__
#define __GUILD_MGR_H__

#include <AvalonSingleton.h>
#include <AvalonDailyTimer.h>
#include "Guild.h"

class WSPlayer;
/**
*@brief 公会管理器
*/
class GuildMgr : public Avalon::Singleton<GuildMgr>
{
public:
	GuildMgr();
	~GuildMgr();

public:
	/**
	*@brief 初始化
	*/
	bool Init();

	/**
	*@brief Tick
	*/
	bool OnTick(const Avalon::Time& now);

	/**
	*@brief 换了一天
	*/
	void OnDayChanged();

	/**
	*@brief 换了一周
	*/
	void OnWeekChanged();

	/**
	*@brief 跨周判断公会兼并
	*/
	void OnWeekChangedGuildMerge();

	/**
	*@brief 跨周发放领地积分排行奖励
	*/
	void OnWeekChangedGuidTerrScoreRankReward();

	void OnMemberNameChanged(ObjID_t roleId, const std::string& name);

	/**
	*@brief 刷排行榜
	*/
	void InitGuildLevelSortList();

	/**
	*@brief 获取公会
	*/
	Guild* FindGuildByID(ObjID_t id);
	Guild* FindGuildByName(const std::string& name);

	/**
	*@brief 公会成员
	*/
	void AddGuildMember(GuildMember* member);
	GuildMember* FindGuildMember(ObjID_t id);

	/**
	*@brief 验证字符串合法性
	*/
	bool IsStrValid(const std::string& str, UInt32 maxWidth, bool isName = false);

	/**
	*@brief 是否初始化完成
	*/
	bool IsInitFinish() const { return m_LoadFinish; }

	/**
	*@brief 验证名字合法性
	*/
	UInt32 IsNameValid(const std::string& name);

	/**
	*@brief 清除该玩家的所有公会申请列表
	*/
	void ClearJoinGuildRequest(ObjID_t id);

	/**
	*@brief 账号永久性删除
	*/
	void OnPlayerForeverDeleted(ObjID_t roleId);

	/**
	*@brief 是否在同一公会
	*/
	bool IsInSameGuild(ObjID_t roleId1, ObjID_t roleId2);

public:
	/* 公会战 */

	/**
	*@brief 公会领地占领情况
	*/
	void SetTerrOccupy(UInt8 terrId, ObjID_t guildId);

	/**
	*@brief 清理公会战报名
	*/
	void ClearEnroll();

	/**
	*@brief 公会战报名开始事件
	*/
	void OnEnrollBegin();

	/**
	*@brief 获取公会战鼓舞信息
	*/
	UInt32 GetGuildBattleInspireInfos(ObjID_t guildId, std::vector<GuildBattleInspireInfo>& inspireInfos);

	/**
	*@brief 设置公会战抽奖几率
	*/
	UInt32 SetGuildStorageSetting(WSPlayer* player, GuildStorageSetting type, UInt32 value);

public:
	/* 公会仓库 */
	UInt32 GetStorageList(WSPlayer * player, UInt32& maxSize, std::vector<GuildStorageItemInfo>& items, std::vector<GuildStorageOpRecord>& record);

	/**
	*@brief 增加仓库物品
	*/
	UInt32 AddStorageItem(WSPlayer* player, std::vector<GuildStorageItemInfo> inItems);

	/**
	*@brief 删除仓库物品
	*/
	UInt32 DelStorageItem(WSPlayer* player, std::vector<GuildStorageDelItemInfo> delItems);

public:
	/* 玩家请求 */

	/**
	*@brief 请求创建公会
	*/
	void OnPlayerRequestCreateGuild(WSPlayer* player, std::string name, std::string declaration);

	/**
	*@brief 请求公会列表
	*/
	void OnPlayerRequestGuildList(WSPlayer* player, UInt16 start, UInt16 num);

	/**
	*@brief 请求加入公会
	*/
	void OnPlayerRequestJoinGuild(WSPlayer* player, ObjID_t id);

	/**
	*@brief 请求可兼并列表
	*/
	void OnPlayerRequestCanMergerGuildList(WSPlayer* &player, UInt16 start, UInt16 num);

	/**
	*@brief 兼并操作
	*/
	void OnPlayerMergerRequestOperator(WSPlayer* &player, UInt64 guildId, UInt8 opType);

	/**
	*@brief 是否收到有兼并请求
	*/
	void OnPlayerQueryIsHaveMergerRequest(WSPlayer* &player);

	/**
	*@brief 查询收到的兼并请求列表
	*/
	void OnPlayerQueryReceiveMergerRequestList(WSPlayer* &player);
	
	/**
	*@brief 对兼并请求操作
	*/
	void OnPlayerOperaMergerRequest(WSPlayer* &player, UInt64 guildId, UInt8 opType);

	/**
	*@brief 开服天数是否足够开启兼并
	*/
	bool IsMergerEnoughOpenDay();
	
public:
	/* 玩家事件 */

	/**
	*@brief 玩家上线
	*/
	void OnPlayerOnline(WSPlayer* player, bool dayChanged);

	/**
	*@brief 玩家下线
	*/
	void OnPlayerOffline(WSPlayer* player);

	/**
	*@brief 玩家属性修改（等级、职业等等）
	*/
	void OnPlayerPropertyChanged(WSPlayer* player);

	/**
	*@brief 玩家在scene上获得贡献
	*/
	void OnScenePlayerAddContri(ObjID_t roleId, UInt32 num);

public:
	/* 事务相关 */

	/**
	*@brief 请求创建公会
	*/
	bool OnCheckCreateGuildRet(WSPlayer* player, std::string name, std::string declaration);

public:
	/* 数据库相关 */

	/**
	*@brief 加载公会信息返回
	*/
	bool OnSelectGuildRet(CLRecordCallback* callback);

	/**
	*@brief 加载公会成员信息返回
	*/
	bool OnSelectGuildMemberRet(CLRecordCallback* callback);

	/**
	 *@brief  加载公会事件回调
	 */
	bool OnSelectGuildEventRet(CLRecordCallback* callback);

public:

	/**
	*@brief 广播消息
	*/
	void Broadcast(Avalon::Protocol& protocol);
	void Broadcast(Avalon::Packet* packet);
	void Broadcast(Avalon::Protocol& protocol, UInt32 lvlLimit, UInt32 memberLimitLvl = 0);

	/**
	*@brief 公会聊天消息
	*/
	void GuildChat(const std::string& content, UInt32 lvlLimit = 0, bool bLink = false);

private:
	/* 内部接口 */

	/**
	*@brief 添加公会
	*/
	void _AddGuild(Guild* guild);

	/**
	*@brief 删除公会
	*/
	void _DelGuild(Guild* guild);

	/**
	*@brief 初始化兼并
	*/
	void InitMerger();

private:
	bool							m_LoadFinish;

	HashMap<ObjID_t, Guild*>		m_GuildsByID;
	HashMap<std::string, Guild*>	m_GuildsByName;

	// 所有公会成员都保存在这里
	HashMap<ObjID_t, GuildMember*>	m_GuildMembers;

	// 公会成员保存定时器
	Avalon::SimpleTimer				m_GuildMemberSaveTimer;

	// 每日定时器
	Avalon::DailyTimer				m_DailyTimer;

	// 公会领地占领情况
	HashMap<UInt8, ObjID_t>			m_TerrOccupy;

	//可兼并公会列表
	std::set<Guild*>                m_MidGuild;
	//可被兼并公会列表
	std::set<Guild*>                m_LowAndDismissGuild;
};


#endif //__GUILD_MGR_H__