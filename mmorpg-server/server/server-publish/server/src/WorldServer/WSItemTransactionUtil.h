#ifndef _WS_ITEM_TRANSACTION_UTIL_H_
#define _WS_ITEM_TRANSACTION_UTIL_H_

#include <CLRoleProtocol.h>
#include <CLItemProtocol.h>
#include <CLAccountShopItemDataEntry.h>
#include <CLAdventureTeamProtocol.h>
#include "WSItemTransaction.h"
#include "WSPlayer.h"
#include "WSShopMgr.h"


class AdventureTeamRenameTransaction : public ItemTransaction
{
public:
	AdventureTeamRenameTransaction(WSPlayer *player, const CLProtocol::WorldAdventureTeamRenameReq& req)
		: ItemTransaction(player), m_RenameReq(req) {}

	virtual void OnCheckSucced(WSPlayer* player)
	{
		if (!player) return;

		if (player->RenameAdventureTeam(m_RenameReq.newName))
		{
			RemoveItem(GetReason(SOURCE_TYPE_ADVENTURE_TEAM_RENAME));

			CLProtocol::WorldAdventureTeamRenameRes protocol;
			protocol.resCode = ErrorCode::SUCCESS;
			protocol.newName = player->GetAdventureTeamName();
			player->SendProtocol(protocol);
			InfoLogStream << PLAYERINFO(player) << " rename success, new name: " << protocol.newName << LogStream::eos;
		}
		else
		{
			UnLock();
			SendRespose(player, ErrorCode::ADVENTURE_TEAM_RENAME_FAILED);
		}
	}

	virtual void OnCheckFailed(WSPlayer* player)
	{
		UnLock();
		SendRespose(player, GetErrorCode());
	}

	void SendRespose(WSPlayer* player, UInt32 errorCode)
	{
		if (!player) return;

		CLProtocol::WorldAdventureTeamRenameRes protocol;
		protocol.resCode = errorCode;
		player->SendProtocol(protocol);
	}

private:
	CLProtocol::WorldAdventureTeamRenameReq m_RenameReq;
};

class UnlockExtensibleRoleFieldTransaction : public ItemTransaction
{
public:
	UnlockExtensibleRoleFieldTransaction(WSPlayer *player) : ItemTransaction(player) {}

	virtual void OnCheckSucced(WSPlayer* player)
	{
		if (!player) return;

		if (player->IncUnlockedExtensibleRoleField())
		{
			RemoveItem(GetReason(SOURCE_TYPE_UNLOCK_EXTENSIBLE_ROLE_FIELD));
			player->SendUnlockExtensibleRoleFieldRespose(ErrorCode::SUCCESS);
			InfoLogStream << PLAYERINFO(player) << " unlock extensible role field success." << LogStream::eos;
		}
		else
		{
			UnLock();
			player->SendUnlockExtensibleRoleFieldRespose(ErrorCode::ENTERGAME_EXTENSIBLE_ROLE_FIELD_UNLOCK_FAILED);
			ErrorLogStream << PLAYERINFO(player) << " increase unlocked extensible role field faield!" << LogStream::eos;
		}
	}

	virtual void OnCheckFailed(WSPlayer* player)
	{
		UnLock();
		if (player != NULL)
		{
			player->SendUnlockExtensibleRoleFieldRespose(ErrorCode::ITEM_USE_EXTENSIBLE_ROLE_FIELD_UNLOCK_FAILED);
			ErrorLogStream << PLAYERINFO(player) << " unlock role field transaction check faield!" << LogStream::eos;
		}
	}
};

#endif