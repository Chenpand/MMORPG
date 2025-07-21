#ifndef _WS_CONSOLE_CMD_HANDLER_H_
#define _WS_CONSOLE_CMD_HANDLER_H_

#include <ConsoleAgent.h>

class WSConsoleCmdHandler : public ConsoleCmdHandler
{
public:
	virtual ~WSConsoleCmdHandler(){}

	void TransmitToScene(UInt32 nodeid, json::Object& jsonobj);
};

/**
 *@brief 在线人数查询
 */
class ConsoleOnlineNumHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "stateOnline"; }
	std::string GetSubCmd() const{ return "userNums"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief 按场景显示在线人数
 */
class ConsoleSceneOnlineNumHandler : public ConsoleCmdHandler
{
public:
	std::string	GetCmd() const{ return "stateOnline"; }
	std::string GetSubCmd() const{ return "sceneUserNums"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief 查询当前注册角色数                                                                     
 */
class ConsoleRoleNumHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "stateOnline"; }
	std::string GetSubCmd() const { return "roleNum"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief 查询玩家信息
 */
class ConsoleQueryPlayerHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "playerQueries"; }
	std::string GetSubCmd() const{ return "player"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 批量查询玩家信息
*/
class ConsoleBatchQueryPlayerHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "gameControl"; }
	std::string GetSubCmd() const{ return "queryPlayers"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief 查询玩家所有道具
 */
class ConsoleQueryPlayerItemHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "playerQueries"; }
	std::string GetSubCmd() const{ return "items"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 删除道具
*/
class ConsoleDeleteItemHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "userControl"; }
	std::string GetSubCmd() const{ return "deleteItem"; }

	void operator()(UInt32 connid, json::Object& jsonobj);

};

/**
*@brief 删除钱币
*/
class ConsoleDeleteMoneyHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "userControl"; }
	std::string GetSubCmd() const{ return "removeMoney"; }

	void operator()(UInt32 connid, json::Object& jsonobj);

};

/**
*@brief 设置玩家等级
*/
class ConsoleSetPlayerLvHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "userControl"; }
	std::string GetSubCmd() const{ return "setLevel"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 设置玩家vip等级
*/
class ConsoleSetPlayerVipLvHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "userControl"; }
	std::string GetSubCmd() const{ return "setVipLevel"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 跳过任务
*/
class ConsoleSetPlayerFinishTaskHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "userControl"; }
	std::string GetSubCmd() const{ return "finishTask"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief 禁言封号
 */
class ConsoleUserControlHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "userControl"; }
	std::string GetSubCmd() const{ return "forbidPlayer"; }

	void operator()(UInt32 connid, json::Object& jsonobj);

	void DoForbid(UInt8 type, UInt32 connId, ObjID_t roleUid, UInt32 accId, std::string name, UInt32 time);
	void DoForbidTalk(UInt32 connId, ObjID_t roleUid, UInt32 accId, std::string name, UInt32 time);
	void DoForbigAccount(UInt32 connId, UInt32 accId, UInt32 time);
};

/**
*@brief 查看玩家改名记录By名字
*/
class QueryPlayerChangeNameRecordByNameHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "userControl"; }
	std::string GetSubCmd() const{ return "queryChangeNameRecordByName"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 查看玩家改名记录
*/
class QueryPlayerChangeNameRecordHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "userControl"; }
	std::string GetSubCmd() const{ return "queryChangeNameRecord"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief 全服禁言
 */
class ForbidTalkAllHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "userControl"; }
	std::string GetSubCmd() const{ return "forbidTalk_All"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 删号
*/
class DeleteRoleHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "userControl"; }
	std::string GetSubCmd() const{ return "deletePlayer"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief 删号恢复                                                                     
 */
class RecoverRoleHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "userControl"; }
	std::string GetSubCmd() const{ return "recoveryPlayer"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief 发布公告
 */
class PublicAnnouncementHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "gameControl"; }
	std::string GetSubCmd() const{ return "announce"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};


/**
*@brief 查询邮件
*/
class QueryMailHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "gameControl"; }
	std::string GetSubCmd() const{ return "queryMail"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief 发送邮件
 */
class BroadcastMailHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "gameControl"; }
	std::string GetSubCmd() const{ return "sendMail"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 发送邮件详细道具
*/
class SendDetailMailHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "gameControl"; }
	std::string GetSubCmd() const { return "sendDetailMail"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 批量发送邮件
*/
class SendMailBatchHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "gameControl"; }
	std::string GetSubCmd() const{ return "sendMailBatch"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 发送奖励                                                                   
 */
class SendAwardHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "gameControl"; }
	std::string GetSubCmd() const{ return "sendAward"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief 列出所有公告
 */
class ShowAnnouncementsHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "gameControl"; }
	std::string GetSubCmd() const{ return "showAnnouncements"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief 删除指定公告
 */
class DelAnnounceHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "gameControl"; }
	std::string GetSubCmd() const{ return "delAnnouncements"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief 加载脚本
 */
class ReloadScriptsHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "gameControl"; }
	std::string GetSubCmd() const{ return "reloadScripts"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief 恢复道具                                                                     
 */
class ReturnItemHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "gameControl"; }
	std::string GetSubCmd() const{ return "returnItem"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 发放物品
*/
class GiveItemHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "itemManage"; }
	std::string GetSubCmd() const{ return "giveItem"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 发放物品
*/
class QueryPlayerHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "playerManage"; }
	std::string GetSubCmd() const{ return "query"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 发放物品
*/
class PunishPlayerHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "playerManage"; }
	std::string GetSubCmd() const{ return "punish"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 手机绑定
*/
class PhoneBindHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "playerManage"; }
	std::string GetSubCmd() const{ return "phoneBind"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 充值
*/
class ChargeHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "chargeManage"; }
	std::string GetSubCmd() const{ return "charge"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 充值订单查询
*/
class QueryChargeOrderHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "playerQueries"; }
	std::string GetSubCmd() const{ return "chargeQuery"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 充值订单查询
*/
class QueryChargeMoneyHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "chargeManage"; }
	std::string GetSubCmd() const{ return "chargeNum"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 充值排行查询
*/
class QueryChargeRankHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "chargeManage"; }
	std::string GetSubCmd() const{ return "chargeRank"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 商城配置查询
*/
class QueryMallConfigHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "mallControl"; }
	std::string GetSubCmd() const{ return "queryMall"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 商城配置修改
*/
class SetMallConfigHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "mallControl"; }
	std::string GetSubCmd() const{ return "setMall"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 商城配置删除
*/
class DelMallConfigHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "mallControl"; }
	std::string GetSubCmd() const{ return "delMallGood"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 活动配置查询
*/
class QueryActConfigHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "activeControl"; }
	std::string GetSubCmd() const{ return "queryActive"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 活动配置修改
*/
class SetActConfigHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "activeControl"; }
	std::string GetSubCmd() const{ return "setActive"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 活动配置删除
*/
class DelActConfigHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "activeControl"; }
	std::string GetSubCmd() const{ return "delActive"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 发放绑定点劵补偿
*/
class SendBindPointHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "test"; }
	std::string GetSubCmd() const{ return "sendBindPoint"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 发放点劵补偿
*/
class SendPointHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "test"; }
	std::string GetSubCmd() const{ return "sendPoint"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 查询公会
*/
class QueryGuildHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "guildManager"; }
	std::string GetSubCmd() const{ return "query"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 解散公会
*/
class DismissGuildHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "guildManager"; }
	std::string GetSubCmd() const{ return "dismiss"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 清理刷塔数据
*/
class ClearTowerDataHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "towerManager"; }
	std::string GetSubCmd() const { return "clearTower"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief cdkey礼包
*/
class CDKDataHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "activity"; }
	std::string GetSubCmd() const { return "cdkey"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 清除安全锁
*/
class ClearSecurityLockHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "securityLock"; }
	std::string GetSubCmd() const { return "clear"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 增加工会贡献
*/
class GuildAddFundHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "guildManager"; }
	std::string GetSubCmd() const { return "addfund"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 异常交易查询
*/
class AbnormalTransQueryHandler : public WSConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "abnormalTrans"; }
	std::string GetSubCmd() const { return "query"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 异常交易解冻
*/
class AbnormalTransUnfreezeHandler : public WSConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "abnormalTrans"; }
	std::string GetSubCmd() const { return "unfreeze"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};


/**
*@brief 设置拍卖行平均交易价格
*/
class AuctionSetAveraPriceHandler : public WSConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "auctionSys"; }
	std::string GetSubCmd() const { return "setAveraPrice"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 设置账号计数
*/
class AccounterSetCounterSHandler : public WSConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "accCnt"; }
	std::string GetSubCmd() const { return "setAccCnt"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 设置玩家gm权限
*/
class SetPlayerGMAuthoriyHandler : public WSConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "GMAuthoriy"; }
	std::string GetSubCmd() const { return "set"; }

	void operator()(UInt32 nodeid, json::Object& jsonobj);
};

/**
*@brief 增加地精纪念币
*/
class IncGnomeCoinHandler : public WSConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "GnomeGoin"; }
	std::string GetSubCmd() const { return "increase"; }

	void operator()(UInt32 nodeid, json::Object& jsonobj);
};

/**
*@brief 处理资产冻结
*/
class CurrencyFrozenHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "CurrencyFrozen"; }
	std::string GetSubCmd() const { return "handler"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 添加头衔
*/
class AddTitleHandler : public WSConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "NewTitle"; }
	std::string GetSubCmd() const { return "addTitle"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 信用点券
*/
class CreditPointHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "CreditPoint"; }
	std::string GetSubCmd() const { return "remove"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

#endif
