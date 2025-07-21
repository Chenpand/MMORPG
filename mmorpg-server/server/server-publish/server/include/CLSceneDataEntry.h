#ifndef _CL_SCENE_DATAENTRY_H_
#define _CL_SCENE_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum SceneSubType
{
	SCENE_SUB_NULL,
	SCENE_SUB_TRADITION,
	SCENE_SUB_WUDAO,
	SCENE_SUB_GUILD_BATTLE,
	// �ͽ�����
	SCENE_SUB_PREMIUM_LEAGUE,
	//����
	SCENE_SUB_ROOM,
	// �������ս
	SCENE_SUB_CROSS_GUILD_BATTLE,
	//33����������
	SCENE_SUB_SCORE_WAR,
	//���᳡��
	SCENE_SUB_GUILD,
	//ս������
	SCENE_SUB_BATTLE,
	//�Լ�׼������
	SCENE_SUB_CHIJI,
	//��ƽ������׼��
	SCENE_EQUAL_PVP,
	//�ű�׼������
	SCENE_SUB_TEAM_COPY_PREPARE = 12,
	//�ű����᳡��
	SCENE_SUB_TEAM_COPY_ATTACK = 13,
	//2V2�Ҷ������
	SCENE_SUB_2V2_ACTIVITY = 14,
	//��ʧ����ս������
	SCENE_SUB_LOST_DUNGEON_BATTLE = 15,
	//��ʧ���ε��³�ѡ�񳡾�
	SCENE_SUB_LOST_DUNGEON_ENTERY = 16,
	//�ھ���׼������
	SCENE_SUB_CHAMPION_PREPARE = 17,
};

/**
 *@brief scene������
 */
class SceneDataEntry : public Avalon::DataEntry
{
public:
	SceneDataEntry();
	~SceneDataEntry();

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

	const CLPosition& GetRevivePos() const;

	// �ж��Ƿ��ǿ������
	bool IsCrossScene() const { return subType == SCENE_SUB_CROSS_GUILD_BATTLE || subType == SCENE_SUB_SCORE_WAR || subType == SCENE_SUB_ROOM; }

	// �Լ�ս������
	bool IsBattleScene() const { return subType == SCENE_SUB_BATTLE; }

	// �Լ�׼������
	bool IsChiJiScene() const { return subType == SCENE_SUB_CHIJI; }

	// �ű�׼������
	bool IsTeamCopyPrepareScene() const { return subType == SCENE_SUB_TEAM_COPY_PREPARE; }

	// �ű����᳡��
	bool IsTeamCopyAttackScene() const { return subType == SCENE_SUB_TEAM_COPY_ATTACK; }

	// �ű�����
	bool IsTeamCopyScene() const { return subType == SCENE_SUB_TEAM_COPY_PREPARE || subType == SCENE_SUB_TEAM_COPY_ATTACK; }

	// 2v2�Ҷ��
	bool Is2V2Activity() const { return subType == SCENE_SUB_2V2_ACTIVITY; }

	//�ھ�������
	bool IsChampionScene() const { return subType == SCENE_SUB_CHAMPION_PREPARE; }


	// ��ʧ����ս������
	bool IsLostDungeonBattleScene() const { return subType == SCENE_SUB_LOST_DUNGEON_BATTLE || subType == SCENE_SUB_LOST_DUNGEON_ENTERY;}

	// ��ʧ���ε��³���ڳ���
	bool IsLostBattleDungeonEntryScene() const { return  subType == SCENE_SUB_LOST_DUNGEON_ENTERY; }
public:
	//id
	UInt32	id;
	//����
	std::string	name;
	//����
	UInt8	type;
	//����ȼ�
	UInt16	level;
	//ѡ��
	UInt16	option;
	SceneSubType subType;
	//ͬ����Χ
	UInt32	syncRange;
    //��������
    UInt32	birthCity;
	//����
	UInt32	majorCity;
	//�������
	UInt32	maxPlayerNum;
	//����ʱ��
	UInt32	reviveTime;
	//��������ָ���
	UInt32	recoverPercent;
	//�����
	CLPosition	revivePos[3];
	//��ڵ�X
	UInt16	enterX;
	//��ڵ�Y
	UInt16	enterY;
	//��������
	UInt8	dailytimes;
	//���ĵ���
	UInt32	itemId;
    //���еĵ��³�
    std::vector<UInt32>     dungeonList;
	//�ȼ�����
	UInt16	levelLimit;
	//���³�id
	UInt32	dungeonId;
};

typedef Avalon::DataEntryMgr<SceneDataEntry> SceneDataEntryMgr;

typedef Avalon::DataEntryVisitor<SceneDataEntry> SceneDataEntryVisitor;

#endif
