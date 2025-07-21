#ifndef _CL_SCENE_DEFINE_H_
#define _CL_SCENE_DEFINE_H_

#include <CLDefine.h>
#include <AvalonRandom.h>

//����
#define MAJORCITY_MAPID 5000
#define MAPID_CHIJI_SINGLE	10001
#define MAPID_CHIJI_BATTLE	10100
#define MAPID_CHIJI_PREPARE	10101
#define	MAPID_LOST_DUNGEON	11000
#define MAPID_LOST_DUNGEON_ENTRY	11001

//��ȡ����������
inline UInt32 GetNeutralRandMajorCity()
{
	const static UInt32 maps[] = {713, 714, 715, 716, 717};
	return maps[Avalon::Random::RandBetween(0, sizeof(maps) / sizeof(UInt32) - 1)];
}

/**
 *@brief �赲λ
 */
enum SceneBlockFlag
{
	SCENE_BLOCK_NONE =			0,		//���赲
	SCENE_BLOCK_PHYSICS =	1 << 0,		//�����赲
	SCENE_BLOCK_MAGIC =		1 << 1,		//ħ���赲
	SCENE_BLOCK_ALPHA =		1 << 2,		//͸�������ͻ�����
	SCENE_BLOCK_SAFEAREA =	1 << 3,		//��ȫ��

	SCENE_BLOCK_TRAP =		1 << 4,		//����
	SCENE_BLOCK_TRANSPOINT =	1 << 5,	//��ת��
	SCENE_BLOCK_NPC =		1 << 6,		//npc�赲
	SCENE_BLOCK_ITEM =		1 << 7,		//�����赲
};

/**
 *@brief ��ͼ����
 */
enum SceneType
{
	SCENE_TYPE_INVALID =	0,		// ��Ч����
	SCENE_TYPE_NORMAL =		1,		// ��ͨ��ͼ
	SCENE_TYPE_PRIVATESCENE =	2,	// ���˳���
	SCENE_TYPE_TEAMCOPY =	3,		// ���鸱��
	SCENE_TYPE_BATTLE =		4,		// ս������
	SCENE_TYPE_PRIVATE_TEAMCOPY = 5,// ����ӵ�еĶ��鸱��
	SCENE_TYPE_PUBLICCOPY = 6,		// ��������
	SCENE_TYPE_TRIBE = 7,			// ���丱��
	SCENE_TYPE_GHOST = 8,			// ���񳡾�
    SCENE_TYPE_SINGLE = 9,          // ���˳���  
    SCENE_TYPE_DUNGEON_ENTRY = 10,  // ѡ����³ǵĳ���
	SCENE_TYPE_PK_PREPARE = 11,     // ս��׼�����ĳ���
	SCENE_TYPE_PK = 12,				// pk����
	SCENE_TYPE_ACTIVITY = 13,		// �����
	SCENE_TYPE_CHIJI_PREPARE = 14,	// �Լ�׼�����ĳ���
	SCENE_TYPE_TEAM_COPY = 15,		// �ű�
	SCENE_TYPE_LOST_DUNGEON = 16,	// ��ʧ���γ���

	SCENE_TYPE_CHAMPION = 17,		//�ھ���
	SCENE_TYPE_DUNGEON_ENTRY_DYNAMIC = 18,  // ��̬ѡ����³ǵĳ���
	SCENE_TYPE_MAX,
};

/**
 *@brief �ж��ǲ��Ǿ�̬��ͼ
 */
inline bool IsStaticMap(UInt8 type)
{
	switch(type)
	{
	case SCENE_TYPE_NORMAL:
	case SCENE_TYPE_GHOST:
    case SCENE_TYPE_SINGLE:
    case SCENE_TYPE_DUNGEON_ENTRY:
	case SCENE_TYPE_PK_PREPARE:
	case SCENE_TYPE_PK:
	case SCENE_TYPE_ACTIVITY:
	case SCENE_TYPE_CHIJI_PREPARE:
	case SCENE_TYPE_TEAM_COPY:
	case SCENE_TYPE_CHAMPION:
		return true;
	}
	return false;
}

/**
 *@brief �ж��ǲ��Ƕ�̬��ͼ
 */
inline bool IsDynMap(UInt8 type)
{
	if(type == SCENE_TYPE_INVALID || type >= SCENE_TYPE_MAX) return false;
	return !IsStaticMap(type);
}

/**
 *@brief �ж��ǲ��Ǹ���
 */
inline bool IsCopyScene(UInt8 type)
{
	switch(type)
	{
	case SCENE_TYPE_PRIVATESCENE:
	case SCENE_TYPE_TEAMCOPY:
	case SCENE_TYPE_PRIVATE_TEAMCOPY:
	case SCENE_TYPE_PUBLICCOPY:
		return true;
	}
	return false;
}

/**
 *@brief �ж��ܷ�������������                                                                     
 */
inline bool IsTourCopyScene(UInt8 type)
{
	switch(type)
	{
	case SCENE_TYPE_PUBLICCOPY:
		return true;
	}
	return false;
}

/**
 *@brief ��̬��ͼ��ʼ
 */
#define MIN_DYNSCENE_ID 1000000

/**
 *@brief ��ȡ���񳡾�id
 */
inline UInt32 MakeGhostSceneId(UInt32 mapId, UInt32 serialId)
{
	return serialId * MIN_DYNSCENE_ID / 10 + mapId;
}

/**
 *@brief ��ȡ��ʵ����id
 */
inline UInt32 GetRealSceneId(UInt32 sceneId)
{
	if(sceneId >= MIN_DYNSCENE_ID) return sceneId;
	return sceneId % (MIN_DYNSCENE_ID / 10);
}

/**
 *@brief �ж��Ƿ�����˲�Ƶ���
 */
inline bool IsCanUseTeleportItem(UInt8 type)
{
	if(type == SCENE_TYPE_INVALID || type >= SCENE_TYPE_MAX) return false;
	switch(type)
	{
	case SCENE_TYPE_BATTLE:
		return false;
	}
	return true;
}

/**
 *@brief �ж��Ƿ�ȫ����
 */
inline bool IsFullExpItemReward(UInt8 type)
{
	switch(type)
	{
	case SCENE_TYPE_TEAMCOPY:
	case SCENE_TYPE_PRIVATE_TEAMCOPY:
		return true;
	}
	return false;
}

/**
 *@brief ���ݳ���id�Ƿ�̬��ͼ
 */
inline bool IsStaticMapId(UInt32 id)
{
	return id < MIN_DYNSCENE_ID;
}

/**
 *@brief ��ͼѡ��
 */
enum SceneOption
{
	SCENE_OPTION_RIDE =	1 << 0,				//�Ƿ�����
	SCENE_OPTION_FLY =	1 << 1,				//�Ƿ�ɷ���    ----����
	SCENE_OPTION_PK =	1 << 2,				//�Ƿ��pk
	SCENE_OPTION_DAILY = 1 << 3,			//�Ƿ���ʾ���ճ������
	SCENE_OPTION_PKFREE = 1 << 4,			//�Ƿ�����pk����
	SCENE_OPTION_FORBIDFLY = 1 << 5,		//��ֹʹ��С��Ь�������˲�Ƶ���
	SCENE_OPTION_NO_NPCBLOCK = 1 << 6,		//��npc�赲
	SCENE_OPTION_PICKUPBIND = 1 << 7,		//ʰȡ��
    SCENE_OPTION_NO_ENTER_SCENE = 1 << 8,   //����Ҫ�����볡����Э��
};

/**
 *@brief ��������Ѷ�
 */
#define COPYSCENE_DIFFICULTY_NORMAL 0
#define MAX_DIFFICULTY_LEVEL 2
/**
 *@brief �����ʱ��
 */
#define MAX_COOPYSCENE_TIME (3 * HOUR_TO_SEC)
/**
 *@brief ������������ʱ��
 */
#define COPYSCENE_REVIVE_TIME 20
/**
 *@brief �����������˳�ʱʱ��
 */
#define COPYSCENE_PULL_TIMEOUT 60

/**
 *@brief ����������Ϣ
 */
struct CopySceneTeamInfo
{
	CopySceneTeamInfo():id(0), playerNum(0), avgLevel(0), hasPwd(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & title & playerNum & avgLevel & hasPwd;
	}

	//����id
	UInt32	id;
	//�������
	std::string	title;
	//����
	UInt8	playerNum;
	//���ƽ���ȼ�
	UInt16	avgLevel;
	//�Ƿ�������
	UInt8	hasPwd;
};

/**
 *@brief ������������
 */
enum CopySceneEnterType
{
	COPYSCENE_ENTER_NORMAL = 0,		//�������

	COPYSCENE_ENTER_PULL = 3,		//���˽���
	COPYSCENE_ENTER_CHARGE = 4,		//��Ӧ�۷ѽ���
};


/**
 *@brief npcλ����Ϣ
 */
struct NpcPosInfo
{
	NpcPosInfo():mapId(0){}

	UInt32	mapId;
	CLPosition	pos;
};

/**
 *@brief ����̨
 */
#define SCENE_FENGSHENTAI 402

/**
 *@brief Զ��ʢ��
 */
#define SCENE_YUANGUSHENGYAN 403

/**
 *@brief ��.������
 */
#define SCENE_ZHENHUQIU   506

/**
 *@brief ��̨����ͼid
 */
#define SCENE_CHALLENGE_ID	703

/**
 *@brief �ɽٵ�ͼid
 */
#define SCENE_DUJIE_ID 900

// ս������
enum BATTLESCENE_TYPE
{
	BATTLESCENE_WUDOU_LOW	= 1,	// �ͼ��䶷��
	BATTLESCENE_WUDOU_MIDDLE = 2,	// �м��䶷��
	BATTLESCENE_WUDOU_HIGH  = 3,	// �߼��䶷��

	BATTLESCENE_TRIBEWAR	= 5,	// ����ս
	BATTLESCENE_WORLDCHALLENGE = 6,	// �����ս��
	BATTLESCENE_TRIBEMELEE	= 7,	// ˮ��ս
	BATTLESCENE_JIEJIE		= 8,	// ���
	BATTLESCENE_WORLD_WUDOU_LOW = 9,		//��������䶷��
	BATTLESCENE_WORLD_WUDOU_MIDDLE = 10,	//�м�����䶷��
	BATTLESCENE_WORLD_WUDOU_HIGH = 11,		//�߼�����䶷��
	BATTLESCENE_WORLD_WUDOU_FINAL = 12,		//�ռ�����䶷��
};

enum TOWER_TYPE
{
	TOWER_FENGMO = 1,			//��ħ��
	TOWER_TONGTIAN = 2,			//ͨ����
	TOWER_CHENGSHEN = 3			//����֮·
};



const static UInt32 DROP_NOTIFY_MAPS[] = { 112, 126, 127, 128, 402 };

inline bool IsDropNotifyMapId(UInt32 id)
{
	for(UInt32 i = 0; i < sizeof(DROP_NOTIFY_MAPS) / sizeof(DROP_NOTIFY_MAPS[0]); ++i)
	{
		if(DROP_NOTIFY_MAPS[i] == id) return true;
	}
	return false;
}

inline bool IsGrowthCopyScene(UInt32 id)
{
	if (id >= 1000 && id <= 1053) return true;
	return false;
}

inline bool IsWudouBattleScene(int type)
{
	if((type >= BATTLESCENE_WUDOU_LOW && type <= BATTLESCENE_WUDOU_LOW)
		||(type >= BATTLESCENE_WORLD_WUDOU_LOW && type <= BATTLESCENE_WORLD_WUDOU_LOW))
		return true;
	return false;
}

inline bool IsSceneNeedCheckLevel(int type)
{
	if (type >= SCENE_TYPE_SINGLE && type <= SCENE_TYPE_ACTIVITY)
	{
		return false;
	}

	return true;
}

#endif

