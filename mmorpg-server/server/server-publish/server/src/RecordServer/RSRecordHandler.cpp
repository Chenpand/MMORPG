#include "RSRecordHandler.h"

#include <AvalonLogger.h>
#include <CLRecordProtocol.h>
#include <CLErrorCode.h>

#include "RSNetwork.h"
#include "RSTableMgr.h"
#include "RSApplication.h"
#include "RSRouter.h"

void InsertFunction::operator()(Avalon::Packet* packet) const
{
	Handle(0, packet->GetConnID(), packet);
}

void InsertFunction::Handle(UInt32 srcZoneId, UInt32 srcNodeId, Avalon::Packet* packet) const
{
	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());

	UInt8 needTrans = 0;
	UInt32	requestid = 0;
	std::string tablename;
	DBKey_t key = 0;
	UInt32	zoneId = 0;
	try
	{
		stream & needTrans & zoneId & requestid & tablename & key;
	}
	catch(const Avalon::NetException&)
	{
		ErrorLogStream << "InsertFunction NetException!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return;
	}

	if(SERVER_TYPE != ST_RECORD_SLAVER && zoneId == 0 && !RSTable::SaveGuidSeed(key))
	{
		ErrorLogStream << "save table(" << tablename << ")guid(" << key << ") failed." << LogStream::eos;
		CLProtocol::RecordInsertRet ret;
		ret.requestid = requestid;
		ret.result = ErrorCode::RECORD_ERROR;
		Avalon::NetConnectionPtr conn = RSNetwork::Instance()->FindConnection(packet->GetConnID());
		if(conn != NULL) CL_SEND_PROTOCOL(conn, ret);
		return;
	}

	// 如果需要转发，并且有slaver，就转发到slaver
	if (SERVER_TYPE == ST_RECORD && needTrans && Router::HasSlaverRecord())
	{
		Router::TransRequestToSlaver(packet->GetConnID(), packet);
		return;
	}

	//转发请求到目标区
	if(zoneId != 0 && zoneId != ZONE_ID)
	{
		Router::TransRequest(zoneId, packet->GetConnID(), packet);
		return;
	}

	RSTable* table = RSTableMgr::Instance()->FindTable(tablename);
	if(table == NULL)
	{
		ErrorLogStream << "unknow table " << tablename << " when inserting record!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return;
	}

#ifdef _NEW_DB_MGR_
	auto result = table->NewInsertRecord(requestid, packet->GetConnID(), key, stream);
	if(result != ErrorCode::SUCCESS && requestid > 0)
	{
		CLProtocol::RecordInsertRet ret;
		ret.requestid = requestid;
		ret.result = result;

		if(srcZoneId == 0)
		{
			Avalon::NetConnectionPtr conn = RSNetwork::Instance()->FindConnection(packet->GetConnID());
			if(conn != NULL) CL_SEND_PROTOCOL(conn, ret);
		}
		else 
		{
			Router::TransResponse(srcZoneId, srcNodeId, ret);
		}
	}

#else
	UInt32 result = table->InsertRecord(key, stream);

	if(requestid > 0)
	{
		CLProtocol::RecordInsertRet ret;
		ret.requestid = requestid;
		ret.result = result;

		// 如果是备用服务器，把返回转发给原服务器
		if (SERVER_TYPE == ST_RECORD_SLAVER)
		{
			Router::TransResponseFromSlaver(srcNodeId, ret);
			return;
		}

		if(srcZoneId == 0)
		{
			Avalon::NetConnectionPtr conn = RSNetwork::Instance()->FindConnection(packet->GetConnID());
			if(conn != NULL) CL_SEND_PROTOCOL(conn, ret);
		}
		else Router::TransResponse(srcZoneId, srcNodeId, ret);
	}
#endif
}

void UpdateFunction::operator()(Avalon::Packet* packet) const
{
	Handle(0, packet->GetID(), packet);
}

void UpdateFunction::Handle(UInt32 srcZoneId, UInt32 srcNodeId, Avalon::Packet* packet) const
{
	if (CL_GET_SERVERTYPE_FROM_NODEID(packet->GetConnID()) == ST_SCENE && RSApplication::Instance()->IsRecordUpdateLog())
	{
		DebugLogStream << "recv packet(" << packet->GetSequence() << ") from scene." << LogStream::eos;
	}
	
	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());

	UInt8 needTrans = 0;
	UInt32 zoneId = 0;
	UInt32 requestid = 0;

#ifdef _NEW_DB_MGR_

	try
	{
		stream & zoneId & requestid;

		std::string tablename;
		DBKey_t key = 0;
		UInt8 bFlush = 0;

		while (stream.GetPos() != packet->GetSize())
		{
			stream & tablename & key & bFlush;

			RSTable* table = RSTableMgr::Instance()->FindTable(tablename);
			if (table == NULL)
			{
				ErrorLogStream << "unknow table " << tablename << "." << LogStream::eos;
				RSApplication::Instance()->Exit();
				return;
			}

			if (key != 0)
			{
				if (!table->NewUpdateRecord(requestid, packet->GetConnID(), key, stream, bFlush != 0)) return;
			}
			else
			{
				if (!table->NewUpdateRecord(requestid, packet->GetConnID(), stream)) return;
			}
		}
	}
	catch(const Avalon::NetException&)
	{
		ErrorLogStream << "UpdateFunction NetException!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return;
	}
#else
	try
	{
		stream & needTrans & zoneId & requestid;
		
		// 如果需要转发，并且有slaver，就转发到slaver
		if (SERVER_TYPE == ST_RECORD && needTrans && Router::HasSlaverRecord())
		{
			Router::TransRequestToSlaver(packet->GetConnID(), packet);
			return;
		}

		//转发请求到目标区
		if(zoneId != 0 && zoneId != ZONE_ID)
		{
			Router::TransRequest(zoneId, packet->GetConnID(), packet);
			return;
		}

		while(stream.GetPos() != packet->GetSize())
		{
			std::string tablename;
			DBKey_t key = 0;
			UInt8 bFlush = 0;

			stream & tablename & key & bFlush;

			if (CL_GET_SERVERTYPE_FROM_NODEID(packet->GetConnID()) == ST_SCENE && RSApplication::Instance()->IsRecordUpdateLog())
			{
				DebugLogStream << "start to update table(" << tablename << ") key(" << key << ") flush(" << bFlush << ")." << LogStream::eos;
			}

			RSTable* table = RSTableMgr::Instance()->FindTable(tablename);
			if(table == NULL)
			{
				ErrorLogStream << "unknow table " << tablename << "." << LogStream::eos;
				RSApplication::Instance()->Exit();
				return;
			}

			if(key != 0)
			{
				if(!table->UpdateRecord(key, stream, bFlush != 0)) return;
			}
			else
			{
				if(!table->UpdateRecord(stream)) return;
			}
		}
	}
	catch(const Avalon::NetException&)
	{
		ErrorLogStream << "UpdateFunction NetException!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return;
	}

	if(requestid > 0)
	{
		CLProtocol::RecordUpdateRet ret;
		ret.requestid = requestid;

		// 如果是备用服务器，把返回转发给原服务器
		if (SERVER_TYPE == ST_RECORD_SLAVER)
		{
			Router::TransResponseFromSlaver(srcNodeId, ret);
			return;
		}
		
		if(srcZoneId == 0)
		{
			Avalon::NetConnectionPtr conn = RSNetwork::Instance()->FindConnection(packet->GetConnID());
			if(conn != NULL) CL_SEND_PROTOCOL(conn, ret);
		}
		else Router::TransResponse(srcZoneId, srcNodeId, ret);
	}
#endif
}

void DeleteFunction::operator()(Avalon::Packet* packet) const
{
	Handle(0, packet->GetID(), packet);
}

void DeleteFunction::Handle(UInt32 srcZoneId, UInt32 srcNodeId, Avalon::Packet* packet) const
{
	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());
	UInt8 needTrans = 0;
	UInt32	requestid = 0;
	UInt32	zoneId = 0;
	try
	{
		stream & needTrans & zoneId & requestid;

		// 如果需要转发，并且有slaver，就转发到slaver
		if (SERVER_TYPE == ST_RECORD && needTrans && Router::HasSlaverRecord())
		{
			Router::TransRequestToSlaver(packet->GetConnID(), packet);
			return;
		}

		//转发请求到目标区
		if(zoneId != 0 && zoneId != ZONE_ID)
		{
			Router::TransRequest(zoneId, packet->GetConnID(), packet);
			return;
		}

		while(stream.GetPos() != packet->GetSize())
		{
			std::string tablename;
			DBKey_t key = 0;
			std::string cond;

			stream & tablename & key;
			if(key == 0) stream & cond;

			RSTable* table = RSTableMgr::Instance()->FindTable(tablename);
			if(table == NULL)
			{
				ErrorLogStream << "unknow table " << tablename << "." << LogStream::eos;
				RSApplication::Instance()->Exit();
				return;
			}

			if(key != 0)
			{
				if(!table->DeleteRecord(key)) return;
			}
			else
			{
				if(!table->DeleteRecord(cond)) return;
			}
		}
	}
	catch(const Avalon::NetException&)
	{
		ErrorLogStream << "DeleteFunction NetException!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return;
	}

	if(requestid > 0)
	{
		CLProtocol::RecordDeleteRet ret;
		ret.requestid = requestid;

		// 如果是备用服务器，把返回转发给原服务器
		if (SERVER_TYPE == ST_RECORD_SLAVER)
		{
			Router::TransResponseFromSlaver(srcNodeId, ret);
			return;
		}

		if(srcZoneId == 0)
		{
			Avalon::NetConnectionPtr conn = RSNetwork::Instance()->FindConnection(packet->GetConnID());
			if(conn != NULL) CL_SEND_PROTOCOL(conn, ret);
		}
		else Router::TransResponse(srcZoneId, srcNodeId, ret);
	}
}

void SelectFunction::operator()(Avalon::Packet* packet) const
{
	Handle(0, packet->GetConnID(), packet);
}

void SelectFunction::Handle(UInt32 srcZoneId, UInt32 srcNodeId, Avalon::Packet* packet) const
{
	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());

	UInt8 needTrans = 0;
	UInt32	requestid = 0;
	UInt32	zoneId = 0;
	std::string tablename;

	try
	{
		stream & needTrans & zoneId & requestid  & tablename;
	}
	catch(const Avalon::NetException&)
	{
		ErrorLogStream << "SelectFunction NetException!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return;
	}

	// 如果需要转发，并且有slaver，就转发到slaver
	if (SERVER_TYPE == ST_RECORD && needTrans && Router::HasSlaverRecord())
	{
		Router::TransRequestToSlaver(packet->GetConnID(), packet);
		return;
	}

	//转发请求到目标区
	if(zoneId != 0 && zoneId != ZONE_ID)
	{
		Router::TransRequest(zoneId, packet->GetConnID(), packet);
		return;
	}

	RSTable* table = RSTableMgr::Instance()->FindTable(tablename);
	if(table == NULL)
	{
		ErrorLogStream << "unknow table " << tablename << "." << LogStream::eos;
		RSApplication::Instance()->Exit();
		return;
	}

	Avalon::NetConnectionPtr conn = RSNetwork::Instance()->FindConnection(packet->GetConnID());

#ifdef _NEW_DB_MGR_
	table->SelectRecordNew(requestid, 0, stream, conn);
#else
	table->SelectRecord(srcZoneId, srcNodeId, requestid, 0, stream, conn);
#endif
}

void FlushFunction::operator()(Avalon::Packet* packet) const
{
	Handle(0, packet->GetConnID(), packet);
}

void FlushFunction::Handle(UInt32 srcZoneId, UInt32 srcNodeId, Avalon::Packet* packet) const
{
	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());

	UInt8 needTrans = 0;
	UInt32	requestid = 0;
	UInt32	zoneId = 0;
	std::string tablename;

	try
	{
		stream & needTrans & zoneId & requestid & tablename;
	}
	catch(const Avalon::NetException&)
	{
		ErrorLogStream << "FlushFunction NetException!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return;
	}

	// 如果需要转发，并且有slaver，就转发到slaver
	if (SERVER_TYPE == ST_RECORD && needTrans && Router::HasSlaverRecord())
	{
		Router::TransRequestToSlaver(packet->GetConnID(), packet);
		return;
	}

	//转发请求到目标区
	if(zoneId != 0 && zoneId != ZONE_ID)
	{
		Router::TransRequest(zoneId, packet->GetConnID(), packet);
		return;
	}

	RSTable* table = RSTableMgr::Instance()->FindTable(tablename);
	if(table == NULL)
	{
		ErrorLogStream << "unknow table " << tablename << "." << LogStream::eos;
		RSApplication::Instance()->Exit();
		return;
	}

	table->Flush();
	InfoLogStream << "flush table " << tablename << "!" << LogStream::eos;

	if(requestid != 0)
	{
		CLProtocol::RecordFlushRet ret;
		ret.requestid = requestid;

		// 如果是备用服务器，把返回转发给原服务器
		if (SERVER_TYPE == ST_RECORD_SLAVER)
		{
			Router::TransResponseFromSlaver(srcNodeId, ret);
			return;
		}
		
		if(srcZoneId == 0)
		{
			Avalon::NetConnectionPtr conn = RSNetwork::Instance()->FindConnection(packet->GetConnID());
			if(conn != NULL) CL_SEND_PROTOCOL(conn, ret);
		}
		else Router::TransResponse(srcZoneId, srcNodeId, packet);
	}
}

void HandleProtocol(CLProtocol::RecordGuidseedReq& protocol)
{
	CLProtocol::RecordGuidseedRet ret;
	ret.seed = RSTableMgr::Instance()->GetGuidSeed(protocol.nodetype, protocol.nodeid);
	protocol.SendBack(ret);
}

void ChangeNameFunction::operator()(Avalon::Packet* packet) const
{
	Handle(0, packet->GetID(), packet);
}

void ChangeNameFunction::Handle(UInt32 srcZoneId, UInt32 srcNodeId, Avalon::Packet* packet) const
{
	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetMaxSize());

	UInt8 needTrans = 0;
	UInt32 requestid = 0;
	UInt32 zoneId = 0;
	std::string tableName;
	DBKey_t key = 0;
	std::string name;
	UInt8 isInsert = 0;

	try
	{
		stream & needTrans & zoneId & requestid & tableName & key & name & isInsert;
	}
	catch(const Avalon::NetException&)
	{
		ErrorLogStream << "ChangeNameFunction NetException!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return;
	}

	//转发请求到目标区
	if(zoneId != 0 && zoneId != ZONE_ID)
	{
		Router::TransRequest(zoneId, packet->GetConnID(), packet);
		return;
	}

	RSTable* table = RSTableMgr::Instance()->FindTable(tableName);
	if(table == NULL)
	{
		ErrorLogStream << "unknow table " << tableName << "." << LogStream::eos;
		RSApplication::Instance()->Exit();
		return;
	}

	UInt32 result = table->ChangeName(key, name, isInsert);

	if(requestid != 0)
	{
		CLProtocol::RecordChangeNameRet ret;
		ret.requestid = requestid;
		ret.result = result;

		if(srcZoneId == 0)
		{
			Avalon::NetConnectionPtr conn = RSNetwork::Instance()->FindConnection(packet->GetConnID());
			if(conn != NULL) CL_SEND_PROTOCOL(conn, ret);
		}
		else Router::TransResponse(srcZoneId, srcNodeId, packet);
	}
}

void AutoGuidInsertFunction::operator()(Avalon::Packet* packet) const
{
	Handle(0, packet->GetID(), packet);
}

void AutoGuidInsertFunction::Handle(UInt32 srcZoneId, UInt32 srcNodeId, Avalon::Packet* packet) const
{
	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetMaxSize());

	UInt8 needTrans = 0;
	UInt32 requestid = 0;
	UInt32 zoneId = 0;
	std::string tableName;

	try
	{
		stream & needTrans & zoneId & requestid & tableName;
	}
	catch (const Avalon::NetException&)
	{
		ErrorLogStream << "AutoGuidInsertFunction NetException!" << LogStream::eos;
		RSApplication::Instance()->Exit();
		return;
	}

	//转发请求到目标区
	if (zoneId != 0 && zoneId != ZONE_ID)
	{
		Router::TransRequest(zoneId, packet->GetConnID(), packet);
		return;
	}

	RSTable* table = RSTableMgr::Instance()->FindTable(tableName);
	if (table == NULL)
	{
		ErrorLogStream << "unknow table " << tableName << "." << LogStream::eos;
		RSApplication::Instance()->Exit();
		return;
	}

	table->InsertAutoGuidRecord(stream, requestid, packet->GetConnID());
}

void HandleProtocol(CLProtocol::RecordTransmitRequest& protocol)
{
	CLPacketFunction* ptr = RSNetwork::Instance()->GetPacketExecutor().FindFunction(protocol.packet->GetID());
	if(ptr == NULL) return;

	RecordFunction* func = dynamic_cast<RecordFunction*>(ptr);
	if(func == NULL) return;

	func->Handle(protocol.zoneId, protocol.nodeId, protocol.packet);
}

void HandleProtocol(CLProtocol::RecordTransmitResponse& protocol)
{
	Avalon::NetConnectionPtr conn = RSNetwork::Instance()->FindConnection(protocol.nodeId);
	if(conn == NULL) return;

	conn->SendPacket(protocol.packet);
}

void HandleProtocol(CLProtocol::RecordNotifyOtherDisconnected& protocol)
{
	Router::BroadcastToScene(protocol);
}
