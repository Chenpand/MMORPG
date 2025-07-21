#ifndef _TEAM_DEFINE_H_
#define _TEAM_DEFINE_H_

#include "CLDefine.h"
#include "CLGameDefine.h"

//����������
#define MAX_TEAM_MEMBER_NUM 5

/**
 *@brief �������ģʽ
 */
enum TeamDistributeMode
{
	TEAM_DISTRIBUTE_FREE = 0,		//���ɷ���ģʽ
	TEAM_DISTRIBUTE_TAKETURNS = 1,	//��������ģʽ
	TEAM_DISTRIBUTE_RANDOM = 2,		//���
};

/**
*@brief ��Ա����
*/
enum TeamMemberProperty
{
	// �ȼ�
	TEAM_MEMBER_PROPERTY_LEVEL,
	// ����ID
	TEAM_MEMBER_PROPERTY_GUIDID,
	// ʣ�����
	TEAM_MEMBER_PROPERTY_REMAIN_TIMES,
	// ְҵ
	TEAM_MEMBER_PROPERTY_OCCU,
	// ״̬
	TEAM_MEMBER_PROPERTY_STATUS_MASK,
	// vip�ȼ�
	TEAM_MEMBER_PROPERTY_VIP_LEVEL,
	// ��ĥֵ
	TEAM_MEMBER_PROPERTY_RESIST_MAGIC,
};

// ��Ա״̬����
enum TeamMemberStatusMask
{
    // �Ƿ�����
    TEAM_MEMBER_MASK_ONLINE     = 1 << 0,
    // ׼��
    TEAM_MEMBER_MASK_READY      = 1 << 1,
	// ��ս
	TEAM_MEMBER_MASK_ASSIST		= 1 << 2,
	// �Ƿ���ս����
	TEAM_MEMBER_MASK_RACING		= 1 << 3,
};

/**
*@brief �����Ա��Ϣ
*/
struct TeammemberBaseInfo
{
	TeammemberBaseInfo() :id(0), level(0), occu(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & level & occu & playerLabelInfo;
	}

	//id
	ObjID_t	id;
	//����
	std::string name;
	//�ȼ�
	UInt16	level;
	//ְҵ
	UInt8	occu;
	//��ұ�ǩ��Ϣ
	PlayerLabelInfo playerLabelInfo;
};

typedef TeammemberBaseInfo TeamRequester;

/**
 *@brief �����Ա��Ϣ
 */
struct TeammemberInfo : TeammemberBaseInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & level & occu & statusMask & avatar & remainTimes & guildId & vipLevel & resistMagic & playerLabelInfo;
	}

	// ״̬���루��Ӧö��TeamMemberStatusMask��
    UInt8			statusMask;
	// �����Ϣ
	PlayerAvatar	avatar;
	// ʣ�����
	UInt32			remainTimes;
	// ����ID
	ObjID_t			guildId;
	// vip�ȼ�
	UInt8			vipLevel;
	// ��ħֵ
	UInt32			resistMagic;
	//��ұ�ǩ��Ϣ
	PlayerLabelInfo playerLabelInfo;
};

struct TeamBaseInfo
{
    AVALON_DEFINE_NETSERIALIZATION()
    {
		stream & teamId & target & masterInfo & memberNum & maxMemberNum;
    }

    // ������
    UInt32					teamId;
    // ����Ŀ��
	UInt32					target;
    // �ӳ�
    TeammemberBaseInfo		masterInfo;
    // ��Ա����
    UInt8					memberNum;
    // �������
    UInt8					maxMemberNum;
};

struct SceneTeamMember
{
    SceneTeamMember() : roleId(0), statusMask(0) {}

    AVALON_DEFINE_NETSERIALIZATION()
    {
        stream & roleId & statusMask;
    }

    bool isReady() const
    {
        return (statusMask & TEAM_MEMBER_MASK_READY) != 0;
    }

    ObjID_t roleId;
    UInt8   statusMask;
};

// ����ѡ���޸�����
enum TeamOptionOperType
{
    // Ŀ��
    TEAM_OPTION_TARGET,
	// �Զ�ͬ��
	TEAM_OPTION_AUTO_AGREE,
	// ��Ԩ�Զ�����
	TEAM_OPTION_HELL_AUTO_CLOSE,
};

// ���ѡ��
enum TeamOption
{
	// ������Ԩ�Զ���������
	TO_HELL_AUTO_CLOSE = 1 << 0,

	// ����ѡ��Ĭ��ֵ
	TO_INIT = 0
};

#endif
