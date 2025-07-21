#ifndef _CL_SCRIPTDEFINE_H_
#define _CL_SCRIPTDEFINE_H_

#include "CLDefine.h"
#include <AvalonScriptState.h>

/**
 *@brief �ű�������
 */
class CLScriptErrorHandler : public Avalon::ScriptErrorHandler
{
public:
	void OnError(const char* str)
	{
		SYSLOG_ERROR(str);
		++SCRIPT_ERROR;
	}
};

//���ð�·��
#define PACKAGE_PATH		"Scripts/Common/?.lua"


//ϵͳ��ʾ
#define SYSNOTIFY_SCRIPT_PATH	"Scripts/SysNotify.lua"

//npc�ű�
#define NPC_SCRIPT_PATH		"Scripts/Npc/"
//����
#define TASK_SCRIPT_PATH	"Scripts/Task/"
//��������
#define LOST_DUNGEON_TASK_SCRIPT_PATH	"Scripts/LostDungeonTask/"
//�˺�����
#define ACCOUNT_TASK_SCRIPT_PATH	"Scripts/AccountTask/"
//����
#define ITEM_SCRIPT_PATH	"Scripts/Item/"
//����
#define SCENE_SCRIPT_PATH	"Scripts/Scene/"
//AI
#define AI_SCRIPT_PATH		"Scripts/AI/"
//vehicle ai
#define VEHICLEAI_SCRIPT_PATH	"Scripts/VehicleAI/"
//AI bind
#define AI_BIND_SCRIPT_PATH	"Scripts/AI/AI_Bind.lua"

//ɳ��
#define SANDBOX_SCRIPT_PATH		"Scripts/SandBox.lua"

//����
#define COUNTER_SCRIPT_PATH	"Scripts/Counter.lua"
//�ճ�
#define ROUTINE_SCRIPT_PATH	"Scripts/Routine.lua"
//���
#define PLAYER_SCRIPT_PATH	"Scripts/Player.lua"
//���
#define GIFTBAG_SCRIPT_PATH	"Scripts/GiftBag.lua"
//��������
#define TERRITORY_SCRIPT_PATH "Scripts/Territory.lua"
//�ռ�װ��
#define FINALEQUIP_SCRIPT_PATH "Scripts/FinalEquip.lua"
//װ������
#define EQUIPCALC_SCRIPT_PATH "Scripts/EquipCalc.lua"
//���ͽű�·��
#define ESCORT_SCRIPT_PATH		"Scripts/Escort.lua"
//�����ű�
#define ACGIFTBAG_SCRIPT_PASH	"Scripts/ActivityGiftbag.lua"

//world��ʱ��
#define WORLD_TIMER_SCRIPT_PATH "Scripts/WorldTimer.lua"
//ս��
#define BATTLESCENE_SCRIPT_PATH "Scripts/BattleScene.lua"
//��̨��
#define CHALLENGE_SCRIPT_PATH "Scripts/Challenge.lua"
//�����
#define GSACTIVITY_SCRIPT_PATH "Scripts/SortlistActivity.lua"
//�°濪���
#define NEW_GSACTIVITY_SCRIPT_PATH "Scripts/SortlistActivity_new.lua"
//�̳�
#define MALL_SCRIPT_PATH "Scripts/Mall.lua"
//world���
#define WORLD_PLAYER_SCRIPT_PATH "Scripts/WorldPlayer.lua"
//world����
#define WORLD_CONFIG_SCRIPT_PATH "Scripts/WorldConfig.lua"

//�����ս
#define WORLD_CHALLENGE_SCRIPT_PATH "Scripts/WorldChallenge.lua"

//�
#define ACTIVITY_SCRIPT_PATH "Scripts/Activity/"

//���ݿⲹ��
#define DBPATCH_SCRIPT_PATH	"Scripts/DBPatch.lua"

#endif
