#ifndef _CL_BET_HORSE_DEFINE_H_
#define _CL_BET_HORSE_DEFINE_H_

// ���ʷŴ�1000��
#define RATE_EXPAND_NUM		(10000)

enum WeatherType
{
	WEATHER_RAIN = 1,	// ����
	WEATHER_SUNNY = 2,	// ����
	WEATHER_FOGGY = 3,	// ����
};

enum TerrainType
{
	TERRAIN_MOUNTAIN = 1,	// ɽ��
	TERRAIN_TOWN = 2,	// ����
	TERRAIN_PLAIN = 3,	// ƽԭ
};

enum ShooterStatusType
{
	SHOOTER_STATUS_EXCELLENT = 1, // ����(��ɫ)
	SHOOTER_STATUS_GOOD = 2, // ����(��ɫ)
	SHOOTER_STATUS_INSTABLE = 3, // ���ȶ�(��ɫ)
	SHOOTER_STATUS_COMMONLY = 4, // �ϲ�(��ɫ)
};

enum BetHorsePhaseType
{
	PHASE_TYPE_READY = 1,	// ������׼��
	PHASE_TYPE_STAKE = 2,	// Ѻע�׶�(1-90����)
	PHASE_TYPE_ADJUST = 3,	// �����׶�(91-100����)
	PHASE_TYPE_RESULT = 4,	// ����׶�(101-120����)
	PHASE_TYPE_DAY_END = 5,	// ���ձ�������
};

enum BetHorseOccupationType
{
	OCCUPATION_TYPE_SNIPER	= 1,	// �ѻ�ǹ
	OCCUPATION_TYPE_RIFLE = 2,	// ���ǹ
	OCCUPATION_TYPE_ASSAULT = 3,	// ͻ����ǹ
};

enum MapPhaseType
{
	MAP_PHASE_FIRST	= 1,	// ���ֵ�ͼ
	MAP_PHASE_SECOND = 2,	// ���ֵ�ͼ
	MAP_PHASE_LAST = 3,		// ���һ��
};


#endif