#include "WSItemCheck.h"
#include "CLItemProtocol.h"
#include "WSPlayerMgr.h"
#include "WSNetwork.h"

WSItemCheck::WSItemCheck() 
: m_serial_id(0)
{}

UInt32 WSItemCheck::RequestOneCheck(UInt8 checkType, ObjID_t owerId, ObjID_t itemId, ObjID_t param, OBFUNCTION ff)
{
	CLProtocol::SceneCheckItemReq req;
	req.serialId = IncReqId();
	req.type = checkType;
	req.owerId = owerId;
	req.itemId = itemId;
	req.param = param;
	
	auto sceneServerID = WSPlayerMgr::Instance()->GetPlayerSceneServerID(owerId);
	if (sceneServerID == 0)
	{
		sceneServerID = WSNetwork::Instance()->SelectRandSceneServer();
	}
	auto sceneConn = WSNetwork::Instance()->FindConnection(sceneServerID);
	if (sceneConn != NULL)
	{
		CL_SEND_PROTOCOL(sceneConn, req);
		m_ffs[req.serialId] = ff;
		return req.serialId;
	}
	else
	{
		ErrorLogStream << "request one item check fail type:" << (UInt32)checkType << ", owerId:" << owerId << ", param:" << param << LogStream::eos;
		return 0;
	}
}

UInt32 WSItemCheck::IncReqId()
{
	if (m_serial_id == 0xFFFFFFFF)
	{
		m_serial_id = 0;
	}

	return ++m_serial_id;
}

void WSItemCheck::RmoveOneHandler(UInt32 serialId)
{
	auto ff = m_ffs.find(serialId);
	if (ff != m_ffs.end())
	{
		m_ffs.erase(ff);
	}
}

void WSItemCheck::HandleReponse(Avalon::NetInputStream &stream)
{
	UInt32 serialId = 0;
	UInt32 type = 0;
	UInt64 param = 0;
	UInt32 res = 0;
	WSItem* item = NULL;
	ObjID_t srcItemGuid = 0;
	try
	{
		stream & serialId & type & param & res;
		if (res == ErrorCode::SUCCESS)
		{
			item = new WSItem;
			stream & srcItemGuid & item->m_DataId;
			item->Decode(stream);
			item->SetID(srcItemGuid);

			ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(item->m_DataId);
			if (dataEntry)
			{
				item->SetDataEntry(dataEntry);
			}
			else{
				ErrorLogStream << "not find ItemDataEntry dataid = " << item->m_DataId << LogStream::eos;
				CL_SAFE_DELETE(item);
				RmoveOneHandler(serialId);
				return;
			}

			if (item->m_Num == 0)
			{
				ErrorLogStream << "itemcheck(" << serialId << ") get item(" << srcItemGuid << ", " << item->m_DataId << ") decode item num = 0." << LogStream::eos;
				item->m_Num = 1;
			}
		}
	}
	catch (Avalon::NetException&)
	{
		if (NULL != item)
			CL_SAFE_DELETE(item);
		RmoveOneHandler(serialId);
		ErrorLogStream << "handle one item check reponse decode fail type:" << type << ", param: " << param << LogStream::eos;
		return;
	}
	DebugLogStream << "handle one item check reponse serialId:" << serialId << ", type:" << type << ", param: " << param << LogStream::eos;
	auto ff = m_ffs.find(serialId);
	if (ff == m_ffs.end())
	{
		ErrorLogStream << "handle one item check reponse fail type:" << type << ", param:" << param << LogStream::eos;
		CL_SAFE_DELETE(item);
		return;
	}

	if (ff->second != nullptr)
	{
		ff->second(ff->first, res, item);
	}
	m_ffs.erase(ff);
}