#ifndef _CL_RELATION_DEFINE_H_
#define _CL_RELATION_DEFINE_H_

#include "CLDefine.h"
#include "CLItemDefine.h"
#include "CLRetinueDefine.h"
#include "CLPkStatisticDefine.h"
#include "CLPetDefine.h"

/**
 *@brief ��ϵ����
 */
enum RelationType
{
	RELATION_FRIEND =		1,	//���ѹ�ϵ
	RELATION_BLACKLIST =	2,	//������
	RELATION_STRANGER = 3,		//İ����
	RELATION_MASTER = 4,		//ʦ��
	RELATION_DISCIPLE = 5,		//ͽ��

	RELATION_MAX,
};

/**
*@brief �Ƽ���ϵ����
*/
enum RelationFindType
{
	RFT_FRIEND = 1,
	RFT_TEAM = 2,
	RFT_MASTER = 3,
	RFT_DISCIPLE = 4,
	RFT_ROOM = 5,
};


/**
 *@brief ��ϵ����                                                                     
 */
enum RelationAttr
{
	RA_INVALID,
	RA_NAME,		//�Է�����(std::string)
	RA_SEASONLV,			//�Է���λ(UInt32)
	RA_LEVEL,		//�Է��ȼ�(UInt16)
	RA_OCCU,		//�Է�ְҵ(UInt8)
	RA_LASTGIVETIME,		//��һ������ʱ��(UInt32)
	RA_DAYGIFTNUM,	//ÿ�����ʹ���
	RA_TYPE,			//��ϵ
	RA_CREATETIME,	//��ϵ����ʱ�� (UInt32)
	RA_VIPLV,	//vip�ȼ�(UInt8)
	RA_STATUS,	//״̬(UInt8)
	RA_MASTER_DAYGIFTNUM,	//ʦͽÿ������ʣ�����
	RA_OFFLINE_TIME,		//�������ʱ��

	RA_INTIMACY,	//���ܶ�(UInt16)
	RA_DAILY_MASTERTASK_STATE, //ʦ������״̬
	RA_REMARKS,			//	��ע
	RA_IS_REGRESS,		//   �Ƿ��ǻع����
	RA_MARK,			//��־
	RA_HEAD_FRAME,		//ͷ���
	RA_GUILD_ID,		//����ID
	RA_RETURN_YEAR_TITLE,//�ع�����ƺ�
	//RA_BONLINENOTIFY,	//�Ƿ���������(UInt8)
	//RA_BCLOSEFRIEND,	//�Ƿ�����(UInt8)
};

/**
 *@brief ��ϵͬ������                                                                     
 */
enum RelationSyncType
{
	RST_FRIEND = 1 << RELATION_FRIEND,
	RST_BLACKLIST = 1 << RELATION_BLACKLIST,
	RST_STRANGER = 1 << RELATION_STRANGER,
	RST_MASTER = 1 << RELATION_MASTER,
	RST_DISCIPLE = 1 << RELATION_DISCIPLE,

};

//һ����������
#define QUICK_MAKE_FRIEND_NUM 6
//ף��ƿ�����ȼ�
#define MIN_BLESSING_FRIEND_LEVEL 20
//��ȡף��ƿ�������͵ȼ�
#define MIN_GET_BLESSINGEXP_LEVEL 38
//���ܶ�����
#define MAX_INTIMACY 60000


/**
 *@brief �Ƿ���ʾ�ȼ�
 */
inline bool IsRelationShowLevel(int type)
{
	switch(type)
	{
	case RELATION_FRIEND:
	case RELATION_MASTER:
	case RELATION_DISCIPLE:
		return true;
	}
	return false;
}

/**
*@brief �Ƿ���ʾս����
*/
inline bool IsRelationShowSeasonLv(int type)
{
	switch(type)
	{
	case RELATION_FRIEND:
		return true;
	}
	return false;
}

/**
 *@brief �Ƿ���ʾvip                                                                     
 */
inline bool IsRelationShowVip(int type)
{
	switch(type)
	{
	case RELATION_FRIEND:
	case RELATION_MASTER:
	case RELATION_DISCIPLE:
		return true;
	}
	return false;
}

/**
*@brief �Ƿ���ʾְҵ
*/
inline bool IsRelationShowOccu(int type)
{
	switch (type)
	{
	case RELATION_FRIEND:
	case RELATION_MASTER:
	case RELATION_DISCIPLE:
		return true;
	}
	return false;
}

/**
 *@brief �Ƿ��������������                                                                     
 */
inline bool IsSetOnlineNotify(int type)
{
	switch(type)
	{
	case RELATION_FRIEND:
	case RELATION_MASTER:
	case RELATION_DISCIPLE:
		return true;
	}
	return false;
}

/**
 *@brief �������ܶȻ�ȡ���ܶ�Ч��buff
 */
inline UInt16 GetIntimacyBuff(UInt16 intimacy)
{
	if(intimacy <= 30) return 0;
	else if(intimacy <= 100) return 2000;
	else if(intimacy <= 150) return 2001;
	else if(intimacy <= 200) return 2002;
	else if(intimacy <= 250) return 2003;
	return 2004;
}

/**
 *@brief һ�����ѻ�����Ϣ
 */
struct QuickFriendInfo
{
public:
	QuickFriendInfo() :playerId(0), occu(0), seasonLv(0), level(0), vipLv(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & playerId & name & occu & seasonLv & level & vipLv & masterNote & avatar 
			& activeTimeType & masterType & regionId & declaration & playerLabelInfo;
	}

	//���id
	ObjID_t	playerId;
	//����
	std::string	name;
	//ְҵ
	UInt8	occu;
	//�Ա�
	UInt32	seasonLv;
	//�ȼ�
	UInt16	level;
	//vip�ȼ�
	UInt8	vipLv;
	//ʦ������
	std::string masterNote;
	//�����Ϣ
	PlayerAvatar	avatar;
	//����ʱ������
	UInt8		activeTimeType; 
	//ʦ������
	UInt8		masterType;	
	//����id
	UInt8		regionId;		
	//����
	std::string	declaration;
	//��ұ�ǩ��Ϣ
	PlayerLabelInfo playerLabelInfo;
};

//���ѻ�������
#define FRIEND_PRESENT_DAYS 15

//�������������id
const static UInt32 FRIEND_PRESENT_ITEMS[] = { 0, 85, 86, 87, 88, 89, 90, 91, 92 };

//15�պ�����������
#define FRIEND_PRESENT_ITEM15  31

//ÿ�����������������
#define MAX_RECEIVE_FRIENDGIFT_TIMES 6

//ÿ���͸������ʻ��Ĵ���
#define MAX_GIVE_FLOWER_NUM 5

//���˽��������
#define VALENTINE_GIFT 422

//�����������ID
#define FRIEND_GIVE_GIFT		800000001
//�������ʹ�������ʱ�䣨h��
#define FRIEND_DAY_GIFT			6
//����ÿ�����ʹ���
#define FRIEND_DAY_GIVE_MAX		1


enum RelationPresentFlag
{
	RELATION_PRESENT_GIVE = 1 << 0,		//����
	RELATION_PRESENT_DRAW = 1 << 1,		//��ȡ
	RELATION_PRESENT_GIVE15 = 1 << 2,	//15������
	RELATION_PRESENT_DRAW15 = 1 << 3,	//15����ȡ
};

//õ�廨
enum FlowerType
{
	FLOWER_1 = 424,
	FLOWER_9 = 425,
	FLOWER_99 = 426,
	FLOWER_999 = 427,
};

inline UInt32 GetFlowerNum(UInt32 itemId)
{
	switch(itemId)
	{
	case FLOWER_1: return 1;
	case FLOWER_9: return 9;
	case FLOWER_99:return 99;
	case FLOWER_999: return 999;
	}
	return 0;
}

// ����ƺ�
struct GuildTitle
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & guildName & guildPost;
	}
	// ������
	std::string					guildName;
	// ����ְλ
	UInt8						guildPost;
};

// �鿴��ҵ���Ϣ
struct PlayerWatchInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & occu & level & equips & fashionEquips & retinue 
			& pkInfo & pkValue & matchScore & vipLevel & guildTitle & seasonLevel 
			& seasonStar & pets & avatar & activeTimeType & masterType & regionId 
			& declaration & playerLabelInfo & adventureTeamName & adventureTeamGrade
			& adventureTeamRanking & emblemLevel & totalEquipScore;
	}

	// ���id
	ObjID_t						id;
	// �����
	std::string					name;
	// ְҵ
	UInt8						occu;
	// �ȼ�
	UInt16						level;
	// װ��
	std::vector<ItemBaseInfo>	equips;
	// ʱװ
	std::vector<ItemBaseInfo>	fashionEquips;
	// ���
	RetinueInfo					retinue;
	// ս��
	PkStatisticInfo				pkInfo;
	// ��������
	UInt32						pkValue;
	// ��������
	UInt32						matchScore;
	// VIP�ȼ�
	UInt8						vipLevel;
	// ����ƺ�
	GuildTitle					guildTitle;
	// ������λ�ȼ�
	UInt32						seasonLevel;
	// �����Ǽ�
	UInt32						seasonStar;
	//����
	std::vector<PetBaseInfo>	pets;
	//�����Ϣ
	PlayerAvatar				avatar;
	//����ʱ������
	UInt8						activeTimeType;
	//ʦ������
	UInt8						masterType;
	//����id
	UInt8						regionId;
	//����
	std::string					declaration;
	//��ұ�ǩ��Ϣ
	PlayerLabelInfo				playerLabelInfo;
	//ð�ն���
	std::string					adventureTeamName;
	//ð�ն�����
	std::string					adventureTeamGrade;
	//ð�ն�����
	UInt32						adventureTeamRanking;
	//����ռ�
	UInt32						emblemLevel;
	// ȫ��װ������
	UInt32                      totalEquipScore;
};

// ʦ�������������(byte)
# define MASTER_NOTE_MAX_BYTE	64

enum AddonPayType
{
	APT_SELF = 0,
	APT_OTHER = 1,
};

enum AddonState
{
	AS_UNDO = 0,
	AS_AGREE = 1,
	AS_DISAGREE = 2,
};

// ������������
struct AddonPayOrderData
{
	// ����id
	UInt64	uid;
	// ����ϵ relationType
	UInt8	relationType;
	// ������id
	UInt64	reqId;
	// ����������
	std::string reqName;
	// ������ְҵ
	UInt8	reqOccu;
	// �����ߵȼ�
	UInt32	reqLevel;
	// Ŀ��id
	UInt64	tarId;
	// Ŀ������
	std::string tarName;
	// Ŀ��ְҵ
	UInt8	tarOccu;
	// Ŀ��ȼ�
	UInt16	tarLv;
	// ����ʱ��
	UInt32	dueTime;
	// ״̬ AddonState
	UInt8	state;
	// ��Ʒid
	UInt32	goodId;
	// ��������
	UInt32	buyNum;
};

// �ͻ���ͨ�ŵĴ�����Ϣ
struct AddonPayData
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & uid & type & relationType & tarName & tarOccu & tarLv & dueTime & state & payMoneyType & payMoneyNum & words;
	}

	// ����id
	UInt64	uid;
	// �������� AddonPayType
	UInt8	type;
	// ����ϵ relationType
	UInt8	relationType;
	// Ŀ������
	std::string tarName;
	// Ŀ��ְҵ
	UInt8	tarOccu;
	// Ŀ��ȼ�
	UInt16	tarLv;
	// ����ʱ��
	UInt32	dueTime;
	// ״̬ AddonState
	UInt8	state;
	// ��������
	UInt32	payMoneyType;
	// ��Ҫ��������
	UInt32	payMoneyNum;
	// ����
	std::string words;
};

#define PLAYER_REMARK_MAXLEN 16          //��ע�����
#define QUSETIONNAIRE_DECLAR_MAXLEN 40   //�ʾ����������

//����ʦͽ��ϵ����
enum MakeMasterDiscipleType
{
	MMDT_MASTER_REPLY = 1, //ʦ��ͬ���ʦ����
	MMDT_DISCIPLE_REPLY = 2, //ͽ��ͬ����ͽ����
};

//��ϵ��־
enum RelationMarkFlag
{
	RELATION_MARK_MUTUAL_FRIEND = 1 << 0,		//��Ϊ����
};

//��ѯ������
enum QueryPlayerType
{
	QPT_BATTLE_LOST = 2,// ս������
};

// �ͻ���ͨ�ŵ���ļ��Ϣ
struct HireInfoData
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & taskID & cnt & status;
	}

	//����id
	UInt32 taskID = 0;
	//�������
	UInt32 cnt = 0;
	// ����״̬
	UInt8 status = 0;
};

// �ͻ���ͨ�ŵ���ļ�����Ϣ
struct HirePlayerData
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & userId & name & occu & online & lv;
	}

	//��ɫid
	UInt64 userId = 0;
	//����
	std::string name;
	//ְҵ
	UInt8 occu;
	//����״̬
	UInt8 online;
	//�ȼ�
	UInt32 lv;
};

enum HireTaskType
{
	HTT_LEVEL = 0,				//�ﵽ��ͬ�ȼ�
	HTT_SHENG_YUAN_PIAO = 1,	//����N����ԨƱ
	HTT_YUAN_GU_PIAO = 2,		//����N��Զ��Ʊ
	HTT_GUILD_DUNGEON = 3,		//����N�ι�����³�
	HTT_GUILD_BATTLE = 4,		//����N�ι���ս
	HTT_CHAOS_DUNGEON = 5,		//ͨ��N�λ�����³�
	HTT_TEAM_DUNGEON = 6,		//ͨ��N���Ŷӵ��³�
	HTT_NONE_DUNGEON = 7,		//ͨ��N����յ��³�
	HTT_MAKE_TEAM = 8,			//ÿ���뱻��ļ������������ս������³�10�Σ���ԨԶ���½���ջ����ű�
	HTT_BIND_COMPLETE = 9,		//N������ļ��������������X
	HTT_BIND_OLD_MAN = 10,		//������Ұ�
	HTT_BIND_RETURN_MAN = 11,   //��N���ع����
};


#endif
