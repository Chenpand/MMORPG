#include "CLRecordClient.h"
#include <AvalonThreadAPI.h>

#include "CLApplication.h"
#include "CLRecordProtocol.h"
#include "CLNetwork.h"
#include "CLErrorCode.h"

#define INVALID_SEED UInt64(-1)

CLRecordClient::CLRecordClient()
{
	m_pApp = NULL;

	m_bRun = false;
	m_Now = 0;
	m_Seed = INVALID_SEED;

	m_RequestIdSeed = 0;
	m_Sequence = 0;
	m_ConnectionNum = 0;

	if (SERVER_TYPE == ST_BSCENE)
	{
		m_Seed = 1000;
		m_RequestIdSeed = 1;
	}
}

CLRecordClient::~CLRecordClient()
{
	Final();

	while(!m_Requests.empty())
	{
		RecordRequest req = m_Requests.pop();
		CL_SAFE_DELETE(req.cmd);
		CL_SAFE_DELETE(req.callback);
	}

	while(!m_Callbacks.empty())
	{
		delete m_Callbacks.pop();
	}

	while(!m_Packets.empty())
	{
		Avalon::Packet* packet = m_Packets.pop();
		Avalon::Packet::Destroy(packet);
	}

	for(std::map<UInt32,CLRecordCallback*>::iterator iter = m_WaitCallbacks.begin();
		iter != m_WaitCallbacks.end();++iter)
	{
		delete iter->second;
	}
	m_WaitCallbacks.clear();

	for (UInt32 k = 0;k<m_RequestVec.size();++k)
	{
		while (!m_RequestVec[k].empty())
		{
			RecordRequest req = m_RequestVec[k].pop();
			CL_SAFE_DELETE(req.cmd);
			CL_SAFE_DELETE(req.callback);
		}
	}
}

#define WAIT_TRUE(Expr,Times) \
	while(!(Expr) && Times-- > 0) Avalon::Thread::Sleep(10);

bool CLRecordClient::Init(CLApplication* app, Avalon::NetConnectionPtr conn)
{
	if(m_bRun) return true;
	if(app == NULL || conn == NULL) return false;
	m_pApp = app;
	m_Connection = conn;

	m_bRun = true;
	if(!Activate())
	{
		m_bRun = false; 
		return false;
	}

	CLProtocol::RecordGuidseedReq req;
	req.nodetype = SERVER_TYPE;
	req.nodeid = SERVER_ID;
	CL_SEND_PROTOCOL(m_Connection,req);

	UInt32 times = 300;
	WAIT_TRUE(m_Seed != INVALID_SEED,times);
	if(times == 0)
	{
		m_bRun = false;
		Join();
		return false;
	}

	return true;
}

void CLRecordClient::SetNeedTransTables(const std::vector<std::string>& tables)
{
	m_Trans2SlaverTableNames.clear();
	for (auto& table : tables)
	{
		m_Trans2SlaverTableNames.insert(table);
	}
}

UInt8 CLRecordClient::IsTableNeedTrans(const std::string& tablename)
{
	return m_Trans2SlaverTableNames.find(tablename) != m_Trans2SlaverTableNames.end() ? 1 : 0;
}

void CLRecordClient::Final()
{
	if(!m_bRun) return;

	m_bRun = false;
	Join();

	m_pApp = NULL;
}

void CLRecordClient::ProcessCallbacks()
{
	while(!m_Callbacks.empty())
	{
		CLRecordCallback* callback = m_Callbacks.pop();
		callback->Execute();
		CL_SAFE_DELETE(callback);
	}
}

void CLRecordClient::OnPacketRecved(Avalon::Packet* packet)
{
	if(packet == NULL) return;
	m_Packets.push(packet);
}


ObjID_t CLRecordClient::GenGuid()
{
	if(m_Seed == INVALID_SEED) return 0;

	++m_Seed;
	return CL_MAKE_GUID(m_Seed);
}

CLInsertCommand* CLRecordClient::PrepareInsertCommand(const std::string& table, DBKey_t key, UInt64 roleId, UInt32 tableCount)
{
	if (roleId != 0 && tableCount > 1)
	{
		std::string tableName = table + "_" + std::to_string(roleId % tableCount);
		return new CLInsertCommand(0, tableName, key);
	}
	else
	{
		return new CLInsertCommand(0, table, key);
	}
}

CLInsertCommand* CLRecordClient::PrepareInsertCommand(UInt32 zoneId, const std::string& table, DBKey_t key, UInt64 roleId, UInt32 tableCount)
{
	if (roleId != 0 && tableCount > 1)
	{
		std::string tableName = table + "_" + std::to_string(roleId % tableCount);
		return new CLInsertCommand(zoneId, tableName, key);
	}
	else
	{
		return new CLInsertCommand(zoneId, table, key);
	}
}

CLUpdateCommand* CLRecordClient::PrepareUpdateCommand(const std::string& table, DBKey_t key, bool bFlush, UInt64 roleId, UInt32 tableCount)
{
	if (roleId != 0 && tableCount > 1)
	{
		std::string tableName = table + "_" + std::to_string(roleId % tableCount);
		return new CLUpdateCommand(0, tableName, key, bFlush);
	}
	else
	{
		return new CLUpdateCommand(0, table, key, bFlush);
	}
}

CLUpdateCommand* CLRecordClient::PrepareUpdateCommand(UInt32 zoneId, const std::string& table, DBKey_t key, bool bFlush, UInt64 roleId, UInt32 tableCount)
{
	if (roleId != 0 && tableCount > 1)
	{
		std::string tableName = table + "_" + std::to_string(roleId % tableCount);
		return new CLUpdateCommand(zoneId, tableName, key, bFlush);
	}
	else
	{
		return new CLUpdateCommand(zoneId, table, key, bFlush);
	}
}

CLUpdateCommand* CLRecordClient::PrepareUpdateCommand(const std::string& table, const std::string& cond, UInt64 roleId, UInt32 tableCount)
{
	if (roleId != 0 && tableCount > 1)
	{
		std::string tableName = table + "_" + std::to_string(roleId % tableCount);
		return new CLUpdateCommand(0, tableName, cond);
	}
	else
	{
		return new CLUpdateCommand(0, table, cond);
	}
}

CLUpdateCommand* CLRecordClient::PrepareUpdateCommand(UInt32 zoneId, const std::string& table, const std::string& cond, UInt64 roleId, UInt32 tableCount)
{
	if (roleId != 0 && tableCount > 1)
	{
		std::string tableName = table + "_" + std::to_string(roleId % tableCount);
		return new CLUpdateCommand(zoneId, tableName, cond);
	}
	else
	{
		return new CLUpdateCommand(zoneId, table, cond);
	}
}

CLDeleteCommand* CLRecordClient::PrepareDeleteCommand(const std::string& table, DBKey_t key, UInt64 roleId, UInt32 tableCount)
{
	if(key == 0) return NULL;
	
	if (roleId != 0 && tableCount > 1)
	{
		std::string tableName = table + "_" + std::to_string(roleId % tableCount);
		return new CLDeleteCommand(0, tableName, key);
	}
	else
	{
		return new CLDeleteCommand(0, table, key);
	}
}

CLDeleteCommand* CLRecordClient::PrepareDeleteCommand(UInt32 zoneId, const std::string& table, DBKey_t key, UInt64 roleId, UInt32 tableCount)
{
	if(key == 0) return NULL;
	
	if (roleId != 0 && tableCount > 1)
	{
		std::string tableName = table + "_" + std::to_string(roleId % tableCount);
		return new CLDeleteCommand(zoneId, tableName, key);
	}
	else
	{
		return new CLDeleteCommand(zoneId, table, key);
	}
}

CLDeleteCommand* CLRecordClient::PrepareDeleteCommand(const std::string& table, const std::string& cond, UInt64 roleId, UInt32 tableCount)
{
	if (roleId != 0 && tableCount > 1)
	{
		std::string tableName = table + "_" + std::to_string(roleId % tableCount);
		return new CLDeleteCommand(0, tableName, cond);
	}
	else
	{
		return new CLDeleteCommand(0, table, cond);
	}
}

CLDeleteCommand* CLRecordClient::PrepareDeleteCommand(UInt32 zoneId, const std::string& table, const std::string& cond, UInt64 roleId, UInt32 tableCount)
{
	if (roleId != 0 && tableCount > 1)
	{
		std::string tableName = table + "_" + std::to_string(roleId % tableCount);
		return new CLDeleteCommand(zoneId, tableName, cond);
	}
	else
	{
		return new CLDeleteCommand(zoneId, table, cond);
	}
}

CLSelectCommand* CLRecordClient::PrepareSelectCommand(const std::string& table, UInt64 roleId, UInt32 tableCount)
{
	if (roleId != 0 && tableCount > 1)
	{
		std::string tableName = table + "_" + std::to_string(roleId % tableCount);
		return new CLSelectCommand(0, tableName);
	}
	else
	{
		return new CLSelectCommand(0, table);
	}
}

CLSelectCommand* CLRecordClient::PrepareSelectCommand(UInt32 zoneId, const std::string& table, UInt64 roleId, UInt32 tableCount)
{
	if (roleId != 0 && tableCount > 1)
	{
		std::string tableName = table + "_" + std::to_string(roleId % tableCount);
		return new CLSelectCommand(zoneId, tableName);
	}
	else
	{
		return new CLSelectCommand(zoneId, table);
	}
}

CLFlushCommand* CLRecordClient::PrepareFlushCommand(const std::string& table, UInt64 roleId, UInt32 tableCount)
{
	if (roleId != 0 && tableCount > 1)
	{
		std::string tableName = table + "_" + std::to_string(roleId % tableCount);
		return new CLFlushCommand(0, tableName);
	}
	else
	{
		return new CLFlushCommand(0, table);
	}
}

CLFlushCommand* CLRecordClient::PrepareFlushCommand(UInt32 zoneId, const std::string& table, UInt64 roleId, UInt32 tableCount)
{
	if (roleId != 0 && tableCount > 1)
	{
		std::string tableName = table + "_" + std::to_string(roleId % tableCount);
		return new CLFlushCommand(zoneId, tableName);
	}
	else
	{
		return new CLFlushCommand(zoneId, table);
	}
}

CLChangeNameCommand* CLRecordClient::PrepareChangeNameCommand(const std::string& table, DBKey_t key, const std::string& name, UInt8 isInsert)
{
	return new CLChangeNameCommand(0, table, key, name, isInsert);
}

CLChangeNameCommand* CLRecordClient::PrepareChangeNameCommand(UInt32 zoneId, const std::string& table, DBKey_t key, const std::string& name, UInt8 isInsert)
{
	return new CLChangeNameCommand(zoneId, table, key, name, isInsert);
}

CLAutoGuidInsertCommand* CLRecordClient::PrepareAutoGuidInsertCommand(const std::string& table)
{
	if (table != "t_abnormal_transaction")
	{
		return NULL;
	}

	return new CLAutoGuidInsertCommand(0, table);
}

//noted by aprilliu, 仅仅是把请求push到m_Requests队列中， 实际要在CLRecordClient::SendCommands中发送
void CLRecordClient::SendCommand(CLRecordCommand* cmd, CLRecordCallback* callback, UInt64 roleId)
{
	if(!m_bRun || cmd == NULL || !cmd->IsValid()){
		CL_SAFE_DELETE(cmd);
		if(callback != NULL)
		{
			callback->OnFailed(0);
			CL_SAFE_DELETE(callback);
		}
		return;
	}

	RecordRequest req;
	req.cmd = cmd;
	req.callback = callback;

	if (roleId == 0 || m_ConnectionNum == 0)
	{
		m_Requests.push(req);
	}
	else
	{
		m_RequestVec[roleId % m_ConnectionNum].push(req);
	}
}


void CLRecordClient::ProcessPackets()
{
	while(!m_Packets.empty())
	{
		Avalon::Packet* packet = m_Packets.pop();

		switch(packet->GetID())
		{
		case CLProtocol::RECORD_INSERT_RET:
			HandleInsertRet(packet);
			break;
		case CLProtocol::RECORD_UPDATE_RET:
			HandleUpdateRet(packet);
			break;
		case CLProtocol::RECORD_DELETE_RET:
			HandleDeleteRet(packet);
			break;
		case CLProtocol::RECORD_SELECT_RET:
			HandleSelectRet(packet);
			break;
		case CLProtocol::RECORD_GUIDSEED_RET:
			{
				CLProtocol::RecordGuidseedRet ret;
				if(ret.Decode(packet))
				{
					m_Seed = ret.seed;
				}
			}
			break;
		case CLProtocol::RECORD_FLUSH_RET:
			HandleFlushRet(packet);
			break;
		case CLProtocol::RECORD_CHANGENAME_RET:
			HandleChangeNameRet(packet);
			break;
		}

		Avalon::Packet::Destroy(packet);
	}
}

void CLRecordClient::HandleInsertRet(Avalon::Packet* packet)
{
	CLProtocol::RecordInsertRet ret;
	if(!ret.Decode(packet))
	{
		ErrorLogStream << "decode record insert response failed!" << LogStream::eos;
		m_pApp->Exit();
		return;
	}

	SetResult(ret.requestid, ret.result);
}

void CLRecordClient::HandleUpdateRet(Avalon::Packet* packet)
{
	CLProtocol::RecordUpdateRet ret;
	if(!ret.Decode(packet))
	{		
		ErrorLogStream << "decode record update response failed!" << LogStream::eos;
		m_pApp->Exit();
		return;
	}

	SetResult(ret.requestid, ErrorCode::SUCCESS);
}

void CLRecordClient::HandleDeleteRet(Avalon::Packet* packet)
{
	CLProtocol::RecordDeleteRet ret;
	if(!ret.Decode(packet))
	{
		ErrorLogStream << "decode record delete response failed!" << LogStream::eos;
		m_pApp->Exit();
		return;
	}

	SetResult(ret.requestid, ErrorCode::SUCCESS);
}

void CLRecordClient::HandleFlushRet(Avalon::Packet* packet)
{
	CLProtocol::RecordFlushRet ret;
	if(!ret.Decode(packet))
	{
		ErrorLogStream << "decode record flush response failed!" << LogStream::eos;
		m_pApp->Exit();
		return;
	}

	SetResult(ret.requestid, ErrorCode::SUCCESS);
}

void CLRecordClient::HandleChangeNameRet(Avalon::Packet* packet)
{
	CLProtocol::RecordInsertRet ret;
	if(!ret.Decode(packet))
	{
		ErrorLogStream << "decode record changename response failed!" << LogStream::eos;
		m_pApp->Exit();
		return;
	}

	SetResult(ret.requestid, ret.result);
}

void CLRecordClient::HandleSelectRet(Avalon::Packet* packet)
{
	Avalon::NetInputStream stream(packet->GetBuffer(),packet->GetSize());
	UInt32	requestid = 0;
	UInt8	rettype = 0;
	try
	{
		stream & requestid & rettype;
	}
	catch(const Avalon::NetException&)
	{
		ErrorLogStream << "net exception when handle record select response!" << LogStream::eos;
		m_pApp->Exit();
		return;
	}

	CLRecordCallback* callback = FindCallback(requestid);
	if(callback == NULL) return;

	switch(rettype)
	{
	case 1:
		{
			if(!callback->DecodeColumns(stream))
			{
				ErrorLogStream << "request(" << requestid << ") decode columns when handle record select response!" << LogStream::eos;
				m_pApp->Exit();
			}
		}
		break;
	case 2:
		{
			while(stream.GetPos() != packet->GetSize())
			{
				if(!callback->DecodeRow(stream))
				{
					ErrorLogStream << "request(" << requestid << ") decode rows failed when handle record select response!" << LogStream::eos;
					m_pApp->Exit();
					break;
				}
			}
		}
		break;
	default:
		{
			SetResult(requestid, ErrorCode::SUCCESS);
		}
	}
}

void CLRecordClient::SetResult(UInt32 requestid, UInt32 result)
{
	if (requestid == 0)
	{
		return;
	}

	CallbackMap::iterator iter = m_WaitCallbacks.find(requestid);
	if (iter == m_WaitCallbacks.end())
	{
		ErrorLogStream << "[db] set request(" << requestid << ") result(" << result << "), can't find callback." << LogStream::eos;
		return;
	}
	CLRecordCallback* callback = iter->second;
	m_WaitCallbacks.erase(iter);

	callback->SetResult(result);
	m_Callbacks.push(callback);
}

CLRecordCallback* CLRecordClient::FindCallback(UInt32 requestid)
{
	if(requestid == 0) return NULL;

	CallbackMap::iterator iter = m_WaitCallbacks.find(requestid);
	if(iter != m_WaitCallbacks.end())
	{
		return iter->second;
	}
	return NULL;
}

void CLRecordClient::SendCommands()
{
	if(m_Connection == NULL) return;

	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetSequence(++m_Sequence);

	UInt32 zoneId = 0;
	while(!m_Requests.empty())
	{
		RecordRequest req = m_Requests.pop();
		CLRecordCommand* cmd = req.cmd;
		CLRecordCallback* callback = req.callback;
		UInt8 needTrans = IsTableNeedTrans(cmd->GetTableName());

		UInt32 requestid = 0;
		if(callback != NULL)
		{
			//生成请求id
			if(++m_RequestIdSeed == 0) ++m_RequestIdSeed;  //生成唯一的请求序列号
			requestid = m_RequestIdSeed;

			callback->SetRequestTime(m_Now);
			if (!m_WaitCallbacks.insert(std::make_pair(requestid, callback)).second) //noted by aprilliu, callback对象包含了db请求的上下文信息，是由具体发送请求方生成并填充信息的(比如：InsertRoleCallback)。 实际发送的时候以请求的序列号为key 插入到m_WaitCallbacks，这其实处理异步请求。
			{
				CL_SAFE_DELETE(callback);
				CL_SAFE_DELETE(cmd);
				Avalon::Packet::Destroy(packet);
				ErrorLogStream << "duplicate record requestid(" << requestid << ")!" << LogStream::eos;
				m_pApp->Exit();
				return;
			}
		}

		switch(cmd->GetType())
		{
		case CLProtocol::RECORD_INSERT_REQ:
		case CLProtocol::RECORD_SELECT_REQ:
		case CLProtocol::RECORD_CHANGENAME_REQ:
		case CLProtocol::RECORD_AUTO_GUID_INSERT_REQ:
			{
				if(packet->GetSize() > 0)	//不和其他命令一起发送
				{
					m_Connection->SendPacket(packet);
					packet->SetSize(0);
				}

				Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
				stream & needTrans & cmd->GetZoneId() & requestid;
				if(!cmd->Encode(stream))
				{
					ErrorLogStream << "encode (insert|select) record cmd failed! table=" <<
						cmd->GetTableName() << "." << LogStream::eos;
					m_pApp->Exit();
					break;
				}
				packet->SetID(cmd->GetType());
				packet->SetSize(UInt16(stream.GetPos()));
				m_Connection->SendPacket(packet);
				packet->SetSize(0);
			}
			break;
		case CLProtocol::RECORD_DELETE_REQ:
		case CLProtocol::RECORD_UPDATE_REQ:
		case CLProtocol::RECORD_FLUSH_REQ:
			{
				if(packet->GetSize() > 0 && 
					( packet->GetID() != cmd->GetType() 
					|| requestid != 0 
					|| cmd->GetZoneId() != zoneId
					|| needTrans))	//不能和不同类型的命令一起发，两个有回调的不能一起发，如果是需要转发的，也不能一起发
				{
					if (requestid > 0 && cmd->GetType() == CLProtocol::RECORD_UPDATE_REQ && CL_GET_SERVERTYPE_FROM_NODEID(NODE_ID) == ST_SCENE)
					{
						DebugLogStream << "send packet sequence(" << packet->GetSequence() << ") to record." << LogStream::eos;
					}
					
					m_Connection->SendPacket(packet);
					packet->SetSize(0);
					packet->SetSequence(++m_Sequence);
				}

				zoneId = cmd->GetZoneId();
				Avalon::NetOutputStream stream(packet->GetBuffer() + packet->GetSize(), packet->GetMaxSize() - packet->GetSize());
				if(packet->GetSize() == 0)	//写入第一个请求的请求id
				{
					stream & needTrans & zoneId & requestid;
				}

				if (requestid > 0 && cmd->GetType() == CLProtocol::RECORD_UPDATE_REQ && CL_GET_SERVERTYPE_FROM_NODEID(NODE_ID) == ST_SCENE)
				{
					CLUpdateCommand* updateCmd = (CLUpdateCommand*)cmd;
					DebugLogStream << "packet sequence(" << packet->GetSequence() << ") encode cmd(" << cmd->GetType() << ", " << updateCmd->GetKey() << ")." << LogStream::eos;
				}

				if(!cmd->Encode(stream))
				{
					ErrorLogStream << "encode (delete|update) record cmd failed! table="
						<< cmd->GetTableName() << "." << LogStream::eos;
					m_pApp->Exit();
					break;
				}
				packet->SetID(cmd->GetType());
				packet->SetSize(packet->GetSize() + UInt16(stream.GetPos()));

				// 转发包永远单独发送
				if(needTrans || packet->GetSize() > packet->GetMaxSize() / 3) //不超过最大包大小的三分之一
				{
					if (requestid > 0 && cmd->GetType() == CLProtocol::RECORD_UPDATE_REQ && CL_GET_SERVERTYPE_FROM_NODEID(NODE_ID) == ST_SCENE)
					{
						DebugLogStream << "send packet sequence(" << packet->GetSequence() << ") to record." << LogStream::eos;
					}

					m_Connection->SendPacket(packet);
					packet->SetSize(0);
				}
			}
			break;
		}

		CL_SAFE_DELETE(cmd);
	}// end of while(!m_Requests.empty())

	if(packet->GetSize() > 0)
	{
		m_Connection->SendPacket(packet);
		packet->SetSize(0);
	}

	Avalon::Packet::Destroy(packet);
}

void CLRecordClient::Run()
{

	while(m_bRun || !m_Requests.empty())
	{
		Avalon::Thread::Sleep(5);

		m_Now =(UInt32) Avalon::Time::CurrentTime().Sec();

		ProcessPackets(); //从消息队列中取出消息进行处理

		//判断超时
		for(CallbackMap::iterator iter = m_WaitCallbacks.begin();
			iter != m_WaitCallbacks.end();)
		{
			auto requestid = iter->first;
			CLRecordCallback* callback = iter->second;
			++iter;
			if(m_Now > callback->GetRequestTime() + 300) //检查哪些超时的db请求，直接设置对应的请求的结果为ErrorCode::RECORD_TIMEOUT
			{
				DebugLogStream << "[db]request(" << requestid << ") time(" << callback->GetRequestTime() << ") now(" << m_Now << ")." << LogStream::eos;
				SetResult(requestid, ErrorCode::RECORD_TIMEOUT);
			}
		}

		SendCommands();

		SendCommandNews();
	}
}

bool CLRecordClient::Init(CLApplication* app, const std::vector<Avalon::NetConnectionPtr>& connVec)
{
	if (m_bRun) return true;
	if (app == NULL || connVec.empty()) return false;
	m_pApp = app;

	std::map<UInt32, Avalon::NetConnectionPtr> tempMap;
	for (auto& iter : connVec)
	{
		tempMap[CL_GET_SERVERID_FROM_NODEID(iter->GetID())] = iter;
	}

	m_Connection = tempMap.begin()->second;
	tempMap.erase(tempMap.begin());

	if (tempMap.size() != 0)
	{
		m_ConnectionNum = tempMap.size();
		m_RequestVec.assign(m_ConnectionNum, Avalon::DoubleQueue<RecordRequest, 256>());
		m_ConnectionVec.assign(m_ConnectionNum, NULL);

		UInt32 k = 0;
		for (auto& iter : tempMap)
		{
			m_ConnectionVec[k++] = iter.second;
		}
	}

	m_bRun = true;
	if (!Activate())
	{
		m_bRun = false;
		return false;
	}

	// 只向主Record服务请求seed
	CLProtocol::RecordGuidseedReq req;
	req.nodetype = SERVER_TYPE;
	req.nodeid = SERVER_ID;
	CL_SEND_PROTOCOL(m_Connection, req);

	UInt32 times = 300;
	WAIT_TRUE(m_Seed != INVALID_SEED, times);
	if (times == 0)
	{
		m_bRun = false;
		Join();
		return false;
	}

	return true;
}

void CLRecordClient::SendCommandNews()
{
	if (m_ConnectionNum == 0) return;

	for (UInt32 k = 0; k < m_ConnectionNum; ++k)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		packet->SetSequence(++m_Sequence);

		UInt32 zoneId = 0;
		while ( !m_RequestVec[k].empty())
		{
			RecordRequest req = m_RequestVec[k].pop();
			CLRecordCommand* cmd = req.cmd;
			CLRecordCallback* callback = req.callback;
			UInt8 needTrans = IsTableNeedTrans(cmd->GetTableName());

			UInt32 requestid = 0;
			if (callback != NULL)
			{
				//生成请求id
				if (++m_RequestIdSeed == 0) ++m_RequestIdSeed;  //生成唯一的请求序列号
				requestid = m_RequestIdSeed;

				callback->SetRequestTime(m_Now);
				if (!m_WaitCallbacks.insert(std::make_pair(requestid, callback)).second) //noted by aprilliu, callback对象包含了db请求的上下文信息，是由具体发送请求方生成并填充信息的(比如：InsertRoleCallback)。 实际发送的时候以请求的序列号为key 插入到m_WaitCallbacks，这其实处理异步请求。
				{
					CL_SAFE_DELETE(callback);
					CL_SAFE_DELETE(cmd);
					Avalon::Packet::Destroy(packet);
					ErrorLogStream << "duplicate record requestid(" << requestid << ")!" << LogStream::eos;
					m_pApp->Exit();
					return;
				}
			}

			switch (cmd->GetType())
			{
			case CLProtocol::RECORD_INSERT_REQ:
			case CLProtocol::RECORD_SELECT_REQ:
			case CLProtocol::RECORD_CHANGENAME_REQ:
			case CLProtocol::RECORD_AUTO_GUID_INSERT_REQ:
			{
				if (packet->GetSize() > 0)	//不和其他命令一起发送
				{
					m_ConnectionVec[k]->SendPacket(packet);
					packet->SetSize(0);
				}

				Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
				stream & needTrans & cmd->GetZoneId() & requestid;
				if (!cmd->Encode(stream))
				{
					ErrorLogStream << "encode (insert|select) record cmd failed! table=" <<
						cmd->GetTableName() << "." << LogStream::eos;
					m_pApp->Exit();
					break;
				}
				packet->SetID(cmd->GetType());
				packet->SetSize(UInt16(stream.GetPos()));
				m_ConnectionVec[k]->SendPacket(packet);
				packet->SetSize(0);
			}
			break;
			case CLProtocol::RECORD_DELETE_REQ:
			case CLProtocol::RECORD_UPDATE_REQ:
			case CLProtocol::RECORD_FLUSH_REQ:
			{
				if (packet->GetSize() > 0 &&
					(packet->GetID() != cmd->GetType()
						|| requestid != 0
						|| cmd->GetZoneId() != zoneId
						|| needTrans))	//不能和不同类型的命令一起发，两个有回调的不能一起发，如果是需要转发的，也不能一起发
				{
					if (requestid > 0 && cmd->GetType() == CLProtocol::RECORD_UPDATE_REQ && CL_GET_SERVERTYPE_FROM_NODEID(NODE_ID) == ST_SCENE)
					{
						DebugLogStream << "send packet sequence(" << packet->GetSequence() << ") to record." << LogStream::eos;
					}

					m_ConnectionVec[k]->SendPacket(packet);
					packet->SetSize(0);
					packet->SetSequence(++m_Sequence);
				}

				zoneId = cmd->GetZoneId();
				Avalon::NetOutputStream stream(packet->GetBuffer() + packet->GetSize(), packet->GetMaxSize() - packet->GetSize());
				if (packet->GetSize() == 0)	//写入第一个请求的请求id
				{
					stream & needTrans & zoneId & requestid;
				}

				if (requestid > 0 && cmd->GetType() == CLProtocol::RECORD_UPDATE_REQ && CL_GET_SERVERTYPE_FROM_NODEID(NODE_ID) == ST_SCENE)
				{
					CLUpdateCommand* updateCmd = (CLUpdateCommand*)cmd;
					DebugLogStream << "packet sequence(" << packet->GetSequence() << ") encode cmd(" << cmd->GetType() << ", " << updateCmd->GetKey() << ")." << LogStream::eos;
				}

				if (!cmd->Encode(stream))
				{
					ErrorLogStream << "encode (delete|update) record cmd failed! table="
						<< cmd->GetTableName() << "." << LogStream::eos;
					m_pApp->Exit();
					break;
				}
				packet->SetID(cmd->GetType());
				packet->SetSize(packet->GetSize() + UInt16(stream.GetPos()));

				// 转发包永远单独发送
				if (needTrans || packet->GetSize() > packet->GetMaxSize() / 3) //不超过最大包大小的三分之一
				{
					if (requestid > 0 && cmd->GetType() == CLProtocol::RECORD_UPDATE_REQ && CL_GET_SERVERTYPE_FROM_NODEID(NODE_ID) == ST_SCENE)
					{
						DebugLogStream << "send packet sequence(" << packet->GetSequence() << ") to record." << LogStream::eos;
					}

					m_ConnectionVec[k]->SendPacket(packet);
					packet->SetSize(0);
				}
			}
			break;
			}

			CL_SAFE_DELETE(cmd);
		}

		if (packet->GetSize() > 0)
		{
			m_ConnectionVec[k]->SendPacket(packet);
			packet->SetSize(0);
		}

		Avalon::Packet::Destroy(packet);
	}
}

void CLFlushPlayerTables()
{
	CLFlushCommand* cmd = CLRecordClient::Instance()->PrepareFlushCommand("t_player_info");
	CLRecordClient::Instance()->SendCommand(cmd);

	cmd = CLRecordClient::Instance()->PrepareFlushCommand("t_item");
	CLRecordClient::Instance()->SendCommand(cmd);

	cmd = CLRecordClient::Instance()->PrepareFlushCommand("t_pet");
	CLRecordClient::Instance()->SendCommand(cmd);
}

