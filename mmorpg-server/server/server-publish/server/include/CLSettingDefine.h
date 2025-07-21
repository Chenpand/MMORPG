#ifndef _CL_SETTING_DEFINE_H_
#define _CL_SETTING_DEFINE_H_

#include <CLDefine.h>

/**
 *@brief �����������
 */
#define QUICKBAR_GRID_NUM 20
#define SKILLBAR_GRID_NUM 10
#define ITEMBAR_GRID_NUM 1
#define WEAPON_GRID_NUM 1	//������
#define SKILLBAR_NUM	4	//��������  adder by huchenhui 2016.07.22
#define SETTING_MIX		1	//��Сֵ


 /**
 *@brief ��Ч����λ
 */
#define QUICKBAR_GRID_INVALID 0XFF

/**
 *@brief ���������
 */
enum QuickBarType
{
	QUICKBAR_INVALID = 0,	//��Ч
	QUICKBAR_SKILL = 1,		//����
	QUICKBAR_ITEM = 2,		//����
	
	QUICKBAR_MAX,
};


/**
*@brief ��ƽ������ѯ��������
*/
enum EqualPvpSkillConfigReqType
{
	EQUAL_PVP_SKILL_CONFIG_QUERY = 0,	//��ѯ
	EQUAL_PVP_SKIL_LCONFIG_SET = 1,		//����
};

/**
 *@brief ���������
 */
struct QuickBarGrid
{
	QuickBarGrid():type(0), pos(0), id(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & type & pos & id;
	}
	//����
	UInt8 type;
	//λ��
	UInt8 pos;
	//id
	UInt32 id;
};

/**
*@brief ����������
*/
struct SkillBarGrid
{
	SkillBarGrid() :slot(0), id(0) {}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & slot & id;
	}

	//λ��
	UInt8	slot;
	//id
	UInt16	id;
};

typedef std::vector<SkillBarGrid> SkillGridVec;

/**
*@brief ������
*/
struct SkillBarInfo
{
	SkillBarInfo() :index(0) {}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & index & grids;
	}

	UInt8	index;
	SkillGridVec grids;

	UInt32 GetSkillSlotUseNum()
	{
		UInt32 k = 0;
		for (auto& it : grids)
		{
			if (it.id != 0)
				++k;
		}

		return k;
	}
};

/**
*@brief ����������
*/
struct SkillBarInfos
{
	SkillBarInfos(): index(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & index & skillBars;
	}

	UInt8 index;
	std::vector<SkillBarInfo> skillBars;
};

struct ItemBarGrid
{
	ItemBarGrid() :slot(0), id(0) {}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & slot & id;
	}

	//λ��
	UInt8	slot;
	//id
	UInt32	id;
};

/**
 *@brief ��Ϸѡ�λ
 */
enum GameOption
{
	GAME_OPTION_REFUSE_TRIBE_REQUEST = 1 << 0,	//�ܾ���������
	GAME_OPTION_REFUSE_TEAM_REQUEST =  1 << 1,	//�ܾ��������
	GAME_OPTION_REFUSE_TRADE_REQUEST = 1 << 2,	//�ܾ���������
	GAME_OPTION_REFUSE_FRIEND_REQUEST = 1 << 3,	//�ܾ���������
	GAME_OPTION_REFUSE_PRIVATECHAT =	1 << 4,	//�ܾ�˽��
	GAME_OPTION_ELITE_DUNGEON_NOTCOUME_ENERGY = 1 << 5, //��Ӣ���³ǲ����ľ���
};

enum GameSetType
{
	// ��������
	GST_FRIEND_INVATE = 1,
	// ��˽����
	GST_SECRET = 2,

	GST_MAX
};

enum SecretSetType
{
	// ��������
	SST_GUILD_INVATE = 1 << 0,
	// �������
	SST_TEAM_INVATE =  1 << 1,
	// ��������
	SST_DUEL_INVATE = 1 << 2,
};

//
enum SaveOptionMask
{
	SOM_NOT_COUSUME_EBERGY = 1 << 0,	// �Ƿ����ľ���(��Ӣ���³�)
};

//
#define MAX_GAMEFUNCTION_OPEN_NUM 120

#endif
