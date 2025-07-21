#include "DungeonRoll.h"
#include <AvalonRandom.h>
#include <CLItemProtocol.h>

#include "WSRouter.h"
#include "udplog_typedef.h"
#include "WSPlayerMgr.h"

DungeonRollItem::DungeonRollItem()
{
}

DungeonRollItem::~DungeonRollItem()
{
	index = 0;
}

UInt32 DungeonRollItem::RandomRollPoint()
{
	std::set<UInt32> rooledPoints;
	std::vector<UInt32> roolPoints;
	for (auto info : playerRollInfos)
	{
		if (info.opType == !RIE_NEED)
		{
			continue;
		}
		rooledPoints.insert(info.point);
	}

	for (UInt32 i = 1; i <= 100; ++i)
	{
		if (rooledPoints.find(i) != rooledPoints.end())
		{
			continue;
		}
		roolPoints.push_back(i);
	}
	
	if (roolPoints.size() == 0)
	{
		return 0;
	}

	UInt32 index = Avalon::Random::RandBetween(0, roolPoints.size()-1);

	return roolPoints[index];
}

void DungeonRollItem::RandomRollPointForAllModest()
{
	std::vector<UInt32> roolPoints;
	for (UInt32 i = 1; i <= 100; ++i)
	{
		roolPoints.push_back(i);
	}

	std::random_shuffle(roolPoints.begin(), roolPoints.end());

	for (UInt32 i = 0; i < playerRollInfos.size(); ++i)
	{
		if (i >= roolPoints.size())
		{
			continue;
		}
		playerRollInfos[i].opType = RIE_MODEST;
		playerRollInfos[i].point = roolPoints[i];
	}
}


void DungeonRollItem::SendDropItem(UInt64 raceId, UInt32 dungeonId)
{
	//选择点数最大的发送掉落物品
	UInt32 maxPoint = 0;
	UInt64 playerId = 0;
	for (auto rollInfo : playerRollInfos)
	{
		if (rollInfo.point > maxPoint)
		{
			maxPoint = rollInfo.point;
			playerId = rollInfo.playerId;
		}
	}

	if (playerId > 0)
	{
		//发送掉落物品
		WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(playerId);
		if (player)
		{
			CLProtocol::SceneAddRewardReq req;
			req.reason = GetReason(LogSourceType::SOURCE_DUNGEON_ROLL_ADD_REWARD, raceId, dungeonId);
			req.notice = (UInt8)false;
			req.roleId = playerId;
			req.rewards.push_back(ItemReward(dropItem.typeId, dropItem.num, 0, dropItem.strenth, 0, dropItem.equipType));
			Router::BroadcastToScene(req);

			InfoLogStream << "SendDropItem success index =" << index << ", item id = " << dropItem.typeId <<  ", get item player = " << playerId << ", point : "
				<< maxPoint << LogStream::eos;
		}
		else
		{
			//不在线,发送邮件
			char buffer[1024];
			memset(buffer, 0, sizeof(buffer));
			std::string strSenderName;
			std::string	strTitle;
			std::string	strContent;
			std::string strReason = GetReason(LogSourceType::SOURCE_DUNGEON_ROLL_ADD_REWARD, raceId, dungeonId);;
			std::vector<ItemReward> depositItems;

			depositItems.push_back(ItemReward(dropItem.typeId, dropItem.num, 0, dropItem.strenth, 0, dropItem.equipType));

			// 发送者
			SysNotifyMgr::Instance()->MakeNotify(12014, buffer, sizeof(buffer));
			strSenderName = buffer;

			//标题
			SysNotifyMgr::Instance()->MakeNotify(12015, buffer, sizeof(buffer));
			strTitle = buffer;

			//正文
			SysNotifyMgr::Instance()->MakeNotify(12016, buffer, sizeof(buffer));
			strContent = buffer;
			
			if (MailBox::SendSysMail(playerId, strSenderName, strTitle, strContent, strReason, depositItems) != ErrorCode::SUCCESS)
			{
				// 发送邮件失败
				ErrorLogStream << "send roll reward mail failed, item id(" << dropItem.typeId << "), num("<< dropItem.num << "), " <<  "to role(" << playerId << ") title("
					<< strTitle << ")." << LogStream::eos;
			}
			InfoLogStream << "SendDropItem by mail success index =" << index <<", item id = " << dropItem.typeId <<  ", get item player = " << playerId << ", point : "
				<< maxPoint << LogStream::eos;
		}
	}
}

RollItemInfo DungeonRollItem::ToClient()
{
	RollItemInfo info;
	info.rollIndex = index;
	info.dropItem = dropItem;
	return info;
}

PlayerRollInfo* DungeonRollItem::GetOnePlayerInfo(ObjID_t roleId)
{
	for (UInt32 i = 0; i < playerRollInfos.size(); ++i)
	{
		if (roleId == playerRollInfos[i].playerId)
		{
			return &playerRollInfos[i];
		}
	}

	return NULL;
}