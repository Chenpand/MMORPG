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
 *@brief ����������ѯ
 */
class ConsoleOnlineNumHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "stateOnline"; }
	std::string GetSubCmd() const{ return "userNums"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief ��������ʾ��������
 */
class ConsoleSceneOnlineNumHandler : public ConsoleCmdHandler
{
public:
	std::string	GetCmd() const{ return "stateOnline"; }
	std::string GetSubCmd() const{ return "sceneUserNums"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief ��ѯ��ǰע���ɫ��                                                                     
 */
class ConsoleRoleNumHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "stateOnline"; }
	std::string GetSubCmd() const { return "roleNum"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief ��ѯ�����Ϣ
 */
class ConsoleQueryPlayerHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "playerQueries"; }
	std::string GetSubCmd() const{ return "player"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ������ѯ�����Ϣ
*/
class ConsoleBatchQueryPlayerHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "gameControl"; }
	std::string GetSubCmd() const{ return "queryPlayers"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief ��ѯ������е���
 */
class ConsoleQueryPlayerItemHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "playerQueries"; }
	std::string GetSubCmd() const{ return "items"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ɾ������
*/
class ConsoleDeleteItemHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "userControl"; }
	std::string GetSubCmd() const{ return "deleteItem"; }

	void operator()(UInt32 connid, json::Object& jsonobj);

};

/**
*@brief ɾ��Ǯ��
*/
class ConsoleDeleteMoneyHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "userControl"; }
	std::string GetSubCmd() const{ return "removeMoney"; }

	void operator()(UInt32 connid, json::Object& jsonobj);

};

/**
*@brief ������ҵȼ�
*/
class ConsoleSetPlayerLvHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "userControl"; }
	std::string GetSubCmd() const{ return "setLevel"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief �������vip�ȼ�
*/
class ConsoleSetPlayerVipLvHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "userControl"; }
	std::string GetSubCmd() const{ return "setVipLevel"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ��������
*/
class ConsoleSetPlayerFinishTaskHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "userControl"; }
	std::string GetSubCmd() const{ return "finishTask"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief ���Է��
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
*@brief �鿴��Ҹ�����¼By����
*/
class QueryPlayerChangeNameRecordByNameHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "userControl"; }
	std::string GetSubCmd() const{ return "queryChangeNameRecordByName"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief �鿴��Ҹ�����¼
*/
class QueryPlayerChangeNameRecordHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "userControl"; }
	std::string GetSubCmd() const{ return "queryChangeNameRecord"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief ȫ������
 */
class ForbidTalkAllHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "userControl"; }
	std::string GetSubCmd() const{ return "forbidTalk_All"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ɾ��
*/
class DeleteRoleHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "userControl"; }
	std::string GetSubCmd() const{ return "deletePlayer"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief ɾ�Żָ�                                                                     
 */
class RecoverRoleHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "userControl"; }
	std::string GetSubCmd() const{ return "recoveryPlayer"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief ��������
 */
class PublicAnnouncementHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "gameControl"; }
	std::string GetSubCmd() const{ return "announce"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};


/**
*@brief ��ѯ�ʼ�
*/
class QueryMailHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "gameControl"; }
	std::string GetSubCmd() const{ return "queryMail"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief �����ʼ�
 */
class BroadcastMailHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "gameControl"; }
	std::string GetSubCmd() const{ return "sendMail"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief �����ʼ���ϸ����
*/
class SendDetailMailHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "gameControl"; }
	std::string GetSubCmd() const { return "sendDetailMail"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ���������ʼ�
*/
class SendMailBatchHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "gameControl"; }
	std::string GetSubCmd() const{ return "sendMailBatch"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ���ͽ���                                                                   
 */
class SendAwardHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "gameControl"; }
	std::string GetSubCmd() const{ return "sendAward"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief �г����й���
 */
class ShowAnnouncementsHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "gameControl"; }
	std::string GetSubCmd() const{ return "showAnnouncements"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief ɾ��ָ������
 */
class DelAnnounceHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "gameControl"; }
	std::string GetSubCmd() const{ return "delAnnouncements"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief ���ؽű�
 */
class ReloadScriptsHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "gameControl"; }
	std::string GetSubCmd() const{ return "reloadScripts"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
 *@brief �ָ�����                                                                     
 */
class ReturnItemHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "gameControl"; }
	std::string GetSubCmd() const{ return "returnItem"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ������Ʒ
*/
class GiveItemHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "itemManage"; }
	std::string GetSubCmd() const{ return "giveItem"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ������Ʒ
*/
class QueryPlayerHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "playerManage"; }
	std::string GetSubCmd() const{ return "query"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ������Ʒ
*/
class PunishPlayerHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "playerManage"; }
	std::string GetSubCmd() const{ return "punish"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief �ֻ���
*/
class PhoneBindHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "playerManage"; }
	std::string GetSubCmd() const{ return "phoneBind"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ��ֵ
*/
class ChargeHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "chargeManage"; }
	std::string GetSubCmd() const{ return "charge"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ��ֵ������ѯ
*/
class QueryChargeOrderHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "playerQueries"; }
	std::string GetSubCmd() const{ return "chargeQuery"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ��ֵ������ѯ
*/
class QueryChargeMoneyHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "chargeManage"; }
	std::string GetSubCmd() const{ return "chargeNum"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ��ֵ���в�ѯ
*/
class QueryChargeRankHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "chargeManage"; }
	std::string GetSubCmd() const{ return "chargeRank"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief �̳����ò�ѯ
*/
class QueryMallConfigHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "mallControl"; }
	std::string GetSubCmd() const{ return "queryMall"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief �̳������޸�
*/
class SetMallConfigHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "mallControl"; }
	std::string GetSubCmd() const{ return "setMall"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief �̳�����ɾ��
*/
class DelMallConfigHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "mallControl"; }
	std::string GetSubCmd() const{ return "delMallGood"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ����ò�ѯ
*/
class QueryActConfigHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "activeControl"; }
	std::string GetSubCmd() const{ return "queryActive"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ������޸�
*/
class SetActConfigHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "activeControl"; }
	std::string GetSubCmd() const{ return "setActive"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief �����ɾ��
*/
class DelActConfigHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "activeControl"; }
	std::string GetSubCmd() const{ return "delActive"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ���Ű󶨵ㄻ����
*/
class SendBindPointHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "test"; }
	std::string GetSubCmd() const{ return "sendBindPoint"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ���ŵㄻ����
*/
class SendPointHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "test"; }
	std::string GetSubCmd() const{ return "sendPoint"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ��ѯ����
*/
class QueryGuildHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "guildManager"; }
	std::string GetSubCmd() const{ return "query"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ��ɢ����
*/
class DismissGuildHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "guildManager"; }
	std::string GetSubCmd() const{ return "dismiss"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ����ˢ������
*/
class ClearTowerDataHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "towerManager"; }
	std::string GetSubCmd() const { return "clearTower"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief cdkey���
*/
class CDKDataHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "activity"; }
	std::string GetSubCmd() const { return "cdkey"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief �����ȫ��
*/
class ClearSecurityLockHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "securityLock"; }
	std::string GetSubCmd() const { return "clear"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ���ӹ��ṱ��
*/
class GuildAddFundHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "guildManager"; }
	std::string GetSubCmd() const { return "addfund"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief �쳣���ײ�ѯ
*/
class AbnormalTransQueryHandler : public WSConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "abnormalTrans"; }
	std::string GetSubCmd() const { return "query"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief �쳣���׽ⶳ
*/
class AbnormalTransUnfreezeHandler : public WSConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "abnormalTrans"; }
	std::string GetSubCmd() const { return "unfreeze"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};


/**
*@brief ����������ƽ�����׼۸�
*/
class AuctionSetAveraPriceHandler : public WSConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "auctionSys"; }
	std::string GetSubCmd() const { return "setAveraPrice"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief �����˺ż���
*/
class AccounterSetCounterSHandler : public WSConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "accCnt"; }
	std::string GetSubCmd() const { return "setAccCnt"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief �������gmȨ��
*/
class SetPlayerGMAuthoriyHandler : public WSConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "GMAuthoriy"; }
	std::string GetSubCmd() const { return "set"; }

	void operator()(UInt32 nodeid, json::Object& jsonobj);
};

/**
*@brief ���ӵؾ������
*/
class IncGnomeCoinHandler : public WSConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "GnomeGoin"; }
	std::string GetSubCmd() const { return "increase"; }

	void operator()(UInt32 nodeid, json::Object& jsonobj);
};

/**
*@brief �����ʲ�����
*/
class CurrencyFrozenHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "CurrencyFrozen"; }
	std::string GetSubCmd() const { return "handler"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ���ͷ��
*/
class AddTitleHandler : public WSConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "NewTitle"; }
	std::string GetSubCmd() const { return "addTitle"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ���õ�ȯ
*/
class CreditPointHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "CreditPoint"; }
	std::string GetSubCmd() const { return "remove"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

#endif
