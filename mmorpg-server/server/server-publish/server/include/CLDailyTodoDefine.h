#ifndef _CL_DAILY_TODO_DEFINE_H_
#define _CL_DAILY_TODO_DEFINE_H_
#include <CLDefine.h>

enum DailyTodoSubType
{
	DTSTP_NONE = 0,
	DTSTP_DIALY_TASK,			//�ճ�����
	DTSTP_MAIN_DUNGEON,			//���ߵ��³�
	DTSTP_SHENYUAN_DUNGEON,		//��Ԩ���³�
	DTSTP_YUANGU_DUNGEON,		//Զ�ŵ��³�
	DTSTP_CITY_MONSTER_DUNGEON,	//���﹥��
	DTSTP_XUKONG_DUNGEON,		//����ѷ�
	DTSTP_HUNDUN_DUNGEON,		//������³�
	DTSTP_COMMON_TEAM_COPY_ONE = 8,		//��������ͨ�ű�
	DTSTP_DIFF_TEAM_COPY_ONE = 18,		//������ج���ű�
	DTSTP_COMMON_TEAM_COPY_TWO = 19,	//Ⱥ�絺��ͨ�ű�
	DTSTP_DIFF_TEAM_COPY_TWO = 20,		//Ⱥ�絺ج���ű�

	DTSTP_MAX,
};

//ÿ�ձ���
struct DailyTodoInfo
{
	DailyTodoInfo()
	{
		id = 0;
		dataId = 0;
		dayProgress = 0;
		dayProgTime = 0;
		dayProgMax = 0;
		weekProgress = 0;
		weekProgTime = 0;
		weekProgMax = 0;
	}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & dataId & dayProgress & dayProgTime & dayProgMax & weekProgress & weekProgTime & weekProgMax;
	}
	UInt64		id;				//Ψһid
	UInt32		dataId;			//��id
	UInt32		dayProgress;	//ÿ�ս���
	UInt32		dayProgTime;	//ÿ�ս��ȸ���ʱ���
	UInt32		dayProgMax;		//ÿ�ս������ֵ
	UInt32		weekProgress;	//ÿ�ܽ���
	UInt32		weekProgTime;	//ÿ�ܽ��ȸ���ʱ���
	UInt32		weekProgMax;	//ÿ�ܽ������ֵ
};


#endif