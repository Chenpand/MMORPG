#ifndef _CL_SEASON_DEFINE_H_
#define _CL_SEASON_DEFINE_H_

#define SEASON_ID_KEY "season_id"
#define SEASON_START_TIME_KEY "season_start_time"
#define SEASON_DURATION_TIME_KEY "season_duration_time"

#define SEASON_CHANGE_ATTR_TIME "season_change_attr_time"

enum SeasonEventType
{
	SEASON_EVENT_TYPE_INVALID = 0,
	SEASON_EVENT_TYPE_SEASON = 1,
	SEASON_EVENT_TYPE_SEASON_ATTR = 2,
};

enum SeasonCyclesType
{
	SCT_INVALID,
	SCT_MONTH,	//��
	SCT_WEEK,	//��
};

// ����״̬
enum SeasonStatus
{
	GS_INVALID,			//δ��ʼ
	GS_PROGRESS,		//������
};

enum SeasonPlayStatus
{
	SPS_INVALID,			//
	SPS_NEW = 1,			//������
	SPS_NEW_ATT = 2,		//������
};

enum SeasonUplevelStatus
{ 
	SUS_INVALID,			//δ��ʼ������
	SUS_UPLEVEL,			//��������
};

enum SeasonMainLevel
{
	SML_BRONZE = 1,		//��ͭ
	SML_SILVER,			//����
	SML_GOLD,			//�ƽ�
	SML_PLATINUM,		//����
	SML_DIAMOND,		//��ʯ
	SML_KING,			//��ǿ����
};


struct SeasonExpEntry
{
	SeasonExpEntry(UInt32 _winExp, UInt32 _loseExp)
	{
		winExp = _winExp;
		loseExp = _loseExp;
	}

	UInt32 winExp;
	UInt32 loseExp;
};


#endif	//_CL_SEASON_DEFINE_H_
