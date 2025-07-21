#include "DungeonMgr.h"
#include "Player.h"
#include <CLRecordClient.h>
#include <CLDungeonProtocol.h>
#include <CLTeamProtocol.h>
#include <CLDungeonTimesDataEntry.h>
#include <CLQuickBuyDataEntry.h>
#include <CLGuildBattleDataEntry.h>
#include <CLTeamDungeonDataEntry.h>
#include <CLUltimateChallengeDungeonDataEntry.h>
#include <CLUltimateChallengeBuffDataEntry.h>
#include <CLFunctionDataEntry.h>
#include <CLMallShopMultiIDataEntry.h>

#include "Scene.h"
#include "TaskMgr.h"
#include "ActivityMgr.h"
#include "SSDungeonConfigMgr.h"
#include "SSRouter.h"
#include "SceneMgr.h"
#include "CityMonster.h"
#include "SSApplication.h"
#include "SSServiceSwitchMgr.h"
#include "Team.h"
#include "ActiveRecordMgr.h"

DungeonMgr::DungeonMgr()
{

}

DungeonMgr::~DungeonMgr()
{
    for (auto itr : m_dungeons)
    {
        delete itr.second;
    }
	m_dungeons.clear();

    for (auto itr : m_dungeonHardInfoes)
    {
        delete itr.second;
    }
	m_dungeonHardInfoes.clear();
}

void DungeonMgr::OnOnline()
{
    {
        CLProtocol::SceneDungeonInit dungeonInit;
        for (auto itr : m_dungeons)
        {
            auto dungeon = itr.second;
            if (!dungeon)
            {
                continue;
            }

            dungeonInit.allInfo.push_back(dungeon->GetSceneDungeonInfo());
        }

        GetOwner()->SendProtocol(dungeonInit);
    }

    {
        CLProtocol::SceneDungeonHardInit hardInit;
        for (auto itr : m_dungeonHardInfoes)
        {
            auto hard = itr.second;
            if (!hard)
            {
                continue;
            }

            hardInit.allInfo.push_back(hard->GetSceneDungeonHardInfo());
        }

        GetOwner()->SendProtocol(hardInit);
    }

    SyncDungeonList(true);
}

void DungeonMgr::SyncDungeonList(bool syncAll)
{
    std::vector<DungeonOpenInfo> newOpenList;
    std::vector<UInt32> newCloseList;
    _GenNewOpenDungeonList(newOpenList, newCloseList);

    for (auto info : newOpenList)
    {
        m_openedDungeonList[info.id] = info;
    }

    for (auto id : newCloseList)
    {
        m_openedDungeonList.erase(id);
    }

    CLProtocol::SceneDungeonSyncNewOpenedList sync;
    if (syncAll)
    {
        for (auto itr : m_openedDungeonList)
        {
            sync.newOpenedList.push_back(itr.second);
        }
    }
    else
    {
        sync.newOpenedList = newOpenList;
        sync.newClosedList = newCloseList;
    }

    if (sync.newOpenedList.empty() && sync.newClosedList.empty())
    {
        return;
    }

    GetOwner()->SendProtocol(sync);
}


void DungeonMgr::OnStartDungeonReq(UInt32 dungeonId, std::vector<UInt32> bufDrugs, const SceneNpc& cityMonster)
{
	CLProtocol::SceneDungeonStartRes res;

	dungeonId = AdjustDungeonId(dungeonId);

	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonData)
	{
		ErrorLogStream << "player(" << GetOwner()->GetID() << ") want to start a invalid dungeon(" << dungeonId << ")." << LogStream::eos;

		res.result = ErrorCode::DUNGEON_START_DUNGEON_NOT_EXIST;
		GetOwner()->SendProtocol(res);
		return;
	}

	if (dungeonData->subType == DST_CITY_MONSTER)
	{
		if (cityMonster.guid == 0)
		{
			DebugLogStream << "player(" << GetOwner()->GetID() << ") want to start monster dungeon(" << dungeonId << ") monster(" << cityMonster.guid << ") not exist." << LogStream::eos;
			res.result = ErrorCode::DUNGEON_START_MONSTER_NOT_EXIST;
			GetOwner()->SendProtocol(res);
			return;
		}

		auto monster = SceneMgr::Instance()->FindCityMonster(cityMonster.guid);
		if (!monster)
		{
			DebugLogStream << "player(" << GetOwner()->GetID() << ") want to start monster dungeon(" << dungeonId << ") monster(" << cityMonster.guid << ") not exist." << LogStream::eos;
			res.result = ErrorCode::DUNGEON_START_MONSTER_NOT_EXIST;
			GetOwner()->SendProtocol(res);
			return;
		}

		if (monster->GetDungeonID() != dungeonId)
		{
			DebugLogStream << "player(" << GetOwner()->GetID() << ") want to start monster dungeon(" << dungeonId << ") monster(" << cityMonster.guid << ")'s dungeon(" << monster->GetDungeonID() << ")." << LogStream::eos;
			res.result = ErrorCode::DUNGEON_START_MONSTER_NOT_EXIST;
			GetOwner()->SendProtocol(res);
			return;
		}

		auto tmpResult = monster->CanFight();
		if (tmpResult != ErrorCode::SUCCESS)
		{
			DebugLogStream << "player(" << GetOwner()->GetID() << ") want to start monster dungeon(" << dungeonId << ") monster(" << monster->ToString() << ") result(" << tmpResult << ")." << LogStream::eos;
			res.result = tmpResult;
			GetOwner()->SendProtocol(res);
			return;
		}
	}

	// ����ǲ�����ѡ��ؿ��ĳ�����
	// �����󣨻ͼ����Ҫ�ڹؿ�ѡ����棩
	if ((dungeonData->type != LT_ACTIVITY && dungeonData->type != LT_TOWER) && (!GetOwner()->GetScene() || GetOwner()->GetScene()->GetType() != SCENE_TYPE_DUNGEON_ENTRY))
	{
#ifdef _DEBUG
		if (dungeonData->subType != DST_RAID_DUNGEON)
		{
#endif
			InfoLogStream << "player(" << GetOwner()->GetID() << ") request to start dungeon(" << dungeonId << ") not in dungeon entry scene." << LogStream::eos;

			res.result = ErrorCode::DUNGEON_START_NOT_IN_ENTRY_SCENE;
			GetOwner()->SendProtocol(res);
			return;
#ifdef _DEBUG
		}
#endif
	}

	bool isHell = dungeonData->IsHellEntry();
	bool isWeekHell = dungeonData->IsWeekHellEntry();
	UInt32 ret = CanPlayThisMap(dungeonId, true);
	res.dungeonId = dungeonId;

	if (ret != 0)
	{
		CheckMallGiftActivateCondition(ret, dungeonData->subType);

		res.result = ret;
		GetOwner()->SendProtocol(res);
		return;
	}

	// ��ҩ
	for (auto drug : bufDrugs)
	{
		BuffDrugData* cfgData = BuffDrugDataMgr::Instance()->FindEntry(drug);
		if (!cfgData)
		{
			continue;
		}

		if (GetOwner()->GetLevel() <= cfgData->freeLevel)
		{
			GetOwner()->GetItemMgr().UseItem(drug);
			continue;
		}

		// �����������ߣ���ֱ���ñ������
		if (GetOwner()->GetItemNum(drug) > 0)
		{
			auto result = GetOwner()->GetItemMgr().UseItem(drug);
			if (result == ErrorCode::SUCCESS)
			{
				GetOwner()->RemoveItem(GetReason(SOURCE_TYPE_DUNGEON_BUFF_DRUG, drug).c_str(), drug, 1);
			}
		}
		else
		{
			// ���ٹ���ʹ��
			auto quickData = QuickBuyDataEntryMgr::Instance()->FindEntry(drug);
			if (!quickData)
			{
				ErrorLogStream << "player(" << GetOwner()->GetID() << ") want to buy unexist drug(" << drug << ")." << LogStream::eos;
				continue;
			}

			//Ǯ����
			if (quickData->costNum > m_pOwner->GetItemNum(quickData->costItemId))
			{
				InfoLogStream << "player(" << GetOwner()->GetID() << ") want to buy drug(" << drug << ") no enough money." << LogStream::eos;
				continue;
			}

			// ��ʹ�õ���
			auto result = GetOwner()->GetItemMgr().UseItem(drug);
			if (result == ErrorCode::SUCCESS)
			{
				if (quickData->costItemId == ITEM_POINT)
				{
					UInt32 rate = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TICKET_CONVERT_MALL_INTEGRAL_RATE);
					UInt8 multiple = 0;
					UInt32 endTime = 0;
					MallShopMultiIDataEntryMgr::Instance()->GetMallItemMultiple(quickData->id, multiple, endTime);
					GetOwner()->AddMallPoint(GetReason().c_str(), quickData->costNum * (quickData->multiple + multiple) * rate);
				}
				GetOwner()->RemoveItem(GetReason(SOURCE_TYPE_DUNGEON_BUFF_DRUG_QUICK_BUY, drug).c_str(), quickData->costItemId, quickData->costNum);
			}
		}
	}

	//����������
	bool isHellGuide = false;
	if (dungeonData->guideTasks.size() > 0) //�߻�Ҫȥ�Ժ�֧�ִӶ������������ѡ
	{
		if (dungeonData->guideTasks.size() == 1) //������������ʱ
		{
			std::map<UInt32, UInt32>::iterator it = dungeonData->guideTasks.begin();
			UInt32 guideTaskId = it->first;
			UInt32 guideTaskDungeonId = it->second;
			BaseTask* task = GetOwner()->GetTask(guideTaskId);
			if (task && task->GetStatus() == TASK_UNFINISH)
			{
				auto guideTaskdungeonData = DungeonDataEntryMgr::Instance()->FindEntry(guideTaskDungeonId);
				if (guideTaskdungeonData)
				{
					UInt32	oldDungeonId = dungeonId;
					dungeonId = guideTaskDungeonId;
					dungeonData = guideTaskdungeonData;
					isHell = false;
					if (dungeonData->IsHellDungeon())
					{
						isHellGuide = true;
					}
					else
					{
						isHellGuide = false;
					}
					InfoLogStream << "player(" << GetOwner()->GetID() << ") start guide dungeon oldDungeonId(" << oldDungeonId << ") guidedungeonId(" << dungeonId << ") guideTaskId(" << guideTaskId << ")" << LogStream::eos;
				}
			}
		}
		else //�ж����������
		{
		}
	}

	CLProtocol::WorldDungeonStartRaceReq req;
	req.cityMonster = cityMonster;
	req.session = 0;
	req.dungeonId = dungeonId;
	req.isHell = isHell;
	req.isHellGuide = isHellGuide;
	req.isWeekHell = isWeekHell;

	if (dungeonData->type == LT_TOWER)
	{
		req.passedAreaIndex = GetOwner()->GetCounterMgr().GetCounter(COUNTER_TOWER_TOP_FLOOR);
	}

	auto playerInfo = GetDungeonPlayerInfo(dungeonId);
	if (cityMonster.type == SOT_CITYMONSTER && cityMonster.funcType == CITY_MONSTER_ACTIVITY && GetOwner()->GetDailyCityMonsterRemainTimes() == 0)
	{
		playerInfo.isAssist = TEAM_MEMBER_MASK_ASSIST;
	}

	req.playerInfoes.push_back(playerInfo);

	//�ռ������������
	if (dungeonData->subType == DST_ZJSL_TOWER)
	{
		// ���ܿ�������
		if (!SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_ZJSL_TOWER))
		{
			return;
		}

		// ���ŵȼ�����
		auto functionData = FuncNotifyDataEntryMgr::Instance()->FindEntry(FUNC_ZJSL);
		if (!functionData)
		{
			return;
		}

		if (GetOwner()->GetLevel() < functionData->endLevel)
		{
			return;
		}

		// ÿ����ս�������ƣ���սʧ�ܵ�ʱ������������1
		if (GetOwner()->GetCounter(COUNTER_ZJSL_CHALLENGE_TIMES) > 0)
		{
			return;
		}

		UltimateChallengeDungeonDataEntry* zjslDataEntry = UltimateChallengeDungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
		if (!zjslDataEntry)
		{
			return;
		}

		// ������һ�㰤��һ��ͨ��
		UInt32 floor = GetOwner()->GetCounter(COUNTER_ZJSL_TOP_FLOOR);
		if (zjslDataEntry->level != (floor + 1))
		{
			return;
		}

		// ͬһ��ÿ������N��
		if (GetOwner()->GetCounter(COUNTER_ZJSL_DUNGEON_TIMES) > SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ZJSL_DUNGEON_TIMES_DAILY))
		{
			return;
		}
		
		// ÿ�쿪��N������
		Int32 weekDay = GetLogicDate(CURRENT_TIME.MSec()).WDay();

		UInt32 topFloor = weekDay * SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ZJSL_TOWER_FLOOR_OPEN_DAILY);
		if (zjslDataEntry->level > topFloor)
		{
			return;
		}
	}

	req.zjslDungeonInfo.boss1ID = GetOwner()->GetCounter(COUNTER_ZJSL_BOSS1_ID);
	req.zjslDungeonInfo.boss2ID = GetOwner()->GetCounter(COUNTER_ZJSL_BOSS2_ID);
	req.zjslDungeonInfo.boss1RemainHp = GetOwner()->GetCounter(COUNTER_ZJSL_BOSS1_HP);
	req.zjslDungeonInfo.boss2RemainHp = GetOwner()->GetCounter(COUNTER_ZJSL_BOSS2_HP);

	CLProtocol::ZjslDungeonBuff buff;
	buff.buffId = GetOwner()->GetCounter(COUNTER_ZJSL_DUNGEON_BUFF);
	buff.buffLvl = 1;
	buff.buffTarget = GetOwner()->GetCounter(COUNTER_ZJSL_DUNGEON_BUFF_TARGET);
	req.zjslDungeonInfo.buffVec.push_back(buff);

	buff.buffId = GetOwner()->GetCounter(COUNTER_ZJSL_INSPIRE_BUFF);
	buff.buffLvl = 1;
	buff.buffTarget = 1;
	req.zjslDungeonInfo.buffVec.push_back(buff);

	// ������﹥�ǻ���ڿ�ʼʱ�Ϳ۴���
// 	if (cityMonster.guid > 0)
// 	{
// 		auto monster = SceneMgr::Instance()->FindCityMonster(cityMonster.guid);
// 		if (monster && monster->GetMonsterType() == CITY_MONSTER_ACTIVITY)
// 		{
// 			monster->OnKilled(CLRecordClient::Instance()->GenGuid());
// 		}
// 	}

	Router::SendToWorld(req);
}

void DungeonMgr::OnWorldStartDungeonRes(UInt32 dungeonId, bool isHell, bool isContinue, bool isAssist, UInt32 result)
{
	if (result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << "player(" << GetOwner()->GetID() << ") start dungeon(" << dungeonId << ") isHell(" << (UInt8)isHell << ") failed, result(" << result << ")." << LogStream::eos;

		CLProtocol::SceneDungeonStartRes res;
		res.dungeonId = dungeonId;
		res.result = result;
		GetOwner()->SendProtocol(res);

		return;
	}

	auto dungeonRes = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonRes)
	{
		ErrorLogStream << "player(" << GetOwner()->GetID() << ") start dungeon(" << dungeonId << ") can't find dungeon data." << LogStream::eos;
		return;
	}

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_DUNGEON, dungeonId);

	// ������
	if (!isAssist && dungeonRes->costFatiguePerArea > 0)
	{
		UInt16 removeNum = dungeonRes->costFatiguePerArea;
		UInt32 rmAddition = 0;
		
		// ƣ��ȼ�ռӳ�
		switch ((DungeonSubType)dungeonRes->subType)
		{
		case DST_NORMAL:
		case DST_WUDAOHUI:
			rmAddition = GetOwner()->GetActiveTaskMgr().GetOpActTaskFatigueRmAddition(OAT_FATIGUE_BURNING, dungeonRes);
			break;

		default: break;
		}

		if (rmAddition > 0)
		{
			removeNum *= ((float)rmAddition / 100 + 1);
		}

		GetOwner()->RemoveFatigue(reason.c_str(), removeNum);
	}

	// ����Ʊ
	if (dungeonRes->ticket.num > 0)
	{
		UInt32 vipFreeTimes = 0;
		GetOwner()->GetVipValue(VIP_PRIVILEGE_HELL_TICKET, vipFreeTimes);
		// ���ȿ�vip��Ѵ���
		if (isHell && vipFreeTimes > GetOwner()->GetCount(COUNTER_VIP_FREE_HELL_TIMES))
		{
			GetOwner()->IncCounter(COUNTER_VIP_FREE_HELL_TIMES, 1);
		}
		else
		{
			UInt32 needTickets = dungeonRes->ticket.num;

			if (GetOwner()->GetActiveTaskMgr().HaveDungeonRightCard() && 
				(((dungeonRes->subType == DST_HELL || dungeonRes->subType == DST_HELL_ENTRY) && dungeonRes->minLevel >= 65) || (dungeonRes->subType == DST_LIMIT_TIME_HELL)))
			{
				needTickets = needTickets * SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ABYSS_TICKET_WAIVER) / 100;
			}

			do 
			{
				// ���������
				UInt32 dungeonTimes = 0;
				UInt32 limitTimes = 0;
				UInt32 reduceTickets = 0;
				if (dungeonRes->subType == DST_HELL)
				{
					dungeonTimes = GetOwner()->GetCounter(COUNTER_DUNGEON_HELL_TIMES);
				}
				else if (dungeonRes->subType == DST_YUANGU)
				{
					dungeonTimes = GetOwner()->GetCounter(COUNTER_DUNGEON_YUANGU_TIMES);
				}
				else
				{
					break;
				}

				GetOwner()->GetActiveTaskMgr().VisitOpenOpActAndTaskByOpType(OAT_SECOND_ANNIVERSARY_PRAY, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task) {
					if (task->state != TASK_FINISHED)
					{
						return true;
					}

					UInt32 type = taskData.completeNum;

					if (type != SAPABT_HELL_TICKET_REDUCE && type != SAPABT_YUANGU_TICKET_REDUCE)
					{
						return true;
					}

					if (taskData.variables2.size() != 2)
					{
						return true;
					}

					limitTimes = taskData.variables2[0];
					reduceTickets = taskData.variables2[1];
					return true;
				});

				if (limitTimes > 0 && dungeonTimes < limitTimes && needTickets > reduceTickets)
				{
					needTickets -= reduceTickets;
				}

			} while (0);
			
			Int32 removeTicket = GetOwner()->RemoveItemWithEqualItem(reason.c_str(), dungeonRes->ticket.itemId, needTickets);
			if ((isHell || dungeonRes->subType == DST_LIMIT_TIME_HELL) && removeTicket > 0)
			{
				GetOwner()->GetActiveTaskMgr().OnUsedHellTicket(removeTicket);
			}

			if (dungeonRes->subType == DST_YUANGU && removeTicket > 0)
			{
				GetOwner()->GetActiveTaskMgr().OnUsedYuanGuTicket(removeTicket);
			}

		}
	}

	// ��buffer
	GetOwner()->GetBuffMgr().OnUseDungeonBuffers(dungeonRes);

	// ��¼����(���ֻ�н��������ͼҪ�ڽ�����¼����)
	if (!isAssist && !dungeonRes->onlyRaceEndSettlement)
	{
		GetOwner()->GetDungeonMgr().TryRecordDailyCount(dungeonId);
		GetOwner()->GetDungeonMgr().TryRecordWeekCount(dungeonId);
	}
	else
	{
		// �������磬ֱ�ӿ۴���������¼�����ڴ�ļ�¼
		if (dungeonRes->subType == DST_DEVILDDOM)
		{
			std::vector<UInt32> param = { GetNormalHardDungeonID(dungeonId) };
			// ֻ��֮ǰû������������Ҫ�۴���
			if (GetOwner()->GetCounterMgr().GetCounterDynamic(COUNTER_DUNGEON_PROCESS_PREFIX, param) == 0)
			{
				InfoLogStream << "player(" << GetOwner()->GetID() << ") start devilddom dungeon(" << dungeonId << ")." << LogStream::eos;
				GetOwner()->GetDungeonMgr().TryRecordDailyCount(dungeonId);
				GetOwner()->GetCounterMgr().SetCounterDynamic(COUNTER_DUNGEON_PROCESS_PREFIX, 1, param);
			}
		}
	}
	
	// ������Ǽ���������֮����Ҫ��֮ǰ��¼�����������
	if (dungeonRes->type == LT_TOWER)
	{
		GetOwner()->GetCounterMgr().SetCounter(COUNTER_TOWER, dungeonId);

		GetOwner()->OnDeathTowerBegin(dungeonId);
	}

	// �����Ҳ��ڵ��³�ѡ�������߻����
	//if (dungeonRes->type == LT_ACTIVITY || dungeonRes->type == LT_TOWER)
	{
		auto scene = GetOwner()->GetScene();
		if (scene && scene->GetType() != SCENE_TYPE_DUNGEON_ENTRY && scene->GetType() != SCENE_TYPE_ACTIVITY)
		{
			CLPosition pos;
			GetOwner()->ChangeScene(SceneMgr::Instance()->GetActivitySceneID(), pos, 0);
		}
	}

	// �ռ�����
	if (dungeonRes->subType == DST_ZJSL_TOWER)
	{
		// ���ӵ����ۼ���ս����
		GetOwner()->IncCounter(COUNTER_ZJSL_DUNGEON_TIMES, 1);

		// ����ÿ���ۼ���ս����
		UInt32 nowSec = Avalon::Time::CurrentTime().Sec();
		if (!CheckInSameLogicDay(GetOwner()->GetCounterMgr().GetCounterUpdateTime(COUNTER_ZJSL_CHALLENGE_DAYS), nowSec) ||
			0 == GetOwner()->GetCounter(COUNTER_ZJSL_CHALLENGE_DAYS))
		{
			GetOwner()->IncCounter(COUNTER_ZJSL_CHALLENGE_DAYS, 1);
		}
	}

	GetOwner()->OnBeginDungeon(dungeonId, isHell);

	// ��������¼��Ԩ��Զ�ŵ�ÿ��ͨ�ش���
	if (dungeonRes->subType == DST_HELL)
	{
		GetOwner()->IncCounter(COUNTER_DUNGEON_HELL_TIMES, 1);
	}
	else if (dungeonRes->subType == DST_YUANGU)
	{
		GetOwner()->IncCounter(COUNTER_DUNGEON_YUANGU_TIMES, 1);
	}

	// ������³ǻ
	EnterDungeonParam param(dungeonId);
	CLEventMgr::Instance()->SendPlayerEvent(GetOwner()->GetID(), CLEventMgr::ET_ENTER_DUNGEON, &param);
}

UInt32 DungeonMgr::CanPlayThisMap(UInt32 dungeonId, bool isCheckTicket, bool checkStoryTask, bool checkHard, bool checkFatigue, bool checkTimes)
{
	// �Ƿ����ű�
	if (GetOwner()->GetTeamCopyMgr().IsTeamCopy())
	{
		ErrorLogStream << "player(" << GetOwner()->GetID() << ") in team copy dungeon(" << dungeonId << ")." << LogStream::eos;
		return ErrorCode::DUNGEON_START_IN_TEAM_COPY;
	}

    // �ж�����ͼ�ܲ��ܴ�
    DungeonDataEntry* data = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
    if (!data)
    {
		ErrorLogStream << "player(" << GetOwner()->GetID() << ") want to start a invalid dungeon(" << dungeonId << ")." << LogStream::eos;	
        return ErrorCode::DUNGEON_START_DUNGEON_NOT_EXIST;
    }

	// ��������ͼ���ж��Ƿ���
	if (data->subType == DST_DEVILDDOM && SSApplication::Instance()->IsCloseDevil())
	{
		ErrorLogStream << "player(" << GetOwner()->GetID() << ") want to start devil dungeon(" << dungeonId << "), closed." << LogStream::eos;
		return ErrorCode::DUNGEON_START_NOT_OPEN_TIME;
	}

	if (data->singleDungeon && GetOwner()->GetTeam() != NULL)
	{
		ErrorLogStream << "player(" << GetOwner()->GetID() << ") want to start dungeon(" << dungeonId << "), must single." << LogStream::eos;
		return ErrorCode::DUNGEON_START_MUST_SINGLE;
	}

	// ��Ԩ���³�����������ģ�����ֱ�ӽ���
	if (data->subType == DST_HELL)
	{
		ErrorLogStream << "player(" << GetOwner()->GetID() << ") want to start a invalid dungeon(" << dungeonId << ")." << LogStream::eos;
		return ErrorCode::DUNGEON_START_DUNGEON_NOT_EXIST;
	}

	bool isHellMode = data->IsHellEntry();

	// ��������
	/*if (!BagHasEnoughSpace())
	{
		//ErrorLogStream << "player(" << GetOwner()->GetID() << ") want to start dungeon(" << dungeonId << ") bag don't have enough space." << LogStream::eos;
		//return ErrorCode::DUNGEON_START_BAG_FULL;
		//GetOwner()->SendNotify(ErrorCode::DUNGEON_START_BAG_FULL);
	}*/

	if (checkFatigue && data->costFatiguePerArea > 0 && GetOwner()->GetFatigue() < data->costFatiguePerArea)
    {
		ErrorLogStream << "player(" << GetOwner()->GetID() << ") want to start dungeon(" << dungeonId << ") bag don't have enough fatigue." << LogStream::eos;
        return ErrorCode::DUNGEON_START_NO_FATIGUE;
    }

    UInt32 normalDungeonId = GetNormalHardDungeonID(dungeonId);
    DungeonDataEntry* normalDungeonData = DungeonDataEntryMgr::Instance()->FindEntry(normalDungeonId);
    if (!normalDungeonData)
    {
		ErrorLogStream << "player(" << GetOwner()->GetID() << ") want to start dungeon(" << dungeonId
						<< ") but can't find normal hard dungeon(" << normalDungeonId << ")." << LogStream::eos;
        return ErrorCode::DUNGEON_START_DUNGEON_NOT_EXIST;
    }

	//�ܳ���Ԩǰ�ùؿ�,��Ҫ���ǰ������
	if (data->subType == DST_WEEK_HELL_PER && data->storyTaskId > 0)
	{
		UInt32 storyTaskId = normalDungeonData->storyTaskId;
		auto task = GetOwner()->GetTaskMgr().FindTask(storyTaskId);
		if (!task || task->GetStatus() != TASK_UNFINISH)
		{
			return ErrorCode::DUNGEON_START_CONDITION_WEEK;
		}
	}

	//�ܳ���Ԩ��ڹؿ����
	if (data->subType == DST_WEEK_HELL_ENTRY && data->preTaskId > 0)
	{
		UInt32 preTaskId = normalDungeonData->preTaskId;
		auto task = GetOwner()->GetTaskMgr().FindTask(preTaskId);
		if (!task && !GetOwner()->CheckTaskFinished(normalDungeonData->preTaskId))
		{
			return ErrorCode::DUNGEON_START_CONDITION;
		}
		if (task && task->GetStatus() != TASK_FINISHED
			&& task->GetStatus() != TASK_SUBMITTING
			&& task->GetStatus() != TASK_OVER)
		{
			return ErrorCode::DUNGEON_START_CONDITION;
		}
	}

	// û�������Ҫ�ж��Ƿ񿪷ţ�����Ͳ���Ҫ��
	if (!FindDungeonByDataId(dungeonId))
	{
		// ���Ѷ��Ƿ񿪷�
		DungeonHardInfo* hardInfo = FindDungeonHardInfoById(GetNormalDungeonID(dungeonId));
		if (checkHard && ((!hardInfo && data->hardType > LHT_NORMAL) || (hardInfo && data->hardType > hardInfo->GetUnlockedHardType())))
		{
			ErrorLogStream << "player(" << GetOwner()->GetID() << ") want to start not opened hard's dungeon(" << dungeonId << ")." << LogStream::eos;
			return ErrorCode::DUNGEON_START_HARD_NOT_OPEN;
		}

		if (!IsMatchDungeonOpenCond(dungeonId, checkStoryTask))
		{
			ErrorLogStream << "player(" << GetOwner()->GetID() << ") don't match dungeon(" << dungeonId << ")'s open conditions." << LogStream::eos;
			return ErrorCode::DUNGEON_START_CONDITION;
		}
	}
    
    // �����Ʊ������
	if (isCheckTicket && normalDungeonData->ticket.num > 0)
    {
		UInt32 needTickets = normalDungeonData->ticket.num;

		if (GetOwner()->GetActiveTaskMgr().HaveDungeonRightCard() &&
			(((data->subType == DST_HELL || data->subType == DST_HELL_ENTRY) && data->minLevel >= 65) || (data->subType == DST_LIMIT_TIME_HELL)))
		{
			needTickets = needTickets * SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ABYSS_TICKET_WAIVER) / 100;
		}

		do 
		{
			// ���������
			UInt32 dungeonTimes = 0;
			UInt32 limitTimes = 0;
			UInt32 reduceTickets = 0;
			if (data->subType == DST_HELL_ENTRY)
			{
				dungeonTimes = GetOwner()->GetCounter(COUNTER_DUNGEON_HELL_TIMES);
			}
			else if (data->subType == DST_YUANGU)
			{
				dungeonTimes = GetOwner()->GetCounter(COUNTER_DUNGEON_YUANGU_TIMES);
			}
			else
			{
				break;
			}

			GetOwner()->GetActiveTaskMgr().VisitOpenOpActAndTaskByOpType(OAT_SECOND_ANNIVERSARY_PRAY, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task) {
				if (task->state != TASK_FINISHED)
				{
					return true;
				}

				UInt32 type = taskData.completeNum;

				if (type != SAPABT_HELL_TICKET_REDUCE && type != SAPABT_YUANGU_TICKET_REDUCE)
				{
					return true;
				}

				if (taskData.variables2.size() != 2)
				{
					return true;
				}

				limitTimes = taskData.variables2[0];
				reduceTickets = taskData.variables2[1];
				return true;
			});

			if (limitTimes > 0 && dungeonTimes < limitTimes && needTickets > reduceTickets)
			{
				needTickets -= reduceTickets;
			}
		} while (0);

        if (GetOwner()->GetItemNumWithEqualItem(normalDungeonData->ticket.itemId) < needTickets)
        {
			ErrorLogStream << "player(" << GetOwner()->GetID() << ") want to start dungeon(" << dungeonId
							<< ") but don't have enough ticket(" << normalDungeonData->ticket.itemId << ")." << LogStream::eos;

            return ErrorCode::DUNGEON_START_NO_TICKET;
        }
    }

	// ����ͼ��û�п���
	if (!SSActivityDungeonMgr::Instance()->IsDungeonOpened(dungeonId))
	{
		ErrorLogStream << "player(" << GetOwner()->GetID() << ") want to start dungeon(" << dungeonId
			<< ") but not in open time." << LogStream::eos;
		return ErrorCode::DUNGEON_START_NOT_OPEN_TIME;
	}

	// ����ܴ���
	if (!SSActivityDungeonMgr::Instance()->CheckWeekTimes(GetOwner(), normalDungeonData->subType))
	{
		ErrorLogStream << "player(" << GetOwner()->GetID() << ") want to start dungeon(" << dungeonId
			<< ") no remain week times." << LogStream::eos;
		return ErrorCode::DUNGEON_START_NO_TIMES;
	}

	// ����˺Ŵ���
	if (GetAccountDailyRemainTimes(dungeonId) <= 0)
	{
		ErrorLogStream << "player(" << GetOwner()->GetID() << ") want to start dungeon(" << dungeonId
			<< ") no remain account times." << LogStream::eos;
		return ErrorCode::DUNGEON_START_NO_TIMES;
	}

	// ���ʣ�����
	do 
	{
		auto maxTime = SSActivityDungeonMgr::Instance()->GetDungeonMaxTimes(GetOwner(), normalDungeonData->subType);
		if (checkTimes && maxTime > 0)
		{
			// �������磬���֮ǰ�Ѿ��ڴ��ˣ��Ͳ���Ҫ������
			if (normalDungeonData->subType == DST_DEVILDDOM)
			{
				// ���������
				GetOwner()->GetActiveTaskMgr().VisitOpenOpActAndTaskByOpType(OAT_SECOND_ANNIVERSARY_PRAY, [&](const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task) {
					if (task->state != TASK_FINISHED)
					{
						return true;
					}

					UInt32 type = taskData.completeNum;

					if (type != SAPABT_XUKONG_TIMES_ADD)
					{
						return true;
					}

					if (taskData.variables2.size() != 1)
					{
						return true;
					}

					maxTime += taskData.variables2[0];
					return true;
				});

				std::vector<UInt32> param = { normalDungeonId };
				if (GetOwner()->GetCounterMgr().GetCounterDynamic(COUNTER_DUNGEON_PROCESS_PREFIX, param) > 0)
				{
					InfoLogStream << "player(" << GetOwner()->GetID() << ") is in devilddom dungeon(" << dungeonId << ") no need daily count." << LogStream::eos;
					break;
				}
			}

			auto dayCount = GetDailyCount(normalDungeonId);
			if (dayCount > maxTime)
			{
				ErrorLogStream << "player(" << GetOwner()->GetID() << ") want to start dungeon(" << dungeonId
					<< ") no remain times." << LogStream::eos;
				return ErrorCode::DUNGEON_START_NO_TIMES;
			}


		}
	} while (0);
	

    // �������Ԩģʽ�������û����Ԩģʽ��Ʊ
    if (isHellMode)
    {
		if (!normalDungeonData->IsHellEntry())
        {
			ErrorLogStream << "player(" << GetOwner()->GetID() << ") want to start dungeon(" << dungeonId
				<< ") hell mode is not exist." << LogStream::eos;
            return ErrorCode::DUNGEON_START_NO_HELL_MODE;
        }

        if (!IsDungeonOpenHellMode(normalDungeonData->id))
        {
			ErrorLogStream << "player(" << GetOwner()->GetID() << ") want to start dungeon(" << dungeonId
				<< ") hell mode is not opened." << LogStream::eos;
            return ErrorCode::DUNGEON_START_CONDITION;
        }

		UInt32 vipFreeTimes = 0;
		GetOwner()->GetVipValue(VIP_PRIVILEGE_HELL_TICKET, vipFreeTimes);
		// vip��Ԩ��Ѵ���
		if (GetOwner()->GetCount(COUNTER_VIP_FREE_HELL_TIMES) >= vipFreeTimes)
		{
			if (GetOwner()->GetItemNum(HELL_TICKET_ITEM) < normalDungeonData->hellTicketNum)
			{
				ErrorLogStream << "player(" << GetOwner()->GetID() << ") want to start dungeon(" << dungeonId
					<< ") in hell mode, but don't have enough ticket." << LogStream::eos;
				return ErrorCode::DUNGEON_START_NO_HELL_TICKET;
			}
		}
    }

    return ErrorCode::SUCCESS;
}

bool DungeonMgr::BagHasEnoughSpace()
{
	PackType checkPack[] = { PACK_EQUIP, PACK_MATERIAL, PACK_EXPENDABLE};
	for (auto pack : checkPack)
	{
		if (GetOwner()->GetPackGridLeftByPack(pack) < EnterDungeonNeedBagSpace)
		{
			return false;
		}
	}

	return true;
}

DungeonRacePlayerInfo DungeonMgr::GetDungeonPlayerInfo(UInt32 dungeonId) const
{
	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);

	bool hasFriend = false,hasSameGuild = false;
	auto team = GetOwner()->GetTeam();
	if (team != nullptr)
	{
		hasFriend = team->HasFriend(GetOwner());
		hasSameGuild = team->HasSameGuild(GetOwner());
	}
	// ����
	GetOwner()->GetActiveTaskMgr().OnStartDungeon(dungeonData, hasFriend, hasSameGuild);

	//�Զ�����ҩˮ
	GetOwner()->GetItemMgr().AutoSetPotion();

	DungeonRacePlayerInfo info;
	info.accId = GetOwner()->GetAccID();
	info.zoneId = GetOwner()->GetZoneId();
	info.raceInfo = GetOwner()->GetMatchRacePlayerInfo(false);
	info.soureSceneId = NODE_ID;
	info.vipLvl = GetOwner()->GetVipLvl();
	GetOwner()->GetTaskMgr().GetDungeonTaskList(GetNormalHardDungeonID(dungeonId), info.taskList);

	if (!dungeonData || dungeonData->type != LT_ACTIVITY)
	{
		//GetOwner()->GetBuffMgr().GetDungeonBufferAddition(info.addition.buffExpRate, info.addition.goldRate, info.addition.taskDropNumRate, info.addition.pinkDropRate);
		//info.addition.expRate += info.addition.buffExpRate;
		GetOwner()->GetBuffMgr().GetDungeonBufferAddition(info.addition, dungeonData);
		info.addition.additionRate[DART_EXP_RATE] += info.addition.additionRate[DART_BUFF_EXP_RATE];
	}

	if (dungeonData)
	{
		// ���»�ȡ��buff�����Ϣ
		GetOwner()->GetBuffMgr().GetDungeonRaceBuffInfo(info, dungeonData);

		// �Ժ�buff��صľ�����������ӿ�����,��������Ҫ�����������Ƶ��ظ�д��

		// ���⴦���Ժ�����»��ƺ�ϳ�
		switch ((DungeonSubType)dungeonData->subType)
		{
		case DST_NORMAL:
		case DST_YUANGU:
		case DST_HELL:
		case DST_HELL_ENTRY:
		case DST_LIMIT_TIME_HELL:
		case DST_FREE_LIMIT_TIME_HELL:
		case DST_WUDAOHUI:
			//�Ӿ���
			GetOwner()->GetBuffMgr().GetExpBuffAddition(info.addition);
			break;

		default: 
		{
			// �ϱ����buff���������͵��³ǲ�������
			std::vector<OpActivityRegInfo*> buffAddOpActivities;
			GetOwner()->GetPlayerActivityInfo().FindOpActivity(OAT_RETURN_PRIVILEGE, buffAddOpActivities);

			for (auto opAct : buffAddOpActivities)
			{
				if (!opAct) continue;

				if (opAct->state != AS_IN)
				{
					continue;
				}

				if (!GetOwner()->GetActiveTaskMgr().IsMeetOpActRequireByPlayer(opAct))
				{
					continue;
				}

				for (auto& iter : opAct->tasks)
				{
					for (auto& buffId : iter.variables2)
					{
						for (auto itr = info.raceInfo.buffs.begin(); itr != info.raceInfo.buffs.end();)
						{
							auto& buff = *itr;
							if (buffId == buff.m_id)
								itr = info.raceInfo.buffs.erase(itr);
							else
								++itr;
						}
					}
				}
			}

			break;
		}
		}
	}

	// vip��ѻƽ������
	UInt32 maxFreeGoldChestNum = 0;
	GetOwner()->GetVipValue(VIP_PRIVILEGE_FREE_GOLDBOX, maxFreeGoldChestNum);
	if (!dungeonData->IsHellEntry() && !dungeonData->IsHellDungeon() && GetOwner()->GetCounter(COUNTER_VIP_FREE_GOLD_CHEST_TIMES) < maxFreeGoldChestNum)
	{
		info.freeGoldChest = 1;
	}
	else
	{
		info.freeGoldChest = 0;
	}

	if (dungeonData)
	{
		switch (dungeonData->subType)
		{
		case DST_LIMIT_TIME_HELL:
		case DST_FREE_LIMIT_TIME_HELL:
			info.freeGoldChest = 0;
			break;

		default:
			break;
		}
	}

	// ţͷ����԰����ӳ�
	if (dungeonData->subType == DST_NIUTOUGUAI)
	{
		UInt32 addition = 0;
		GetOwner()->GetVipValue(VIP_PRIVILEGE_MINOTAUR_PARADISE_EXP, addition);
		//info.addition.vipExpRate = addition / 10;
		//info.addition.expRate += addition / 10;
		info.addition.additionRate[DART_VIP_EXP_RATE] = addition / 10;
		info.addition.additionRate[DART_EXP_RATE] += addition / 10;
	}

	// VIP����ӳ�
	UInt32 vipExpAddition = 0;
	GetOwner()->GetVipValue(VIP_PRIVILEGE_DUNGEON_EXP, vipExpAddition);
	//info.addition.vipExpRate += vipExpAddition / 10;
	//info.addition.expRate += vipExpAddition / 10;
	info.addition.additionRate[DART_VIP_EXP_RATE] += vipExpAddition / 10;
	info.addition.additionRate[DART_EXP_RATE] += vipExpAddition / 10;

	// VIP��Ҽӳ�
	UInt32 vipGoldAddition = 0;
	GetOwner()->GetVipValue(VIP_PRIVILEGE_DUNGEON_GOLD, vipGoldAddition);
	//info.addition.vipGoldRate = vipGoldAddition / 10;
	//info.addition.goldRate += vipGoldAddition / 10;
	info.addition.additionRate[DART_VIP_GOLD_RATE] = vipGoldAddition / 10;
	info.addition.additionRate[DART_GOLD_RATE] += vipGoldAddition / 10;

	// ʦͽ����ӳ�
	if (dungeonData && dungeonData->masterExpAddition && GetOwner()->GetOnlineMaster())
	{
		UInt32 masterExpAddition = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MASTER_EXP_ADDITION);
		//info.addition.masterExpRate += masterExpAddition / 10;
		info.addition.additionRate[DART_MASTER_EXP_RATE] += masterExpAddition / 10;
	}

	// ���Ἴ�ܼӳ�
	GetOwner()->GetSkillMgr().GetGuildSkillDungeonAddition(info.addition);

	// ��Ӫ�����ӳ�
	switch ((DungeonSubType)dungeonData->subType)
	{
	case DST_NIUTOUGUAI:
	case DST_NANBUXIGU:
	case DST_SIWANGZHITA:
	case DST_GOLD:
	case DST_TEAM_BOSS:
		break;

	default:
		info.addition.additionRate[DART_OP_ACTVITY_EXP_RATE] = ActivityMgr::Instance()->GetOpActivityAddition(OAT_DUNGEON_EXP_ADDITION, GetOwner());
		info.addition.additionRate[DART_EXP_RATE] += info.addition.additionRate[DART_OP_ACTVITY_EXP_RATE];
		break;
	}

	// ƣ��ȼ�ռӳ�
	switch ((DungeonSubType)dungeonData->subType)
	{
	case DST_NORMAL:
	case DST_WUDAOHUI:
		GetOwner()->GetActiveTaskMgr().GetOpActivityTaskAddition(OAT_FATIGUE_BURNING, info.addition, dungeonData);
		break;

	default: break;
	}

	// ��Ӫ��ӳ�(�Ժ�������ӿ�)
	GetOwner()->GetActiveTaskMgr().GetOpActivityTaskAddition(dungeonData, info.addition);
	GetOwner()->GetActiveTaskMgr().GetOpActivityTaskBuff(dungeonData, info.raceInfo.buffs);

	info.dailyCount = GetDailyCount(dungeonId);
	info.totalCount = GetTotalCount(dungeonId);

	// ������ؼӳ�
	do{
		UInt32 enterGuildTime = GetOwner()->GetGuildEnterTime();
		UInt32 occupyTerrTime = GuildBattleTimeDataEntryMgr::Instance()->GetGuildCrossBattleEndTime(CURRENT_TIME);

		// ռ�����֮ǰ���빫����˲���Ч
		if (occupyTerrTime <= enterGuildTime)
		{
			break;
		}

		// ��ǰռ������ID
		UInt32 terrId = GetOwner()->GetGuildOccupyCrossTerrId();
		if (terrId == 0)
		{
			break;
		}

		auto terrData = GuildTerritoryDataEntryMgr::Instance()->FindEntry(terrId);
		if (!terrData || terrData->type != GTT_CROSS)
		{
			break;
		}

		auto normalDungeonId = GetNormalHardDungeonID(dungeonId);
		if (terrData->IsChestDoubleDungeon(normalDungeonId))
		{
			// ����ˢ�£����ռ��֮��Ҫˢ��һ��
			RefreshGuildTerrDungeonTimes();

			// ���Ƽӳ�
			if (GetOwner()->GetCounter(COUNTER_GUILD_TERR_DUNGEON_TIMES) < terrData->dailyChestDoubleTimes)
			{
				info.addition.additionRate[DART_GUILD_TERR_CHEST_DOUBLE_FLAG] = terrData->chestDoubleFlag;
			}
		}

		// ���ʼӳ�
		if (terrData->dropAddition > 0 && terrData->IsDropAdditionDungeon(normalDungeonId))
		{
			info.addition.additionRate[DART_PINK_DROP_RATE] += terrData->dropAddition;
		}
	} while (0);

	//�ƺżӳ�
	UInt32 actTitleId = GetOwner()->GetActiveTitle();
	if (actTitleId > 0 && dungeonData && dungeonData->IsHellEntry())
	{
		ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(actTitleId);
		if (itemData)
		{
			EquipAttrDataEntry* attrData = EquipAttrDataMgr::Instance()->FindEntry(itemData->attrId);
			if (attrData)
			{
				info.addition.dropAddition[DIT_SS] += attrData->DungeonEpicDropAddition;
			}
		}
	}
	//װ��buff����ӳ�
	class EquipVisitor : public ItemVisitor
	{
	public:
		EquipVisitor() : expAddition(0) {}

		bool operator()(Item* item)
		{
			EquipAttrDataEntry* equipAttrDataEntry = item->GetAttrDataEntry();
			if (equipAttrDataEntry && equipAttrDataEntry->attchBuffInfoIds.size() > 0)
			{
				for (size_t i = 0; i < equipAttrDataEntry->attchBuffInfoIds.size(); ++i)
				{
					BuffInfoEntry* buffInfoDataEntry = BuffInfoEntryMgr::Instance()->FindEntry(equipAttrDataEntry->attchBuffInfoIds[i]);
					if (buffInfoDataEntry)
					{
						BuffDataEntry* buffDataEntry = BuffDataEntryMgr::Instance()->FindEntry(buffInfoDataEntry->bufferId);
						if (buffDataEntry)
						{
							expAddition += buffDataEntry->expAddRate.GetValue();
						}
					}
				}
			}
			return true;
		}
	public:
		UInt32 expAddition;
	};

	// ��ʱ���ܣ��������籾����ùؿ��Ķ���������
	if (dungeonData->subType == DST_DEVILDDOM)
	{
		info.addedMonsterDropNum = SSActivityDungeonMgr::Instance()->GetDungeonBuyTimes(GetOwner(), dungeonData->subType);
	}

	EquipVisitor visitor;
	GetOwner()->VisitItems(PACK_WEAR, visitor);
	info.addition.additionRate[DART_EXP_RATE] += visitor.expAddition / 10;

	if (dungeonData->subType == DST_ZJSL_TOWER)
	{
		info.raceInfo.remainHp = GetOwner()->GetCounterMgr().GetCounter(COUNTER_ZJSL_PLAYER_HP);
		info.raceInfo.remainMp = GetOwner()->GetCounterMgr().GetCounter(COUNTER_ZJSL_PLAYER_MP);
	}

	return info;
}

void DungeonMgr::RefreshGuildTerrDungeonTimes() const
{
	UInt32 occupyTerrTime = GuildBattleTimeDataEntryMgr::Instance()->GetGuildCrossBattleEndTime(CURRENT_TIME);

	if (GetOwner()->GetCounterMgr().GetCounterUpdateTime(COUNTER_GUILD_TERR_DUNGEON_TIMES) < occupyTerrTime)
	{
		GetOwner()->SetCounter(COUNTER_GUILD_TERR_DUNGEON_TIMES, 0);
	}
}

bool DungeonMgr::CanContinuePlayThisMap(UInt32 dungeonId)
{
    // ���ĺͱ���Ĺؿ���һ��
    UInt32 savedDungeonId = GetOwner()->GetCounterMgr().GetCounter(COUNTER_TOWER);
    if (savedDungeonId != dungeonId)
    {
        ErrorLogStream << "player(" << GetOwner()->GetID() << ") wants continue play dungeon(" << dungeonId
                        << ") but saved dungeon is (" << savedDungeonId << ")." << LogStream::eos;
        return false;
    }

    return true;
}

void DungeonMgr::OnDungeonRaceEnd(UInt32 dungeonId, LevelScore score, UInt16 beHitCount, UInt16 enteredAreaCount, UInt32 usedTime, UInt16 reviveCount, UInt32 endAreaId, bool isAssist, std::vector<ObjID_t> teammateIds)
{
	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonData)
	{
		return;
	}

	// ֻ�н��������ͼ���ڽ�����¼��ʱ
	// �������磬ʤ��֮��Ҫ������ڴ�ı��
	if (dungeonData->subType == DST_DEVILDDOM && score > LS_C)
	{
		std::vector<UInt32> param = { GetNormalHardDungeonID(dungeonId) };
		GetOwner()->GetCounterMgr().SetCounterDynamic(COUNTER_DUNGEON_PROCESS_PREFIX, 0, param);

		// ֪ͨWorld���øõ��³ǵķ���Index
		CLProtocol::WorldDungeonNotifyResetAreaIndex notify;
		notify.roleId = GetOwner()->GetID();
		notify.dungeonId = GetNormalHardDungeonID(dungeonId);
		Router::SendToWorld(notify);

		InfoLogStream << "player(" << GetOwner()->GetID() << ") finish devilddom dungeon(" << dungeonId << ")." << LogStream::eos;
	}
	else if (dungeonData->onlyRaceEndSettlement && !isAssist && score > LS_C)
	{
		TryRecordDailyCount(dungeonId);
		TryRecordWeekCount(dungeonId);
	}

	// C���Ͼ���ͨ���ˣ�ֻ��ͨ����Ҫ��¼
	if (score <= LS_C)
	{
		auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
		if (dungeonData->type == LT_TOWER)
		{
			UInt32 floor = Player::GetTowerFloorByAreaID(dungeonId, endAreaId);
			if (floor > 0)
			{
				GetOwner()->OnDeathTower(dungeonId, floor - 1, usedTime);
			}
			
		}
		if (dungeonData->subType == DST_WEEK_HELL) //�ܳ���Ԩ
		{
			GetOwner()->OnWeekHellFail(*dungeonData);
		}

		return;
	}

	// ��¼�ؿ���Ϣ
	RecordDungeonInfo(dungeonId, score, usedTime);

	// ��¼�˺�ͨ�ش���
	AddAccountDailyCount(dungeonId);

	// �������Ԩ���³ǣ��ټ�¼����ڵ���Ϣ
	
	if (dungeonData && dungeonData->IsHellDungeon())
	{
		UInt32 hellEntryDungeonId = DungeonDataEntryMgr::Instance()->GetHellEntry(dungeonData->hellSplitLevel, dungeonData->hardType);
		RecordDungeonInfo(hellEntryDungeonId, score, usedTime);
	}
	else{
		UInt32 entryId = DungeonDataEntryMgr::Instance()->GetDungeonEntryIdByDungeonId(dungeonId);
		if (entryId)
		{
			RecordDungeonInfo(entryId, score, usedTime);
		}
	}

	// �����ܷ������һ�Ѷ�
	TryToUnlockNextHard(dungeonId, score, beHitCount, reviveCount);
	GetOwner()->OnClearDungeon(dungeonId, beHitCount, enteredAreaCount, score, usedTime, reviveCount, endAreaId, teammateIds, isAssist);

	// ��ǩ�������������֮���������ĵ��³Ƕ���¼
	if (dungeonData->type != LT_TOWER)
	{
		GetOwner()->GetActiveTaskMgr().WeekSignTrigger(WEEK_SIGN_ACT_DUNGEON);
	}

	//������Ӣ���³�
	if (score == LS_SSS && dungeonData->type == LT_NORMAL && dungeonData->subType == DST_NORMAL
		&& dungeonData->threeType == DTT_NORMAL)
	{
		SyncDungeonList();
	}
}

void DungeonMgr::ClearDungeonHardInfo()
{
	for (auto itr : m_dungeonHardInfoes)
	{
		delete itr.second;
	}
	m_dungeonHardInfoes.clear();
}

bool DungeonMgr::LoadDungeonHardInfo(DungeonHardInfo* hard)
{
    if (!hard)
    {
        return false;
    }

    if (FindDungeonHardInfoById(hard->GetDungeonID()))
    {
        ErrorLogStream << "player(" << GetOwner()->GetID() << ") has repeat dungeon(" << hard->GetDungeonID() << ") hard data." << LogStream::eos;
        return false;
    }

    m_dungeonHardInfoes[hard->GetDungeonID()] = hard;
    return true;
}

DungeonHardInfo* DungeonMgr::FindDungeonHardInfoById(UInt32 dungeonId)
{
    auto itr = m_dungeonHardInfoes.find(dungeonId);
    return itr == m_dungeonHardInfoes.end() ? NULL : itr->second;
}

UInt32 DungeonMgr::GetDailyCount(UInt32 dungeonId) const
{
	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonData || SSActivityDungeonMgr::Instance()->GetDungeonMaxTimes(GetOwner(), dungeonData->subType) == 0)
	{
		return 0;
	}

	auto dungeonTimesData = DungeonTimesDataEntryMgr::Instance()->FindEntry(dungeonData->subType);
	if (!dungeonTimesData)
	{
		return 0;
	}

	auto dailyCount = GetOwner()->GetCounter(dungeonTimesData->usedTimesCounter.c_str());
	return dailyCount + 1;
}

UInt32 DungeonMgr::GetDailyCountBySubtype(DungeonSubType subType) const
{
	auto dungeonTimesData = DungeonTimesDataEntryMgr::Instance()->FindEntry(subType);
	if (!dungeonTimesData)
	{
		return 0;
	}

	auto dailyCount = GetOwner()->GetCounter(dungeonTimesData->usedTimesCounter.c_str());
	return dailyCount;
}

UInt32 DungeonMgr::GetTotalCount(UInt32 dungeonId) const
{
	auto dungeon = FindDungeonByDataId(dungeonId);
	if (dungeon)
	{
		return dungeon->GetNum();
	}
	return 0;
}

UInt32 DungeonMgr::GetRemainTimes(UInt32 dungeonId) const
{
	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonData)
	{
		return 0;
	}

	auto maxTime = SSActivityDungeonMgr::Instance()->GetDungeonMaxTimes(GetOwner(), dungeonData->subType);
	if (maxTime == 0)
	{
		return 0;
	}

	auto dayCount = GetDailyCount(dungeonId);
	if (dayCount > maxTime)
	{
		return 0;
	}

	return maxTime - dayCount + 1;
}

UInt32 DungeonMgr::TryRecordDailyCount(UInt32 dungeonId)
{
	UInt32 entryId = DungeonDataEntryMgr::Instance()->GetDungeonEntryIdByDungeonId(dungeonId);
	dungeonId = (entryId == 0) ? dungeonId : entryId;

	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonData || SSActivityDungeonMgr::Instance()->GetDungeonMaxTimes(GetOwner(), dungeonData->subType) == 0)
	{
		return 0;
	}

	auto dungeonTimesData = DungeonTimesDataEntryMgr::Instance()->FindEntry(dungeonData->subType);
	if (!dungeonTimesData)
	{
		return 0;
	}

	auto dailyCount = GetOwner()->GetCounter(dungeonTimesData->usedTimesCounter.c_str());
	GetOwner()->IncCounter(dungeonTimesData->usedTimesCounter.c_str(), 1);

	// ֪ͨWorld���øõ��³ǵķ���Index
// 	CLProtocol::WorldDungeonNotifyResetAreaIndex notify;
// 	notify.roleId = GetOwner()->GetID();
// 	notify.dungeonId = dungeonId;
// 	Router::SendToWorld(notify);

	// ����ж��飬ͬ��������world
	if (GetOwner()->GetTeam())
	{
		SyncRemainTimes2World(dungeonId);
	}

	return dailyCount + 1;
}

void DungeonMgr::SyncRemainTimes2World(UInt32 dungeonId) const
{
	auto teamTarget = TeamDungeonDataEntryMgr::Instance()->FindEntryByDungeonID(dungeonId);
	if (!teamTarget)
	{
		return;
	}

	CLProtocol::WorldTeamQueryTargetCondRes res;
	res.roleId = GetOwner()->GetID();
	res.teamTargetId = teamTarget->id;
	res.remainTimes = GetRemainTimes(dungeonId);
	Router::SendToWorld(res);
}

UInt32 DungeonMgr::TryRecordWeekCount(UInt32 dungeonId)
{
	UInt32 entryId = DungeonDataEntryMgr::Instance()->GetDungeonEntryIdByDungeonId(dungeonId);
	dungeonId = (entryId == 0) ? dungeonId : entryId;

	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonData || SSActivityDungeonMgr::Instance()->GetDungeonWeekMaxTimes(GetOwner(), dungeonData->subType) == 0)
	{
		return 0;
	}
	auto dungeonTimesData = DungeonTimesDataEntryMgr::Instance()->FindEntry(dungeonData->subType);
	if (!dungeonTimesData)
	{
		return 0;
	}
	auto weekCount = GetOwner()->GetCounter(dungeonTimesData->weekUsedTimesCounter.c_str());
	GetOwner()->IncCounter(dungeonTimesData->weekUsedTimesCounter.c_str(), 1);

	return weekCount + 1;
}

void DungeonMgr::RecordDungeonInfo(UInt32 dungeonId, LevelScore score, UInt32 usedTime)
{
    bool isSyncToPlayer = false;
    bool isNewDungeon = false;
    Dungeon* dungeon = FindDungeonByDataId(dungeonId);
    if (!dungeon)
    {
        isNewDungeon = true;
        isSyncToPlayer = true;
        dungeon = Dungeon::CreateDungeon(dungeonId);
        if (!dungeon)
        {
            ErrorLogStream << "player(" << GetOwner()->GetID() << ") create dungeon(" << dungeonId << ") instance failed." << LogStream::eos;
            return;
        }

        dungeon->SetID(CLRecordClient::Instance()->GenGuid());
        m_dungeons[dungeonId] = dungeon;
    }

    dungeon->IncNum();
    if (score > dungeon->GetBestScore())
    {
        isSyncToPlayer = true;
        dungeon->SetBestScore(score);
    }

    if (dungeon->GetBestRecordTime() == 0 || usedTime < dungeon->GetBestRecordTime())
    {
        isSyncToPlayer = true;
        dungeon->SetBestRecordTime(usedTime);
    }

    CLSaveCommand* cmd = NULL;
    if (isNewDungeon)
    {
        cmd = CLRecordClient::Instance()->PrepareInsertCommand(GetOwner()->GetSrcZoneId(), "t_dungeon", dungeon->GetID());
    }
    else
    {
        cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_dungeon", dungeon->GetID(), true);
    }
    
    dungeon->GetDBData(cmd, false);
    cmd->PutData("owner", GetOwner()->GetID());
    CLRecordClient::Instance()->SendCommand(cmd, NULL, GetOwner()->GetID());

    if (isSyncToPlayer)
    {
        CLProtocol::SceneDungeonUpdate update;
        update.info = dungeon->GetSceneDungeonInfo();

        GetOwner()->SendProtocol(update);
    }
}

void DungeonMgr::RecordDungeonHardInfo(UInt32 dungeonId, DungeonHardType type)
{
    if (type == LHT_NORMAL)
    {
        return;
    }

	bool isNewDungeon = false;
    DungeonHardInfo* hard = FindDungeonHardInfoById(dungeonId);
    if (!hard)
    {
        isNewDungeon = true;
        hard = DungeonHardInfo::CreateDungeonHardInfo(dungeonId);
        if (!hard)
        {
            ErrorLogStream << "player(" << GetOwner()->GetID() << ") create dungeon(" << dungeonId << ") hard info instance failed." << LogStream::eos;
            return;
        }

        hard->SetDungeonID(dungeonId);
        m_dungeonHardInfoes[dungeonId] = hard;
    }

    if (type < hard->GetUnlockedHardType() && !isNewDungeon)
    {
        return;
    }

    hard->SetUnlockedHardType(type);

	// ͬ�����ͻ���
	CLProtocol::SceneDungeonHardUpdate update;
	update.info = hard->GetSceneDungeonHardInfo();
	GetOwner()->SendProtocol(update);

	// ͬ����world
	CLProtocol::WorldDungeonHardUpdate updateWorld;
	updateWorld.accid = GetOwner()->GetAccID();
	updateWorld.info = hard->GetSceneDungeonHardInfo();
	Router::SendToWorld(updateWorld);
}

void DungeonMgr::TryToUnlockNextHard(UInt32 dungeonId, LevelScore score, UInt16 beHitCount, UInt16 reviveCount)
{
    DungeonDataEntry* data = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
    if (!data || data->type == LT_STORY)
    {
        return;
    }

    DungeonHardType nextHard = (DungeonHardType)((int)data->hardType + 1);
    if (nextHard >= LHT_NUM)
    {
        return;
    }

	// �������Ԩ��Ҫ������Ԩ��ڵ��Ѷ�
	if (data->subType == DST_HELL)
	{
		dungeonId = DungeonDataEntryMgr::Instance()->GetHellEntry(data->hellSplitLevel,data->hardType);
	}

    UInt32 normalDungeonId = GetNormalDungeonID(dungeonId);
	UInt32 nextHardDungeonId = GetDungeonIDByHard(normalDungeonId, nextHard);
	auto nextHardDungeonData = DungeonDataEntryMgr::Instance()->FindEntry(nextHardDungeonId);
	if (!nextHardDungeonData)
	{
		return;
	}

    DungeonHardInfo* hardInfo = FindDungeonHardInfoById(normalDungeonId);
    if (hardInfo && hardInfo->GetUnlockedHardType() >= nextHard)
    {
        return;
    }

    if (!CanUnlockThisHard(nextHard, score, beHitCount, reviveCount))
    {
        return;
    }

    RecordDungeonHardInfo(normalDungeonId, nextHard);
}

bool DungeonMgr::CanUnlockThisHard(DungeonHardType type, LevelScore score, UInt16 beHitCount, UInt16 reviveCount)
{
    // �ж��ܷ���һ�Ѷ�
    /*if (type == LHT_RISK)
    {
        if (score >= LS_A)
        {
            return true;
        }
    }
    else if(type == LHT_WARRIOR)
    {
        if (score >= LS_S && reviveCount == 0)
        {
            return true;
        }
    }
    else if (type == LHT_KING)
    {
		if (score >= LS_S && beHitCount <= 20 && reviveCount == 0)
        {
            return true;
        }
    }*/

	if (score >= LS_S)
	{
		return true;
	}

    return false;
}

bool DungeonMgr::IsMatchDungeonOpenCond(UInt32 dungeonId, bool checkStoryTask)
{
	// �ж�����ͼ�ܲ��ܴ�
	DungeonDataEntry* data = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!data)
	{
		return false;
	}

	UInt32 normalDungeonId = GetNormalHardDungeonID(dungeonId);
	DungeonDataEntry* normalDungeonData = DungeonDataEntryMgr::Instance()->FindEntry(normalDungeonId);
	if (!normalDungeonData)
	{
		return false;
	}

	// �ȼ�Ҫ��
	if (GetOwner()->GetLevel() < normalDungeonData->minLevel)
	{
		return false;
	}

	// ǰ�õ��³�
	if (normalDungeonData->prevDungeonId && !IsDungeonAnyHardPassed(normalDungeonData->prevDungeonId))
	{
		return false;
	}

	// ǰ���½�
	if (normalDungeonData->eliteDungeonPrevChapter && !IsChapterAnyHardPassedSpecScore(normalDungeonData->eliteDungeonPrevChapter, LS_SSS))
	{
		return false;
	}

	// ����Ǿ���ؿ�����Ҫ�жϾ������񣬱�����˾���������δ��ɲ��ܴ�����ؿ�
	if (normalDungeonData->type == LT_STORY && normalDungeonData->storyTaskId)
	{
		do
		{
			// ����Ѿ��������ؿ����Ǿ���Ϊ���Դ�
			if (!checkStoryTask && IsDungeonAnyHardPassed(normalDungeonId))
			{
				break;
			}

			UInt32 storyTaskId = normalDungeonData->storyTaskId;
			auto task = GetOwner()->GetTaskMgr().FindTask(storyTaskId);
			if (!task || task->GetStatus() != TASK_UNFINISH)
			{
				return false;
			}
		} while (0);
	}

	// ǰ�����񣬱������
	if (normalDungeonData->preTaskId)
	{
		//�ܳ���Ԩ���,ǰ��������ɼ���
		if (normalDungeonData->subType == DST_WEEK_HELL_ENTRY)
		{
			UInt32 preTaskId = normalDungeonData->preTaskId;

			auto task = GetOwner()->GetTaskMgr().FindTask(preTaskId);
			if (!task && !GetOwner()->CheckTaskFinished(normalDungeonData->preTaskId))
			{
				return false;
			}
			if (task && task->GetStatus() != TASK_FINISHED 
				&& task->GetStatus() != TASK_SUBMITTING 
				&& task->GetStatus() != TASK_OVER)
			{
				return false;
			}
		}
		else if (!GetOwner()->CheckTaskFinished(normalDungeonData->preTaskId))
		{
			return false;
		}
	}

	return true;
}

bool DungeonMgr::IsDungeonOpened(UInt32 dungeonId)
{
    return m_openedDungeonList.find(dungeonId) != m_openedDungeonList.end();
}

bool DungeonMgr::IsDungeonHellModeOpened(UInt32 dungeonId)
{
    auto itr = m_openedDungeonList.find(dungeonId);
    return (itr == m_openedDungeonList.end()) ? (false) : (itr->second.hellMode != 0);
}

bool DungeonMgr::IsDungeonOpenHellMode(UInt32 dungeonId)
{
    DungeonDataEntry* dungeon = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
    if (!dungeon)
    {
        return false;
    }

    if (!dungeon->IsHellEntry())
    {
        return false;
    }

    // �ж���Ԩ������û�����
    if (dungeon->hellTask > 0 && !GetOwner()->CheckTaskFinished(dungeon->hellTask))
    {
        return false;
    }

    return true;
}

// ��ȡ��ͨ�Ѷȵĵ��³�ID
UInt32 DungeonMgr::GetNormalHardDungeonID(UInt32 dungeonId)
{
    // ��ʱ�ȸ���ID��ֱ�Ӽ������
    // ID�ĸ�λ���Ѷȱ���
    return dungeonId / 10 * 10;
}

// ��ȡ��ͨ���³�ID
UInt32 DungeonMgr::GetNormalDungeonID(UInt32 dungeonId)
{
    UInt32 normalHardDungeonId = GetNormalHardDungeonID(dungeonId);
    // ��ʱ�ȸ���ID��ֱ�Ӽ������
    // ID��ʮλ�Ͱ�λ�Ǿ���ؿ�����
    return normalHardDungeonId / 1000 * 1000;
}

// ��ȡǰ�þ�����³�ID
UInt32 DungeonMgr::GetPrevStoryDungeonID(UInt32 dungeonId)
{
    UInt32 normalHardDungeonId = GetNormalHardDungeonID(dungeonId);
    UInt32 normalDungeonId = GetNormalDungeonID(dungeonId);

    DungeonDataEntry* dungeonEntry = DungeonDataEntryMgr::Instance()->FindEntry(normalDungeonId);
    if (!dungeonEntry)
    {
        ErrorLogStream << "can't find dungeon(" << dungeonId << ")'s normal dungeon(" << normalDungeonId << ")." << LogStream::eos;
        return 0;
    }

    for (UInt32 i = 0; i < dungeonEntry->storyDungeonIds.size(); i++)
    {
        if (dungeonEntry->storyDungeonIds[i] != normalHardDungeonId)
        {
            continue;
        }

        if (i == 0)
        {
            return 0;
        }

        return dungeonEntry->storyDungeonIds[i - 1];
    }

    ErrorLogStream << "dungeon(" << dungeonId << ") not in normal dungeon(" << normalDungeonId << ")'s story dungeon list." << LogStream::eos;
    return 0;
}

UInt32 DungeonMgr::GetDungeonIDByHard(UInt32 normalHardDungeonId, DungeonHardType type)
{
    // ��ʱ�ȸ���ID��ֱ�Ӽ������
    // ID�ĸ�λ���Ѷȱ���
    return normalHardDungeonId / 10 * 10 + (UInt32)type;
}

Dungeon* DungeonMgr::FindDungeonByDataId(UInt32 dataId) const
{
    auto itr = m_dungeons.find(dataId);
    return itr == m_dungeons.end() ? NULL : itr->second;
}

bool DungeonMgr::IsDungeonPassed(UInt32 dungeonId)
{
    Dungeon* dungeon = FindDungeonByDataId(dungeonId);
    if (!dungeon)
    {
        return false;
    }

    return dungeon != NULL;
}

bool DungeonMgr::IsDungeonPassedSpecScore(UInt32 dungeonId, LevelScore score)
{
	Dungeon* dungeon = FindDungeonByDataId(dungeonId);
	if (!dungeon)
	{
		return false;
	}

	return dungeon->GetBestScore() >= score;
}

bool DungeonMgr::IsDungeonAnyHardPassed(UInt32 dungeonId)
{
    for (int hard = LHT_NORMAL; hard < LHT_NUM; ++hard)
    {
        if (IsDungeonPassed(GetDungeonIDByHard(dungeonId, (DungeonHardType)hard)))
        {
            return true;
        }
    }

    return false;
}

bool DungeonMgr::IsChapterAnyHardPassedSpecScore(UInt32 chapter, LevelScore score)
{
	std::vector<UInt32> dungeonIds;
	if (!DungeonDataEntryMgr::Instance()->GetDungeonIdsByChapterHard(chapter, LHT_NORMAL, dungeonIds))
	{
		return false;
	}
	if (dungeonIds.size()==0)
	{
		return false;
	}

	for (auto dungeonId : dungeonIds)
	{
		if (!IsDungeonAnyHardPassedSpecScore(dungeonId, score))
		{
			return false;
		}	
	}
	return true;
}

bool DungeonMgr::IsDungeonAnyHardPassedSpecScore(UInt32 dungeonId, LevelScore score)
{
	for (int hard = LHT_NORMAL; hard < LHT_NUM; ++hard)
	{
		if (IsDungeonPassedSpecScore(GetDungeonIDByHard(dungeonId, (DungeonHardType)hard), score))
		{
			return true;
		}
	}
	return false;
}

bool DungeonMgr::LoadDungeon(Dungeon* dungeon)
{
    if (!dungeon)
    {
        return false;
    }

    if (FindDungeonByDataId(dungeon->GetDataID()))
    {
        ErrorLogStream << "player(" << GetOwner()->GetID() << ") has repeat dungeon(" << dungeon->GetDataID() << ") data." << LogStream::eos;
        delete dungeon;
        return false;
    }

    m_dungeons[dungeon->GetDataID()] = dungeon;
    return true;
}

void DungeonMgr::Input(Avalon::NetInputStream &stream)
{
    UInt32 num = 0;
    stream & num;

    for (UInt32 i = 0; i < num; i++)
    {
        UInt64 id = 0;
        stream & id;

        UInt32 dungeonId = 0;
        stream & dungeonId;

        //auto dungeon = Dungeon::CreateDungeon(dungeonId);
        //if (!dungeon)
        //{
        //    ErrorLogStream << "player(" << GetOwner()->GetID() << ") create dungeon(" << dungeonId << ") failed." << LogStream::eos;
        //    continue;
        //}

        //dungeon->SetID(id);
        //dungeon->SetDataID(dungeonId);
        //dungeon->Decode(stream);
    }

    stream & num;
    for (UInt32 i = 0; i < num; i++)
    {
        UInt64 id = 0;
        stream & id;

        UInt32 dungeonId = 0;
        stream & dungeonId;

        //auto hard = DungeonHardInfo::CreateDungeonHardInfo(dungeonId);
        //if (!hard)
        //{
        //    ErrorLogStream << "player(" << GetOwner()->GetID() << ") create dungeon(" << dungeonId << ") hard info failed." << LogStream::eos;
        //    continue;
        //}

        //hard->SetID(id);
        //hard->SetDungeonID(dungeonId);
        //hard->Decode(stream);
    }
}

void DungeonMgr::Output(Avalon::NetOutputStream &stream)
{
    stream & (UInt32)m_dungeons.size();

    for (auto itr : m_dungeons)
    {
        auto dungeon = itr.second;
        if (!dungeon)
        {
            continue;
        }

        stream & dungeon->GetID() & dungeon->GetDataID();
        stream & dungeon->Encode(stream, SST_SCENE);
    }

    stream & (UInt32)m_dungeonHardInfoes.size();

    for (auto itr : m_dungeonHardInfoes)
    {
        auto hard = itr.second;
        if (!hard)
        {
            continue;
        }

        stream & hard->GetID() & hard->GetDungeonID();
        stream & hard->Encode(stream, SST_SCENE);
    }
}

void DungeonMgr::_GenNewOpenDungeonList(std::vector<DungeonOpenInfo>& newOpenList, std::vector<UInt32>& newCloseList)
{
    class DungeonDataVistor : public Avalon::DataEntryVisitor<DungeonDataEntry>
    {
    public:
        bool operator()(DungeonDataEntry *dataEntry)
        {
            datas.push_back(dataEntry);
            return true;
        }

        std::vector<DungeonDataEntry*> datas;
    };

    DungeonDataVistor vistor;
    DungeonDataEntryMgr::Instance()->Visit(vistor);
    for (auto data : vistor.datas)
    {
        if (!data)
        {
            continue;
        }

        // ֻ������ͨ�Ѷȵ�
        if (data->hardType != LHT_NORMAL)
        {
            continue;
        }

		bool open = IsMatchDungeonOpenCond(data->id, true);
        bool openHell = IsDungeonOpenHellMode(data->id);
        if (open == IsDungeonOpened(data->id) && openHell == IsDungeonHellModeOpened(data->id))
        {
            continue;
        }

        if (open)
        {
            DungeonOpenInfo info;
            info.id = data->id;
            info.hellMode = openHell ? 1 : 0;
            newOpenList.push_back(info);
        }
        else if(!open)
        {
            newCloseList.push_back(data->id);
        }
    }
}

void DungeonMgr::CheckMallGiftActivateCondition(UInt32 canPlayMapRet, DungeonSubType subType)
{
	switch (canPlayMapRet)
	{
	case (UInt32)ErrorCode::DUNGEON_START_NO_FATIGUE: 
		ActivateFatigue();
		break;

	case (UInt32)ErrorCode::DUNGEON_START_NO_TICKET:
		ActivateTicket(subType);
		break;

	default:
		break;
	}
}

void DungeonMgr::ActivateFatigue()
{
	if (!GetOwner()->HasMonthCard())
	{
		return;
	}

	UInt32 smallFatigueNum = GetOwner()->GetItemNum(800000035);
	UInt32 middleFatigueNum = GetOwner()->GetItemNum(800000084);
	UInt32 largeFatigueNum = GetOwner()->GetItemNum(800000085);

	UInt32 totalNum = smallFatigueNum + middleFatigueNum + largeFatigueNum;
	if (totalNum == 0)
	{
		GetOwner()->ActivateMallGiftPackReq(MGPAC_NO_FATIGUE);
	}
}

void DungeonMgr::ActivateTicket(DungeonSubType subType)
{
	switch (subType)
	{
	case DST_YUANGU: 
		GetOwner()->ActivateMallGiftPackReq(MGPAC_NO_ANCIENT_TICKET);
		break;
		
	case DST_HELL_ENTRY:
		GetOwner()->ActivateMallGiftPackReq(MGPAC_NO_HELL_TICKET);
		break;

	default:
		break;
	}
}

UInt32 DungeonMgr::AdjustDungeonId(UInt32 dungeonId)
{
	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonData)
	{
		ErrorLogStream << "player(" << GetOwner()->GetID() << ") want to start a invalid dungeon(" << dungeonId << ")." << LogStream::eos;
		return 0;
	}

	if (dungeonData->subType == DST_WEEK_HELL)
	{
		UInt32 entryId = DungeonDataEntryMgr::Instance()->GetDungeonEntryIdByDungeonId(dungeonId);

		return (entryId != 0) ? entryId : dungeonId;
	}

	return dungeonId;
}


UInt32 DungeonMgr::GetDungeonBestScore(UInt32 dungeonId)
{
	LevelScore score = LevelScore::LS_C;

	while (true)
	{
		Dungeon* dungeon = FindDungeonByDataId(dungeonId);
		if (dungeon == NULL)
		{
			break;
		}

		if (score < dungeon->GetBestScore())
		{
			score = dungeon->GetBestScore();
		}

		if (score >= LevelScore::LS_SSS || score == LevelScore::LS_C)
		{
			break;
		}

		DungeonDataEntry* data = dungeon->GetDataEntry();
		if (data == NULL)
		{
			break;
		}

		DungeonHardType hardType = (DungeonHardType)(data->hardType + 1);
		if (hardType >= DungeonHardType::LHT_NUM)
		{
			break;
		}

		dungeonId = GetDungeonIDByHard(dungeonId, hardType);
	}

	return (UInt32)score;
}

void DungeonMgr::SetChapterPass(UInt8 chapter)
{
	std::vector<UInt32> dungeonIds;
	if (!DungeonDataEntryMgr::Instance()->GetDungeonIdsByChapterHard(chapter, LHT_NORMAL, dungeonIds))
	{
		return;
	}
	if (dungeonIds.size() == 0)
	{
		return;
	}
	for (auto dungeonId : dungeonIds)
	{
		UInt32 dungeon = GetDungeonIDByHard(dungeonId, LHT_NORMAL);
		if (IsDungeonPassedSpecScore(dungeon, LS_SSS))
		{
			continue;
		}
		RecordDungeonInfo(dungeon, LS_SSS, 120);
	}
}

void DungeonMgr::PassAllChapterDungeon()
{
	std::map<DungeonChapterKey, std::vector<UInt32>>& idMap = DungeonDataEntryMgr::Instance()->GetAllChapterIds();
	for (auto it : idMap)
	{
		std::vector<UInt32>& dungeonIds = it.second;
		for (auto dungeonId : dungeonIds)
		{
			RecordDungeonHardInfo(dungeonId, LHT_KING);
		}
	}
}

UInt32 DungeonMgr::GetAccountDailyCount(UInt32 dungeonId) const
{
	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonData)
	{
		return 0;
	}

	auto dungeonTimesData = DungeonTimesDataEntryMgr::Instance()->FindEntry(dungeonData->subType);
	if (!dungeonTimesData || dungeonTimesData->accountDailyTimesLimit <= 0)
	{
		return 0;
	}

	auto dailyCount = OpActivityRecordMgr::Instance()->GetRecordValue(OP_ACTIVITY_BEHAVIOR_DAILY_DUNGEON_COUNT, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), dungeonData->subType);
	return dailyCount + 1;
}

UInt32 DungeonMgr::AddAccountDailyCount(UInt32 dungeonId)
{
	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonData)
	{
		return 0;
	}

	auto dungeonTimesData = DungeonTimesDataEntryMgr::Instance()->FindEntry(dungeonData->subType);
	if (!dungeonTimesData || dungeonTimesData->accountDailyTimesLimit <= 0)
	{
		return 0;
	}

	OpActivityRecordMgr::Instance()->UpdateRecord(OP_ACTIVITY_BEHAVIOR_DAILY_DUNGEON_COUNT, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), dungeonData->subType, 1);
	return 1;
}

UInt32 DungeonMgr::GetAccountDailyRemainTimes(UInt32 dungeonId) const
{
	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonData)
	{
		return 10000;
	}

	auto dungeonTimesData = DungeonTimesDataEntryMgr::Instance()->FindEntry(dungeonData->subType);
	if (!dungeonTimesData || dungeonTimesData->accountDailyTimesLimit <= 0)
	{
		return 10000;
	}

	auto dailyCount = OpActivityRecordMgr::Instance()->GetRecordValue(OP_ACTIVITY_BEHAVIOR_DAILY_DUNGEON_COUNT, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, GetOwner()->GetAccID(), dungeonData->subType);
	if (dungeonTimesData->accountDailyTimesLimit <= dailyCount)
	{
		return 0;
	}
	
	return (dungeonTimesData->accountDailyTimesLimit - dailyCount);
}


