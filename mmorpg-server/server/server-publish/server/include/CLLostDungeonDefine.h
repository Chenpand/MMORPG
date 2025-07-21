#ifndef _CL_LOST_DUNGEON_DEFINE_H_
#define _CL_LOST_DUNGEON_DEFINE_H_
#include <CLDefine.h>
#include <AvalonNetStream.h>
#include <CLLostDungeonDataEntry.h>

//��ʧ����״̬
enum  LostDungeonState
{
	LDS_CLOSE = 0,   //�ر���
	LDS_PROGRESSING = 1,  //������
	LDS_END_CAN_REWARD = 2, //��������ȡ����
};

//��ʧ�������״̬(battleServer)
enum LostDungeonPlayerState
{
	LDBPS_NONE,
	LDBPS_ENTERING, //����ս����
	LDBPS_NORAML,	//����״̬
	LDBPS_MATCHING,	//ƥ�������
	LDBPS_RACING,	//���ڱ���
	LDBPS_LEAVE,	//�뿪ս��
};

//��ʧ�������״̬(battleScene)
enum LostDungeonPlayerBattleSt
{
	LDPBT_NORMAL = 0, //��ս��
	LDPBT_BATTLE_PVE = 1, //PVEս����
	LDPBT_BATTLE_PVP = 2, //PVPս����
};

//��ʧ����ս���Ѷ�����
enum LostDungeonBattleHardType
{
	LDBHT_NORMAL = 1, //��ͨ��
	LDBHT_HARD	= 2,  //���ѵ�
};

//��ʧ����¥��ڵ�����
enum LostDungeonElementType
{
	LDET_NONE = 0,			//��Ч
	LDET_NORMAL_BATTLE = 1,	//��ͨս��
	LDET_HARD_BATTLE = 2,	//����ս��
	LDET_TREAS_CHEST = 3,	//����
	LDET_NPC = 4,			//npc
};

//��ʧ����ս����������
enum LostDungeonRaceType
{
	LDRT_NONE = 0,		//��Ч
	LDRT_PVP = 1,		//pvp
	LDRT_MONSTER = 2,	//��ɱС��
	LDRT_DUNGEON = 3,	//���³ǹؿ�
};

enum LostDungeonFloorState
{
	LDFS_NONE  = 0, //
	LDFS_LOCK = 1, //����
	LDFS_UNLOCK_UNPASS = 2, //����δͨ��
	LDFS_UNLOCK_PASS = 3, //��ͨ��
};

enum LostDungeonNodeState
{
	LDNS_NONE = 0, 
	LDNS_LOCK = 1, //����
	LDNS_CLOSE = 2, //�ر�
	LDNS_OPEN = 3, //��
	LDNS_HALF_OPEN = 4, //�뿪
};

enum LostDungeonBoxState
{
	LDBXS_NONE = 0,
	LDBXS_UNOPENED = 1, //δ����
	LDBXS_OPENED = 2,	//�򿪹�
};

enum LostDungeonCloseBattleReason
{
	LDCBR_NONE = 0,
	LDCBR_ACT_CLOSE = 1,  //�����
	LDCBR_BAT_DYNC_ADMIN = 2,  //ս����̬����
};

static UInt8 GetFloorElementId(UInt8 elemetType)
{
	return LostDungeonElementDataEntryMgr::Instance()->GetElementId(elemetType);
}

//¥��ڵ�
struct LostDungeonNode
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream  & id & treasChestId & state;
	}
	LostDungeonNode() : id(0), type(0), treasChestId(0), state(0)
	{}
	LostDungeonNode(UInt8 type_) : type(type_)
	{
		id = GetFloorElementId(type);
		treasChestId = 0;
		state = LDNS_LOCK;
	}
	UInt32  id;
	UInt8   type;
	//����id(����Ļ�)
	UInt32	treasChestId;
	//״̬��ս���Ż���״̬��
	UInt8   state;
};

//��ʧ����¥������
struct LostDungeonFloorData
{
	LostDungeonFloorData()
		:floor(0), state(0)
	{
	}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream  & floor & nodes & state;
	}

	std::string GetStrForDB()
	{
		std::stringstream ss;
		ss << floor << ";" << (UInt32)state;
		for (auto node : nodes)
		{
			ss << ";" << node.id << "," << (UInt32)node.type << "," << node.treasChestId << "," << (UInt32)node.state;
		}
		return ss.str();
	}

	bool LoadFromDB(const std::string& str)
	{
		if (str == "")
		{
			return false;
		}
		std::vector<std::string> ss;
		Avalon::StringUtil::Split(str, ss, ";");
		if (ss.size() >= 3)
		{
			floor = Avalon::StringUtil::ConvertToValue<UInt32>(ss[0]);
			state = Avalon::StringUtil::ConvertToValue<UInt8>(ss[1]);
			for (UInt32 i = 2; i < ss.size(); ++i)
			{
				std::string nodeStr = ss[i];
				std::vector<std::string> sss;
				Avalon::StringUtil::Split(nodeStr, sss, ",");
				if (sss.size() == 4)
				{
					LostDungeonNode node;
					node.id = Avalon::StringUtil::ConvertToValue<UInt32>(sss[0]);
					node.type = Avalon::StringUtil::ConvertToValue<UInt8>(sss[1]);
					node.treasChestId = Avalon::StringUtil::ConvertToValue<UInt32>(sss[2]);
					node.state = Avalon::StringUtil::ConvertToValue<UInt8>(sss[3]);
					nodes.push_back(node);
				}
				else
				{
					ErrorLogStream << "LostDungeonFloorData err  sss size = " << (UInt32)ss.size()
						<<", floor : " << floor << LogStream::eos;
					return false;
				}
			}
		}
		else
		{
			ErrorLogStream << "LostDungeonFloorData err  ss size = " << (UInt32)ss.size() 
				<< ", floor : " << floor << LogStream::eos;
			return false;
		}

		return true;
	}

	//¥��
	UInt32  floor;
	//���ɵĽڵ�
	std::vector<LostDungeonNode>	nodes;
	//¥��״̬
	UInt8	state;
};

//���³ǵ���ģʽ
enum LostDungChangeMode
{
	LDCM_SINGLE = 1, //����
	LDCM_TEAM = 2, //���
};

struct LostDungTeamMember
{
	LostDungTeamMember()
	{
		roleId = 0;
		teamId = 0;
		pos = 0;
		name = "";
		level = 0;
		occu = 0;
		timeStamp = 0;
	}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream  & roleId & teamId & pos & name & level & occu;
	}
	ObjID_t		roleId;
	UInt32		teamId;
	UInt8		pos;
	std::string name;
	UInt16		level;
	UInt8		occu;
	UInt32		timeStamp; //�������ʱ���
};

enum LostDungTeamBattleSt
{
	LDTBS_NORMAL = 0, //��ս��״̬
	LDTBS_MATCH = 1, //ƥ����,����һ���ɹ���
	LDTBS_BATTLE = 2, //ս��״̬
	LDTBS_MAX,
};

//���³ǹؿ�������Ϣ����
enum  SyncDungeonEnterTeamInfoType
{
	SDETIT_NONE		  = 0,
	SDETIT_ENTER_SYNC = 1, //����ʱ��ͬ��
	SDETIT_ADD		  = 2, //��Ӷ�Ա
	SDETIT_LEAVE      = 3, //��Ա�뿪
	SDETIT_UPDATE     = 4, //ˢ�¶�Ա��Ϣ
	SDETIT_CHG_MODE	  = 5, //ˢ����սģʽ
	SDETIT_TEAM_STATE = 6, //ˢ�¶���״̬
	SDETIT_MAX,
}; 

struct LostDungRaceMember
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream  & roleId & name & level;
	}

	ObjID_t		roleId;
	std::string name;
	UInt16		level;
};

struct LostDungTeamInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream  & teamId & index & battleState;
	}
	LostDungTeamInfo()
	{
		teamId = 0;
		index = 0;
		battleState = 0;
	};
	UInt32	teamId;
	UInt8	index;
	UInt8   battleState;
};

enum LostDungBattleState
{
	LDBS_NONE		= 0,
	LDBS_CREATING	= 1, //������
	LDBS_NORMAL     = 2, //����״̬
	LDBS_CLOSEING   = 3, //���ڹر���
	LDBS_WAIT_DESTORY = 4, // �ȴ�����
};

struct WaitEnterBatPlayer
{
	UInt64	roleId;
	UInt32	startTime;	//��ʼʱ��
};

//��ʧս��ƥ���ս����ʱʱ��
#define LOST_BATTLE_MATCH_OVERTIME  10

//��������
enum LostDungeonTreasCheatType
{
	LDTCT_NONE = 0,
	LDTCT_GIFT = 1, //���
	LDTCT_EVENT_CHANGEBATTLE = 2,
};

//ս�����
enum LostDungeonBattleReasult
{
	LDBR_NONE = 0,
	LDBR_PASS = 1, //ͨ��
	LDBR_FAIL = 2, //ʧ��
	LDBR_OVER = 3, //����
	LDBR_ABN_EXIT = 4, //�쳣�˳�
};

//¥��ʧ�ܴ�������
enum LostDungeonFailHandle
{
	LDFH_NONE = 0, //��Ӱ��
	LDFH_FALL = 1, //����ۻ���
	LDFH_SETTLE = 2, //ʧ�ܲ�����
};

struct LostDungeonPlayerPos
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream  & playerId & sceneId & posX & posY;
	}
	LostDungeonPlayerPos()
	{
		playerId = 0;
		sceneId = 0;
		posX = 0;
		posY = 0;
	}
	UInt64  playerId;
	UInt32	sceneId;
	UInt32	posX;
	UInt32	posY;
};

#endif