#ifndef _CL_VIP_DEFINE_H_
#define _CL_VIP_DEFINE_H_

#include <CLDefine.h>

/**
 *@brief vip�ȼ�
 */
enum VipLevel
{
	VIP_LEVEL1	=	1,	//�ܿ�
	VIP_LEVEL2	=	2,	//�¿�
	VIP_LEVEL3	=	3,	//���꿨
};

/**
 *@brief vip��Ȩ����
 */
enum VipPrivilegeType
{
	VIP_PRIVILEGE_INVALID = 0,

	VIP_PRIVILEGE_DUNGEON_EXP = 1,				//���³ǽ��㾭������
	VIP_PRIVILEGE_FREE_REVIVE = 2,				//ÿ����Ѹ���
	VIP_PRIVILEGE_DUNGEON_GOLD = 3,				//����������
	VIP_PRIVILEGE_PKCOIN_MAX = 4,				//����ÿ�վ���������
	VIP_PRIVILEGE_FREE_GOLDBOX = 5,				//�����ƽ���
	VIP_PRIVILEGE_SHOP_LIMIT = 6,				//�\���̵�VIP������Ʒ�ɹ���
	VIP_PRIVILEGE_DEATH_TOWER_RESET = 7,		//����֮��ÿ�����ô���
	VIP_PRIVILEGE_MAGIC_VEIN_NUM = 8,			//ħ����������
	VIP_PRIVILEGE_MYSTERIOUS_SHOP_REFRESH = 9,	//�����̵�ÿ��ˢ�´���
	VIP_PRIVILEGE_GUILD_DAILY_DONATE_NUM = 10,	//���������ÿ�մ���
	VIP_PRIVILEGE_OFFLINE_FATIGUE_LIMIT = 11,	//������������
	VIP_PRIVILEGE_GUILD_LUXURY_ORZ = 12,		//����Ĥ��
	VIP_PRIVILEGE_PERFECT_FIND = 13,			//�����һ�
	VIP_PRIVILEGE_SIGN_IN_DOUBLE  = 14,			//ǩ��˫��
	VIP_PRIVILEGE_FATIGUE_USE_NUM = 15,			//Сƣ��ҩÿ��ʹ�ô���
	VIP_PRIVILEGE_GUILD_RED_PACKET = 16,		//�����ԷѺ��
	VIP_TEAM_BOSS_NUM = 17,						//VIP���boss����
	VIP_AUTO_EATING = 18,						//VIP�Զ���ҩ
	VIP_WORLD_CHAT_FREE = 19,					//VIP����������Ѵ���
	VIP_AUCTION_COUNTER_FEE = 20,				//�������ۿ�
	VIP_FASHION_PACKAGE_SIZE_ADDITION = 21,		//ʱװ���������ӳ�
	VIP_TITLE_PACKAGE_SIZE_ADDITION = 22,		//�ƺŰ��������ӳ�
	VIP_PRIVILEGE_CREDIT_POINT_MONTH_GET_LIMIT = 24,	//���õ�ȯ�»������
	VIP_PRIVILEGE_CREDIT_POINT_HOLD_LIMIT = 25,		//���õ�ȯӵ������
	VIP_PRIVILEGE_CREDIT_POINT_TRANSFER_LIMIT = 26,		//���õ�ȯת������

	VIP_PRIVILEGE_MAX,

	VIP_PRIVILEGE_DECOMPOSE = 100,				//һ���ֽ�
	VIP_PRIVILEGE_MINOTAUR_PARADISE_EXP,		//ţͷ����԰��������
	VIP_PRIVILEGE_RETINUE_SKILL,				//ÿ����Ӽ���ϴ������
	VIP_PRIVILEGE_STORAGE_MAX,					//�ֿ�����
	VIP_PRIVILEGE_SOUTH_VALLEY_THREE,			//�ϲ�Ϫ�� 3������
	VIP_PRIVILEGE_HELL_TICKET,					//��Ԩ��Ʊ
	VIP_PRIVILEGE_SOUTH_VALLEY_NUM,				//�ϲ�Ϫ��ÿ�մ���
	VIP_PRIVILEGE_DAILY_TASK_MANAGER,			//�ճ�����ܼ�
	VIP_PRIVILEGE_RETINUE_SKILL_POINT,			//���ϴ������ʹ�õ��
	VIP_PRIVILEGE_STRENGTHEN_GOLD,				//ǿ��װ�����Ľ�Ҽ���
	VIP_PRIVILEGE_GUILD_GOLDDONATE_TIMES,		//�����Ҿ���ÿ�մ���
	VIP_PRIVILEGE_GUILD_POINTDONATE_TIMES,		//����ㄻ����ÿ�մ���

};

inline bool IsIncrementPrivilege(VipPrivilegeType type)
{
	return type == VIP_PRIVILEGE_STORAGE_MAX;
}

#endif