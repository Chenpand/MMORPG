#include "ChallengeMgr.h"

#include <CLSceneProtocol.h>
#include <CLSceneDefine.h>
#include <CLSceneDataEntry.h>

#include "CSApplication.h"
#include "GameParamMgr.h"
#include "ChallengeScene.h"
#include "CSNetwork.h"

#define SEASON_TIME_VAR	"season_time"
#define SEASON_END_VAR "season_end"

ChallengeMgr::ChallengeMgr()
{
	m_RoundTime = 0;
	m_RoundStatus = 0;
	m_SceneServer = 0;

	m_CreateSceneTimer.SetLastTime(CURRENT_TIME.MSec());
	m_CreateSceneTimer.SetInterval(10000);
}

ChallengeMgr::~ChallengeMgr()
{
}

struct ChallengerSorter
{
	bool operator()(const Challenger* challenger1, const Challenger* challenger2) const
	{
		return challenger1->GetScore() > challenger2->GetScore();
	}
};

bool ChallengeMgr::Init()
{
	/*Avalon::RetryDBConnection* dbconn = CSApplication::Instance()->GetDBConnection();

	Avalon::ResultSet* resultSet = dbconn->Query("SELECT * FROM `t_worldchallenger`");
	if(resultSet == NULL) return false;
	while(resultSet->NextRow())
	{
		Challenger* challenger = new Challenger();
		challenger->SetID(resultSet->GetField("guid").GetUInt64());
		challenger->SetName(resultSet->GetField("name").GetString());
		challenger->SetLevel(resultSet->GetField("level").GetUInt16());
		challenger->SetOccu(resultSet->GetField("occu").GetUInt8());
		challenger->SetScore(resultSet->GetField("score").GetUInt32());
		challenger->SetZoneId(resultSet->GetField("zoneId").GetUInt32());
		challenger->SetPower(resultSet->GetField("power").GetUInt32());
		challenger->SetGroup(resultSet->GetField("group").GetUInt8());
		challenger->SetAwardFlag(resultSet->GetField("awardFlag").GetUInt32());

		if(!m_Challengers.AddObject(challenger))
		{
			CL_SAFE_DELETE(challenger);
		}

		m_SortList[challenger->GetGroup()].push_back(challenger);
	}
	dbconn->FreeResultSet(resultSet);

	resultSet = dbconn->Query("SELECT * FROM `t_worldchallenger_ranking`");
	if(resultSet == NULL) return false;
	while(resultSet->NextRow())
	{
		ObjID_t id = resultSet->GetField("guid").GetUInt64();
		UInt32 ranking = resultSet->GetField("ranking").GetUInt32();
		m_Rankings.insert(std::make_pair(id, ranking));
	}
	dbconn->FreeResultSet(resultSet);

	ChallengerSorter sorter;
	for(std::map<UInt8, ChallengerVec>::iterator iter = m_SortList.begin();
		iter != m_SortList.end(); ++iter)
	{
		std::sort(iter->second.begin(), iter->second.end(), sorter);
		if(iter->second.size() > WORLD_CHALLENGE_SORTLIST_NUM)
		{
			iter->second.erase(iter->second.begin() + WORLD_CHALLENGE_SORTLIST_NUM, iter->second.end());
		}
	}

	Avalon::Time now = Avalon::Time::CurrentTime();
	InitRoundTime(now);

	UInt32 dayBeginTime = UInt32(GetDayBeginTime(now) / 1000);
	Avalon::Date date(now);
	UInt32 seasonTime = GameParamMgr::Instance()->GetValue(SEASON_TIME_VAR);
	if(seasonTime != dayBeginTime - (date.WDay() - 1) * DAY_TO_SEC)
	{
		GameParamMgr::Instance()->SetValue(SEASON_TIME_VAR, dayBeginTime - (date.WDay() - 1) * DAY_TO_SEC);
		GameParamMgr::Instance()->SetValue(SEASON_END_VAR, 0);
		OnSeasonBegin();
	}*/

	return true;
}

void ChallengeMgr::Final()
{
	m_SigninList.clear();
	m_Pairs.clear();
	m_SortList.clear();

	CLDeleteVisitor<Challenger>	deleteVisitor;
	m_Challengers.VisitObjects(deleteVisitor);
	m_Challengers.Clear();

	for(SceneVec::iterator iter = m_Scenes.begin();
		iter != m_Scenes.end(); ++iter)
	{
		CL_SAFE_DELETE(*iter);
	}
	m_Scenes.clear();

	m_SceneServers.clear();
}

ChallengeScene* ChallengeMgr::FindChallengeScene(UInt32 sceneId)
{
	for (auto scene : m_Scenes)
	{
		if (scene->GetSceneID() == sceneId)
		{
			return scene;
		}
	}

	return NULL;
}

void ChallengeMgr::Signin(GameZone* zone, ObjID_t id, const std::string& name, UInt32 gateId, UInt16 zoneId, UInt16 level, UInt8 occu, UInt32 power, const UInt8* pkdata, size_t pkdatalen)
{
	/*
	CLProtocol::ChallengeWorldChallengeSigninRet ret;
	ret.id = id;
	ret.result = 0;

	if(CURRENT_TIME.Sec() < m_RoundTime || m_RoundStatus != 0) //非报名时间
	{
		ret.result = 1;
		zone->TransmitToWorld(ret);
		return;
	}

	for(std::vector<ObjID_t>::const_iterator iter = m_SigninList.begin();
		iter != m_SigninList.end(); ++iter)
	{
		if(*iter == id)	//已报名
		{
			ret.result = 0;
			zone->TransmitToWorld(ret);
			return;
		}
	}

	Challenger* challenger = m_Challengers.FindObject(id);
	if(challenger != NULL)
	{
		challenger->SetGateId(gateId);
		bool bDirty = false;
		if(challenger->GetLevel() != level)
		{
			challenger->SetLevel(level);
			bDirty = true;
		}
		if(challenger->GetPower() < power)
		{
			challenger->SetPower(power);
			bDirty = true;
		}
		if(challenger->GetZoneId() != zoneId)
		{
			challenger->SetZoneId(zoneId);
			bDirty = true;
		}
		if(challenger->GetName().compare(name) != 0)
		{
			challenger->SetName(name);
			bDirty = true;
		}
		challenger->SetPkData(pkdata, pkdatalen);
		if(bDirty) challenger->SaveMe();
	}
	else
	{
		UInt8 group = ScriptMgr::Instance()->GetChallengeScript()->GetGroupByLevel(level);
		if(group == 0)	//等级没达到要求
		{
			ret.result = 2;
			zone->TransmitToWorld(ret);
			return;
		}

		challenger = new Challenger();
		challenger->SetGateId(gateId);
		challenger->SetID(id);
		challenger->SetName(name);
		challenger->SetZoneId(zoneId);
		challenger->SetLevel(level);
		challenger->SetOccu(occu);
		challenger->SetPower(power);
		challenger->SetGroup(group);
		challenger->SetPkData(pkdata, pkdatalen);
		challenger->InsertToDB();

		m_Challengers.AddObject(challenger);
		if(m_SortList[challenger->GetGroup()].size() < WORLD_CHALLENGE_SORTLIST_NUM)
		{
			m_SortList[challenger->GetGroup()].push_back(challenger);
		}
	}

	m_SigninList.push_back(id);
	zone->TransmitToWorld(ret);
	*/
}

void ChallengeMgr::Signout(GameZone* zone, ObjID_t id)
{
	for(std::vector<ObjID_t>::iterator iter = m_SigninList.begin();
		iter != m_SigninList.end(); ++iter)
	{
		if(*iter == id)
		{
			m_SigninList.erase(iter);
			return;
		}
	}
}

void ChallengeMgr::SendInfo(GameZone* zone, ObjID_t id, UInt16 level)
{
	/*
	CLProtocol::ChallengeWorldChallengeInfoRet ret;
	ret.playerId = id;
	ret.group = 0;
	ret.ranking = 0;
	ret.score = 0;
	ret.awardFlag = 0;
	Challenger* challenger = m_Challengers.FindObject(id);
	if(challenger != NULL)
	{
		ret.group = challenger->GetGroup();
		ret.score = challenger->GetScore();
		ret.ranking = GetRanking(id);
		ret.awardFlag = challenger->GetAwardFlag();
		if(m_Rankings.find(id) != m_Rankings.end()) ret.awardFlag |= (1 << 0);
	}
	else
	{
		ret.group = ScriptMgr::Instance()->GetChallengeScript()->GetGroupByLevel(level);
	}
	ret.seasonTime = GameParamMgr::Instance()->GetValue(SEASON_TIME_VAR);
	ret.beginTime = ScriptMgr::Instance()->GetChallengeScript()->GetBeginTime();
	zone->TransmitToWorld(ret);
	*/
}

void ChallengeMgr::SendSortList(GameZone* zone, ObjID_t playerId, UInt8 group, UInt16 start, UInt16 num)
{
	/*
	const ChallengeGroup* pGroup  = ScriptMgr::Instance()->GetChallengeScript()->GetGroup(group);
	if(pGroup == NULL) return;

	CLProtocol::ChallengeWorldChallengeSortListRet ret;
	ret.playerId = playerId;
	ret.group = group;
	ret.start = start;
	UInt16 total = 0;
	ChallengerVec& sortlist = m_SortList[group];
	for(ChallengerVec::iterator iter = sortlist.begin();
		iter != sortlist.end(); ++iter)
	{
		Challenger* challenger = *iter;
		
		++total;
		if(total > start && total <= start + num)
		{
			CLProtocol::WorldChallengeSortListEntry entry;
			entry.id = challenger->GetID();
			entry.name = challenger->GetName();
			entry.occu = challenger->GetOccu();
			entry.level = challenger->GetLevel();
			entry.zoneId = challenger->GetZoneId();
			entry.power = challenger->GetPower();
			entry.score = challenger->GetScore();
			ret.entries.push_back(entry);
		}
	}
	ret.total = total;
	zone->TransmitToWorld(ret);
	*/
}

void ChallengeMgr::GetAward(GameZone* zone, ObjID_t playerId, UInt8 type, UInt8 index, UInt32 minVal)
{
	/*
	CLProtocol::ChallengeWorldChallengeGetAwardRet ret;
	ret.playerId = playerId;
	ret.type = type;
	ret.index = index;

	if(type == 1)
	{
		Challenger* challenger = m_Challengers.FindObject(playerId);
		if(challenger == NULL) return;
		if(challenger->TestAwardFlag(1 << index)) return;
		if(challenger->GetScore() < minVal) return;

		challenger->SetAwardFlag(1 << index);
		challenger->SaveMe();
		ret.awardFlag = challenger->GetAwardFlag();
		if(m_Rankings.find(playerId) != m_Rankings.end()) ret.awardFlag |= (1 << 0);

		InfoLogStream << "player(" << challenger->GetZoneId() << "," << challenger->GetID() << "," << challenger->GetName() 
			<< ") get award with score " << challenger->GetScore() << LogStream::eos;
	}
	else
	{
		std::map<ObjID_t, UInt32>::iterator iter = m_Rankings.find(playerId);
		if(iter == m_Rankings.end()) return;

		ret.ranking = iter->second;
		m_Rankings.erase(iter);

		Avalon::RetryDBConnection* dbconn = CSApplication::Instance()->GetDBConnection();
		std::ostringstream oss;
		oss << "DELETE FROM `t_worldchallenger_ranking` WHERE `guid`=" << playerId;
		if(dbconn->Update(oss.str()) < 0)
		{
			ErrorLogStream << dbconn->GetError() << LogStream::eos;
		}
		Challenger* challenger = m_Challengers.FindObject(playerId);
		if(challenger != NULL)
		{
			ret.awardFlag = challenger->GetAwardFlag();
		}

		InfoLogStream << "player(0," << playerId << ",) get award with ranking " << ret.ranking << LogStream::eos;
	}
	zone->TransmitToWorld(ret);
	*/
}

void ChallengeMgr::SetChallengeResult(ObjID_t playerId, UInt8 result, UInt32 score)
{
	/*
	Challenger* challenger = m_Challengers.FindObject(playerId);
	if(challenger == NULL) return;

	challenger->SetScore(challenger->GetScore() + score);
	challenger->SaveMe();

	UpdateSortList(challenger);

	GameZone* zone = GameZoneMgr::Instance()->FindGameZoneByID(challenger->GetZoneId());
	if(zone != NULL)
	{
		CLProtocol::ChallengeWorldChallengeSyncResult sync;
		sync.playerId = playerId;
		sync.result = result;
		zone->TransmitToWorld(sync);
	}
	*/
}

void ChallengeMgr::OnTick(const Avalon::Time& now)
{
	
}

void ChallengeMgr::BeginBattle(ChallengeScene* scene, Challenger* challenger1, Challenger* challenger2)
{
	/*
	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::SCENE_WORLDCHALLENGE_SEND_CHALLENGERS);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	try
	{
		stream & scene->GetSceneID();

		stream & challenger1->GetID() & challenger1->GetName() & challenger1->GetGateId() & challenger1->GetZoneId();
		stream.SeriaArray(challenger1->GetPkData(), challenger1->GetPkDataLen());

		stream & challenger2->GetID() & challenger2->GetName() & challenger2->GetGateId() & challenger2->GetZoneId();
		stream.SeriaArray(challenger2->GetPkData(), challenger2->GetPkDataLen());
	}
	catch (Avalon::NetException&)
	{
		ErrorLogStream << "send challengers to scene failed!" << LogStream::eos;
		Avalon::Packet::Destroy(packet);
		return;
	}
	packet->SetSize(stream.GetPos());
	scene->SendToScene(packet);
	Avalon::Packet::Destroy(packet);

	GameZone* zone1 = GameZoneMgr::Instance()->FindGameZoneByID(challenger1->GetZoneId());
	if(zone1 != NULL)
	{
		CLProtocol::ChallengeWorldChallengeNotifyBattle notify;
		notify.playerId = challenger1->GetID();
		zone1->TransmitToWorld(notify);
	}

	GameZone* zone2 = GameZoneMgr::Instance()->FindGameZoneByID(challenger2->GetZoneId());
	if(zone2 != NULL)
	{
		CLProtocol::ChallengeWorldChallengeNotifyBattle notify;
		notify.playerId = challenger2->GetID();
		zone2->TransmitToWorld(notify);
	}

	InfoLogStream << "player(" << challenger1->GetZoneId() << "," << challenger1->GetID() << "," << challenger1->GetName() << ") PK player("
		<< challenger2->GetZoneId() << "," << challenger2->GetID() << "," << challenger2->GetName() << ")." << LogStream::eos;

	*/
}

void ChallengeMgr::OnSceneCreated(UInt32 nodeId, UInt32 sceneId)
{
	ChallengeScene* scene = new ChallengeScene();
	if(!scene->Init(nodeId, sceneId))
	{
		CL_SAFE_DELETE(scene);
		return;
	}
	m_Scenes.push_back(scene);
}

void ChallengeMgr::OnSceneDestroyed(UInt32 sceneId)
{
	for(SceneVec::iterator iter = m_Scenes.begin();
		iter != m_Scenes.end(); ++iter)
	{
		ChallengeScene* scene = *iter;
		if(scene->GetSceneID() == sceneId)
		{
			CL_SAFE_DELETE(scene);
			m_Scenes.erase(iter);
			return;
		}
	}
}

void ChallengeMgr::CreateScene()
{
	UInt32 nodeId = SelectSceneServer();
	if(nodeId == 0) return;
	Avalon::NetConnectionPtr conn = CSNetwork::Instance()->FindConnection(nodeId);
	if(conn == NULL) return;

	CLProtocol::SceneNotifyLoadBattleScene notify;
	notify.mapid = WORLD_CHALLENGE_SCENEID;
	notify.battleSceneType = BATTLESCENE_WORLDCHALLENGE;
	notify.beginTime = 0;
	notify.lasttime = HOUR_TO_SEC;
	notify.leaveTimeout = 10;
	memset(notify.uParams, 0, sizeof(notify.uParams));
	CL_SEND_PROTOCOL(conn, notify);
}

void ChallengeMgr::OnSceneServerConnected(UInt32 nodeId)
{
	for(SceneServerVec::iterator iter = m_SceneServers.begin();
		iter != m_SceneServers.end(); ++iter)
	{
		if(*iter == nodeId) return;
	}
	m_SceneServers.push_back(nodeId);


	class GetCrossScenesVisitor : public SceneDataEntryVisitor
	{
	public:
		GetCrossScenesVisitor(std::vector<UInt32>& maps)
		:m_Maps(maps){}
		~GetCrossScenesVisitor(){}

		bool operator()(SceneDataEntry* dataEntry)
		{
			if (dataEntry->subType == SceneSubType::SCENE_SUB_CROSS_GUILD_BATTLE 
				|| dataEntry->subType == SceneSubType::SCENE_SUB_SCORE_WAR
				|| dataEntry->subType == SceneSubType::SCENE_SUB_ROOM)
			{
				m_Maps.push_back(dataEntry->id);
			}
			return true;
		}

	private:
		std::vector<UInt32>& m_Maps;
	};

	CLProtocol::SceneNotifyLoadmap protocol;
	protocol.idSeed = 1;
	protocol.maps.push_back(10000);

	GetCrossScenesVisitor visitor(protocol.maps);
	SceneDataEntryMgr::Instance()->Visit(visitor);
	
	auto conn = CSNetwork::Instance()->FindConnection(nodeId);
	if (conn != NULL)
	{
		CL_SEND_PROTOCOL(conn, protocol);
	}
	
}

void ChallengeMgr::OnSceneServerDisconnected(UInt32 nodeId)
{
	for (SceneVec::iterator iter = m_Scenes.begin();
		iter != m_Scenes.end();)
	{
		ChallengeScene* scene = *iter;
		if (scene->GetNodeId() == nodeId)
		{
			iter = m_Scenes.erase(iter);
			CL_SAFE_DELETE(scene);
		}
		else
		{
			++iter;
		}
	}

	for(SceneServerVec::iterator iter = m_SceneServers.begin();
		iter != m_SceneServers.end(); ++iter)
	{
		if(*iter == nodeId)
		{
			m_SceneServers.erase(iter);
			return;
		}
	}
}

UInt32 ChallengeMgr::SelectSceneServer()
{
	UInt32 ret = 0;
	for(SceneServerVec::const_iterator iter = m_SceneServers.begin();
		iter != m_SceneServers.end(); ++iter)
	{
		if(*iter == m_SceneServer)
		{
			if(++iter != m_SceneServers.end())
			{
				ret = *iter;
			}
			break;
		}
	}
	if(ret == 0 && !m_SceneServers.empty())
	{
		ret = *(m_SceneServers.begin());
	}
	m_SceneServer = ret;
	return ret;
}

void ChallengeMgr::OnSeasonBegin()
{
	CLDeleteVisitor<Challenger>	deleteVisitor;
	m_Challengers.VisitObjects(deleteVisitor);
	m_Challengers.Clear();

	m_SigninList.clear();
	m_SortList.clear();
	m_Pairs.clear();

	Avalon::RetryDBConnection* conn = CSApplication::Instance()->GetDBConnection();
	if(conn->Update("DELETE FROM `t_worldchallenger`") < 0)
	{
		ErrorLogStream << "delete challengers:" << conn->GetError() << LogStream::eos;
	}
}

void ChallengeMgr::OnSeasonEnd()
{
	m_Rankings.clear();
	Avalon::RetryDBConnection* dbconn = CSApplication::Instance()->GetDBConnection();
	if(dbconn->Update("DELETE FROM `t_worldchallenger_ranking`") < 0)
	{
		ErrorLogStream << "delete worldchallenger ranking:" << dbconn->GetError() << LogStream::eos;
		return;
	}

	for(std::map<UInt8, ChallengerVec>::iterator iter1 = m_SortList.begin();
		iter1 != m_SortList.end(); ++iter1)
	{
		ChallengerVec& challengers = iter1->second;
		UInt32 ranking = 0;
		for(ChallengerVec::iterator iter2 = challengers.begin();
			iter2 != challengers.end(); ++iter2)
		{
			m_Rankings.insert(std::make_pair((*iter2)->GetID(), ++ranking));
		}
	}

	for(std::map<ObjID_t, UInt32>::iterator iter = m_Rankings.begin();
		iter != m_Rankings.end(); ++iter)
	{
		std::ostringstream oss;
		oss << "INSERT INTO `t_worldchallenger_ranking`(`guid`,`ranking`) VALUES(" << iter->first << "," << iter->second << ")";
		if(dbconn->Update(oss.str()) < 0)
		{
			ErrorLogStream << "insert worldchallenger ranking:" << dbconn->GetError() << LogStream::eos;
			return;
		}
	}
}

void ChallengeMgr::InitRoundTime(const Avalon::Time& now)
{
	/*
	Avalon::Date date(now);
	UInt32	dayBeginTime = UInt32(GetDayBeginTime(now) / 1000);
	UInt32 beginTime = dayBeginTime + ScriptMgr::Instance()->GetChallengeScript()->GetBeginTime();
	UInt32 endTime = dayBeginTime + ScriptMgr::Instance()->GetChallengeScript()->GetEndTime();

	if(date.WDay() >= 1 && date.WDay() <= 6)
	{
		if(now.Sec() >= endTime)
		{
			if(date.WDay() == 6)
			{
				m_RoundTime = beginTime + 2 * DAY_TO_SEC; //跳过周日

				UInt32 bEnd = GameParamMgr::Instance()->GetValue(SEASON_END_VAR);
				if(bEnd == 0)
				{
					GameParamMgr::Instance()->SetValue(SEASON_END_VAR, 1);
					OnSeasonEnd();
				}
			}
			else m_RoundTime = beginTime + DAY_TO_SEC;
		}
		else if(now.Sec() < beginTime)
		{
			m_RoundTime = beginTime;
		}
		else
		{
			m_RoundTime = beginTime  + (UInt32(now.Sec()) - beginTime) / (ROUND_LASTTIME) * ROUND_LASTTIME;
		}
	}
	else
	{
		m_RoundTime = beginTime + DAY_TO_SEC;

		UInt32 bEnd = GameParamMgr::Instance()->GetValue(SEASON_END_VAR);
		if(bEnd == 0)
		{
			GameParamMgr::Instance()->SetValue(SEASON_END_VAR, 1);
			OnSeasonEnd();
		}
	}
	*/
}

void ChallengeMgr::BeginRound()
{
	std::map<UInt32, std::vector<Challenger*> > signinlist;

	for(std::vector<ObjID_t>::iterator iter = m_SigninList.begin();
		iter != m_SigninList.end(); ++iter)
	{
		Challenger* challenger = m_Challengers.FindObject(*iter);
		if(challenger == NULL) continue;

		signinlist[challenger->GetGroup()].push_back(challenger);
	}
	m_SigninList.clear();

	for(std::map<UInt32, std::vector<Challenger*> >::reverse_iterator iter = signinlist.rbegin();
		iter != signinlist.rend(); ++iter)
	{
		std::vector<Challenger*>& challengers = iter->second;

		if(challengers.size() < MIN_GROUP_PLAYER_NUM)
		{
			//UInt32 group = ScriptMgr::Instance()->GetChallengeScript()->GetPreGroupByGroup(iter->first);
			//if(group != 0)
			//{
			//	signinlist[group].insert(signinlist[group].end(), challengers.begin(), challengers.end());
			//	challengers.clear();
			//}
			}

		ChallengerSorter sorter;
		std::sort(challengers.begin(), challengers.end(), sorter);

		Challenger* pWaitChallener = NULL;
		for(std::vector<Challenger*>::iterator challengerIter = challengers.begin();
			challengerIter != challengers.end(); ++challengerIter)
		{
			Challenger* challenger = *challengerIter;

			if(pWaitChallener == NULL) pWaitChallener = challenger;
			else
			{
				m_Pairs.push_back(std::make_pair(pWaitChallener, challenger));
				pWaitChallener = NULL;
			}
		}

		if(pWaitChallener != NULL)
		{
			SetChallengeResult(pWaitChallener->GetID(), 2, 3);
		}
	}

	for(std::vector<ChallengeScene*>::iterator iter = m_Scenes.begin();
		iter != m_Scenes.end(); ++iter)
	{
		ChallengeScene* scene = *iter;
		scene->SetStatus(0);
	}
}

void ChallengeMgr::UpdateSortList(Challenger* challenger)
{
	ChallengerVec& sortlist = m_SortList[challenger->GetGroup()];

	ChallengerVec::iterator iter = sortlist.begin();
	while(iter != sortlist.end())
	{
		if(*iter == challenger) break;
		++iter;
	}

	if(iter != sortlist.end()) //在排行榜中
	{
		bool bUpdate = false;
		if(iter != sortlist.begin())
		{
			ChallengerVec::iterator preIter = iter;
			--preIter;
			if((*iter)->GetScore() >= (*preIter)->GetScore())
			{
				bUpdate = true;
			}
		}
		if(!bUpdate) return;

		sortlist.erase(iter);
	}
	else
	{
		if(sortlist.size() >= WORLD_CHALLENGE_SORTLIST_NUM)
		{
			ChallengerVec::iterator lastIter = --sortlist.end();
			if(challenger->GetScore() < (*lastIter)->GetScore()) return;

			sortlist.erase(lastIter);
		}
	}
	
	iter = sortlist.begin();
	while(iter != sortlist.end())
	{
		if(challenger->GetScore() >= (*iter)->GetScore()) break;
		++iter;
	}

	sortlist.insert(iter, challenger);
}

UInt16 ChallengeMgr::GetRanking(ObjID_t id)
{
	Challenger* challenger = m_Challengers.FindObject(id);
	if(challenger == NULL) return 0;

	const ChallengerVec& sortlist = m_SortList[challenger->GetGroup()];
	if(sortlist.empty()) return 0;
	if(challenger->GetScore() < (*(--sortlist.end()))->GetScore()) return 0;

	UInt16 ranking = 0;
	for(ChallengerVec::const_iterator iter = sortlist.begin();
		iter != sortlist.end(); ++iter)
	{
		Challenger* entry = *iter;

		++ranking;
		if(entry == challenger) break;
	}

	return ranking;
}
