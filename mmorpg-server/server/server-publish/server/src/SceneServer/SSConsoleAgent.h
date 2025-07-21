#ifndef _SS_CONSOLE_AGENT_H_
#define _SS_CONSOLE_AGENT_H_

#include <AvalonSingleton.h>
#include <ConsoleAgent.h>
#include <CLItemDefine.h>

/**
 *@brief 控制台代理
 */
class SSConsoleAgent : public ConsoleAgent, public Avalon::Singleton<SSConsoleAgent>
{
public:
	SSConsoleAgent();
	~SSConsoleAgent();

	const UInt32 forbidTalkMaxMin = 9999999;			//禁言最大分钟数

public:
	void Init();
	void Final();

	/**
	 *@brief 处理玩家查询
	 */
	void QueryPlayerItems(UInt32 connid, ObjID_t playerId, UInt32 accid, UInt8 pack, UInt32 itemid);
	void QueryPlayerInfo(UInt32 connid, UInt8 keyType, UInt32 accId, ObjID_t playerId, const std::string& playerName);

	/**
	*@brief 处理玩家修改
	*/
	void ForbidTalkPlayer(UInt32 connid, UInt8 keyType, UInt32 accId, ObjID_t playerId, const std::string& playerName, UInt32 forbidTime);
	void DeleteItem(UInt32 connid, ObjID_t roleUid, ObjID_t itemUid, UInt16 num);
	void DeleteMoney(UInt32 connid, ObjID_t roleUid, std::vector<ItemReward>& items);
	void SetPlayerLv(UInt32 connid, ObjID_t roleUid, UInt16 newLv);
	void SetPlayerVipLv(UInt32 connid, ObjID_t roleUid, UInt8 newVipLv);
	void FinishTask(UInt32 connid, ObjID_t roleUid, UInt32 taskId);
	void PhoneBind(UInt32 connid, ObjID_t roleid);
	void ClearTower(UInt32 connid, UInt32 accId, ObjID_t roleid);
	void HandleRequest(UInt32 connid, const std::string& cmd, const std::string& subcmd, const std::string& param);
public:
	void DoSendResponse(UInt32 connid, const std::string& cmd, const std::string& subcmd, const std::string& json);
};

#endif
