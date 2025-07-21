#ifndef _WS_TASK_SCRIPT_H_
#define _WS_TASK_SCRIPT_H_

#include <AvalonScriptFile.h>
#include <CLObjectDefine.h>
#include <CLDungeonDefine.h>
#include <CLWarpStoneDefine.h>
#include <CLMatchDefine.h>
#include <CLErrorCode.h>
#include <CLRelationDefine.h>
#include <CLCityMonsterDataEntry.h>

struct AccountInfo;
class Npc;
class RewardGroup;
class AccountTask;
class Item;

class TaskScript : public Avalon::ScriptFile
{
	typedef std::vector<ItemReward> ItemRewardVec;
	typedef std::vector<UInt32>	IdVec;


	//added by aprilliu.2016.04.13
	struct stKillNpcEvent //��ɱ������¼�
	{
		stKillNpcEvent() :id(0), type(0), dungeon(0)
		{
		}

		UInt32	id;				//����id
		UInt8	type;			//��������
		UInt32  dungeon;		//���������ĵ��³�id   added by aprilliu,2016.05.27
	};
	typedef	std::vector<stKillNpcEvent>		KillNPCEventList;

	struct stRoleNumChangeEvent
	{
		stRoleNumChangeEvent():level(0),occu(0)
		{

		}

		// �ȼ�
		UInt16 level;
		// ְҵ
		UInt8 occu;
	};

	struct PlayerClearDungeonEvent
	{
		UInt32 id;
		LevelScore score;
		UInt32 usedTime;
	};

	typedef std::vector<PlayerClearDungeonEvent> PlayerClearDungeonEventList;

	struct SetTaskItemEvent
	{
		SetTaskItemEvent() :quality(ITEM_QUALITY_INVLID), subType(0), level(0), itemId(0), num(0) {}
		ItemQuality quality;
		UInt8  subType;
		UInt32 level;
		UInt32 itemId;
		UInt32 num;
	};
	typedef std::vector<SetTaskItemEvent> SetTaskItemEventList;

public:
	enum CondRoleNumTaskReason
	{
		crntr_level = 1,
		crntr_occu = 2,
	};

public:
	TaskScript(UInt32 id, Avalon::ScriptState* state);
	~TaskScript();

public:
	const char*  GetScriptParam(){ return m_TaskParams.c_str(); }
	void SetScriptParam(std::string scriptParam) { m_TaskParams = scriptParam; };

public:
	/**
	*@brief ��ɱ�������¼�
	*/
	void BindKillNpcEvent(UInt32 id, UInt32 dungeon);
	void BindKillNpcTypeEvent(UInt8 type, UInt32 dungeon);
	bool CheckKillNpcEvent(UInt32 id, UInt8 type, UInt32 dungeon) const;

	/**
	*@brief ��ð�նӵȼ��ı��¼�
	*/
	void BindAdventureTeamLevelChangedEvent();
	bool CheckAdventureTeamLevelChangedEvent();

	/**
	*@brief ��ð�ն������ı��¼�
	*/
	void BindAdventureTeamGradeChangedEvent();
	bool CheckAdventureTeamGradeChangedEvent();

	/**
	*@brief �󶨽�ɫ�����ı�
	*/
	void BindRoleNumChangedEvent(UInt16 level, UInt8 occu);
	void GetRoleNumChangedEventCond(UInt16& level, UInt8& occu);
	bool CheckRoleNumChangedEvent(UInt16 level, UInt8 occu, UInt8 reason);

	/**
	*@brief �����ͨ���¼�
	*/
	void BindPlayerClearDungeonEvent(UInt32 id, UInt32 usedTime, LevelScore score);
	bool CheckPlayerClearDungeonEvent(UInt32 id, UInt32 usedTime, LevelScore score);

	/**
	*@brief �����ͨ���¼�
	*/
	void BindPlayerClearHellEvent(UInt32 id, UInt32 usedTime, LevelScore score);
	bool CheckPlayerClearHellEvent(UInt32 id, UInt32 usedTime, LevelScore score);

	/**
	*@brief �������ɹ��﹥���¼�
	*/
	void BindPlayerClearCityMonsterEvent();
	bool CheckPlayerClearCityMonsterEvent();

	/**
	*@brief ���������ƣ���¼�
	*/
	void BindPlayerRemoveFatigueEvent();
	bool CheckPlayerRemoveFatigueEvent();

	/**
	*@brief ��Ӷ�����ܳ���������¼�
	*/
	void BindPlayerCompAdventWeeklyTaskEvent(UInt32 adventCompWeeklyTaskLevel);
	bool CheckPlayerCompAdventWeeklyTaskEvent(UInt32 taskLevel);

	/**
	*@brief �ύ������Ʒ
	*/
	void BindSetTaskItemEvent(ItemQuality quality, UInt8 subType, UInt32 level);
	void BindSetTaskItemByIdEvent(UInt32 itemId, UInt32 num);
	bool CheckSetTaskItemEvent(ItemQuality quality, UInt8 subType, UInt32 level, UInt32 itemId, UInt32& num, UInt32 checkIndex);
	UInt32 GetSetTaskItemEventNum();

public: //�ű��¼�
	/**
	*@brief ��ʼ��
	*/
	void Init();

	/**
	*@brief ����Ƿ�ɽ�
	*/
	bool CheckAccept(AccountInfo* accountInfo);

	/**
	*@brief �ӵ���������
	*/
	void OnAccepted(AccountInfo* accountInfo, AccountTask* task);

	/**
	*@brief ����Ƿ���ύ
	*/
	bool CheckSubmit(AccountInfo* accountInfo);

	/**
	*@brief �ύ����
	*/
	void OnSubmitted(AccountInfo* accountInfo, AccountTask* task);

	/**
	*@brief ȡ������
	*/
	void OnAbandomed(AccountInfo* accountInfo, AccountTask* task);

	/**
	*@brief ��������Ƿ����
	*/
	void CheckFinish(AccountInfo* accountInfo, AccountTask* task);

public:
	/**
	*@brief ɱ��npc�¼�
	*/
	void OnKillNpc(AccountInfo* player, AccountTask* task, UInt32 npcid, UInt8 type, UInt32 dungeonId);

	/**
	*@brief ð�ն�(Ӷ����)�ȼ��ı�
	*/
	void OnAdventureTeamLevelChanged(AccountInfo* player, AccountTask* task);

	/**
	*@brief ð�ն�(Ӷ����)�����ı�
	*/
	void OnAdventureTeamGradeChanged(AccountInfo* player, AccountTask* task);

	/**
	*@brief ��ɫ�����ı�
	*/
	void OnRoleNumChanged(AccountInfo* player, AccountTask* task, UInt32 num);

	/**
	*@brief ��ʼ����ɫ����
	*/
	void OnInitRoleNum(AccountInfo* player, AccountTask* task, UInt32 num);

	/**
	*@brief ���Ӷ�����ܳ�����
	*/
	void OnCompAdventWeeklyTask(AccountInfo* player, AccountTask* task, UInt32 taskLevel);
public:
	/**
	*@brief ���ͨ��
	*/
	void OnPlayerClearDungeon(AccountInfo* player, AccountTask* task, const DungeonDataEntry* dungeonData, LevelScore score, UInt32 usedTime);

	/**
	*@brief ���ͨ����Ԩ
	*/
	void OnPlayerClearHell(AccountInfo* player, AccountTask* task, const DungeonDataEntry* dungeonData, LevelScore score, UInt32 usedTime);

	/**
	*@brief �����ɹ��﹥��
	*/
	void OnPlayerClearCityMonster(AccountInfo* player, AccountTask* task, UInt32 hardType);

	/**
	*@brief �������ƣ��
	*/
	void OnPlayerRemoveFatigue(AccountInfo* player, AccountTask* task, UInt32 removeFatigue);

private:

	std::string m_TaskParams;

	/***************** �ɾͲ��� *****************/

	//ɱ��npc�¼��б�
	KillNPCEventList	m_killNpcEvents;
	//ð�ն�(Ӷ����)�ȼ��ı��¼�
	bool m_AdventureTeamLevelChangeEvent;
	//ð�ն�(Ӷ����)�����ı��¼�
	bool m_AdventureTeamGradeChangeEvent;
	//��ɫ�����ı��¼�
	stRoleNumChangeEvent m_RoleNumChangeEvent;
	//���ð���������¼�
	bool m_AdventCompWeeklyTaskEvent;
	UInt32	m_AdventCompWeeklyTaskLevel;
	/***************** ���񲿷� *****************/

	//ͨ���¼�
	PlayerClearDungeonEventList m_PlayerClearDungeonEvents;
	//ͨ����Ԩ�¼�
	PlayerClearDungeonEventList m_PlayerClearHellEvents;
	//��ɹ��﹥���¼�
	bool m_PlayerClearCityMonsterEvent;
	//ƣ�������¼�
	bool m_PlayerRemoveFatigue;
	//�Ͻ�������Ʒ
	SetTaskItemEventList m_SetTaskItemEvents;
};



#endif //_WS_TASK_SCRIPT_H_