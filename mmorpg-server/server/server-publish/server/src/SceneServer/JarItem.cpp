#include "JarItem.h"
#include "CLRecordClient.h"
#include "Player.h"

JarItem::JarItem()
{
	m_itemId = 0; 
	m_jarType = 0;
	m_getNum = 0;
	m_missNum = 0;
	m_minusFixNum = 0;
	m_data = NULL;
	m_dbState = SDS_NONE;
	m_fakeWeight.SetOwner(this);
}

JarItem::~JarItem()
{

}

bool JarItem::HasRestNum()
{
	if (0 == m_data->getNumLim)
	{
		return true;
	}
	else if (m_getNum >= m_data->getNumLim)
	{
		return false;
	}

	return true;
}

void JarItem::SetData(JarItemPoolData* data)
{
	if (!data)
		return;

	m_itemId = data->itemId;
	m_jarType = data->type;
	m_occus = data->strOccus;
	m_data = data;

	m_fakeWeight.SetWeightBase(data->wt);
	m_fakeWeight.SetPlusFix(data->fix1);
	m_fakeWeight.SetMinusFix(data->fix2);
	m_fakeWeight.SetMinusFixNumBase(data->fix2Num);
	m_fakeWeight.SetWeightUpper(data->fixMax);
	m_fakeWeight.SetWeightLower(data->fixMin);
	m_fakeWeight.SetMissNum(m_missNum);
	m_fakeWeight.SetMinusFixNum(m_minusFixNum);
}

bool JarItem::SyncDB(Player* player)
{
	if (!player)
	{
		return false;
	}

	if (m_data->fix1 == 0 &&
		m_data->fix2 == 0 &&
		(m_data->getNumLim == 0 || (m_data->getNumLim > 0 && m_getNum == 0)))
	{
		return false;
	}

	m_missNum = m_fakeWeight.GetMissNum();
	m_minusFixNum = m_fakeWeight.GetMinusFixNum();
	
	if (m_dbState == SDS_INSERT)
	{
		InsertDB(player);
		m_dbState = SDS_UPDATE;
	}
	else if (m_dbState == SDS_UPDATE)
	{
		UpdateDB(player);
	}
	else
	{
		ErrorLogStream << PLAYERINFO(player) << "syncDB state error! state = " << m_dbState << LogStream::eos;
		return false;
	}

	return true;
}

void JarItem::UpdateDB(Player* player)
{
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(player->GetSrcZoneId(), "t_jaritem_pool", GetID(), false);
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd, NULL, player->GetID());
}

void JarItem::InsertDB(Player* player)
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand(player->GetSrcZoneId(), "t_jaritem_pool", GetID());
	GetDBData(cmd, false);
	CLRecordClient::Instance()->SendCommand(cmd, NULL, player->GetID());
}

