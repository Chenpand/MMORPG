#ifndef _BASE_TASK_MGR_H_
#define _BASE_TASK_MGR_H_

#include <map>
#include <set>
#include <CLObjectProperty.h>
#include <CLTaskDefine.h>
#include <CLDungeonDefine.h>
#include <CLWarpStoneDefine.h>
#include <CLMatchDefine.h>
#include <CLRelationDefine.h>
#include <CLCityMonsterDataEntry.h>

#include "BaseTask.h"

class Player;
class Npc;

class BaseTaskMgr //: public CLComplexProperty
{
public:
	typedef std::map<UInt32, BaseTask*> TaskMap;
	typedef std::set<UInt32>		TaskIdSet;

public:
	BaseTaskMgr();
	virtual ~BaseTaskMgr();

public:
	void SetOwner(Player* owner){ m_pOwner = owner; }
	Player* GetOwner() const { return m_pOwner; }


public:

	/**
	*@brief ���һ������
	*/
	virtual bool AddTask(BaseTask* task);

	/**
	*@brief ����һ������
	*/
	bool LoadTask(BaseTask* task);

	/**
	*@brief ����һ������
	*/
	BaseTask* FindTask(UInt32 id) const;

	/**
	*@brief ɾ��һ������
	*/
	void RemoveTask(BaseTask* task);

	/**
	*@brief �����������
	*/
	void ClearTask();

	/**
	*@brief ��ȡ������
	*/
	virtual UInt32 GetTaskNum() const;

	/**
	*@brief ������������
	*/
	void VisitTasks(BaseTaskVisitor& visitor);

	/**
	*@brief ��ȡ�����б�
	*/
	void GetTaskList(std::vector<TaskBriefInfo>& task);

	//added by aprilliu, 2016.04.20 
	//���ݵ��³�id ��������ѽӵĸõ��³���ص������б�
	void GetDungeonTaskList(UInt32 dungeonId, std::vector<UInt32>& taskList);

	virtual bool GetInitTaskBriefInfo(UInt32 id, TaskBriefInfo& info);

	virtual void CheckFinish();

public:

	/**
	*@brief ����һ������
	*/
	virtual bool AcceptTask(UInt32 id, UInt8 quality, bool bForce) { return true; }

	/**
	*@brief �ύһ������
	*/
	virtual bool SubmitTask(BaseTask* task, bool force = false) = 0;

	/**
	*@brief ����һ������
	*/
	virtual bool AbandonTask(BaseTask* task) { return true; };

	/**
	*@brief �Ƿ��ܹ���ȡһ������
	*/
	virtual bool IsTaskAcceptable(UInt32 id, bool bNotifyErr = false) { return true; }

	/**
	*@brief ��ȡ���������
	*/
	virtual UInt32 GetMaxTaskNum() = 0;

public:
	/**
	*@brief tick�¼�
	*/
	virtual void OnHeartbeat(const Avalon::Time& now, int tickType) {};

	/**
	*@brief �������
	*/
	virtual void OnOnline(bool bDayChanged) = 0;

	/**
	*@brief �������
	*/
	virtual void OnLevelup() {};

	/**
	*@brief ����ڶ���
	*/
	virtual void OnDayChanged() {};

	/**
	*@brief ��õ���
	*/
	void OnGetItem(UInt32 id, UInt8 quality, UInt8 strengthen, UInt16 num);

	//added by aprilliu, ɾ������ (֮�������ӸĽӿ�����Ϊ �Ѽ������� ��ҿ��������Ѽ����ĵ��ߣ�����������������״̬������ɱ�Ϊδ���
	void OnRmItem(UInt32 id, UInt8 quality, UInt8 strengthen, UInt16 num);

	/**
	*@brief ʹ�õ���
	*/
	void OnUseItem(UInt32 id);

	/**
	*@brief ��Ϸ�ұ仯  numΪ������ʾ����, ������ʾ����   added by huchenhui 2016.06.28
	*/
	void OnChangeMoney(UInt32 type, Int32 num);

	/**
	*@brief ���й���Ȩ��npc����
	*/
	void OnOwnedNpcDied(UInt32 npcid, UInt8 type, UInt32 dungeon);

	/**
	*@brief ָ��NPC����
	*/
	void OnKillCityMonster(CityMonsterType cityMonsterType, UInt32 monsterId, UInt64 monsterGuid);

	/**
	*@brief �ƶ�
	*/
	void OnMoved();

	/**
	*@brief ����װ��
	*/
	void OnRemakeEquip(UInt32 id, UInt8 type);

	/**
	*@brief ����װ����
	*/
	void OnCalculateValuedScore(ItemSubType subType, ItemQuality quality, UInt32 score);

	/**
	*@brief ��Ƕ����
	*/
	void OnAddPreciousBead();

	/**
	*@brief ǿ��װ�� Adder by huchenhui 2016.06.24
	*/
	void OnStrengthenEquip(UInt32 errorCode, UInt8 oldStrengthen, UInt8 newStrengthen);

	/**
	*@brief �ֽ�װ�� Adder by huchenhui 2016.06.24
	*/
	void OnDecomposeEquip();

	/**
	*@brief ����װ�� Adder by huchenhui 2016.06.29
	*/
	void OnPutEquip(UInt16 level, UInt32 subType, UInt8 color, UInt32 strengthen);

	/**
	*@brief ����װ�� Adder by huchenhui 2018.03.20
	*/
	void OnPutFashion(ItemSubType subType, UInt8 color, UInt8 isTransparent);

	/**
	*@brief �������
	*/
	void OnCatchPet(UInt32 id);

	/**
	*@brief �������¼�
	*/
	void OnAddRetinue(RetinueQuality quality);

	/**
	*@brief ������ϴ���¼�
	*/
	void OnRetinueChangeSkill(std::vector<RetinueSkill> newSkills);

	/**
	*@brief ��������¼�
	*/
	void OnRetinueUpLevel(UInt32 retinueId, UInt8 retinueLevel);

	/**
	*@brief ��������¼�
	*/
	void OnRetinueUpStar(UInt32 retinueId, UInt8 retinueStar);

	/**
	*@brief ����ͨ��
	*/
	void OnCopyScenePassed(UInt32 id);

	/**
	*@ ͨ�ص��³� dungeonId - ���³�id�������Ѷ���Ϣ; beated - ͨ��ʱ�ı�������; areas -ͨ���ķ�����; score - �ؿ�����; usedTime - ʹ��ʱ��, reviveCount - �������
	*/
	void OnClearDungeon(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime, UInt32 reviveCount);

	/**
	*@ ͨ�ػ���³� dungeonId - ���³�id�������Ѷ���Ϣ; beated - ͨ��ʱ�ı�������; areas -ͨ���ķ�����; score - �ؿ�����; usedTime - ʹ��ʱ��, reviveCount - �������
	*/
	void OnClearActivityDungeon(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime, UInt32 reviveCount);

	/**
	*@brief ͨ����ͨ��
	*/
	void OnBabelPassed(UInt32 id);

	/**
	*@brief �����̳ǵ���
	*/
	void OnBuyMallItem(UInt32 mallItemId, UInt32 mallItemNum);

	/**
	*@brief ���벿��
	*/
	void OnEnterTribe();

	/**
	*@brief �������
	*/
	void OnStoryEnd(UInt32 id);

	/**
	*@brief ����ɹ�
	*/
	virtual void OnTaskSucced(BaseTask* task);

	//added by aprilliu, 2016.04.18   �ɹ��ύ����
	void OnSubmitTaskSucceed(UInt32 taskId);

	//added by huchenhui
	void OnSubmitActiveTaskSucceed(UInt32 templateId, UInt32 taskId);

	//added by huchenhui
	void OnSubmitCycleTaskSucceed(UInt32 taskId);

	/**
	*@brief ����ʧ��
	*/
	void OnTaskFailed(UInt32 id);

	/**
	*@brief ����
	*/
	void OnDied();

	/**
	*@brief �ճ������ύ
	*/
	void OnDailyTaskSubmitted(UInt32 taskId, UInt8 type);

	/**
	*@brief ��ͳ����
	*/
	void OnPVP(PkType type, PkRaceResult status);

	/**
	*@brief תְ
	*/
	virtual void OnChangeOccu();

	/**
	*@brief ��ħ
	*/
	void OnAddMagic();

	/**
	*@brief ��ħ���ϳ�
	*/
	void OnAddMagicCard();

	/**
	*@brief ������
	*/
	void OnOpenMagicJar(JarType type, UInt32 combo);

	/**
	*@brief ��������
	*/
	void OnFriendGive();

	/**
	*@brief ��Ԫʯ����
	*/
	void OnWarpStoneUpLevel(WarpStoneType type, UInt32 level);

	/**
	*@brief ��Ԫʯ����
	*/
	void OnWarpStoneAddEnergy(UInt32 energy);

	/**
	*@brief �̵�ˢ��
	*/
	void OnRefreshShop();

	/**
	*@brief �̵깺����Ʒ
	*/
	void OnShopBuy(UInt32 shopId, UInt32 shopItemId, UInt32 num);

	/**
	*@brief ƣ��ֵ����
	*/
	void OnRemoveFatigue(UInt16 usedFatigue);

	/**
	*@brief ��ʼ��Ԩ
	*/
	void OnBeginHell(UInt32 dungeonId);

	/**
	*@brief ͨ����Ԩ
	*/
	void OnClearHell(UInt32 dungeonId, UInt32 usedTime, LevelScore score);

	/**
	*@brief ����֮����ʼ
	*/
	void OnDeathTowerBegin(UInt32 towerId);

	/**
	*@brief ����֮��ɨ����ʼ
	*/
	void OnDeathTowerWipeoutBegin();

	/**
	*@brief ����֮��
	*/
	void OnDeathTower(UInt32 towerId, UInt32 layer, UInt32 usedTime);

	/**
	*@brief ����
	*/
	void OnAwaken();

	/**
	*@brief �����
	*/
	void OnReviveCoin(bool isSelf);

	/**
	*@brief ������ ����
	*/
	void OnAddAuction();

	/**
	*@brief ������ ����
	*/
	void OnSellAuction(UInt32 money);

	/**
	*@brief ������ ����
	*/
	void OnBuyAuction(UInt32 money);

	/**
	*@brief �ύ������Ʒ
	*/
	void OnSetTaskItem(ItemQuality quality, UInt32 itemId, UInt32 num);

	/**
	*@brief ιʳ����
	*/
	void OnFeedPet(PetFeedType type);

	/**
	*@brief ��ó���
	*/
	void OnAddPet(PetType petType, PetQuality petQuality);

	/**
	*@brief ��������
	*/
	void OnPetUpLevel(PetType petType, PetQuality petQuality, UInt16 level);

	/**
	*@brief ��������ϵ
	*/
	void OnAddRelation(RelationType type);

	/**
	*@brief ������
	*/
	void OnSendHorn();

	/**
	*@brief ���빤��
	*/
	void OnJoinGuild();

	/**
	*@brief ���ӹ���ս����
	*/
	void OnIncGuildBattleScore(UInt32 score);

	/**
	*@brief ռ�칤��ս���
	*/
	void OnGuildOccupyTerritory(UInt8 terrId);

	/**
	*@brief ������
	*/
	void OnWinWudaodahuiPkEnd(UInt32 winNum);

	/**
	*@brief ��չ���� Adder by huchenhui 2018.06.11
	*/
	void OnEnlargePackage(UInt32 size);

	/**
	*@brief ��չ�ֿ� Adder by huchenhui 2018.06.11
	*/
	void OnEnlargeStorage(UInt32 size);

	/**
	*@brief ���Ѷ�����
	*/
	void OnIncIntimacy(ObjID_t targetId, UInt32 incIntimacy, UInt32 finalIntimacy);

	/**
	*@brief ���ͨ�ص��³�
	*/
	void OnTeamClearDungeon(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime, UInt32 reviveCount, std::vector<ObjID_t>& teammateIds);

	/**
	*@brief ���ͨ�ػ���³�
	*/
	void OnTeamClearActivityDungeon(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime, UInt32 reviveCount, std::vector<ObjID_t>& teammateIds);

	/**
	*@brief ���ͨ����Ԩ���³�
	*/
	void OnTeamClearHell(UInt32 dungeonId, UInt32 usedTime, LevelScore score, std::vector<ObjID_t>& teammateIds);

	/**
	*@brief ��������ճ��������(��Ծ��)
	*/
	void OnIncDailyTaskScore(UInt32 totalScore);

	/*
	*@brief ��ͽ�¼�
	*/
	void OnApprentDisciple();

	/*
	*@brief ͽ�ܳ�ʦ�¼�
	*/
	void OnDiscipleFinsch();

	/*
	*@brief ����ռ�����
	*/
	void OnGuildEmblemUp(UInt32 lvl);

	/**
	*@brief ͨ�ص��³� dungeonId - ���³�id�������Ѷ���Ϣ; score - �ؿ�����; usedTime - ʹ��ʱ��, reviveCount - �������
	*/
	void OnClearDungeonCareScore(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime, UInt32 reviveCount);

	/*
	 *@brief �ű�ͨ��
	 */
	void OnClearTeamCopy(UInt32 teamType, UInt32 grade, UInt32 costTime);

public:
	void Input(Avalon::NetInputStream& stream);
	void Output(Avalon::NetOutputStream& stream);

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

	void SaveToDB();

protected:

	//����map
	TaskMap		m_Tasks;

	//ӵ����
	Player*		m_pOwner;
};

class GetTaskInfoVisitor : public BaseTaskVisitor
{
public:
	GetTaskInfoVisitor(std::vector<TaskBriefInfo>& taskList)
		:m_TaskList(taskList) {}

	bool operator()(BaseTask* task)
	{
		m_TaskList.resize(m_TaskList.size() + 1);
		task->GetBriefInfo(*(m_TaskList.end() - 1));
		return true;
	}
private:
	std::vector<TaskBriefInfo>& m_TaskList;
};

#endif