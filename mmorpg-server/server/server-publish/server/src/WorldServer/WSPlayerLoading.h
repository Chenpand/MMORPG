#ifndef _PLAYER_LOADING_H_
#define _PLAYER_LOADING_H_

#include "WSPlayer.h"
#include "MallGiftPack.h"
#include "CondActivateMgr.h"
#include "AccountActivityTaskMgr.h"
#include "WSShopMgr.h"
#include "MallMgr.h"

#include <CLMallProtocol.h>


// 查询触发条件数据库回调
class SelectMallGiftPackCb : public SelectRoleInfoCallback
{
public:
	SelectMallGiftPackCb(WSPlayer* player) : SelectRoleInfoCallback(player) {}
	~SelectMallGiftPackCb() {}

public:
	void OnFinished(WSPlayer* player)
	{
		while (NextRow())
		{
			UInt64 roleid = GetData("roleid");
			UInt8 cond = GetData("activate_cond");
			UInt16 counter = GetData("activate_counter");
			UInt8 activateType = GetData("activate_type");

			if (player->GetID() == roleid)
			{
				if (activateType == (UInt8)CAT_MALL_LIMIT_TIME_GIFT_PACK)
				{
					MallGiftPack* mallGiftPack = MallGiftPack::Create();
					if (!mallGiftPack)
					{
						ErrorLogStream << "MallGiftPack ptr is null!" << LogStream::eos;
						continue;
					}

					mallGiftPack->SetID(GetKey());
					mallGiftPack->SetDBData(this);

					if (player->GetMallGiftPackMgr().SetRoleMallGiftPack(mallGiftPack))
					{
						player->GetMallGiftPackMgr().SetRoleCondCounter(cond, counter);

						InfoLogStream << PLAYERINFO(player) << " add mall gift pack(" << GetKey() << ")." << LogStream::eos;
					}
					else
					{
						CL_SAFE_DELETE(mallGiftPack);
					}
				}
				else if (activateType == (UInt8)CAT_MALL_PERSONAL_TAILOR)
				{
					CondActivateInfo* tailor = CondActivateInfo::Create();
					if (!tailor)
					{
						ErrorLogStream << "MallPersonalTailor ptr is null!" << LogStream::eos;
						continue;
					}

					tailor->SetID(GetKey());
					tailor->SetDBData(this);

					player->GetCondActivateMgr().AddCondActivateInfo(tailor);
				}
			}

			//switch ((ConditionActivateType)activateType)
			//{
			//case CAT_MALL_LIMIT_TIME_GIFT_PACK:
			//	player->GetMallGiftPackMgr().AddAccountCondCounter(cond, counter);
			//	break;

			//default:
			//	break;
			//}
			
			DebugLogStream << PLAYERINFO(player) << " mall gift pack(" << GetKey() << ")'s activate type(" << (UInt32)activateType << ")." << LogStream::eos;

			if (activateType == (UInt8)CAT_MALL_LIMIT_TIME_GIFT_PACK)
			{
				player->GetMallGiftPackMgr().AddAccountCondCounter(cond, counter);
			}
		}

		CLProtocol::WolrdLoadConditionActivateDBNotify protocol;
		protocol.m_RoleId = player->GetID();
		protocol.m_IsLoaded = (UInt8)CADBLS_DONE;

		player->SendToScene(protocol);
	}

	void OnFailed(WSPlayer* player)
	{
		ErrorLogStream << PLAYERINFO(player) << " Failer to select mall gift pack by t_mall_gift_pack!" << LogStream::eos;
	}
};

/**
*@brief 查询玩家账号下的活动任务回调
*/
class SelectAccountActivityTaskCallback : public SelectRoleInfoCallback
{
public:
	SelectAccountActivityTaskCallback(WSPlayer* player) :SelectRoleInfoCallback(player){}
	~SelectAccountActivityTaskCallback(){}

	void OnFinished(WSPlayer* player)
	{
		AccountActivityTaskMgr::Instance()->OnLoadTaskInfo(this);
	}

	void OnFailed(WSPlayer* player)
	{
		ErrorLogStream << PLAYERINFO(player) << " Failer to select task of activity which bind to account!" << LogStream::eos;
	}
};

/**
*@brief 查询自定义商城道具回调
*/
class LoadMallItemCallback : public SelectRoleInfoCallback
{
public:
	LoadMallItemCallback(WSPlayer* player) :SelectRoleInfoCallback(player){}
	~LoadMallItemCallback(){}

	virtual void OnFinished(WSPlayer* player)
	{
		MallMgr::Instance()->LoadCustomerMallItemFromDB(player, this);
	}

	virtual void OnFailed(WSPlayer* player)
	{
		ErrorLogStream << PLAYERINFO(player) << "select table t_mall_item failed!" << LogStream::eos;
	}
};

/**
*@brief 加载其他角色基础信息
*/
class LoadOtherRoleBaseInfoCallback : public SelectRoleInfoCallback
{
public:
	LoadOtherRoleBaseInfoCallback(WSPlayer* player) :SelectRoleInfoCallback(player) {}
	~LoadOtherRoleBaseInfoCallback() {}

	virtual void OnFinished(WSPlayer* player)
	{
		std::vector<RoleBaseInfo> infoes;
		while (NextRow())
		{
			auto roleid = GetKey();
			if (roleid == player->GetID())
			{
				continue;
			}

			RoleBaseInfo role;
			role.roleid = roleid;
			role.level = GetData("level");
			infoes.push_back(role);
		}

		player->SetOtherRoleBaseInfo(infoes);
	}

	virtual void OnFailed(WSPlayer* player)
	{
		ErrorLogStream << PLAYERINFO(player) << "select table t_player_info failed!" << LogStream::eos;
	}
};

/**
*@brief 查询账号商店角色下的商品购买记录回调
*/
class LoadAccountShopItemRoleBuyRecordCallback : public SelectRoleInfoCallback
{
public:
	LoadAccountShopItemRoleBuyRecordCallback(WSPlayer* player) :SelectRoleInfoCallback(player){}
	~LoadAccountShopItemRoleBuyRecordCallback(){}

	virtual void OnFinished(WSPlayer* player)
	{
		WSShopMgr::Instance()->LoadShopItemRoleBuyRecordFromDB(this);
	}

	virtual void OnFailed(WSPlayer* player)
	{
		ErrorLogStream << PLAYERINFO(player) << "select table t_account_shop_role_buy_record failed!" << LogStream::eos;
	}
};

#endif