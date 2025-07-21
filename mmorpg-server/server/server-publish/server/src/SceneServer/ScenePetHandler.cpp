#include "ScenePetHandler.h"
#include "PetMgr.h"
#include "Player.h"

void HandleProtocol(Player* player, CLProtocol::SceneSetPetSoltReq req)
{

	InfoLogStream << PLAYERINFO(player) << "SceneSetPetSoltReq petType = " << (UInt32)req.petType << ", petId:" << req.petId << LogStream::eos;
	CLProtocol::SceneSetPetSoltRes res;
	res.result = player->SetPetSlot((PetType)req.petType, req.petId);
	res.battlePets = player->GetBattlePets();
	res.followPetId = player->GetPetFollow();
	if (res.result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << PLAYERINFO(player) << "SceneSetPetSoltReq petId = " << req.petId << ", result = " << res.result << LogStream::eos;
	}
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneFeedPetReq req)
{
	InfoLogStream << PLAYERINFO(player) << "SceneFeedPetReq petId = " << req.id << ", feed type:" << req.feedType << LogStream::eos;
	CLProtocol::SceneFeedPetRes res;
	res.feedType = req.feedType;
	res.isCritical = 0;
	res.value = 0;
	res.result = player->FeedPet(req.id, (PetFeedType)req.feedType, res.isCritical, res.value);
	if (res.result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << PLAYERINFO(player) << "SceneFeedPetReq petId = " << req.id << ", result = " << res.result << LogStream::eos;
	}
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneSellPetReq req)
{
	InfoLogStream << PLAYERINFO(player) << "SceneSellPetReq petId = " << req.id << LogStream::eos;
	CLProtocol::SceneSellPetRes res;
	res.result = player->SellPet(req.id);
	if (res.result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << PLAYERINFO(player) << "SceneSellPetReq petId = " << req.id << ", result = " << res.result << LogStream::eos;
	}
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneChangePetSkillReq req)
{
	InfoLogStream << PLAYERINFO(player) << "SceneChangePetSkillReq petId = " << req.id << ", skill index:" << req.skillIndex << LogStream::eos;
	CLProtocol::SceneChangePetSkillRes res;
	res.petId = req.id;
	res.result = player->ChangePetSkill(req.id, req.skillIndex);
	if (res.result == ErrorCode::SUCCESS)
	{
		res.skillIndex = req.skillIndex;
	}
	else
	{
		ErrorLogStream << PLAYERINFO(player) << "SceneChangePetSkillReq petId = " << req.id << ", result = " << res.result << LogStream::eos;
	}
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneSetPetFollowReq req)
{
	InfoLogStream << PLAYERINFO(player) << "SceneSetPetFollowReq petId = " << req.id << LogStream::eos;
	CLProtocol::SceneSetPetFollowRes res;
	res.petId = req.id;
	res.result = player->SetPetFollow(req.id);
	if (res.result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << PLAYERINFO(player) << "SceneSetPetFollowReq petId = " << req.id << ", result = " << res.result  << LogStream::eos;
	}
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneDevourPetReq req)
{
	InfoLogStream << PLAYERINFO(player) << "SceneDevourPetReq petId = " << req.id << ", pets size:" << (UInt32)req.petIds.size() << LogStream::eos;
	CLProtocol::SceneDevourPetRes res;

	do
	{
		if (!player->SecurityLockCheck())
		{
			res.result = ErrorCode::SECURITY_LOCK_FORBID_OP;
			break;
		}

		res.result = player->DevourPet(req.id, req.petIds, res.exp);
		if (res.result != ErrorCode::SUCCESS)
		{
			ErrorLogStream << PLAYERINFO(player) << "SceneDevourPetReq petId = " << req.id << ", result = " << res.result << LogStream::eos;
		}

	} while (0);

	player->SendProtocol(res);
}
