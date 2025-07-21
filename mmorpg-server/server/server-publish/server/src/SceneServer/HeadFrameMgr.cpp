#include "HeadFrameMgr.h"
#include "Player.h"

#include <CLRecordClient.h>
#include <CLHeadFrameProtocol.h>
#include <CLHeadFrameCompensateDataEntry.h>
#include <udplog_typedef.h>

HeadFrameMgr::HeadFrameMgr()
{
	m_Owner = NULL;
}

HeadFrameMgr::~HeadFrameMgr()
{

}

void HeadFrameMgr::Online()
{
	_UdpHeadFrame(UDP_HEAD_FRAME_LOGIN, GetOwner()->GetHeadFrameId());
}

void HeadFrameMgr::Compensate()
{
	UInt32 cnt = GetOwner()->GetCounter(HEAD_FRAME_COMPENSATE);
	if (cnt != 0)
		return;

	// 补发头像框
	class HeadFrameVistor : public Avalon::DataEntryVisitor<HeadFrameCompensateDataEntry>
	{
	public:
		HeadFrameVistor(Player* player, const std::string& reason) : m_Player(player), m_Reason(reason){}

		bool operator() (HeadFrameCompensateDataEntry* entry)
		{
			if (NULL == entry || NULL == m_Player) return false;
			
			bool isOk = false;
			if (HFCT_VIP == entry->type)
			{
				if (m_Player->GetVipLvl() >= entry->needVal)
					isOk = true;
			}
			else if (HFCT_ACHIEVE == entry->type)
			{
				if (m_Player->GetAchievementScore() >= entry->needVal)
					isOk = true;
			}

			if (isOk)
			{
				m_Player->AddItem(m_Reason.c_str(), entry->headFrameId, 1);
			}

			return true;
		}

	private: 
		Player* m_Player;
		std::string m_Reason;
	};
	
	HeadFrameVistor vistor(GetOwner(), GetReason(SOURCE_TYPE_HEAD_FRAME_COMPENSATE));
	HeadFrameCompensateDataEntryMgr::Instance()->Visit(vistor);

	GetOwner()->SetCounter(HEAD_FRAME_COMPENSATE, 1);
}

void HeadFrameMgr::OnHeartbeat(const Avalon::Time& now)
{
	std::map<UInt32, UInt32>::iterator iter = m_HeadFrameMap.begin();
	while (iter != m_HeadFrameMap.end())
	{
		if (iter->second == 0 || now.Sec() < iter->second)
		{
			iter++;
			continue;
		}

		// 如果是当前设置的头像
		if (GetOwner()->GetHeadFrameId() == iter->first)
		{
			GetOwner()->SetHeadFrameId(0);
			GetOwner()->SyncProperty();
			GetOwner()->SyncBaseInfoToWorld();
		}

		// 通知
		NotifyHeadFrame(iter->first, iter->second, false);

		// 删除数据
		_DelHeadFrame(iter->first);

		// 删除
		m_HeadFrameMap.erase(iter++);
	}
}

void HeadFrameMgr::LoadHeadFrame(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		UInt32 id = callback->GetData("head_frame_id");
		UInt32 expireTime = callback->GetData("expire_time");

		m_HeadFrameMap[id] = expireTime;
	}
}

void HeadFrameMgr::AddHeadFrame(UInt32 id, UInt32 expireTime)
{
	auto iter = m_HeadFrameMap.find(id);
	if (iter != m_HeadFrameMap.end())
	{
		ErrorLogStream << "Yet Has HeadFrame Id:" << id << " expireTime:" << expireTime << LogStream::eos;
		return;
	}

	if (expireTime != 0)
	{
		expireTime += UInt32(CURRENT_TIME.Sec());
	}
	
	m_HeadFrameMap[id] = expireTime;

	_InsertHeadFrame(id, expireTime);

	NotifyHeadFrame(id, expireTime, true);
}

void HeadFrameMgr::HandleHeadFrameReq()
{
	CLProtocol::SceneHeadFrameRes res;
	for (auto& iter : m_HeadFrameMap)
	{
		CLProtocol::HeadFrame headF;
		headF.headFrameId = iter.first;
		headF.expireTime = iter.second;
		res.headFrameList.push_back(headF);
	}
	
	GetOwner()->SendProtocol(res);

	Compensate();
}

void HeadFrameMgr::HandleUseHeadFrameReq(UInt32 id)
{
	CLProtocol::SceneHeadFrameUseRes res;
	res.retCode = ErrorCode::SUCCESS;
	do 
	{
		if (id != 0)
		{
			auto iter = m_HeadFrameMap.find(id);
			if (iter == m_HeadFrameMap.end())
			{
				res.retCode = ErrorCode::ITEM_DATA_INVALID;
				break;
			}
		}

		/*if (iter->second > CURRENT_TIME.Sec())
		{
		res.retCode = ErrorCode::ITEM_DATA_INVALID;
		break;
		}*/

		GetOwner()->SetHeadFrameId(id);
		GetOwner()->SyncProperty();
		GetOwner()->SyncBaseInfoToWorld();

		_UdpHeadFrame(UDP_HEAD_FRAME_USE, id);

	} while (0);

	GetOwner()->SendProtocol(res);
}

void HeadFrameMgr::NotifyHeadFrame(UInt32 id, UInt32 expireTime, bool isGet)
{
	CLProtocol::SceneHeadFrameNotify notify;
	notify.isGet = isGet ? 1 : 0;
	notify.headFrame.headFrameId = id;
	notify.headFrame.expireTime = expireTime;
	GetOwner()->SendProtocol(notify);
}

void HeadFrameMgr::_UdpHeadFrame(UInt32 opType, UInt32 id)
{
	GetOwner()->SendUdpLog("head_frame", LOG_HEAD_FRAME, opType, id);
}

void HeadFrameMgr::_InsertHeadFrame(UInt32 id, UInt32 expireTime)
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_head_frame", CLRecordClient::Instance()->GenGuid());
	cmd->PutData("owner_id", GetOwner()->GetID());
	cmd->PutData("head_frame_id", id);
	cmd->PutData("expire_time", expireTime);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void HeadFrameMgr::_DelHeadFrame(UInt32 id)
{
	std::ostringstream cond;
	cond << "owner_id=" << GetOwner()->GetID()<<" and head_frame_id="<<id;
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_head_frame", cond.str());
	CLRecordClient::Instance()->SendCommand(cmd);
}