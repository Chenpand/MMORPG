#include "TaskScript.h"
#include "Player.h"
#include <CLTaskDefine.h>
#include "ScriptUtil.h"

TaskScript::TaskScript(UInt32 id, Avalon::ScriptState* state)
	:Avalon::ScriptFile(state)
{
	SetId(id);
	m_TaskParams = "";
	m_LevelupEvent = 0;
	m_uWudaodahuiWinNumEvent = 0;
	m_bLevelNpcCond = false;
	m_bEnterTribeEvnet = false;
	m_TaskSuccId = m_TaskFailedId = 0;
	m_bDiedEvent = false;
	m_TaskSubmittedType = -1;	//by huchenhui 0表示日常任务 不能使用
	m_uCompleteTime = 0;
	m_bStrengthenEquipEvent = false;
	m_bDecomposeEquipEvent = false;
	m_AddRetinueEvent = RETINUE_NUM;
	m_bChangeOccuEvent = false;
	m_bAddMagicEvent = false;
	m_bFriendGiveEvent = false;
	m_bRefreshShopEvent = false;
	m_uUsedFatigue = -1;
	m_bAwakenEvent = false;
	m_bAddEnergyEvent = false;
	m_bReviveCoinEvent = false;
	m_bAddMagicCardEvent = false;
	m_bAddAuctionEvent = false;
	m_bSellAuctionEvent = false;
	m_bBuyAuctionEvent = false;
	m_bDeathTowerWipeoutBeginEvent = false;
	m_bAddPreciousBeadEvent = false;
	m_bSendHornEvent = false;
	m_bJoinGuildEvent = false;
	m_bIncGuildBattleEvent = false;
	m_bEnlargePackageEvent = false;
	m_bEnlargeStorageEvent = false;
	m_bIncIntimacyEvent = false;
	m_bIncIDailyTaskScoreEvent = false;
	m_bApprentDiscipleEvent = false;
	m_bDiscipleFinschEvent = false;
	m_bGuildEmblemEvent = false;
}

TaskScript::~TaskScript()
{
}

void TaskScript::BindGetItemEvent(UInt32 id, UInt8 quality, UInt8 strengthen)
{
	GetItemEvent	gievent;
	gievent.id = id;
	gievent.quality = quality;
	gievent.strengthen = strengthen;
	m_getItemEvents.push_back(gievent);
}

void TaskScript::BindGetItemQualityEvent(UInt32 id, UInt8 quality)
{
	GetItemEvent	gievent;
	gievent.id = id;
	gievent.quality = quality;
	m_getItemEvents.push_back(gievent);
}

void TaskScript::BindGetItemStrengthenEvent(UInt32 id, UInt8 strengthen)
{
	GetItemEvent	gievent;
	gievent.id = id;
	gievent.strengthen = strengthen;
	m_getItemEvents.push_back(gievent);
}

bool TaskScript::CheckGetItemEvent(UInt32 id, UInt8 quality, UInt8 strengthen) const
{
	for(GetItemEventVec::const_iterator iter = m_getItemEvents.begin();
		iter != m_getItemEvents.end(); ++iter)
	{
		if((iter->id == id || iter->id == 0)
			&& (ItemQuality::ITEM_QUALITY_INVLID == iter->quality || iter->quality <= quality)
			&& (0 == iter->strengthen || iter->strengthen <= strengthen))
			return true;
	}
	return false;
}

//added by aprilliu, 2016.04.18
void TaskScript::BindRmItemEvent(UInt32 itemId)
{
	this->BindGetItemEvent(itemId);
}
//added by aprilliu, 2016.04.18
void TaskScript::BindItemChangeEvent(UInt32 itemId, UInt8 quality)
{
	this->BindGetItemQualityEvent(itemId, quality);
}
//added by aprilliu, 2016.04.18
bool TaskScript::CheckItemChangeEvent(UInt32 id, UInt8 quality, UInt8 strengthen) const
{
	return this->CheckGetItemEvent(id, quality, strengthen);
}


void TaskScript::BindKillNpcEvent(UInt32 id, UInt32 dungeon)
{
	//m_killNpcEvents.push_back(id);
	stKillNpcEvent event;
	event.id = id;
	event.type = 0;
	event.dungeon = dungeon; //added by aprilliu,2016.06.27

	m_killNpcEvents.push_back(event);
}

void TaskScript::BindKillNpcTypeEvent(UInt8 type, UInt32 dungeon)
{
	stKillNpcEvent event;
	event.id = 0;
	event.type = type;
	event.dungeon = dungeon; //added by aprilliu,2016.06.27

	m_killNpcEvents.push_back(event);
}

void TaskScript::BindKillLevelNpcEvent()
{
	m_bLevelNpcCond = true;
}

bool TaskScript::IsBindKillLevelNpcEvent() const
{
	return m_bLevelNpcCond;
}

bool TaskScript::CheckKillNpcEvent(UInt32 id, UInt8 type, UInt32 dungeon) const
{
	for (KillNPCEventList::const_iterator iter = m_killNpcEvents.begin();
		iter != m_killNpcEvents.end(); ++iter)
	{
		//by huchenhui 2016.09.09  0表示不限制
		if ((iter->dungeon == 0 || dungeon == iter->dungeon)
			&& (iter->type == 0 || iter->type == type)
			&& (iter->id == 0 || iter->id == id))
		{
			return true;
		}
	}
	return false;
}

void TaskScript::BindKillCityMonsterEvent(CityMonsterType cityMonsterType, UInt32 monsterId)
{
	KillCityMonsterEvent killCityMonsterEvent(cityMonsterType, monsterId);
	m_KillCityMonsterEvents.push_back(killCityMonsterEvent);
}

bool TaskScript::CheckKillCityMonsterEvent(CityMonsterType cityMonsterType, UInt32 monsterId, UInt64 monsterGuid) const
{
	for (KillCityMonsterEventList::const_iterator iter = m_KillCityMonsterEvents.begin(); iter != m_KillCityMonsterEvents.end(); ++iter)
	{
		if ((iter->monsterId == monsterId || iter->monsterId == 0)
			&& (iter->type == cityMonsterType || iter->type == CityMonsterType::CITY_MONSTER_INVALID))
		{
			return true;
		}
	}
	return false;
}

void TaskScript::BindUseItemEvent(UInt32 id)
{
	m_useItemEvents.push_back(id);
}

bool TaskScript::CheckUseItemEvent(UInt32 id) const
{
	for(IdVec::const_iterator iter = m_useItemEvents.begin();
		iter != m_useItemEvents.end(); ++iter)
	{
		if(*iter == id) return true;
	}
	return false;
}

void TaskScript::BindChangeMoneyEvent(UInt32 type)
{
	m_ChangeMoneyEvents.push_back(type);
}

bool TaskScript::CheckChangeMoneyEvent(UInt32 type) const
{
	for (IdVec::const_iterator iter = m_ChangeMoneyEvents.begin(); iter != m_ChangeMoneyEvents.end(); ++iter)
	{
		if (*iter == type) return true;
	}
	return false;
}

void TaskScript::BindTriggerZone(UInt32 mapid, UInt16 x1, UInt16 y1, UInt16 x2, UInt16 y2)
{
	CLZone zone;
	zone.mapid = mapid;
	zone.pos1.x = x1;
	zone.pos1.y = y1;
	zone.pos2.x = x2;
	zone.pos2.y = y2;
	m_TriggerZones.push_back(zone);
}

bool TaskScript::CheckInTriggerZone(UInt32 mapid, const CLPosition& pos)
{
	for(std::vector<CLZone>::iterator iter = m_TriggerZones.begin();
		iter != m_TriggerZones.end(); ++iter)
	{
		if(mapid == iter->mapid
			&& pos.x >= iter->pos1.x && pos.x <= iter->pos2.x
			&& pos.y >= iter->pos1.y && pos.y <= iter->pos2.y)
		return true;
	}
	return false;
}

void TaskScript::BindEnterZoneEvent(UInt32 mapid, UInt16 x1, UInt16 y1, UInt16 x2, UInt16 y2)
{
	CLZone zone;
	zone.mapid = mapid;
	zone.pos1.x = x1;
	zone.pos1.y = y1;
	zone.pos2.x = x2;
	zone.pos2.y = y2;
	m_BindZones.push_back(zone);
}

bool TaskScript::CheckInEventZone(UInt32 mapid, const CLPosition& pos)
{
	for(std::vector<CLZone>::iterator iter = m_BindZones.begin();
		iter != m_BindZones.end(); ++iter)
	{
		if(mapid == iter->mapid
			&& pos.x >= iter->pos1.x && pos.x <= iter->pos2.x
			&& pos.y >= iter->pos1.y && pos.y <= iter->pos2.y)
		return true;
	}
	return false;
}

void TaskScript::BindRemakeEquipEvent(UInt32 id, UInt8 type)
{
	EquipRemakeEvent remakeEvent;
	remakeEvent.id = id;
	remakeEvent.remakeType = type;
	m_remakeEquipEvents.push_back(remakeEvent);
}

bool TaskScript::CheckRemakeEquipEvent(UInt32 id, UInt8 type) const
{
	for(EquipRemakeEventVec::const_iterator iter = m_remakeEquipEvents.begin();
		iter != m_remakeEquipEvents.end(); ++iter)
	{
		if ((iter->id == 0 || iter->id == id)
			&& (iter->remakeType == 0 || type >= iter->remakeType))
		{
			return true;
		}
	}
	return false;
}

void TaskScript::BindStrengthenEquipEvent(bool isSuccess, UInt8 strengthen)
{
	m_StrengthenEquipEvent.isRegister = true;
	m_StrengthenEquipEvent.isSuccess = isSuccess;
	m_StrengthenEquipEvent.strengthen = strengthen;
}

bool TaskScript::CheckStrengthenEquipEvent(UInt32 errorCode, UInt8 oldStrengthen, UInt8 newStrengthen) const
{
	if (!m_StrengthenEquipEvent.isRegister) return false;
	if (errorCode == ErrorCode::SUCCESS
		&& (m_StrengthenEquipEvent.strengthen == 0 || m_StrengthenEquipEvent.strengthen <= newStrengthen))
	{
		return true;
	}

	//不关心是否成功
	if (m_StrengthenEquipEvent.isSuccess == false)
	{
		//不成功的时候 检查原先的强化等级是否满足要求
		if (errorCode != ErrorCode::SUCCESS
			&& (m_StrengthenEquipEvent.strengthen == 0 || m_StrengthenEquipEvent.strengthen <= oldStrengthen))
		{
			return true;
		}
	}

	return false;
}

void TaskScript::BindDecomposeEquipEvent()
{
	m_bDecomposeEquipEvent = true;
}

bool TaskScript::CheckDecomposeEquipEvent() const
{
	return m_bDecomposeEquipEvent;
}

void TaskScript::BindPutEquipEvent(UInt16 level, UInt8 color, UInt32 strengthen)
{
	PutEquipEvent putEquipEvent;
	putEquipEvent.level = level;
	putEquipEvent.color = color;
	putEquipEvent.strengthen = strengthen;
	m_PutEquipEvents.push_back(putEquipEvent);
}

bool TaskScript::CheckPutEquipEvent(UInt16 level, UInt8 color, UInt32 strengthen) const
{
	for (PutEquipEventList::const_iterator iter = m_PutEquipEvents.begin(); iter != m_PutEquipEvents.end();++iter)
	{
		if ((iter->color == 0 || iter->color == color)
			&& iter->strengthen <= strengthen 
			&& iter->level <= level)
		{
			return true;
		}
	}
	return false;
}

void TaskScript::BindPutFashionEvent(ItemSubType subType, UInt8 color)
{
	PutFashionEvent putFashionEvent(subType, color);
	m_PutFashionEvents.push_back(putFashionEvent);
}

bool TaskScript::CheckPutFashionEvent(ItemSubType subType, UInt8 color) const
{
	for (PutFashionEventList::const_iterator iter = m_PutFashionEvents.begin(); iter != m_PutFashionEvents.end(); ++iter)
	{
		if (iter->color <= color
			&& (iter->subType == ItemSubType::ST_ITEM_INVALID || iter->subType == subType ))
		{
			return true;
		}
	}
	return false;
}

void TaskScript::BindEscortSuccedEvent(UInt32 id)
{
	m_EscortSuccedEvents.push_back(id);
}

bool TaskScript::CheckEscortSuccedEvent(UInt32 id) const
{
	for(IdVec::const_iterator iter = m_EscortSuccedEvents.begin();
		iter != m_EscortSuccedEvents.end(); ++iter)
	{
		if(*iter == id) return true;
	}
	return false;
}

void TaskScript::BindCatchPetEvent(UInt32 id)
{
	m_catchPetEvents.push_back(id);
}

bool TaskScript::CheckCatchPetEvent(UInt32 id) const
{
	for(IdVec::const_iterator iter = m_catchPetEvents.begin();
		iter != m_catchPetEvents.end(); ++iter)
	{
		if(*iter == id) return true;
	}
	return false;
}

void TaskScript::BindAddRetinueEvent(UInt32 quality)
{
	if (quality < RETINUE_NUM)
	{
		m_AddRetinueEvent = (RetinueQuality)quality;
	}
	else
	{
		m_AddRetinueEvent = RETINUE_BLUE;
	}
}

bool TaskScript::CheckAddRetinueEvent(RetinueQuality quality) const
{
	return quality >= m_AddRetinueEvent;
}

void TaskScript::BindRetinueChangeSkillEvent(UInt32 buffId)
{
	m_RetinueChangeSkillEvents.push_back(buffId);
}

bool TaskScript::CheckRetinueChangeSkillEvent(UInt32 buffId) const
{
	for (IdVec::const_iterator iter = m_RetinueChangeSkillEvents.begin();
		iter != m_RetinueChangeSkillEvents.end(); ++iter)
	{
		if (*iter == 0 || buffId == *iter)
			return true;
	}
	return false;
}

void TaskScript::BindRetinueUpLevelSkillEvent(UInt32 id, UInt8 level)
{
	RetinueUpLevelEvent rulEvent;
	rulEvent.retinueId = id;
	rulEvent.retinueLevel = level;
	m_bRetinueUpLevelEvents.push_back(rulEvent);
}

bool TaskScript::CheckRetinueUpLevelSkillEvent(UInt32 id, UInt8 level)
{
	for (RetinueUpLevelEventList::const_iterator iter = m_bRetinueUpLevelEvents.begin();
		iter != m_bRetinueUpLevelEvents.end(); ++iter)
	{
		if ((iter->retinueId == 0 || id == iter->retinueId) &&
			(iter->retinueLevel == 0 || level >= iter->retinueLevel))
		{
			return true;
		}
	}
	return false;
}

void TaskScript::BindRetinueUpStarEvent(UInt32 id, UInt8 star)
{
	RetinueUpLevelEvent rulEvent;
	rulEvent.retinueId = id;
	rulEvent.retinueLevel = star;
	m_RetinueUpStarEvents.push_back(rulEvent);
}

bool TaskScript::CheckRetinueUpStarEvent(UInt32 id, UInt8 star)
{
	for (RetinueUpLevelEventList::const_iterator iter = m_RetinueUpStarEvents.begin();iter != m_RetinueUpStarEvents.end(); ++iter)
	{
		if ((iter->retinueId == 0 || id == iter->retinueId) &&
			(iter->retinueLevel == 0 || star >= iter->retinueLevel))
		{
			return true;
		}
	}
	return false;
}


void TaskScript::BindCopyScenePassedEvent(UInt32 dungeonId, LevelScore score, UInt32 usedTime, UInt32 reviveCount)
{
	ClearDungeonEvent cdEvent;
	cdEvent.id = dungeonId;
	cdEvent.score = score;
	cdEvent.usedTime = usedTime;
	cdEvent.reviveCount = reviveCount;
	m_CopyScenePassedEvents.push_back(cdEvent);
}

//added by aprilliu, 2016.04.18
void TaskScript::BindClearDungeonEvent(UInt32 dungeonId, LevelScore score, UInt32 usedTime, UInt32 reviveCount)
{
	this->BindCopyScenePassedEvent(dungeonId, score, usedTime, reviveCount);
}

void TaskScript::BindClearDungeonEventByRevive(UInt32 dungeonId, UInt32 reviveCount)
{
	this->BindCopyScenePassedEvent(dungeonId, LS_C, 0, reviveCount);
}

bool TaskScript::CheckCopyScenePassedEvent(UInt32 id, LevelScore score, UInt32 usedTime, UInt32 reviveCount) const
{
	for (ClearDungeonEventList::const_iterator iter = m_CopyScenePassedEvents.begin();
		iter != m_CopyScenePassedEvents.end(); ++iter)
	{
		//modified by huchenhui, 2016.06.24 id==0 表示所有地下城 usedtime==0表示不限定时间.
		if ((score >= iter->score) &&
			(iter->usedTime == 0 || iter->usedTime > usedTime) &&
			(iter->id == 0 || iter->id == id) &&
			(iter->reviveCount == 0 || iter->reviveCount > reviveCount))
			return true;
	}
	return false;
}

void TaskScript::BindClearDungeonEventCareScore(UInt32 dungeonId, LevelScore score, UInt32 usedTime, UInt32 reviveCount)
{
	ClearDungeonEvent cdEvent;
	cdEvent.id = dungeonId;
	cdEvent.score = score;
	cdEvent.usedTime = usedTime;
	cdEvent.reviveCount = reviveCount;
	m_CopyScenePassedCareScoreEvents.push_back(cdEvent);
}

bool TaskScript::CheckCopyScenePassedCareScoreEvent(UInt32 id, LevelScore score, UInt32 usedTime, UInt32 reviveCount) const
{
	for (ClearDungeonEventList::const_iterator iter = m_CopyScenePassedCareScoreEvents.begin();
		iter != m_CopyScenePassedCareScoreEvents.end(); ++iter)
	{
		//modified by huchenhui, 2016.06.24 id==0 表示所有地下城 usedtime==0表示不限定时间.
		if ((score >= iter->score) &&
			(iter->usedTime == 0 || iter->usedTime > usedTime) &&
			(iter->id == 0 || iter->id == id) &&
			(iter->reviveCount == 0 || iter->reviveCount > reviveCount))
			return true;
	}
	return false;
}

void TaskScript::BindTeamClearDungeonEvent(UInt32 dungeonId, LevelScore score, UInt32 usedTime, UInt32 reviveCount)
{
	ClearDungeonEvent cdEvent;
	cdEvent.id = dungeonId;
	cdEvent.score = score;
	cdEvent.usedTime = usedTime;
	cdEvent.reviveCount = reviveCount;
	m_TeamCopyScenePassedEvents.push_back(cdEvent);
}

bool TaskScript::CheckTeamClearDungeonEvent(UInt32 id, LevelScore score, UInt32 usedTime, UInt32 reviveCount) const
{
	for (ClearDungeonEventList::const_iterator iter = m_TeamCopyScenePassedEvents.begin();
		iter != m_TeamCopyScenePassedEvents.end(); ++iter)
	{
		if ((score >= iter->score) &&
			(iter->usedTime == 0 || iter->usedTime > usedTime) &&
			(iter->id == 0 || iter->id == id) &&
			(iter->reviveCount == 0 || iter->reviveCount > reviveCount))
			return true;
	}
	return false;
}

void TaskScript::BindClearActivityDungeonEvent(UInt32 dungeonId, LevelScore score, UInt32 usedTime, UInt32 reviveCount)
{
	ClearDungeonEvent cdEvent;
	cdEvent.id = dungeonId;
	cdEvent.score = score;
	cdEvent.usedTime = usedTime;
	cdEvent.reviveCount = reviveCount;
	m_ClearActivityDungeonEvents.push_back(cdEvent);
}

bool TaskScript::CheckClearActivityDungeonEvent(UInt32 id, LevelScore score, UInt32 usedTime, UInt32 reviveCount) const
{
	for (ClearDungeonEventList::const_iterator iter = m_ClearActivityDungeonEvents.begin(); iter != m_ClearActivityDungeonEvents.end(); ++iter)
	{
		if ((score >= iter->score) &&
			(iter->usedTime == 0 || iter->usedTime > usedTime) &&
			(iter->id == 0 || iter->id == id) &&
			(iter->reviveCount == 0 || iter->reviveCount > reviveCount))
			return true;
	}
	return false;
}

void TaskScript::BindTeamClearActivityDungeonEvent(UInt32 dungeonId, LevelScore score, UInt32 usedTime, UInt32 reviveCount)
{
	ClearDungeonEvent cdEvent;
	cdEvent.id = dungeonId;
	cdEvent.score = score;
	cdEvent.usedTime = usedTime;
	cdEvent.reviveCount = reviveCount;
	m_TeamActivityDungeonEvents.push_back(cdEvent);
}

bool TaskScript::CheckTeamClearActivityDungeonEvent(UInt32 id, LevelScore score, UInt32 usedTime, UInt32 reviveCount) const
{
	for (ClearDungeonEventList::const_iterator iter = m_TeamActivityDungeonEvents.begin(); iter != m_TeamActivityDungeonEvents.end(); ++iter)
	{
		if ((score >= iter->score) &&
			(iter->usedTime == 0 || iter->usedTime > usedTime) &&
			(iter->id == 0 || iter->id == id) &&
			(iter->reviveCount == 0 || iter->reviveCount > reviveCount))
			return true;
	}
	return false;
}

void TaskScript::BindBabelPassedEvent(UInt32 id)
{
	m_BabelPassedEvents.push_back(id);
}

bool TaskScript::CheckBabelPassedEvent(UInt32 id) const
{
	for (IdVec::const_iterator iter = m_BabelPassedEvents.begin(); iter != m_BabelPassedEvents.end(); ++iter)
	{
		if (*iter == id) return true;
	}
	return false;
}

void TaskScript::BindBuyMallItemEvent(UInt32 mallItemid)
{
	m_buyMallItemEvents.push_back(mallItemid);
}

bool TaskScript::CheckBuyMallItemEvent(UInt32 mallItemid) const
{
	for(IdVec::const_iterator iter = m_buyMallItemEvents.begin(); iter != m_buyMallItemEvents.end(); iter++)
	{
		if (*iter == mallItemid || *iter == 0)
			return true;
	}
	return false;
}

void TaskScript::BindStoryEndEvent(UInt32 id)
{
	m_StoryEndEvents.push_back(id);
}

bool TaskScript::CheckStoryEndEvent(UInt32 id) const
{
	for(IdVec::const_iterator iter = m_StoryEndEvents.begin(); iter != m_StoryEndEvents.end(); iter++)
	{
		if(*iter == id) return true;
	}
	return false;
}

void TaskScript::BindSubmitTaskSucceedEvent(UInt32 taskId)
{
	m_SubmitTaskSucceedIdList.push_back(taskId);
}

bool TaskScript::CheckSubmitTaskSucceedEvent(UInt32 taskId) const
{
	for (IdVec::const_iterator iter = m_SubmitTaskSucceedIdList.begin(); iter != m_SubmitTaskSucceedIdList.end(); ++iter)
	{
		if (*iter == taskId || *iter == 0) return true;
	}
	return false;
}

void TaskScript::BindSubmitActiveTaskSucceedEvent(UInt32 templateId, UInt32 taskId)
{
	ActiveTaskEvent atEvent;
	atEvent.taskId = taskId;
	atEvent.templateId = templateId;
	m_SubmitActiveTaskSucceedEvents.push_back(atEvent);
}

bool TaskScript::CheckSubmitActiveTaskSucceedEvent(UInt32 templateId, UInt32 taskId)
{
	for (ActiveTaskEventList::const_iterator iter = m_SubmitActiveTaskSucceedEvents.begin(); iter != m_SubmitActiveTaskSucceedEvents.end(); ++iter)
	{
		if ((iter->taskId == taskId || iter->taskId == 0) &&
			(iter->templateId == templateId || iter->templateId == 0))
		{
			return true;
		}
	}
	return false;
}

void TaskScript::BindSubmitCycleTaskSucceedEvent(UInt32 taskId)
{
	m_SubmitCycleTaskSucceedEvents.push_back(taskId);
}

bool TaskScript::CheckSubmitCycleTaskSucceedEvent(UInt32 taskId)
{
	for (IdVec::const_iterator iter = m_SubmitCycleTaskSucceedEvents.begin(); iter != m_SubmitCycleTaskSucceedEvents.end(); ++iter)
	{
		if (*iter == taskId || *iter == 0) return true;
	}
	return false;
}

void TaskScript::BindDailyTaskSubmittedEvent(UInt32 taskId)
{
	m_DailyTaskSubmittedEvents.push_back(taskId);
}

bool TaskScript::CheckDailyTaskSubmittedEvent(UInt32 taskId) const
{
	for (IdVec::const_iterator iter = m_DailyTaskSubmittedEvents.begin(); iter != m_DailyTaskSubmittedEvents.end(); ++iter)
	{
		if (*iter == taskId || *iter == 0) return true;
	}
	return false;
}

void TaskScript::BindPVPEvent(PkType type, PkRaceResult raceResult)
{
	PVPEvent pEvent;
	pEvent.type = type;
	pEvent.raceResult = raceResult;
	m_PVPEvent.push_back(pEvent);
}

bool TaskScript::CheckPVPEvent(PkType type, PkRaceResult raceResult) const
{
	for (PVPEventList::const_iterator iter = m_PVPEvent.begin(); iter != m_PVPEvent.end(); iter++)
	{
		if (iter->type == type &&
			(iter->raceResult == PKRR_INVALID || iter->raceResult == raceResult))
			return true;
	}
	return false; 
}

void TaskScript::BindChangeOccuEvent()
{
	m_bChangeOccuEvent = true;
}

bool TaskScript::CheckChangeOccuEvent() const
{
	return m_bChangeOccuEvent;
}

void TaskScript::BindAddMagicEvent()
{
	m_bAddMagicEvent = true;
}

bool TaskScript::CheckAddMagicEvent() const
{
	return m_bAddMagicEvent;
}

void TaskScript::BindOpenMagicJarEvent(JarType type, UInt32 combo)
{
	OpenMagicJarEvent omjEvent(type, combo);
	m_OpenMagicJarComboEventList.push_back(omjEvent);
}

bool TaskScript::CheckOpenMagicJarEvent(JarType type, UInt32 combo) const
{
	for (OpenMagicJarEventList::const_iterator iter = m_OpenMagicJarComboEventList.begin(); iter != m_OpenMagicJarComboEventList.end(); iter++)
	{
		if ((iter->type == JarType::JT_NONE || iter->type == type) &&
			(iter->combo == 0 || iter->combo == combo))
			return true;
	}
	return false;
}

void TaskScript::BindAddMagicCardEvent()
{
	m_bAddMagicCardEvent = true;
}

bool TaskScript::CheckAddMagicCardEvent() const
{
	return m_bAddMagicCardEvent;
}

void TaskScript::BindFriendGiveEvent()
{
	m_bFriendGiveEvent = true;
}

bool TaskScript::CheckFriendGiveEvent() const
{
	return m_bFriendGiveEvent;
}

void TaskScript::BindWarpStoneUpLevelEvent(WarpStoneType type, UInt32 level)
{
	WarpStoneUpLevelEvent wsulEvent;
	wsulEvent.type = type;
	wsulEvent.level = level;
	m_WarpStoneUpLevelEvents.push_back(wsulEvent);
}

bool TaskScript::CheckWarpStoneUpLevelEvent(WarpStoneType type, UInt32 level) const
{
	for (WarpStoneUpLevelEventList::const_iterator iter = m_WarpStoneUpLevelEvents.begin(); 
		iter != m_WarpStoneUpLevelEvents.end(); ++iter)
	{
		if ((!IsWarpStone(iter->type) || type == iter->type)
			&& (iter->level == 0 || level >= iter->level) )
		{
			return true;
		}
	}
	return false;
}

void TaskScript::BindWarpStoneAddEnergyEvent()
{
	m_bAddEnergyEvent = true;
}

bool TaskScript::CheckWarpStoneAddEnergyEvent() const
{
	return m_bAddEnergyEvent;
}

void TaskScript::BindRefreshShopEvent()
{
	m_bRefreshShopEvent = true;
}

bool TaskScript::CheckRefreshShopEvent() const
{
	return m_bRefreshShopEvent;
}

void TaskScript::BindShopBuyEvent(UInt32 shopId, UInt32 itemId)
{
	ShopBuyEvent sbEvent(shopId, itemId);
	m_ShopBuyEvents.push_back(sbEvent);
}

bool TaskScript::CheckShopBuyEvent(UInt32 shopId, UInt32 itemId) const
{
	for (ShopBuyEventList::const_iterator iter = m_ShopBuyEvents.begin(); iter != m_ShopBuyEvents.end(); ++iter)
	{
		if ((iter->shopId == 0 || iter->shopId == shopId) &&
			(iter->shopItemId == 0 || iter->shopItemId == itemId))
			return true;
	}
	return false;
}

void TaskScript::BindRemoveFatigueEvent(UInt16 fatigue)
{
	m_uUsedFatigue = fatigue;
}

bool TaskScript::CheckRemoveFatigueEvent(UInt16 fatigue) const
{
	if (m_uUsedFatigue == 0 || m_uUsedFatigue <= fatigue) return true;
	return false;
}

void TaskScript::BindBeginHellEvent(UInt32 id)
{
	m_BeginHellEvents.push_back(id);
}

bool TaskScript::CheckBeginHellEvent(UInt32 id)
{
	for (IdVec::const_iterator iter = m_BeginHellEvents.begin(); iter != m_BeginHellEvents.end(); ++iter)
	{
		if (*iter == id || *iter == 0)
		{
			return true;
		}
	}
	return false;
}

void TaskScript::BindClearHellEvent(UInt32 id, UInt32 usedTime, LevelScore score)
{
	ClearDungeonEvent hellEvent;
	hellEvent.id = id;
	hellEvent.usedTime = usedTime;
	hellEvent.score = score;
	m_ClearHellEvents.push_back(hellEvent);
}

bool TaskScript::CheckClearHellEvent(UInt32 id, UInt32 usedTime, LevelScore score)
{
	for (ClearDungeonEventList::const_iterator iter = m_ClearHellEvents.begin(); iter != m_ClearHellEvents.end(); ++iter)
	{
		if ((iter->usedTime == 0 || iter->usedTime > usedTime) &&
			(iter->id == 0 || iter->id == id) &&
			(score >= iter->score))
			return true;
	}
	return false;
}

void TaskScript::BindTeamClearHellEvent(UInt32 id, UInt32 usedTime, LevelScore score)
{
	ClearDungeonEvent hellEvent;
	hellEvent.id = id;
	hellEvent.usedTime = usedTime;
	hellEvent.score = score;
	m_TeamClearHellEvents.push_back(hellEvent);
}

bool TaskScript::CheckTeamClearHellEvent(UInt32 id, UInt32 usedTime, LevelScore score)
{
	for (ClearDungeonEventList::const_iterator iter = m_TeamClearHellEvents.begin(); iter != m_TeamClearHellEvents.end(); ++iter)
	{
		if ((iter->usedTime == 0 || iter->usedTime > usedTime) &&
			(iter->id == 0 || iter->id == id) &&
			(score >= iter->score))
			return true;
	}
	return false;
}

void TaskScript::BindDeathTowerBeginEvent(UInt32 towerId)
{
	m_DeathTowerBeginEvents.push_back(towerId);
}

bool TaskScript::CheckDeathTowerBeginEvent(UInt32 towerId)
{
	for (IdVec::const_iterator iter = m_DeathTowerBeginEvents.begin(); iter != m_DeathTowerBeginEvents.end(); ++iter)
	{
		if (*iter == towerId || *iter == 0)
		{
			return true;
		}
	}
	return false;
}

void TaskScript::BindDeathTowerWipeoutBeginEvent()
{
	m_bDeathTowerWipeoutBeginEvent = true;
}

bool TaskScript::CheckDeathTowerWipeoutBeginEvent()
{
	return m_bDeathTowerWipeoutBeginEvent;
}

void TaskScript::BindDeathTowerEvent(UInt32 towerId, UInt32 layer, UInt32 usedTime)
{
	DeathTowerEvent dtEvent;
	dtEvent.id = towerId;
	dtEvent.layer = layer;
	dtEvent.usedTime = usedTime;
	m_DeathTowerEvents.push_back(dtEvent);
}

bool TaskScript::CheckDeathTowerEvent(UInt32 towerId, UInt32 layer, UInt32 usedTime)
{
	for (DeathTowerEventList::const_iterator iter = m_DeathTowerEvents.begin(); iter != m_DeathTowerEvents.end(); ++iter)
	{
		if ((iter->id == 0 || iter->id == towerId)
			&& (iter->layer == 0 || iter->layer <= layer)
			&& (iter->usedTime == 0 || iter->usedTime >= usedTime))
		{
			return true;
		}
	}
	return false;
}

void TaskScript::BindAwakenEvent()
{
	m_bAwakenEvent = true;
}

bool TaskScript::CheckAwakenEvent()
{
	return m_bAwakenEvent;
}

void TaskScript::BindReviveCoinEvent()
{
	m_bReviveCoinEvent = true;
}

bool TaskScript::CheckReviveCoinEvent()
{
	return m_bReviveCoinEvent;
}

void TaskScript::BindAddAuctionEvent()
{
	m_bAddAuctionEvent = true;
}

bool TaskScript::CheckAddAuctionEvent()
{
	return m_bAddAuctionEvent;
}

void TaskScript::BindSellAuctionEvent()
{
	m_bSellAuctionEvent = true;
}

bool TaskScript::CheckSellAuctionEvent()
{
	return m_bSellAuctionEvent;
}

void TaskScript::BindBuyAuctionEvent()
{
	m_bBuyAuctionEvent = true;
}

bool TaskScript::CheckBuyAuctionEvent()
{
	return m_bBuyAuctionEvent;
}

void TaskScript::BindSetTaskItemEvent(ItemQuality quality, UInt32 level)
{
	SetTaskItemEvent taskItemEvent;
	taskItemEvent.quality = quality;
	taskItemEvent.level = level;
	m_SetTaskItemEvents.push_back(taskItemEvent);
}

void TaskScript::BindSetTaskItemByIdEvent(UInt32 itemId, UInt32 num)
{
	SetTaskItemEvent taskItemEvent;
	taskItemEvent.itemId = itemId;
	taskItemEvent.num = num;
	m_SetTaskItemEvents.push_back(taskItemEvent);
}

bool TaskScript::CheckSetTaskItemEvent(ItemQuality quality, UInt32 level, UInt32 itemId, UInt32& num)
{
	for (SetTaskItemEventList::const_iterator iter = m_SetTaskItemEvents.begin(); iter != m_SetTaskItemEvents.end(); ++iter)
	{
		if (iter->itemId == itemId && itemId != 0)
		{
			num = iter->num;
			return true;
		}
		if ((quality != ITEM_QUALITY_INVLID && iter->quality <= quality)
			&& iter->level <= level)
		{
			return true;
		}
	}
	return false;
}

void TaskScript::BindFeedPetEvent(PetFeedType type)
{
	FeedPetEvent fpEvent;
	fpEvent.type = type;
	m_FeedPetEvents.push_back(fpEvent);
}

bool TaskScript::CheckFeedPetEvent(PetFeedType type)
{
	for (FeedPetEventList::const_iterator iter = m_FeedPetEvents.begin(); iter != m_FeedPetEvents.end(); ++iter)
	{
		if (iter->type == PetFeedType::PET_FEED_INVALID || iter->type == type)
		{
			return true;
		}
	}
	return false;
}

void TaskScript::BindAddPetEvent(PetType petType, PetQuality petQuality)
{
	AddPetEvent addPetEvent;
	addPetEvent.petType = petType;
	addPetEvent.petQuality = petQuality;
	m_AddPetEvents.push_back(addPetEvent);
}

bool TaskScript::CheckAddPetEvent(PetType petType, PetQuality petQuality)
{
	for (AddPetEventList::const_iterator iter = m_AddPetEvents.begin(); iter != m_AddPetEvents.end(); ++iter)
	{
		if ((iter->petType == PetType::PET_TYPE_INVALID || iter->petType == petType)
			&& (iter->petQuality == PetQuality::PET_QUALITY_INVALID || iter->petQuality <= petQuality))
		{
			return true;
		}
	}
	return false;
}

void TaskScript::BindPetUpLevelEvent(PetType petType, PetQuality petQuality, UInt16 level)
{
	PetUpLevelEvent petUpLevelEvent;
	petUpLevelEvent.petType = petType;
	petUpLevelEvent.petQuality = petQuality;
	petUpLevelEvent.level = level;
	m_PetUpLevelEvents.push_back(petUpLevelEvent);
}

bool TaskScript::CheckPetUpLevelEvent(PetType petType, PetQuality petQuality, UInt16 level)
{
	for (PetUpLevelEventList::const_iterator iter = m_PetUpLevelEvents.begin(); iter != m_PetUpLevelEvents.end(); ++iter)
	{
		if ((iter->petType == PetType::PET_TYPE_INVALID || iter->petType == petType)
			&& (iter->petQuality == PetQuality::PET_QUALITY_INVALID || iter->petQuality <= petQuality)
			&& (level >= iter->level))
		{
			return true;
		}
	}
	return false;
}

void TaskScript::BindAddRelationEvent(RelationType type)
{
	RelationEvent relationEvent;
	relationEvent.type = type;
	m_RelationEvents.push_back(relationEvent);
}

bool TaskScript::CheckAddRelationEvent(RelationType type)
{
	for (RelationEventList::const_iterator iter = m_RelationEvents.begin(); iter != m_RelationEvents.end(); ++iter)
	{
		if (iter->type == 0 || iter->type == type)
		{
			return true;
		}
	}
	return false;
}

void TaskScript::BindSendHornEvent()
{
	m_bSendHornEvent = true;
}

bool TaskScript::CheckSendHornEvent()
{
	return m_bSendHornEvent;
}

void TaskScript::BindCalculateEquipScoreEvent(ItemSubType subType, ItemQuality quality, UInt32 score)
{
	EquitScoreEvent equitScoreEvent;
	equitScoreEvent.subType = subType;
	equitScoreEvent.quality = quality;
	equitScoreEvent.score = score;
	m_EquipScoreEventList.push_back(equitScoreEvent);
}

bool TaskScript::CheckCalculateEquipScoreEvent(ItemSubType subType, ItemQuality quality, UInt32 score)
{
	for (EquitScoreEventList::const_iterator iter = m_EquipScoreEventList.begin(); iter != m_EquipScoreEventList.end(); ++iter)
	{
		if ((iter->subType == ItemSubType::ST_ITEM_INVALID || iter->subType == subType)
			&& (iter->quality == ItemQuality::ITEM_QUALITY_INVLID || iter->quality <= quality)
			&& (iter->score == 0 || iter->score <= score))
		{
			return true;
		}
	}
	return false;
}

void TaskScript::BindAddPreciousBeadEvent()
{
	m_bAddPreciousBeadEvent = true;
}

bool TaskScript::CheckAddPreciousBeadEvent()
{
	return m_bAddPreciousBeadEvent;
}

void TaskScript::BindJoinGuildEvent()
{
	m_bJoinGuildEvent = true;
}

bool TaskScript::CheckJoinGuildEvent()
{
	return m_bJoinGuildEvent;
}

void TaskScript::BindIncGuildBattleScoreEvent()
{
	m_bIncGuildBattleEvent = true;
}

bool TaskScript::CheckIncGuildBattleScoreEvent()
{
	return m_bIncGuildBattleEvent;
}

void TaskScript::BindGuildOccupyTerritoryEvent(UInt32 terrId)
{
	m_GuildOccupyTerritoryEvent.push_back(terrId);
}

bool TaskScript::CheckGuildOccupyTerritoryEvent(UInt32 terrId)
{
	for (IdVec::const_iterator iter = m_GuildOccupyTerritoryEvent.begin(); iter != m_GuildOccupyTerritoryEvent.end(); ++iter)
	{
		if (*iter == 0 || *iter == terrId)
		{
			return true;
		}
	}
	return false;
}

void TaskScript::BindWinWudaodahuiPkEndEvent(UInt32 winNum)
{
	m_uWudaodahuiWinNumEvent = winNum;
}

bool TaskScript::CheckWinWudaodahuiPkEndEvent(UInt32 winNum)
{
	return winNum >= m_uWudaodahuiWinNumEvent;
}

void TaskScript::BindEnlargePackageEvent()
{
	m_bEnlargePackageEvent = true;
}

bool TaskScript::CheckEnlargePackageEvent()
{
	return m_bEnlargePackageEvent;
}

void TaskScript::BindEnlargeStorageEvent()
{
	m_bEnlargeStorageEvent = true;
}

bool TaskScript::CheckEnlargeStorageEvent()
{
	return m_bEnlargeStorageEvent;
}

void TaskScript::BindIncIntimacyEvent()
{
	m_bIncIntimacyEvent = true;
}

bool TaskScript::CheckIncIntimacyEvent()
{
	return m_bIncIntimacyEvent;
}

void TaskScript::BindIncDailyTaskScoreEvent()
{
	m_bIncIDailyTaskScoreEvent = true;
}

bool TaskScript::CheckIncDailyTaskScoreEvent()
{
	return m_bIncIDailyTaskScoreEvent;
}

void TaskScript::BindApprentDiscipleEvent()
{
	m_bApprentDiscipleEvent = true;
}

bool TaskScript::CheckApprentDiscipleEvent()
{
	return m_bApprentDiscipleEvent;
}

void TaskScript::BindDiscipleFinschEvent()
{
	m_bDiscipleFinschEvent = true;
}

bool TaskScript::CheckDiscipleFinschEvent()
{
	return m_bDiscipleFinschEvent;
}

void TaskScript::BindGuildEmblemEvent()
{
	m_bGuildEmblemEvent = true;
}

bool TaskScript::CheckGuildEmblemEvent()
{
	return m_bGuildEmblemEvent;
}

UInt32 TaskScript::GetTaskExp(Player* player, UInt8 quality)
{
	return Call<UInt32>("GetTaskExp", player, quality);
}

RewardGroup* TaskScript::GetTaskRewards(Player* player, UInt8 quality)
{
	return Call<RewardGroup*>("GetTaskRewards", player, quality);
}

void TaskScript::Init()
{
	Call<void>("Init", this);
}

bool TaskScript::CheckAccept(Player* player)
{
	return Call<bool>("CheckAccept", player);
}

void TaskScript::OnAccepted(Player* player, BaseTask* task)
{
	//modified by aprilliu, 2016.04.25  增加Task* task参数，避免在script里查找
	//modified by huchenhui, 2016.06.24 Task* task参数从外面传递进来,避免再次查找.
	//Task* task = player->FindTask(GetId());
	if (task)
	{
		Call<void>("OnAccepted", player, task);
	}
}

void TaskScript::OnSubmitted(Player* player , BaseTask* task)
{
	//modified by aprilliu, 2016.04.25  增加Task* task参数，避免在script里查找
	//modified by huchenhui, 2016.06.24 Task* task参数从外面传递进来,避免再次查找.
	//Task* task = player->FindTask(GetId());
	if (task)
	{
		Call<void>("OnSubmitted", player, task);
	}
}

void TaskScript::OnAbandomed(Player* player, BaseTask* task)
{
	//modified by aprilliu, 2016.04.25  增加Task* task参数，避免在script里查找
	//modified by huchenhui, 2016.06.24 Task* task参数从外面传递进来,避免再次查找.
	//Task* task = player->FindTask(GetId());
	if (task)
	{
		Call<void>("OnAbandomed", player, task);
	}
}

void TaskScript::CheckFinish(Player* player, BaseTask* task)
{
	if (task)
	{
		Call<void>("CheckFinish", player, task);
	}
}

void TaskScript::OnGetItem(Player* player, UInt32 itemid, UInt16 num, BaseTask* task)
{
	//modified by aprilliu, 2016.04.25  增加Task* task参数，避免在script里查找
	//modified by huchenhui, 2016.06.24 Task* task参数从外面传递进来,避免再次查找.
	//Task* task = player->FindTask(GetId());
	if (task)
	{
		Call<void>("OnGetItem", player, task, itemid, num);
	}

}

void TaskScript::OnUseItem(Player* player, UInt32 itemid, BaseTask* task)
{
	//modified by aprilliu, 2016.04.25  增加Task* task参数，避免在script里查找
	//modified by huchenhui, 2016.06.24 Task* task参数从外面传递进来,避免再次查找.
	//Task* task = player->FindTask(GetId());
	if (task)
	{
		Call<void>("OnUseItem", player, task, itemid);
	}

}

void TaskScript::OnChangeMoney(Player* player, UInt32 type, Int32 num, BaseTask* task)
{
	if (task)
	{
		Call<void>("OnChangeMoney", player, task, type, num);
	}
}


void TaskScript::OnRmItem(Player* player, UInt32 itemid, UInt16 num, BaseTask* task)
{
	//  //脚本里应该有 OnRmItem 函数
	//modified by aprilliu, 2016.04.25  增加Task* task参数，避免在script里查找
	//modified by huchenhui, 2016.06.24 Task* task参数从外面传递进来,避免再次查找.
	//Task* task = player->FindTask(GetId());
	if (task)
	{
		Call<void>("OnRmItem", player,task, itemid, num);
	}
}


void TaskScript::OnKillNpc(Player* player, UInt32 npcid, UInt8 type, BaseTask* task, UInt32 dungeonId)
{
	
	//modified by aprilliu, 2016.04.25  增加Task* task参数，避免在script里查找
	//modified by huchenhui, 2016.06.24 Task* task参数从外面传递进来,避免再次查找.
	//Task* task = player->FindTask(GetId());
	if (task)
	{
		Call<void>("OnKillNpc", player, task, npcid, type, dungeonId);
	}
}


void TaskScript::OnKillCityMonster(Player* player, BaseTask* task, UInt32 cityMonsterType, UInt32 monsterId, UInt64 monsterGuid)
{
	if (task)
	{
		std::string strMonsterGuid = Avalon::StringUtil::ConvertToString(monsterGuid);

		Call<void>("OnKillCityMonster", player, task, cityMonsterType, monsterId, strMonsterGuid.c_str());
	}
}

void TaskScript::OnTeamKillCityMonster(Player* player, BaseTask* task, UInt32 cityMonsterType, UInt32 monsterId, UInt64 monsterGuid, std::vector<ObjID_t>& teammateIds)
{
	if (task)
	{
		//std::string strMonsterGuid = Avalon::StringUtil::ConvertToString(monsterGuid);
		ScriptVector64 teammateVec(teammateIds);

		Call<void>("OnKillCityMonster", player, task, cityMonsterType, monsterId, teammateVec);
	}
}

void TaskScript::OnEnterZone(Player* player)
{
	//noted by aprilliu, 2016.04.25 暂时没有使用，所以暂时没有添加task参数
	Call<void>("OnEnterZone", player);

}

void TaskScript::OnRemakeEquip(Player* player, BaseTask* task, UInt32 id, UInt8 type)
{
	if (task)
	{
		Call<void>("OnRemakeEquip", player, task, id, type);
	}
}

void TaskScript::OnCalculateValuedScore(Player* player, BaseTask* task, ItemSubType subType, ItemQuality quality, UInt32 score)
{
	if (task)
	{
		Call<void>("OnCalculateValuedScore", player, task, subType, quality, score);
	}
}

void TaskScript::OnAddPreciousBead(Player* player, BaseTask* task)
{
	if (task)
	{
		Call<void>("OnAddPreciousBead", player, task);
	}
}

void TaskScript::OnStrengthenEquip(Player* player, BaseTask* task, UInt32 errorCode, UInt8 oldStrengthen, UInt8 newStrengthen)
{
	//added by huchenhui 2016.08.20	强化装备
	if (task)
	{
		Call<void>("OnStrengthenEquip", player, task, errorCode, oldStrengthen, newStrengthen);
	}
}

void TaskScript::OnDecomposeEquip(Player* player, BaseTask* task)
{
	//added by huchenhui 2016.08.20	分解装备
	if (task)
	{
		Call<void>("OnDecomposeEquip", player, task);
	}
}

void TaskScript::OnPutEquip(Player* player, BaseTask* task, UInt16 level, UInt32 subType, UInt32 strength)
{
	//added by huchenhui 2016.08.20	穿戴装备
	if (task)
	{
		Call<void>("OnPutEquip", player, task, level, subType, strength);
	}
}

void TaskScript::OnPutFashion(Player* player, BaseTask* task, ItemSubType subType, UInt32 color)
{
	//added by huchenhui 2018.03.20	穿戴时装
	if (task)
	{
		Call<void>("OnPutFashion", player, task, subType, color);
	}
}

void TaskScript::OnCatchPet(Player* player, UInt32 id)
{
	//noted by aprilliu, 2016.04.25 暂时没有使用，所以暂时没有添加task参数
	Call<void>("OnCatchPet", player, id);
}

void TaskScript::OnAddRetinue(Player* player, BaseTask* task, RetinueQuality quality)
{
	//added by huchenhui 2016.08.20 添加随从事件
	if (task)
	{
		Call<void>("OnAddRetinue", player, task, quality);
	}
}

void TaskScript::OnRetinueChangeSkill(Player* player, BaseTask* task, UInt32 buffId)
{
	//added by huchenhui 2016.08.20 随从洗练事件
	if (task)
	{
		Call<void>("OnRetinueChangeSkill", player, task, buffId);
	}
}

void TaskScript::OnRetinueUpLevel(Player* player, BaseTask* task, UInt32 retinueId, UInt8 retinueLevel)
{
	if (task)
	{
		Call<void>("OnRetinueUpLevel", player, task, retinueId, retinueLevel);
	}
}

void TaskScript::OnRetinueUpStar(Player* player, BaseTask* task, UInt32 retinueId, UInt8 retinueStar)
{
	if (task)
	{
		Call<void>("OnRetinueUpStar", player, task, retinueId, retinueStar);
	}
}

void TaskScript::OnEscortSucced(Player* player, Npc* npc)
{
	//noted by aprilliu, 2016.04.25 暂时没有使用，所以暂时没有添加task参数
	Call<void>("OnEscortSucced", player, npc);
}

void TaskScript::OnEscortFailed(Player* player, UInt32 npcId)
{
	//noted by aprilliu, 2016.04.25 暂时没有使用，所以暂时没有添加task参数
	Call<void>("OnEscortFailed", player, npcId);
}

void TaskScript::OnEscortTargetKilled(Npc* npc, Player* killer)
{
	//noted by aprilliu, 2016.04.25 暂时没有使用，所以暂时没有添加task参数
	Call<void>("OnEscortTargetKilled", npc, killer);
}

void TaskScript::OnPlayerLevelup(Player* player, BaseTask* task)
{
	//modified by aprilliu, 2016.04.25  增加Task* task参数，避免在script里查找
	//modified by huchenhui, 2016.06.24 Task* task参数从外面传递进来,避免再次查找.
	//Task* task = player->FindTask(GetId());
	if (task)
	{
		Call<void>("OnPlayerLevelup", player, task);
	}
}

void TaskScript::OnCopyScenePassed(Player* player, UInt32 id, BaseTask* task)
{
	//modified by aprilliu, 2016.04.25  增加Task* task参数，避免在script里查找
	//modified by huchenhui, 2016.06.24 Task* task参数从外面传递进来,避免再次查找.
	//Task* task = player->FindTask(GetId());
	if (task)
	{
		Call<void>("OnCopyScenePassed", player, task, id);
	}
}

//added by aprilliu 
void TaskScript::OnClearDungeon(Player * player, BaseTask* task, UInt32 dungeonId, UInt16 beated, UInt16 areas)
{
	//modified by aprilliu, 2016.04.25  增加Task* task参数，避免在script里查找
	//modified by huchenhui, 2016.06.24 Task* task参数从外面传递进来,避免再次查找.
	//Task* task = player->FindTask(GetId());
	if (task)
	{
		Call<void>("OnClearDungeon", player, task, dungeonId, beated, areas);
	}
	
}

void TaskScript::OnClearActivityDungeon(Player* player, BaseTask* task, UInt32 dungeonId, UInt16 beated, UInt16 areas)
{
	if (task)
	{
		Call<void>("OnClearActivityDungeon", player, task, dungeonId, beated, areas);
	}
}

void TaskScript::OnBabelPassed(Player* player, UInt32 id)
{
	//noted by aprilliu, 2016.04.25 暂时没有使用，所以暂时没有添加task参数
	Call<void>("OnBabelPassed", player, id);
}

void TaskScript::OnBuyMallItem(Player *pPlayer, BaseTask* pTask, UInt32 mallItemId, UInt32 mallItemNum)
{
	//noted by aprilliu, 2016.04.25 暂时没有使用，所以暂时没有添加task参数
	//modified by huchenhui, 2017.01.02	增加task参数
	if (pTask)
	{
		Call<void>("OnBuyMallItem", pPlayer, pTask, mallItemId, mallItemNum);
	}
}

void TaskScript::OnEnterTribe(Player* pPlayer)
{
	//noted by aprilliu, 2016.04.25 暂时没有使用，所以暂时没有添加task参数
	Call<void>("OnEnterTribe", pPlayer);
}

void TaskScript::OnStoryEnd(Player* player, UInt32 id, BaseTask* task)
{
	//modified by aprilliu, 2016.04.25  增加Task* task参数，避免在script里查找
	//modified by huchenhui, 2016.06.24 Task* task参数从外面传递进来,避免再次查找.
	//Task* task = player->FindTask(GetId());
	if (task)
	{
		Call<void>("OnStoryEnd", player,task, id);
	}
	
}

void TaskScript::OnTimeout(Player* player)
{
	//noted by aprilliu, 2016.04.25 暂时没有使用，所以暂时没有添加task参数
	Call<void>("OnTimeout", player);
}

void TaskScript::OnTaskSucced(Player* player, UInt32 taskId)
{
	//noted by aprilliu, 2016.04.25 暂时没有使用，所以暂时没有添加task参数
	Call<void>("OnTaskSucced", player, taskId);
}

//added by aprilliu, 2016.04.18
void TaskScript::OnSubmitTaskSucceed(Player* player, UInt32 taskId, BaseTask* task)
{
	//modified by aprilliu, 2016.04.25  增加Task* task参数，避免在script里查找
	//modified by huchenhui, 2016.06.24 Task* task参数从外面传递进来,避免再次查找.
	//Task* task = player->FindTask(GetId());
	if (task)
	{
		Call<void>("OnSubmitTaskSucceed", player, task, taskId);   //脚本里应该有OnSubmitTaskSucceed 函数
	}
}

void TaskScript::OnSubmitActiveTaskSucceed(Player* player, BaseTask* task, UInt32 templateId, UInt32 taskId)
{
	if (task)
	{
		Call<void>("OnSubmitActiveTaskSucceed", player, task, templateId, taskId);
	}
}

void TaskScript::OnSubmitCycleTaskSucceed(Player* player, BaseTask* task, UInt32 taskId)
{
	if (task)
	{
		Call<void>("OnSubmitCycleTaskSucceed", player, task, taskId);
	}
}

void TaskScript::OnTaskFailed(Player* player, UInt32 taskId)
{
	//noted by aprilliu, 2016.04.25 暂时没有使用，所以暂时没有添加task参数
	Call<void>("OnTaskFailed", player, taskId);
}

void TaskScript::OnOwnerDied(Player* player)
{
	//noted by aprilliu, 2016.04.25 暂时没有使用，所以暂时没有添加task参数
	Call<void>("OnOwnerDied", player);
}

void TaskScript::OnDailyTaskSubmitted(Player* player, BaseTask* task, UInt8 type)
{
	//noted by aprilliu, 2016.04.25 暂时没有使用，所以暂时没有添加task参数
	if (task)
	{
		Call<void>("OnDailyTaskSubmitted", player, task, type);
	}
}

void TaskScript::OnPVP(Player* player, BaseTask* task, UInt32 type, UInt32 status)
{
	//added by huchenhui 2016.08.05 传统决斗
	if (task)
	{
		Call<void>("OnPVP", player, task, type, status);
	}
}

void TaskScript::OnChangeOccu(Player* player, BaseTask* task)
{
	//added by huchenhui 2016.08.20 转职
	if (task)
	{
		Call<void>("OnChangeOccu", player, task);
	}
}

void TaskScript::OnAddMagic(Player* player, BaseTask* task)
{
	//added by huchenhui 2016.08.20 附魔
	if (task)
	{
		Call<void>("OnAddMagic", player, task);
	}
}

void TaskScript::OnAddMagicCard(Player* player, BaseTask* task)
{
	//added by huchenhui 2016.09.08 合成附魔卡
	if (task)
	{
		Call<void>("OnAddMagicCard", player, task);
	}
}

void TaskScript::OnOpenMagicJar(Player* player, BaseTask* task, JarType type, UInt32 combo)
{
	//added by huchenhui 2016.08.20 开罐子
	if (task)
	{
		Call<void>("OnOpenMagicJar", player, task, type, combo);
	}
}


void TaskScript::OnFriendGive(Player* player, BaseTask* task)
{
	//added by huchenhui 2016.08.23 好友赠送
	if (task)
	{
		Call<void>("OnFriendGive", player, task);
	}
}

void TaskScript::OnWarpStoneUpLevel(Player* player, BaseTask* task, WarpStoneType type, UInt32 level)
{
	//added by huchenhui 2016.08.23 次元石升级
	if (task)
	{
		Call<void>("OnWarpStoneUpLevel", player, task, type, level);
	}
}

void TaskScript::OnWarpStoneAddEnergy(Player* player, BaseTask* task, UInt32 energy)
{
	//added by huchenhui 2016.08.23 次元石充能
	if (task)
	{
		Call<void>("OnWarpStoneAddEnergy", player, task, energy);
	}
}

void TaskScript::OnRefreshShop(Player* player, BaseTask* task)
{
	//added by huchenhui 2016.08.23 刷新商店
	if (task)
	{
		Call<void>("OnRefreshShop", player, task);
	}
}

void TaskScript::OnShopBuy(Player* player, BaseTask* pTask, UInt32 shopId, UInt32 shopItemId, UInt32 num)
{
	if (pTask)
	{
		Call<void>("OnShopBuy", player, pTask, shopId, shopItemId, num);
	}
}

void TaskScript::OnRemoveFatigue(Player* player, BaseTask* task, UInt16 usedFatigue)
{
	//added by huchenhui 2016.08.29	消耗疲劳值
	if (task)
	{
		Call<void>("OnRemoveFatigue", player, task, usedFatigue);
	}
}

void TaskScript::OnBeginHell(Player* player, BaseTask* task, UInt32 dungeonId)
{
	//added by huchenhui 2018.01.08 开始深渊
	if (task)
	{
		Call<void>("OnBeginHell", player, task, dungeonId);
	}
}

void TaskScript::OnClearHell(Player* player, BaseTask* task, UInt32 dungeonId, UInt32 usedTime)
{
	//added by huchenhui 2016.08.30	通关深渊
	if (task)
	{
		Call<void>("OnClearHell", player, task, dungeonId, usedTime);
	}
}

void TaskScript::OnDeathTowerBegin(Player* player, BaseTask* task, UInt32 towerId)
{
	//added by huchenhui 2016.10.22	死亡之塔开始
	if (task)
	{
		Call<void>("OnDeathTowerBegin", player, task, towerId);
	}
}

void TaskScript::OnDeathTowerWipeoutBegin(Player* player, BaseTask* task)
{
	//added by huchenhui 2017.05.18 死亡之塔扫荡开始
	if (task)
	{
		Call<void>("OnDeathTowerWipeoutBegin", player, task);
	}
}

void TaskScript::OnDeathTower(Player* player, BaseTask* task, UInt32 towerId, UInt32 layer, UInt32 usedTime)
{
	//added by huchenhui 2016.08.30	死亡之塔
	if (task)
	{
		Call<void>("OnDeathTower", player, task, towerId, layer, usedTime);
	}
}

void TaskScript::OnAwaken(Player* player, BaseTask* task, AwakenStatus status)
{
	//added by huchenhui 2016.09.01	觉醒
	if (task)
	{
		Call<void>("OnAwaken", player, task, status);
	}
}

void TaskScript::OnReviveCoin(Player* player, BaseTask* task, bool isSelf)
{
	if (task)
	{
		Call<void>("OnReviveCoin", player, task, isSelf);
	}
}

void TaskScript::OnAddAuction(Player* player, BaseTask* task)
{
	if (task)
	{
		Call<void>("OnAddAuction", player, task);
	}
}

void TaskScript::OnSellAuction(Player* player, BaseTask* task, UInt32 money)
{
	if (task)
	{
		Call<void>("OnSellAuction", player, task, money);
	}
}

void TaskScript::OnBuyAuction(Player* player, BaseTask* task, UInt32 money)
{
	if (task)
	{
		Call<void>("OnBuyAuction", player, task, money);
	}
}

void TaskScript::OnSetTaskItem(Player* player, BaseTask* task, Item* item, UInt32 itemId, UInt32 num)
{
	if (task)
	{
		Call<void>("OnSetTaskItem", player, task, item, itemId, num);
	}
}

void TaskScript::OnFeedPet(Player* player, BaseTask* task, PetFeedType type)
{
	if (task)
	{
		Call<void>("OnFeedPet", player, task, type);
	}
}

void TaskScript::OnAddPet(Player* player, BaseTask* task, PetType petType, PetQuality petQuality)
{
	if (task)
	{
		Call<void>("OnAddPet", player, task, petType, petQuality);
	}
}

void TaskScript::OnPetUpLevel(Player* player, BaseTask* task, PetType petType, PetQuality petQuality, UInt16 level)
{
	if (task)
	{
		Call<void>("OnPetUpLevel", player, task, petType, petQuality, level);
	}
}

void TaskScript::OnAddRelation(Player* player, BaseTask* task, RelationType type)
{
	if (task)
	{
		Call<void>("OnAddRelation", player, task, type);
	}
}

void TaskScript::OnSendHorn(Player* player, BaseTask* task)
{
	if (task)
	{
		Call<void>("OnSendHorn", player, task);
	}
}

void TaskScript::OnJoinGuild(Player* player, BaseTask* task)
{
	if (task)
	{
		Call<void>("OnJoinGuild", player, task);
	}
}

void TaskScript::OnIncGuildBattleScore(Player* player, BaseTask* task, UInt32 score)
{
	if (task)
	{
		Call<void>("OnIncGuildBattleScore", player, task, score);
	}
}

void TaskScript::OnGuildOccupyTerritory(Player* player, BaseTask* task, UInt32 terrId)
{
	if (task)
	{
		Call<void>("OnGuildOccupyTerritory", player, task, terrId);
	}
}

void TaskScript::OnWinWudaodahuiPkEnd(Player* player, BaseTask* task, UInt32 winNum)
{
	if (task)
	{
		Call<void>("OnWinWudaodahuiPkEnd", player, task, winNum);
	}
}

void TaskScript::OnEnlargePackage(Player* player, BaseTask* task, UInt32 size)
{
	if (task)
	{
		Call<void>("OnEnlargePackage", player, task, size);
	}
}

void TaskScript::OnEnlargeStorage(Player* player, BaseTask* task, UInt32 size)
{
	if (task)
	{
		Call<void>("OnEnlargeStorage", player, task, size);
	}
}

void TaskScript::OnIncIntimacy(Player* player, ObjID_t targetId, BaseTask* task, UInt32 incIntimacy, UInt32 finalIntimacy)
{
	if (task)
	{
		Call<void>("OnIncIntimacy", player, targetId, task, incIntimacy, finalIntimacy);
	}
}

void TaskScript::OnIncDailyTaskScore(Player* player, BaseTask* task, UInt32 totalDailyTaskScore)
{
	if (task)
	{
		Call<void>("OnIncDailyTaskScore", player, task, totalDailyTaskScore);
	}
}

void TaskScript::OnTeamClearDungeon(Player* player, BaseTask* task, UInt32 dungeonId, UInt16 beated, UInt16 areas, std::vector<ObjID_t>& teammateIds)
{
	if (task)
	{
		ScriptVector64 teammateVec(teammateIds);
		Call<void>("OnTeamClearDungeon", player, task, dungeonId, teammateVec);
	}
}

void TaskScript::OnTeamClearActivityDungeon(Player* player, BaseTask* task, UInt32 dungeonId, UInt16 beated, UInt16 areas, std::vector<ObjID_t>& teammateIds)
{
	if (task)
	{
		ScriptVector64 teammateVec(teammateIds);
		Call<void>("OnTeamClearActivityDungeon", player, task, dungeonId, teammateVec);
	}
}

void TaskScript::OnTeamClearHell(Player* player, BaseTask* task, UInt32 dungeonId, UInt32 usedTime, std::vector<ObjID_t>& teammateIds)
{
	if (task)
	{
		ScriptVector64 teammateVec(teammateIds);
		Call<void>("OnTeamClearHell", player, task, dungeonId, usedTime, teammateVec);
	}
}

void TaskScript::OnTeamPVP(Player* player, BaseTask* task, UInt32 type, UInt32 status, std::vector<ObjID_t>& teammateIds)
{
	//added by huchenhui 2016.08.05 传统决斗
	if (task)
	{
		ScriptVector64 teammateVec(teammateIds);
		Call<void>("OnTeamPVP", player, task, type, status, teammateVec);
	}
}

void TaskScript::OnApprentDisciple(Player* player, BaseTask* task)
{
	if (task)
	{
		Call<void>("OnApprentDisciple", player, task);
	}
}

void TaskScript::OnDiscipleFinsch(Player* player, BaseTask* task)
{
	if (task)
	{
		Call<void>("OnDiscipleFinsch", player, task);
	}
}

void TaskScript::OnGuildEmblemUp(Player* player, BaseTask* task, UInt32 lvl)
{
	if (task)
	{
		Call<void>("OnGuildEmblemUp", player, task, lvl);
	}
}

void TaskScript::OnClearDungeonCareScore(Player* player, BaseTask* task, UInt32 dungeonId, LevelScore score)
{
	if (task)
	{
		Call<void>("OnClearDungeonCareScore", player, task, dungeonId, (UInt32)score);
	}
}

// *******************爬塔*************************//

void TaskScript::BindLostDungeonKillRoleNumEvent(UInt32 killNum)
{
	KillRoleNumEvent event;
	event.num = killNum;
	m_KillRoleNumEventList.push_back(event);
}

void TaskScript::BindLostDungeonAliveTimeEvent(UInt32 minTime)
{
	AliveTimeEvent event;
	event.aliveTime = minTime;
	m_AliveTimeEventList.push_back(event);
}

void TaskScript::BindLostDungeonClearEvent(UInt32 dungeonId, UInt32 clearNum)
{
	ClearLostDungeonEvent event;
	event.clearNum = clearNum;
	event.dungeonId = dungeonId;
	m_ClearLostDungeonEventList.push_back(event);
}

void TaskScript::BindLostDungeonKillMonsterEvent(UInt32 monsterId, UInt32 killNum)
{
	KillMonsterEvent event;
	event.killNum = killNum;
	event.monsterId = monsterId;
	m_KillMonsterEventList.push_back(event);
}

void TaskScript::BindLostDungeonGetItemEvent(UInt32 itemId, UInt32 itemNum)
{
	GetLostDungeonItemEvent event;
	event.itemId = itemId;
	event.itemNum = itemNum;
	m_GetItemEventList.push_back(event);
}

void TaskScript::OnLostDungeonKillRoleNum(Player* player, BaseTask* task, UInt32 num)
{
	if (task)
	{
		Call<void>("OnLostDungeonKillRoleNum", player, task, num);
	}
}

void TaskScript::OnLostDungeonKillFixRole(Player* player, BaseTask* task, UInt64 dieId)
{
	if (task)
	{
		Call<void>("OnLostDungeonKillFixRole", player, task, dieId);
	}
}

void TaskScript::OnLostDungeonAliveTime(Player* player, BaseTask* task, UInt32 time)
{
	if (task)
	{
		Call<void>("OnLostDungeonAliveTime", player, task, time);
	}
}

void TaskScript::OnLostDungeonClearNum(Player* player, BaseTask* task, UInt32 num)
{
	if (task)
	{
		Call<void>("OnLostDungeonClearNum", player, task, num);
	}
}

void TaskScript::OnLostDungeonFixClear(Player* player, BaseTask* task, UInt32 dungeonId, UInt32 num)
{
	if (task)
	{
		Call<void>("OnLostDungeonFixClear", player, task, dungeonId, num);
	}
}

void TaskScript::OnLostDungeonKillFixMonster(Player* player, BaseTask* task, UInt32 monsterId, UInt32 num)
{
	if (task)
	{
		Call<void>("OnLostDungeonKillFixMonster", player, task, monsterId, num);
	}
}

void TaskScript::OnLostDungeonCollectItem(Player* player, BaseTask* task, UInt32 itemId, UInt32 itemNum)
{
	if (task)
	{
		Call<void>("OnLostDungeonCollectItem", player, task, itemId, itemNum);
	}
}