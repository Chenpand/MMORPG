#include "BetHorseInfo.h"
#include <CLBetHorseDefine.h>
#include <CLRecordClient.h>
#include "CLBetHorseDataEntry.h"

HorseYard::HorseYard()
{
	m_id = 0;
	m_terrain = 0;
	m_winShooter = 0;
	m_shooter_1 = 0;
	m_shooter_2 = 0;
}

void HorseYard::UpdateToDB()
{
	std::ostringstream cond;
	cond << "map_id=" << m_id;
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_bet_horse_map", cond.str());
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void HorseYard::InsertToDB()
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_bet_horse_map", CLRecordClient::Instance()->GenGuid());
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}

//-------------------------------------------

ShooterData::ShooterData()
{
	m_id = 0;
	m_stakeNum = 0;
	m_status = 0;

	// 获得赌马的配置
	BetHorseCfgEntry* cfg = BetHorseCfgEntryMgr::Instance()->FindEntry(1);
	if (NULL == cfg)
	{
		ErrorLogStream << "not find bet horse config !!!" << LogStream::eos;
	}

	// 初始赔率随机一个
	m_odds = Avalon::Random::RandBetween(cfg->initOddsMin, cfg->initOddsMax) * RATE_EXPAND_NUM;
	m_cfg = NULL;
}

ShooterData::~ShooterData()
{

}

void ShooterData::UpdateOdds(UInt32 _totalStake)
{
	if (m_stakeNum == 0) 
		return;

	m_odds = UInt32(_totalStake * RATE_EXPAND_NUM / m_stakeNum);

	// 确保保留一位小数
	m_odds = m_odds / 1000;
	m_odds = m_odds * 1000;
} 

void ShooterData::UpdateToDB()
{
	std::ostringstream cond;
	cond << "shooter_id=" << m_id;
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_bet_horse_shooter", cond.str());
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void ShooterData::InsertToDB()
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_bet_horse_shooter", CLRecordClient::Instance()->GenGuid());
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}

