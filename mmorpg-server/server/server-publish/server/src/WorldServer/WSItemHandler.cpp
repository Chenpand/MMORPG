#include "WSItemHandler.h"
#include "WSPlayerMgr.h"
#include "WSPlayer.h"
#include "DungeonRaceMgr.h"
#include "AccountMgr.h"
#include "WSRouter.h"
#include "WSJarMgr.h"
#include "AnnouncementMgr.h"
#include "CLMsgParser.h"
#include "WSItemCheck.h"
#include "ItemFriendPresent.h"
#include "SysRecordMgr.h"

void HandleProtocol(CLProtocol::SceneCheckItemNumRet& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if(player == NULL) return;

	player->OnCheckItemRet(protocol.serial, protocol.result, protocol.counterTimes);
}

void HandleProtocol(CLProtocol::SceneQuickBuyTransactionReq& req)
{
	CLProtocol::SceneQuickBuyTransactionRes res;
	res.roleId = req.roleId;
	res.transactionId = req.transactionId;

	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(req.roleId);
	if (player == NULL)
	{
		res.result = ErrorCode::QUICK_BUY_SYSTEM_ERROR;
		req.SendBack(res);
		return;
	}

	if (req.type == QUICK_BUY_REVIVE)
	{
		auto race = DungeonRaceMgr::Instance()->FindDungeonRaceByRoleId(player->GetID());
		if (!race)
		{
			res.result = ErrorCode::QUICK_BUY_REVIVE_NOT_IN_RACE;
		}
		else
		{
			res.result = race->OnCheckReviveRet(player, (UInt32)req.param2, req.param1);
		}
		
	}
	else
	{
		res.result = ErrorCode::QUICK_BUY_INVALID_TYPE;
	}

	req.SendBack(res);
}

void HandleProtocol(CLProtocol::SWEnlargeStorageReq& req)
{
	UInt32 retCode = ErrorCode::SUCCESS;

	AccountInfo* account = AccountMgr::Instance()->FindAccount(req.accId);
	if (!account)
	{
		retCode = ErrorCode::ITEM_DATA_INVALID;
		ErrorLogStream << "accid(" << req.accId << ") can't find account info " << LogStream::eos;
	}
	else
	{
		account->SetStorageSize(req.newSize);
		account->SaveToDB();
	}

	CLProtocol::WSEnlargeStorageRet ret;
	ret.playerId = req.playerId;
	ret.code = retCode;
	ret.cost = req.cost;
	ret.newSize = req.newSize;
	ret.costItemId = req.costItemId;
	req.SendBack(ret);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldOpenJarRecordReq& req)
{
	CLProtocol::WorldOpenJarRecordRes res;
	JarRewardRecordsMgr::Instance()->QueryRecords(req.jarId, res.records);

	res.jarId = req.jarId;
	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldEquipRecoOpenJarsRecordReq& req)
{
	CLProtocol::WorldEquipRecoOpenJarsRecordRes res;
	JarRewardRecordsMgr::Instance()->QueryEqrecRecords(res.records);

	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::WorldAddJarRecordReq& req)
{
	//announce
	ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(req.itemId);
	if (!itemData)
	{
		return;
	}

	JarRewardRecordsMgr::Instance()->AddRecord(req.jarId, req.name, req.itemId, req.num, itemData->color);

	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(req.playerUId);
	if (!player)
	{
		return;
	}

	std::string linkStr;
	std::string playerLinkStr;
	UInt32 announcementId = 34;
	bool announcement = true;
	if (!MsgPackage::GetPlayerMsgTag(playerLinkStr, player->GetID(), player->GetName(), player->GetOccu(), player->GetLevel()))
	{
		playerLinkStr = player->GetName();
	}

	if (MsgPackage::GetItemMsgTag(linkStr, 0, req.itemId, 0))
	{
		std::string systemLinkStr;
		SystemSuffixesType sysType = SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_JAR;
		if (req.isActive)
		{
			sysType = SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_TIME_JAR;
		}
		else if (req.isEqReco)
		{
			sysType = SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_EQUIPRECO_JAR;
			announcementId = 72;
			if (itemData->color <= ITEM_QUALITY_PURPLE)
			{
				announcement = false;
			}
		}
		MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, sysType , 0);
		if (announcement)
		{
			AnnouncementMgr::Instance()->PublicAnnouncement(player->GetGuildID() ,announcementId, (UInt32)CURRENT_TIME.Sec() + 15, playerLinkStr.c_str(), req.jarName, linkStr.c_str(), systemLinkStr.c_str());
		}
	}
}

void HandleProtocol(CLProtocol::WorldSyncResistMagic& req)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(req.roleId);
	if (player == NULL)
	{
		return;
	}

	player->HandleSceneSyncResistMagic(req.resist_magic);
}

void ItemCheckRetFunction::operator()(Avalon::Packet *pPacket) const
{
	Avalon::NetInputStream	stream(pPacket->GetBuffer(), pPacket->GetSize());

	WSItemCheck::Instance()->HandleReponse(stream);
}

void HandleProtocol(CLProtocol::WorldGotAccountMoneyReq& req)
{
	auto player = WSPlayerMgr::Instance()->FindPlayer(req.roleid);
	if (!player)
	{
		ErrorLogStream << "player(" << req.accid << ", " << req.roleid << ") want add account money("
			<< req.type << ", " << req.num << ", " << req.reason << ") failed, cant find account info." << LogStream::eos;
		return;
	}

	player->IncAccountMoney(req.reason, (ItemSubType)req.type, req.num);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGetItemFriendPresentInfosReq& req)
{
	CLProtocol::WorldGetItemFriendPresentInfosRes  res;
	res.dataId = req.dataId;
	res.recvedTotal = 0;
	res.recvedTotalLimit = 0;
	FriendPresentMgr::Instance()->GetPlayerItemPresentInfo(player, req.dataId, res.presentInfos, res.recvedTotal, res.recvedTotalLimit);
	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldItemFriendPresentReq& req)
{
	CLProtocol::WorldItemFriendPresentRes res;
	res.itemId = req.itemId;
	res.itemTypeId = req.itemTypeId;
	res.retCode = FriendPresentMgr::Instance()->PlayerSendItem(player, req.itemId, req.itemTypeId, req.friendId);
	if (res.retCode != ErrorCode::SUCCESS)
	{
		player->SendProtocol(res);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGetSysRecordReq& req)
{
	CLProtocol::WorldGetSysRecordRes res;
	res.behavoir = req.behavoir;
	res.role = req.role;
	res.param = req.param;

	if (req.role != 0)
	{
		res.value = SysRecordMgr::Instance()->GetRecordValue((SysBehavior)req.behavoir, SYS_BEHAVIOR_OWNER_ROLE, player->GetID(), req.param);
	}
	else
	{
		res.value = SysRecordMgr::Instance()->GetRecordValue((SysBehavior)req.behavoir, SYS_BEHAVIOR_OWNER_ACCOUNT, player->GetAccId(), req.param);
	}
	
	player->SendProtocol(res);
}

class SelectAccountAddCreditPoint : public AccountCallback
{
public:
	SelectAccountAddCreditPoint(UInt32 accid, CLProtocol::SceneGetCreditPoint& req)
		: AccountCallback(accid), m_Req(req) {}

	void OnFinish(AccountInfo* info)
	{
		if (NULL == info) return;
		
		m_Req.onlineId = 0;

		UInt32 oldNum = info->GetCreditPoint();
		info->AddCreditPoint(m_Req.creditPoint);
		UInt32 newNum = info->GetCreditPoint();
		info->SendUdpLog("asset", LOG_ASSET_FORMAT, m_Req.reason.c_str(), AssetType::ASSET_CREDIT_POINT, oldNum, m_Req.creditPoint, newNum - oldNum, newNum);

		// 这里先广播到scene
		// 正常情况应该是选择一个scene
		UInt32 sceneServerID = WSPlayerMgr::Instance()->GetPlayerSceneServerID(m_Req.roleId);
		if (sceneServerID == 0)
		{
			sceneServerID = WSNetwork::Instance()->SelectRandSceneServer();
		}

		auto sceneConn = WSNetwork::Instance()->FindConnection(sceneServerID);
		if (sceneConn != NULL)
		{
			CL_SEND_PROTOCOL(sceneConn, m_Req);
		}
		else
		{
			ErrorLogStream << "send scene error player:" << m_Req.roleId << " get credit point:" << m_Req.creditPoint
				<< " reason:" << m_Req.reason << " orderId:" << m_Req.orderId << LogStream::eos;
		}
	}

	void OnFailure()
	{
		ErrorLogStream << "load account add credit point error player:" << m_Req.roleId << " get credit point:" << m_Req.creditPoint
			<< " reason:" << m_Req.reason << " orderId:" << m_Req.orderId << LogStream::eos;
	}

private:

	CLProtocol::SceneGetCreditPoint m_Req;
};

void HandleProtocol(CLProtocol::SceneGetCreditPoint& req)
{
	InfoLogStream << "player:" << req.roleId << " get credit point:" << req.creditPoint << " reason:" << req.reason << " orderId:" << req.orderId << LogStream::eos;

	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayerByAccid(req.accId);
	if (player != NULL)
	{
		req.onlineId = player->GetID();
		player->SendToScene(req);
		return;
	}

	// 没有角色在线，直接加到账号上
	AccountInfo* accInfo = AccountMgr::Instance()->FindAccount(req.accId);
	if (accInfo != NULL)
	{
		UInt32 oldNum = accInfo->GetCreditPoint();
		accInfo->AddCreditPoint(req.creditPoint);
		UInt32 newNum = accInfo->GetCreditPoint();
		accInfo->SendUdpLog("asset", LOG_ASSET_FORMAT, req.reason.c_str(), AssetType::ASSET_CREDIT_POINT, oldNum, req.creditPoint, newNum - oldNum, newNum);
	}
	else
	{
		AccountMgr::Instance()->LoadAccountInfo(req.accId, new SelectAccountAddCreditPoint(req.accId, req));
		return;
	}

	req.onlineId = 0;

	// 这里先广播到scene
	// 正常情况应该是选择一个scene
	UInt32 sceneServerID = WSPlayerMgr::Instance()->GetPlayerSceneServerID(req.roleId);
	if (sceneServerID == 0)
	{
		sceneServerID = WSNetwork::Instance()->SelectRandSceneServer();
	}

	auto sceneConn = WSNetwork::Instance()->FindConnection(sceneServerID);
	if (sceneConn != NULL)
	{
		CL_SEND_PROTOCOL(sceneConn, req);
	}
	else
	{
		ErrorLogStream << "send scene error player:" << req.roleId << " get credit point:" << req.creditPoint
			<< " reason:" << req.reason << " orderId:" << req.orderId << LogStream::eos;
	}
}