#include "SysUtility.h"

#include <CLMailProtocol.h>
#include "GameZoneMgr.h"

namespace SysUtility
{
	bool SendMail(UInt32 destConnId, UInt64 playerId, const std::string& senderName, const std::string& title, const std::string& content, 
		const std::string& reason, const ItemRewardVec& rewards, UInt64 mailGuid)
	{
		InfoLogStream << "zoneid:" << destConnId << " player:" << playerId << " send mail(" << senderName << "|" << title << "|" << content << "|" << reason << LogStream::eos;
		CLProtocol::WorldUnionSendMail mail;
		mail.mailGuid = mailGuid;
		mail.roleId = playerId;
		mail.name = senderName;
		mail.title = title;
		mail.content = content;
		mail.reason = reason;
		mail.rewards = rewards;

		GameZone* zone = GameZoneMgr::Instance()->FindGameZoneByID(destConnId);
		if (NULL == zone)
		{
			ErrorLogStream << " not find game zone, send mail failed! connId:" << destConnId << " playerId:" <<
				playerId << " reason"<< reason << LogStream::eos;
			return false;
		}

		zone->TransmitToWorld(mail);

		return true;
	}
};