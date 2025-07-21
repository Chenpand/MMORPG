#include "Utility.h"

#include <CLGlobalActivityProtocol.h>

#include "GASGameZoneMgr.h"
#include "GASAnnouncementMgr.h"

namespace SysUtility
{
	
	bool SendMail(UInt32 destCenterNode, UInt32 destAdminNode, const std::string& uid, const std::string& senderName, const std::string& title, const std::string& content, const std::string& reason, const ItemRewardVec& rewards)
	{
		CLProtocol::GASSendMailReq mail;
		mail.receiverUId = uid;
		mail.senderName = senderName;
		mail.title = title;
		mail.content = content;
		mail.reason = reason;
		mail.rewards = rewards;

		GASGameZone* zone = GASGameZoneMgr::Instance()->FindGASGameZone(destCenterNode);
		if (!zone)
		{
			ErrorLogStream << "Can't find game zone(" << destCenterNode << ") while send mail(sender=" << senderName << ", title=" << title
				<< ", content=" << content << ") to player(" << uid << ")!" << LogStream::eos;
			return false;
		}

		zone->TransmitToWorld(destAdminNode, mail);
		return true;
	}
}