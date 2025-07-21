#include "RedPacketReceiver.h"
#include "CLRecordClient.h"
#include "WSPlayer.h"
#include "RedPacket.h"
#include "CLRedPacketDataEntry.h"

RedPacketReceiver::RedPacketReceiver()
{
	m_RedPacketId = 0;
	m_ReceiverId = 0;
	m_Occu = 0;
	m_Level = 0;
	m_Money = 0;
	m_Time = (UInt32)CURRENT_TIME.Sec();
	m_MoneyId = 0;
	m_isBest = false;
}

RedPacketReceiver::~RedPacketReceiver()
{

}

bool RedPacketReceiver::Init(WSPlayer* player, ObjID_t redPacketId, UInt32 money, RedPacket& redPacket)
{
	if (!player)
	{
		return false;
	}

	SetID(CLRecordClient::Instance()->GenGuid());
	m_RedPacketId = redPacketId;
	m_ReceiverId = player->GetID();
	SetName(player->GetName());
	m_Occu = player->GetOccu();
	m_Level = player->GetLevel();
	m_Money = money;
	m_Time = (UInt32)CURRENT_TIME.Sec();
	m_RedPacketOwnerName = redPacket.GetOwnerName();
	m_reciver_accid = player->GetAccId();
	
	auto dataEntry = RedPacketDataEntryMgr::Instance()->FindEntry(redPacket.GetReason());
	if (dataEntry)
	{
		m_MoneyId = dataEntry->getMoneyId;
	}
	return true;
}

RedPacketReceiverEntry RedPacketReceiver::GetEntry() const
{
	RedPacketReceiverEntry entry;
	entry.icon.id = m_ReceiverId;
	entry.icon.name = GetName();
	entry.icon.occu = m_Occu;
	entry.icon.level = m_Level;
	entry.money = m_Money;

	return entry;
}

void RedPacketReceiver::InsertToDB()
{
	CLInsertCommand* insertCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_red_packet_receiver", GetID());
	insertCmd->PutData("name", GetName());
	GetDBData(insertCmd, false);
	CLRecordClient::Instance()->SendCommand(insertCmd);

	ClearDirty();
}

void RedPacketReceiver::DelFromDB()
{
	CLDeleteCommand* deleteCmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_red_packet_receiver", GetID());
	CLRecordClient::Instance()->SendCommand(deleteCmd);
}

void RedPacketReceiver::Update()
{
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_red_packet_receiver", GetID(), false);
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}

RedPacketRecord RedPacketReceiver::ToClient() const
{
	RedPacketRecord record;
	record.guid = GetID();
	record.time = m_Time;
	record.redPackOwnerName = m_RedPacketOwnerName;
	record.moneyId = m_MoneyId;
	record.moneyNum = m_Money;
	record.isBest = m_isBest;

	return record;
}

void RedPacketReceiver::SetBset()
{
	m_isBest = 1;
	Update();
}