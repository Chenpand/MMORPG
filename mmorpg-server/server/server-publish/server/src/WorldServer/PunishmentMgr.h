#ifndef _PUNISHMENT_MGR_H_
#define _PUNISHMENT_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <vector>
#include <set>
#include <CLDefine.h>
#include <CLRecordCallback.h>
#include <CLCheatDefine.h>

class WSPlayer;

/**
 *@brief 惩罚信息
 */
struct Punishment
{
	Punishment() :id(0), accid(0), type(0), duetime(0), param(0) {}

	ObjID_t	id;
	UInt32	accid;
	int		type;
	UInt32	duetime;
	UInt32	param;
};

enum ForbidLoginReason
{
	// 通用封号，没有特殊原因
	FORBID_LOGIN_COMMON = 0,

	// 第一次作弊
	FORBID_LOGIN_CHEAT_FIRST_TIME = 1,
	// 作弊，永久封停
	FORBID_LOGIN_CHEAT_FOREVER = 2,
	// 团本作弊
	FORBID_LOGIN_CHEAT_TEAM_COPY = 3,

	// GM
	FORBID_LOGIN_GM = 20,
};

/**
 *@brief 惩罚管理器
 */
class PunishmentMgr : public Avalon::Singleton<PunishmentMgr>
{
	typedef std::vector<Punishment>			PunishmentVec;
	typedef std::map<UInt32, PunishmentVec>	PunishmentMap;

	//最大惩罚时间
	const static UInt32	MAX_PUNISH_TIME = 5 * 365 * DAY_TO_SEC;

public:
	PunishmentMgr();
	~PunishmentMgr();

public:
	void Init();
	void Final();

public:
	/**
	 *@brief 惩罚一个玩家
	 */
	void Punish(UInt32 accid, int type, UInt32 time, UInt32 param = 0);
	void UnPunish(UInt32 accid, int type);

	/**
	 *@brief 设置全区禁言列表
	 */
	void SetAllForbidTalkList(const std::vector<UInt32>& players);
	void AddAllForbidTalkPlayer(UInt32 accid);
	void DelAllForbidTalkPlayer(UInt32 accid);

	/**
	 *@brief 查询惩罚剩余时间
	 */
	UInt32 GetPunishRemainTime(UInt32 accid, UInt8 type);

	/**
	*@brief 获取封号原因
	*/
	ForbidLoginReason GetForbidLoginReason(UInt32 accid);

public://事件
	/**
	 *@brief 查询数据返回
	 */
	bool OnSelectDataRet(CLRecordCallback* callback);

	/**
	 *@brief 心跳
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief 玩家上线
	 */
	void OnPlayerOnline(WSPlayer* player);

	/**
	 *@brief 场景服切换
	 */
	void OnServerChanged(WSPlayer* player);

	/**
	*@brief 检测出玩家作弊
	*/
	void OnReportPlayerCheat(WSPlayer* player, PlayerCheatReason reason, UInt32 param1 = 0, UInt32 param2 = 0);

private:
	/**
	 *@brief 同步惩罚到场景
	 */
	void SyncPunishment(UInt32 accid) const;

	/**
	 *@brief 删除惩罚
	 */
	void DeletePunishment(ObjID_t id);

private:
	//惩罚列表
	PunishmentMap		m_Punishments;
	//全区禁言列表
	std::set<UInt32>	m_ALLForbidTalkList;
	//请求禁言列表定时器
	Avalon::SimpleTimer	m_RequestTimer;
	//是否请求到
	bool				m_bResponse;
};

#endif
