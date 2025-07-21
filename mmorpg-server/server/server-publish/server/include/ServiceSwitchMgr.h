#ifndef __SERVICE_SWITCH_MGR_H__
#define __SERVICE_SWITCH_MGR_H__

#include <CLDefine.h>

// ��������
enum ServiceType
{
	// ��Чֵ
	SERVICE_INVALID = 0,

	// ����ƥ��
	SERVICE_1V1_SEASON = 1,
	// ����ս����
	SERVICE_GUILD_BATTLE_ENROLL = 2,
	// ����ս
	SERVICE_GUILD_BATTLE = 3,
	// �Զ�ս��
	SERVICE_AUTO_BATTLE = 4,
	// �����ȼ�����
	SERVICE_SCENE_LEVEL_LIMIT = 5,
	// �µĿ����㷨
	NEW_DAY_CACL = 6,
	// ����ʱ�����
	SERVICE_DAY_ONLINE_TIME = 7,
	// ������Ϣ���ݿ�������
	SERVICE_GUILD_CLEAR_DIRTY = 8,

	//-----------װ��ϵͳ---------------
	// װ��Ʒ������
	EQUIP_SET_QUALITY_LV = 10,
	// ʱװѡ����
	FASHION_SEL_ATTR = 11,
	// ��޻���
	GOLD_JAR_POINT = 12,
	// ħ�޻���
	MAGIC_JAR_POINT = 13,
	// ʱװ�ϳ�
	FASHION_COMPOSE = 14,

	/* ����ϵͳ */
	// Բ������
	SERVICE_GUILD_TABLE = 20,
	// ���Ἴ��
	SERVICE_GUILD_SKILL = 21,
	// �������
	SERVICE_GUILD_DONATE = 22,
	// ����Ĥ��
	SERVICE_GUILD_ORZ = 23,
	// ������
	SERVICE_GUILD_RED_PACKET = 24,
	// �������ս
	SERVICE_GUILD_CROSS_BATTLE = 25,

	/* ���ϵͳ */
	SERVICE_RETINUE = 40,

	/* ��Ԫʯϵͳ */
	SERVICE_WARP_STONE = 60,

	/* ���� */
	SERVICE_ROOM = 70,
	SERVICE_SCORE_WAR = 71,

	/* ��Ƽƻ� */
	SERVICE_MONEY_MANAGE = 80,

	/* �����и��� */
	SERVICE_AUCTION_COPY = 90,

	// ÿ�ճ�ֵ����
	SERVEICE_DAY_CHARGE_WELFARE = 91,

	// �µĶ���״̬���㷽ʽ
	SERVICE_NEW_TEAM_STATUS = 92,

	// �µļ����˺���Ϣ��ʽ
	SERVICE_NEW_LOAD_ACCOUNT_INFO = 93,

	// ���״̬��ʱ�ж�
	SERVICE_PLAYER_STATUS_TIMEOUT = 94,

	// ������Ҽ��ع���
	SERVICE_OFFLINE_PLAYER_MGR = 95,

	/* ������� */
	SERVICE_DIG = 100,

	/* ʦͽϵͳ */
	SERVICE_MASTER = 110,

	/* ��ȫ�� */
	SERVICE_SECURITY_LOCK = 111,

	/* 3v3�Ҷ� */
	SERVICE_3v3_TUMBLE = 112,

	/* ���ḱ�� */
	SERVICE_GUILD_DUNGEON = 145,

	// ��������Ʒ���ƿ���
	SERVICE_AUCTION_TREAS = 210,

	// �����з�ҳ����
	SERVICE_AUCTION_PAGE = 211,

	// �����н�����ȴʱ�俪��
	SERVICE_AUCTION_COOLTIME = 212,

	// ��ɫ��λ
	SERVICE_ROLE_FILED = 213,

	// ��������
	SERVICE_BLACK_MARKET = 214,

	/* �µĳ���ͬ����ʽ */
	SERVICE_NEW_SCENE_SYNC = 220,

	// ð�ն�(Ӷ����)
	SERVICE_ADVENTURE_TEAM = 215,

	// �»ع�
	SERVICE_NEW_RETURN = 216,

	// ð�ն�(Ӷ����)���а�
	SERVICE_ADVENTURE_TEAM_SORTLIST = 217,

	// ������ұ���ʱ��
	SERVICE_SET_PLAYER_SAVE_BACK_TIME = 218,

	// �����ж�ͬһ��
	SERVICE_CHECK_SAME_DAY_ON_DAY_CHANGE = 219,

	// �ճ����߼�����
	SERVICE_CHECK_DAY_CHANGE_FOR_CHARGE_DAY_ACT = 220,

	// �ܳ���Ԩʧ����Ʊ����
	SERVICE_WEEK_HELL_FAIL_RETURN_TICKETS = 221,

	// ��������
	SERVICE_GUILD_AUCTION = 222,

	// ��Ӫ�����ͬ��
	SERVICE_OPACT_ONLINE_SYNC = 223,

	//���ḱ��������ͼ
	SERVICE_GUILD_DUNGEON_SCREEN_SHOT = 224,

	//���ֽ���ID�ж�
	SERVICE_RACE_ID_CHECK = 225,

	//ÿ�ձ���
	SERVICE_DAILY_TODO = 226,

	//��ɾ���˺����ʱ������ID
	SERVICE_CHECK_ROLEID_WHEN_REMOVE_ACCOUNT_PLAYER = 227,

	//װ��ǿ����������
	SERVICE_EQUIP_STRENGTHEN_PROTECT = 228,

	//װ��ǿ��װ���ֽ�ʧ���Ƴ�װ������
	SERVICE_EQUIP_STRENG_DESC_FAIL_REMOVE = 229,

	//����װ������
	SERVICE_ASSIST_EQUIP = 230,

	//����沢����
	SERVICE_GUILD_MERGER = 231,

	//�ռ���������
	SERVICE_ZJSL_TOWER = 232,

	//�ű�
	SERVICE_TEAM_COPY = 233,

	//����ҩ���ܻͬ������
	SERVICE_FLY_UP    = 234,

	// ��־���ͨ����Ϸ������
	SERVICE_NEW_CLIENT_LOG = 235,

	// �µĽ������
	SERVICE_NEW_RACE_END_EXP = 236,

	//����ҳ����
	SERVICE_SKILL_PAGE = 237,

	//װ������
	SERVICE_EQUIP_SCHEME = 238,

	//��Ҽ���
	SERVICE_GOLD_CONSIGNMENT = 239,

	//�ű�2
	SERVICE_TEAM_COPY_TWO = 240,

	SERVICE_NUM,
};

struct ServiceConfig
{
	ServiceConfig() : type(SERVICE_INVALID), open(true) {}

	ServiceType type;
	// �Ƿ񿪷�
	bool open;
};

// ���Ʒ����Ƿ񿪷�
class ServiceSwitchMgr
{
public:
	ServiceSwitchMgr();
	virtual ~ServiceSwitchMgr();

public:
	void OpenService(ServiceType type);
	void CloseService(ServiceType type);
	bool IsOpen(ServiceType type);

public:
	virtual void OnOpenService(ServiceType type) {}
	virtual void OnCloseService(ServiceType type) {}

protected:
	ServiceConfig			m_serviceConfigs[SERVICE_NUM];
};

#endif