#include "ArtifactJarMgr.h"
#include "GASPlayer.h"
#include "GASSelectInitDataCallBack.h"
#include "Utility.h"
#include "GASGameZoneMgr.h"

#include <CLActivityDefine.h>
#include <CLItemProtocol.h>
#include <CLItemDataEntry.h>
#include <CLRecordClient.h>
#include <CLSysNotify.h>
#include <udplog_typedef.h>
#include <CLArtifactJarLotteryDataEntry.h>

// ���ݼ��ز���
#define DB_LOAD_STEP			(3)

void PlayerArtifactJarLottery::InsertToDB()
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_aritfact_jar_lottery", CLRecordClient::Instance()->GenGuid());
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}

struct SelectLastLotteryTimeCallBack : public SelectInitDataCallback
{
	SelectLastLotteryTimeCallBack() {}

	bool OnLoadFinished()
	{
		return ArtifactJarMgr::Instance()->LoadLastLotteryTime(this);
	}
};

struct SelectAritfactJarLotteryCallBack : public SelectInitDataCallback
{
	SelectAritfactJarLotteryCallBack() {}

	bool OnLoadFinished()
	{
		return ArtifactJarMgr::Instance()->LoadArtifactJarLottery(this);
	}
};

struct SelectAritfactJarLotteryRecordCallBack : public SelectInitDataCallback
{
	SelectAritfactJarLotteryRecordCallBack() {}

	bool OnLoadFinished()
	{
		return ArtifactJarMgr::Instance()->LoadArtifactJarLotteryRecord(this);
	}
};

ArtifactJarMgr::ArtifactJarMgr()
{
	m_ActivityState = 0;
	m_LastLotteryTime = 0;
	m_LoadStep = 0;
	m_IsCheckRecv = false;
	m_IsDayChange = false;
}

ArtifactJarMgr::~ArtifactJarMgr()
{

}

void ArtifactJarMgr::OnInit()
{
	// �������齱ʱ��
	CLSelectCommand* cmd_1 = CLRecordClient::Instance()->PrepareSelectCommand("t_artifact_jar_act");
	CLRecordClient::Instance()->SendCommand(cmd_1, new SelectLastLotteryTimeCallBack());

	// ���س齱�ʸ�
	CLSelectCommand* cmd_2 = CLRecordClient::Instance()->PrepareSelectCommand("t_aritfact_jar_lottery");
	CLRecordClient::Instance()->SendCommand(cmd_2, new SelectAritfactJarLotteryCallBack());

	// ���س齱��¼
	CLSelectCommand* cmd_3 = CLRecordClient::Instance()->PrepareSelectCommand("t_aritfact_jar_lottery_record");
	CLRecordClient::Instance()->SendCommand(cmd_3, new SelectAritfactJarLotteryRecordCallBack());
}

void ArtifactJarMgr::OnTick(const Avalon::Time& now)
{
	if (m_ActivityState == AS_END)
		return;

	// �ȴ����ݼ������
	if (m_LoadStep != DB_LOAD_STEP)
		return;

	// ��������Ƿ���ɳ齱
	if (!m_IsCheckRecv)
	{
		m_IsCheckRecv = true;
		Avalon::Time LastTime(m_LastLotteryTime);
		if (!IsSameDay(now, LastTime))
		{
			LotteryToRecord();
		}
	}

	if (!m_IsDayChange)
	{
		if (CURRENT_DATE.Hour() == 0 && CURRENT_DATE.Min() == 0 && CURRENT_DATE.Sec() < 3)
		{
			m_IsDayChange = true;

			// �齱�����ɳ齱��¼
			LotteryToRecord();
		}
	}
	else
	{
		if (CURRENT_DATE.Hour() == 0 && CURRENT_DATE.Min() > 0)
		{
			m_IsDayChange = false;
		}
	}
}

void ArtifactJarMgr::HandleGASArtifactJarLotterySignReq(GASPlayer* player, const std::vector<UInt32>& jarIdVec)
{
	if (m_ActivityState == AS_END || NULL == player)
		return;

	if (player->GetID() == 0 || jarIdVec.empty())
		return;
	
	for (auto& id : jarIdVec)
	{
		PlayerArtifactJarLottery jarLottery;
		jarLottery.jarId = id;
		jarLottery.playerId = player->GetUId();
		jarLottery.playerName = player->GetPlayerName();
		jarLottery.serverName = player->GetServerName();
		jarLottery.adminServerId = player->GetSourceNode(ST_ADMIN);
		jarLottery.centerServerId = player->GetSourceNode(ST_CENTER);

		AddPlayerArtifactJarLottery(jarLottery, true);
	}
}

void ArtifactJarMgr::HandleGASArtifactJarLotteryRecordReq(GASPlayer* player, UInt32 jarId)
{
	if (NULL == player) return;

	CLProtocol::GASArtifactJarLotteryRecordRes res;
	res.jarId = jarId;

	auto iter = m_LotteryRecordMap.find(jarId);
	if (iter != m_LotteryRecordMap.end())
	{
		res.lotteryRecordVec.insert(res.lotteryRecordVec.end(), iter->second.begin(), iter->second.end());
	}

	player->SendProtocol(res);
}

void ArtifactJarMgr::HandleGASArtifactJarLotteryCfgReq(GASPlayer* player)
{
	if (NULL == player) return;
	
	CLProtocol::GASArtifactJarLotteryCfgRes res;
	ArtifactJarLotteryDataEntryMgr::Instance()->GetArtifactJarLotteryCfg(res.artifactJarCfgVec);
	player->SendProtocol(res);
}

bool ArtifactJarMgr::LoadLastLotteryTime(CLRecordCallback* callback)
{
	if (!callback) return false;

	while (callback->NextRow())
	{
		m_LastLotteryTime = callback->GetData("last_lottery_time");
	}

	m_LoadStep++;

	return true;
}

bool ArtifactJarMgr::LoadArtifactJarLottery(CLRecordCallback* callback)
{
	if (!callback) return false;

	while (callback->NextRow())
	{
		PlayerArtifactJarLottery jarLottery;
		jarLottery.SetDBData(callback);
		jarLottery.SetID(UInt32(callback->GetKey()));

		AddPlayerArtifactJarLottery(jarLottery);
	}

	m_LoadStep++;

	return true;
}

bool ArtifactJarMgr::LoadArtifactJarLotteryRecord(CLRecordCallback* callback)
{
	if (!callback) return false;

	while (callback->NextRow())
	{
		UInt32 jarId = callback->GetData("jar_id");
		std::string playerName = callback->GetData("player_name").GetString();
		std::string serverName = callback->GetData("server_name").GetString();
		UInt64 recordTime = callback->GetData("record_time");
		UInt32 itemId = callback->GetData("item_id");

		AddArtifactJarLotteryRecord(jarId, playerName, serverName, recordTime, itemId);
	}

	m_LoadStep++;

	return true;
}

void ArtifactJarMgr::AddPlayerArtifactJarLottery(PlayerArtifactJarLottery& jarLottery, bool isSave)
{
	auto iter = m_LotteryMap.find(jarLottery.jarId);
	if (iter == m_LotteryMap.end())
	{
		PlayerArtifactJarLotteryMap lotteryMap;
		lotteryMap[jarLottery.playerId] = jarLottery;

		m_LotteryMap[jarLottery.jarId] = lotteryMap;

		if (isSave)
			jarLottery.InsertToDB();
	}
	else
	{
		auto it = iter->second.find(jarLottery.playerId);
		if (it != iter->second.end())
			return;

		iter->second[jarLottery.playerId] = jarLottery;

		if (isSave)
			jarLottery.InsertToDB();
	}
}

void ArtifactJarMgr::AddArtifactJarLotteryRecord(UInt32 jarId, const std::string& playerName, const std::string& serverName, UInt64 recordTime, UInt32 itemId, bool isSave)
{
	CLProtocol::ArtifactJarLotteryRecord lotteryRecord;
	lotteryRecord.playerName = playerName;
	lotteryRecord.serverName = serverName;
	lotteryRecord.recordTime = recordTime;
	lotteryRecord.itemId = itemId;

	auto iter = m_LotteryRecordMap.find(jarId);
	if (iter == m_LotteryRecordMap.end())
	{
		ArtifactJarLotteryRecordVec recordVec;
		recordVec.push_back(lotteryRecord);

		m_LotteryRecordMap[jarId] = recordVec;
	}
	else
	{
		iter->second.push_back(lotteryRecord);
	}

	if (isSave)
	{
		CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_aritfact_jar_lottery_record", CLRecordClient::Instance()->GenGuid());
		cmd->PutData("jar_id", jarId);
		cmd->PutData("server_name", serverName);
		cmd->PutData("player_name", playerName);
		cmd->PutData("item_id", itemId);
		cmd->PutData("record_time", recordTime);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}

void ArtifactJarMgr::SetActivityStatus(UInt32 state)
{
	m_ActivityState = state;

	// ��ֹ��12���ʱ��ͻȻ�رգ���û���ü��齱
	if (m_ActivityState == AS_END)
	{
		LotteryToRecord();
	}
}

void ArtifactJarMgr::ClearDBTable(const std::string& _tableName)
{
	std::ostringstream cond;
	cond << "1 = 1";
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand(_tableName, cond.str());
	CLRecordClient::Instance()->SendCommand(cmd);
}

void ArtifactJarMgr::LotteryToRecord()
{
	if (m_LastLotteryTime == 0)
	{
		m_LastLotteryTime = CURRENT_TIME.MSec();
		CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_artifact_jar_act", CLRecordClient::Instance()->GenGuid());
		cmd->PutData("last_lottery_time", m_LastLotteryTime);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
	else
	{
		m_LastLotteryTime = CURRENT_TIME.MSec();
		std::ostringstream cond;
		cond << "1=1";
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_artifact_jar_act", cond.str());
		cmd->PutData("last_lottery_time", m_LastLotteryTime);
		CLRecordClient::Instance()->SendCommand(cmd);
	}

	// ���ǰһ��ĳ齱��¼
	m_LotteryRecordMap.clear();
	ClearDBTable("t_aritfact_jar_lottery_record");

	// ֪ͨ�ͻ��˳齱��
	CLProtocol::GASArtifactJarLotteryNotify notify;
	GASGameZoneMgr::Instance()->BroadcastToGate(notify);

	if (m_LotteryMap.empty())
	{
		InfoLogStream << "Artifact Jar Lottery Empty!" << LogStream::eos;
		return;
	}

	InfoLogStream <<"Start Artifact Jar Lottery!!!"<< LogStream::eos;

	std::string sender = SysNotifyMgr::Instance()->MakeString(9973);
	std::string title = SysNotifyMgr::Instance()->MakeString(9964);
	std::string content = SysNotifyMgr::Instance()->MakeString(9965);

	for (auto& iter : m_LotteryMap)
	{
		std::vector<std::pair<std::string, PlayerArtifactJarLottery>> temp_vec(iter.second.begin(), iter.second.end());

		JarBonusDataEntry* jarData = JarBonusDataMgr::Instance()->FindEntry(iter.first);
		if (NULL == jarData)
		{
			ErrorLogStream << "Not Find Jar Bonus Config JarId:" << iter.first << LogStream::eos;
			continue;
		}

		// �ҵ�����
		ArtifactJarLotteryPool lotteryPool = ArtifactJarLotteryDataEntryMgr::Instance()->GetArtifactJarRewardPool(iter.first);
		if (lotteryPool.empty())
		{
			ErrorLogStream << "Artifact Jar:" << iter.first << " lotteryPool Empty!" << LogStream::eos;
			continue;
		}

		// ����齱�������������õ��������������
		UInt32 num = temp_vec.size();
		if (num > jarData->actifactJarRewardNum)
		{
			std::random_shuffle(temp_vec.begin(), temp_vec.end());
			num = jarData->actifactJarRewardNum;
		}
		
		InfoLogStream << "Artifact Jar:"<<iter.first<<" Lottery:"<<num<<" Player Obtain Reward!!!" << LogStream::eos;

		std::string reason = GetReason(SOURCE_TYPE_ARTIFACT_JAR_LOTTERY, CURRENT_TIME.Sec(), iter.first);
		std::map<UInt32, UInt32> limitItemMap;
		ArtifactJarLotteryPool lotteryCfgMap;
		UInt32 totalWeight = 0;
		bool isResetLotteryPool = true;

		for (UInt32 i = 0; i < num; ++i)
		{
			PlayerArtifactJarLottery& jarLottery = temp_vec[i].second;
			ArtifactJarLotteryDataEntry* rewardCfg = NULL;
		
			// ���ý��أ����н���ҳ齱
			if (isResetLotteryPool)
			{
				totalWeight = 0;
				lotteryCfgMap.clear();
				for (auto& vec : lotteryPool)
				{
					totalWeight += vec.second->weight;
					lotteryCfgMap[totalWeight] = vec.second;
				}
			}

			UInt32 randNum = Avalon::Random::RandBetween(0, totalWeight);
			auto it = lotteryCfgMap.lower_bound(randNum);
			if (it != lotteryCfgMap.end())
			{
				rewardCfg = it->second;
			}
			else
			{
				ErrorLogStream << "Artifact Jar:" << iter.first << " Rand Lottery Error!" << LogStream::eos;
				continue;
			}

			// ����������ȡ������������߳��꣬�ӽ���ɾ��
			if (rewardCfg->limitNum != 0)
			{
				limitItemMap[rewardCfg->rewardItemId]++;

				if (limitItemMap[rewardCfg->rewardItemId] >= rewardCfg->limitNum)
				{
					lotteryPool.erase(rewardCfg->id);
					isResetLotteryPool = true;
				}
			}
			else
			{
				isResetLotteryPool = false;
			}

			// �ʼ����Ž���
			ItemReward reward;
			reward.id = rewardCfg->rewardItemId;
			reward.num = rewardCfg->rewardItemNum;
			std::vector<ItemReward> rewards = { reward };

			SysUtility::SendMail(jarLottery.centerServerId, jarLottery.adminServerId, jarLottery.playerId, sender, title, content, reason, rewards);

			// ���ɳ齱��¼
			AddArtifactJarLotteryRecord(jarLottery.jarId, jarLottery.playerName,jarLottery.serverName, CURRENT_TIME.Sec(), reward.id, true);

			InfoLogStream << "Artifact Jar Lottery Record JarId:" << jarLottery.jarId << " playerId:" << jarLottery.playerId << " playerName:" << jarLottery.playerName << " serverName:" << jarLottery.serverName << " rewardId:" << reward.id << LogStream::eos;
		}
	}

	InfoLogStream << "End Artifact Jar Lottery!!!" << LogStream::eos;

	// ���ǰһ��ĳ齱�ʸ�
	m_LotteryMap.clear();
	ClearDBTable("t_aritfact_jar_lottery");
}
