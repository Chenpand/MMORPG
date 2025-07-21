#ifndef _CL_SYSTEM_VALUE_DATAENTRY_H_
#define _CL_SYSTEM_VALUE_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum SystemValueType
{
	SVT_INVALID = 0,
	SVT_FATIGUE_MAX = 1,						//ÿ��ƣ��ֵ
	SVT_PKCOIN_MAX  = 2,						//ÿ�վ���������
	SVT_RETINUE_SKILL = 3,						//���ϴ������
	SVT_SHOP_REFRESH = 4,						//����Ĭ��ˢ�´���
	SVT_SOUTH_VALLEY = 5,						//�ϲ�Ϫ��Ĭ�ϴ���
	SVT_RETINUE_SOULS = 6,						//1�����Ի�ȡ�Ļ�
	SVT_VIPLEVEL_MAX = 7,						//VIP��ߵȼ�
	SVT_DAILY_TASK_POINT = 8,					//ÿ���ճ��ĵ��
	SVT_WARPSTONE_MAXLEVEL = 9,					//��Ԫʯ��ߵȼ�
	SVT_PKCOIN_BASENUM = 10,					//�����һ���ֵ
	SVT_MAIL_ITEM_MAXNUM = 11,					//�ʼ�������Ʒ��������
	SVT_ONEKEY_DECOMPOSE_LV = 12,				//һ���ֽ���Ҫ�ĵȼ�
	SVT_GOLDAUCTION_DAILY_MAX_ONSALENUM = 13,	//���������ÿ�������������
	SVT_SKILL_DOWNLEVEL_COSTGOLD = 14,			//���ܽ������Ľ�һ�����ֵ
	SVT_CYCLE_TASK_MAX = 15,					//ÿ��ѭ���������
	SVT_CYCLE_TASK_DAILY_MAX = 16,				//ѭ������ÿ�����ѭ����
	SVT_CYCLE_TASK_REFRESH_FREE_COUNT = 17,		//ѭ���������ˢ�´���
	SVT_CYCLE_TASK_REFRESH_CONSUME = 18,		//ѭ������ˢ������
	SVT_DEATH_TOWER_WIPEOUT_SEC = 19,			//����֮��ÿ��ɨ����ʱ
	SVT_DEATH_TOWER_FINISH_WIPEOUT_COST = 20,	//����֮��ÿ��ɨ���������ĵㄻ
	SVT_STRENGTH_WP_COST_MOD = 21,				//����ǿ������ϵ��
	SVT_STRENGTH_DF_COST_MOD = 22,				//����ǿ������ϵ��
	SVT_STRENGTH_JW_COST_MOD = 23,				//����ǿ������ϵ��
	SVT_ITEMAUCTION_MAX_ONSALENUM = 24,			//��Ʒ������ͬʱ�ϼܸ�������
	SVT_STRCOLOR_ID = 25,						//��ɫ����ID
	SVT_STRCOLORLESS_ID = 26,					//��ɫ����ID
	SVT_STRPROTECT_ID = 27,						//ǿ������ȯID
	SVT_STRTOTEN_ID = 28,						//ǿ10ȯID
	SVT_BINDGOLD_ID = 29,						//���ID
	SVT_VIP_RESET_TOWER_COST = 30,				//vip��������֮�����ô������ĵㄻ
	SVT_SP_ITEM_ID = 31,						//���ܵ���ƷID
	SVT_CHANGE_OCCU_SP_NUM = 32,				//תְ��ü��ܵ�
	SVT_AWAKEN_SP_NUM = 33,						//���ѻ�ü��ܵ�
	SVT_PACK_40 = 40,							//��50�񱳰�����
	SVT_PACK_40_COSTITEMID = 41,
	SVT_PACK_50 = 50,							//��50�񱳰�����
	SVT_PACK_50_COSTITEMID = 51,
	SVT_PACK_60 = 60,							//��60�񱳰�����
	SVT_PACK_60_COSTITEMID = 61,
	SVT_PACK_70 = 70,							//��70�񱳰�����
	SVT_PACK_70_COSTITEMID = 71,
	SVT_PACK_80 = 80,							//��80�񱳰�����
	SVT_PACK_80_COSTITEMID = 81,
	SVT_PACK_90 = 90,							//��90�񱳰�����
	SVT_PACK_90_COSTITEMID = 91,
	SVT_PACK_100 = 100,							//��100�񱳰�����
	SVT_PACK_100_COSTITEMID = 101,
	SVT_GUILD_DONATE_GOLD_COST = 110,			//��Ҿ�������
	SVT_GUILD_DONATE_GOLD_CONTRI = 111,			//��Ҿ�����ù���
	SVT_GUILD_DONATE_POINT_COST = 112,			//�ㄻ��������
	SVT_GUILD_DONATE_POINT_CONTRI = 113,		//�ㄻ������ù���
	SVT_GUILD_EXCHANGE_COST = 114,				//������һ����Ĺ���
	SVT_GUILD_CHANGE_NAME_COST = 115,			//�޸Ĺ��������ĵㄻ
	SVT_GUILD_MAIL_MAX_TIMES = 116,				//ÿ�շ��͹����ʼ�����
	SVT_GUILD_CREATE_COST = 117,				//�����������ĵㄻ
	SVT_GUILD_NOTICE_MAX_WORDS = 118,			//�����������
	SVT_GUILD_DECLARATION_MAX_WORDS = 119,		//�����������
	SVT_GUILD_NAME_MAX_WORDS = 120,				//�������������
	SVT_GUILD_MAIL_MAX_WORLDS = 121,			//�����ʼ��������
	SVT_GUILD_EXCHANGE_MAX_TIME = 122,			//���ḣ����ÿ�����һ�����
	SVT_GUILD_WORSHIP_TIME = 123,				//����Ĥ��ÿ�մ���
	SVT_GUILD_PAY_RED_PACKET = 124,				//�����ԷѺ��ÿ�մ���
	SVT_FRIEND_GIVE_TAKE_NUM = 125,				//��������ÿ����ȡ����
	SVT_GUILD_MEMBER_BATTLE_COUNT = 126,		//����ս��Աս������
	SVT_GUILD_INSPIRE_SKILL_ID = 127,			//����ս���輼��ID
	SVT_GUILD_INSPIRE_ENTER_TIME = 128,			//����ս�³�Ա����ʱ��
	SVT_GUILD_EVENT_SHOW_MAX = 129,				//������־�����ʾ��Ŀ
	SVT_WUDAO_MAX_WIN_NUM = 130,				//������һ�����ʤ������
	SVT_WUDAO_MAX_LOSE_NUM = 131,				//������һ�����ʧ�ܳ���
	SVT_WUDAO_TICKET_ID = 132,					//��������ƱID
	SVT_WUDAO_ACTIVITY_ID = 133,				//������ID
	SVT_FASHION_COMPOSE_MATID = 134,
	SVT_GUILD_DONATE_GOLD_NUM = 135,			//�����Ҿ���ÿ�մ���
	SVT_GUILD_CONTRI_FROM_FATIGUE = 136,		//����һ������ֵ��õĹ��ṱ��
	SVT_GUILD_AUTO_CHAGE_LEADER_DAY = 137,		//�᳤�������첻��¼�����������Զ�ת��
	SVT_MONTH_CARD_VALID_DAY = 140,				//�¿���Чʱ��
	SVT_MONTH_CARD_ADD_FATIGUE = 141,			//�¿���ӵ�����
	SVT_MONTH_CARD_ADD_POINT = 142,				//�¿���ӵĵㄻ
	SVT_MAKEUP_NEED_LV = 143,					//ƣ��&�����һؽ�������ȼ�
	SVT_MONEY_MANAGE_CARD_ADD_POINT = 144,		//��ƿ���ӵĵㄻ
	SVT_TEAM_INVITE_INTERVAL_SEC = 150,			//���������ʱ��
	SVT_SEASON_WIN_STREAK_COUNT = 160,			//������ʤ��������
	SVT_SEASON_WIN_STREAK_EXP = 161,			//������ʤ��������
	SVT_AUCTION_MAX_PRICE_PERCENT = 170,        //�����м۸����ޱ���
	SVT_AUCTION_MIN_PRICE_PERCENT = 171,		//�����м۸����ޱ���	
	SVT_WORLD_CHAT_LV_LIMIT = 172,				//��������ȼ�����
	SVT_AROUND_CHAT_LV_LIMIT = 173,				//��������ȼ�����
	SVT_TEAM_CHAT_LV_LIMIT = 174,				//�������ȼ�����
	SVT_EQUIPQL_MATID = 175,					//Ʒ��������Ҫ�Ĳ���ID
	SVT_EQUIPQL_MATNUM = 176,					//Ʒ��������Ҫ�Ĳ�������
	SVT_EQUIPQL_MODE = 177,						//Ʒ������ģʽ1.���õ����ٷֱ�,2.������������
	SVT_WORLDCHAT_ACT_NEED = 178,				//����Ƶ��������Ҫ��Ծ��ֵ
	SVT_JAR_RECORD_QUALITY = 184,				//��������Ҫ��¼����ƷƷ��
	SVT_JAR_RECORD_MAXNUM = 185,				//�����Ӽ�¼������
	SVT_JAR_FIRST_OPEN_GET_ITEM = 186,			//�״ο�ħ�޻�õĵ���
	SVT_JAR_FIRST_OPEN_GET_ITEM_NUM = 187,			//�״ο�ħ�޻�õĵ�������

	SVT_AUCTION_AUTO_REFRESH_SEC = 190,			//�������Զ�ˢ�¼������
	SVT_AUCTION_ON_SALE_WAIT_SEC = 191,			//�������ϼܵȴ�ʱ��
	SVT_AUCTION_DEPOSIT_ITEM_ID = 192,			//������Ѻ�����
	SVT_AUCTION_DEPOSIT_COST = 193,				//������Ѻ��

	SVT_FASHION_ATTRIBUTE_RESET_COST_ITEM_ID = 195, //ʱװ��������ʯID
	SVT_GOLDJARPOINT_RESET_DAY = 196,				//����ӻ���������
	SVT_GOLDJARPOINT_RESET_INTERVAL_TYPE = 197,		//����ӻ������ü������(1.�£�2.��)
	SVT_MAGJARPOINT_RESET_DAY = 198,				//ħ���ӻ���������
	SVT_MAGJARPOINT_RESET_INTERVAL_TYPE = 199,		//ħ���ӻ������ü������(1.�£�2.��)

	SVT_WUDAO_ACTIVITY_ID_BEGIN = 200,			//��������ʼID
	SVT_WUDAO_ACTIVITY_ID_END = 201,			//����������ID

	SVT_HORN_MIN_SEC = 205,						// ������̳���ʱ�䣨����ʱ�䣩
	SVT_HORN_MAX_SEC = 206,						// ���������ʱ��
	SVT_HORN_COMBO_WAIT_SEC = 207,				// ���������ȴ�ʱ�䣨�룩
	SVT_HORN_CONTENT_MAX_WIDTH = 208,			// �������������
	SVT_HORN_ID = 209,							// ���ȵ���ID
	SVT_HORN_10_MIN_SEC = 210,					// ʮ����������̳���ʱ�䣨����ʱ�䣩
	SVT_HORN_10_MAX_SEC = 211,					// ʮ�������������ʱ��
	SVT_ROLE_DELETE_LIMIT_TIME = 212,			// ��ɫɾ������ʱ��(��)
	SVT_ROLE_RECOVER_LIMIT_TIME = 213,			// ��ɫ�ָ�����ʱ��(��)
	SVT_FATIGUE_MAKEUP_STEP = 214,				// ƣ���һ�ÿ���һص�ƣ�Ͳ���

	SVT_PET_MAX_NUM = 220,						// ��������
	SVT_PET_HUNGER_MAX_NUM = 221,				// ���Ｂ��������
	SVT_PET_POINT_FEED_CRITICAL_CHANCE = 222,	// ������ιʳ��������
	SVT_PET_POINT_FEED_CRITICAL_NUM = 223,		// ������ιʳ��������
	SVT_PET_DEVOUR_GOLD_RATIO = 224,			// ��ʳ����������ϵ��
	SVT_PET_DEVOUR_EXP_RATIO = 225,				// ��ʳ���ﾭ����ϵ��
	SVT_PET_REST_INTERVAL_SEC = 226,			// ���ӱ�ʳ��ʱ����
	SVT_PET_POINT_FEED_MAX = 227,				// ������ι��������
	SVT_PET_GOLD_FEED_MAX = 228,				// ������ι��������
	SVT_PET_BATTLE_INTERVAL_SEC = 229,			// ���ı�ʳ��ʱ����


	SVT_MONTH_CARD_RACE_GOLD_RATE = 230,		// �¿���һ�ñ���
	SVT_TIME_ITEM_DELETE_INTERVAL = 231,		// ʱ�޵���ɾ��ʱ��
	SVT_PL_LMI_ID = 232,							//ƣ���һصͼ�ID
	SVT_PL_HMI_ID = 233,							//ƣ���һظ߼�ID
	SVT_SIGN_MONEY_ID = 234,						//ǩ������ID
	SVT_CHANGE_PLAYERNAME_CARD = 235,					// ��ɫ������
	SVT_CHANGE_GUILDNAME_CARD = 236,					// ���������
	SVT_PET_HUNGRY_LIMIT = 240,							//���Ｂ���ٽ�ֵ
	SVT_AUTO_SETPOTION = 241,					// �Զ�����ҩƷ
	SVT_RP_SIGHIN_COST_ITEMID = 242,			// ��ǩ���ĵõ���id
	SVT_RP_SIGHIN_COST_NUM	= 243,				// ��ǩ���ĵ�����
	SVT_SIGHIN_FREE_NUM = 244,					// ���ǩ������
	SVT_MASTER_EXP_ADDITION = 245,				// ʦͽ����ӳ�

	SVT_GUILD_BATTLE_CHALLENGE_ITEM_ID = 260,	//������ս��ƷID
	SVT_GUILD_BATTLE_CHALLENGE_ITEM_NUM = 261,	// ������ս��Ʒ����
	SVT_GUILD_BATTLE_CHALLENGE_TERR_ID = 262,	// ������ս���ID
	SVT_GUILD_BATTLE_STATUE_SKILL_ID = 263,		// ����ս������ID
	SVT_GUILD_BATTLE_ASSISTANT_STATUE_SKILL_ID = 264,	// ����ս������ID

	SVT_CHEAT_FIRST_TIME_PUNISH_SEC = 270,		// ��һ�����׳ͷ�ʱ��
	SVT_CHEAT_FOREVER_PUNISH_SEC = 271,			// �������÷�ͣʱ��

	SVT_GUILD_STORAGE_INIT_SIZE = 272,			// ����ֿ��ʼ��С
	SVT_GUILD_STORAGE_OP_RECORD_MAXNUM = 273,	// ����ֿ������¼�������
	SVT_NOTIFY_ITEM_ALREAY_DUE = 274,		// ֪ͨʱ�޵��߼�������ʱ��(s)
	SVT_GUILD_POINT_FUND_NUM = 275,			// ÿ��ȯ��ȡ�Ĺ����ʽ�

	SVT_TAP_SYSTEM_OPEN_LEVEL = 276,		//ʦͽϵͳ���ŵȼ�
	SVT_TAP_SYSTEM_TEACHER_MIN_LEVEL = 277,		//�����ͽ����͵ȼ�
	SVT_TAP_SYSTEM_PUPIL_MAX_COUNT = 278,		//������ͽ�ܸ���
	SVT_TAP_SYSTEM_ANNOUNCE_MAX_LENGTH = 279,	//��ͽ�����������
	SVT_TAP_SYSTEM_MASTER_MAX_COUNT = 280,		//������ʦ������
	SVT_TAP_SYSTEM_FRIEND_MAX_COUNT = 281,		//��������Ѹ���
	SVT_TAP_SYSTEM_ADDONPAY_OVERDUE_TIME = 282,		//������������ʱ�� (s)
	SVT_TAP_SYSTEM_DISCIPLE_USED_FATIGUE = 283,		//ͽ����ʹ�õ�ƣ��
	SVT_TAP_SYSTEM_MASTER_GET_FATIGUE = 284,		//ʦ��һ���ܻ�õ�ƣ��
	SVT_TAP_SYSTEM_MASTER_GET_FATIGUE_MAX = 285,		//ʦ��ÿ������ܻ�õ�ƣ��
	SVT_TAP_SYSTEM_MASTER_GIVE_MAX = 286,		//ʦ������ÿ���������

	SVT_PREMIUM_LEAGUE_LEVEL = 290,				// �ͽ��������ŵȼ�
	SVT_PREMIUM_LEAGUE_ENROLL_COST_ITEM = 291,	// �ͽ������������ĵĵ���ID
	SVT_PREMIUM_LEAGUE_ENROLL_COST = 292,		// �ͽ������������ĵĵ�������
	SVT_PREMIUM_LEAGUE_REENROLL_COST = 293,		// �ͽ��������±������ĵㄻ
	SVT_PREMIUM_LEAGUE_ENROLL_MAX_COUNT = 294,	// �ͽ�������౨������
	SVT_PREMIUM_LEAGUE_REWARD_ITEM = 295,		// �ͽ�������������
	SVT_PREMIUM_LEAGUE_CHAMP_DEF_VALUE = 296,	// �ͽ�������ھ�Ĭ�Ͻ���
	SVT_MGCBOX_FIRST_OPEN_GET_ITEM = 297,			//�״ο�ħ�л�õĵ���

	SVT_SIGN_IN_SEVEN_TASK_RE_SUBMIT_COST_ITEM = 298, //����ǩ����ǩ���ĵ���
	SVT_SIGN_IN_SEVEN_TASK_RE_SUBMIT_COST_NUM = 299, //����ǩ����ǩ��������

	SVT_OPEN_MAGIC_BOX_NEED_LV = 300, //��ħ����Ҫ�ĵȼ�

	SVT_ROOM_SLOT_MAX_SIZE = 310,				//��������������
	SVT_ROOM_ID_MIN = 311,						//��С�����
	SVT_ROOM_ID_MAX = 312,						//��󷿼��
	SVT_ROOM_REJECT_ENTER_TIME = 313,			//�Զ��ܾ����뷿��ʱ��
	SVT_ROOM_REJECT_CHANGEPOS_TIME = 314,		//�Զ��ܾ�����λ��ʱ��
	SVT_ROOM_CHANGEPOS_PROTECT_TIME = 315,		//���𽻻�λ�ü��ʱ��
	SVT_ROOM_REJECT_BATTLE_READY_TIME = 316,	//�Զ��ܾ�����ս��׼��ʱ��
	SVT_MALL_VALID_DISCOUNT_NUM = 317,			//�̳�ʱװ��Ч��������
	SVT_ROOM_KICKOUT_JOIN_TIME = 318,			//�Զ��ܾ�����ս��׼��ʱ��
	SVT_REPLAY_MASTER_SCORE_LOWER_LIMIT = 319,	//���ֶԾ�¼���������
	SVT_3V3_ROOM_LIMIT_NUM = 320,				//3v3Ĭ��һҳ��ʾ���ٸ�����
	SVT_DUELREDSHOW = 321,						//������װ���Ƽ���ʾ���ȼ�װ��

	SVT_NEW_YEAR_RED_PACKET_REWARD_NEED_NUM = 330,		//������������Ҫ����
	SVT_NEW_YEAR_RED_PACKET_REWARD_ID = 331,			//����������ID
	SVT_NEW_YEAR_RED_PACKET_TITLE_NEED_NUM = 332,		//�������ƺŽ�����Ҫ����
	SVT_NEW_YEAR_RED_PACKET_TITLE_ID = 333,				//�������ƺ�ID


	SVT_NEW_YEAR_RED_PACKET_ACTIVITY_ID = 334,	// �������ID
	SVT_GOLD_TITLE_DUNGEON_GOLD_RATE = 335,		// ��ҳƺ�ת������
	SVT_NEW_YEAR_RED_PACKET_SORT_LIST_ACTIVITY_ID = 336,	// ���������лID

	SVT_PREMIUM_LEAGUE_ENROLL_REWARD_ITEM = 340,		// �ͽ�����������������
	SVT_PREMIUM_LEAGUE_ENROLL_REWARD_ITEM_NUM = 341,	// �ͽ���������������������
	SVT_PREMIUM_LEAGUE_RACE_REWARD_ITEM = 342,			// �ͽ�����ÿ�ֽ�������
	SVT_PREMIUM_LEAGUE_EVERY_RACE_MAX_REWARD_NUM = 343,	// �ͽ�����ÿ����ཱ������
	SVT_PREMIUM_LEAGUE_TOTAL_RACE_MAX_REWARD_NUM = 344,	// �ͽ�����������ཱ������
	SVT_PREMIUM_LEAGUE_REENROLL_COST_ITEM = 345,		// �ͽ��������±������ĵĵ���ID

	SVT_APPOINTMENT_MAX_ROLE = 350,						//���ԤԼ��ɫ����
	SVT_ONE_TOKEN_COIN_TO_FATIGUE_NUM = 351,			// 1���ҿ�ת��ƣ����
	SVT_REPLAY_LIST_TYPE_MASTER_WAIT_LIMIT = 352,		// ���ֶԾ��ȴ��б���������
	SVT_REPLAY_LIST_TYPE_MASTER_LIMIT = 353,			// ���ֶԾ��б���������
	SVT_REPLAY_MASTER_SCORE_UPPER_LIMIT = 354,			// ���ֶԾ�¼���������
	SVT_PK_USE_ROBOT_WAIT_SEC = 355,					// pkʱ�ȴ�������ʱ��(s)
	SVT_PK_GOLD_USE_ROBOT_WAIT_SEC = 356,				// �ƽ��λpkʱ�ȴ�������ʱ��(s)

	SVT_PET_SELECT_SKILL_COUNT_MAX = 360,				// ���＼����ѡ��������
	SVT_PET_SELECT_SKILL_ITEM = 361,					// ���＼����ѡ������ƷID
	SVT_PET_SELECT_SKILL_ITEM_NUM = 362,				// ���＼����ѡ������Ʒ����

	SVT_EQUIPQL_PERF_MATID = 370,						//����ϴ��Ʒ��������Ҫ�Ĳ���ID
	SVT_EQUIPQL_PERF_MATNUM = 371,						//����ϴ��Ʒ��������Ҫ�Ĳ�������

	SVT_EQUIP_TRANSFER_COLOR = 373,					//װ��ת��Ҫ��װ����ɫ

	SVT_EQUIP_RECO_TIMES = 374,						//׼������һ��������
	SVT_EQUIP_RECO_UPSHB = 375,						//װ�����ջ����������Ļ�������
	SVT_EQ_DOUBLESWORD_OCC = 376,					//��ʹ��˫��ְҵ
	SVT_GAMBING_PARTICIPANT_LIST_CAPACITY = 378,		// �ᱦ�������б�����
	SVT_GAMBING_BEGIN_ANNOUNCE_INTERVAL = 379,			// �ᱦ��������������ʱ���
	SVT_GAMBING_ANNOUNCE_INTERVAL = 380,				// �ᱦ����������
	SVT_EQUIP_RECO_SHUHUI_HBTYID = 383,						//װ������װ��������Ļ�������id
	SVT_EQUIP_RECO_SHUHUI_HBNUM = 384,						//װ������װ��������Ļ����������
	SVT_EQUIP_RECO_SHUHUI_PRICERATION = 385,			//װ������װ��������Ļ��������������

	SVT_AUCTION_RECENT_PRICE_NUM_MIN = 386,					//�����н���ƽ���۸������С����
	SVT_AUCTION_RECENT_PRICE_NUM_MAX = 387,					//�����н���ƽ���۸����������
	SVT_AUCTION_RECENT_PRICE_TIME_SPAN = 388,				//�����н���ƽ���۸����ʱ����
	SVT_GOLD_ITEM_DATA_ID = 389,							//���ID
	SVT_CHARGE_UN_FREEZE_MONEY_RATE = 390,					//��ֵ�ⶳ����
	SVT_DAILY_TASK_SCORE_UN_FREEZE_REQUIREMENT = 391,		//�ճ������Ծ�Ƚⶳ����ֵ
	SVT_DAILL_TASK_SCORE_UN_FREEZE_DAYS = 392,				//�ճ������Ծ�Ƚⶳ����
	SVT_FREEZE_TAIL_BASE_DAYS = 393,						//���ᱣ���ڻ�������
	SVT_FREEZE_TAIL_ACCUMULATE_DAYS = 394,					//���ᱣ�����ۼ�����
	SVT_FREEZE_TAIL_MAX_DAYS = 395,							//���ᱣ�����������

	SVT_CITY_MONSTER_DAILY_COUNT = 400,						//�������ÿ�մ���
	SVT_ONE_FATIGUE_TO_MAGPIE_BRIDGE_PROGRESS = 401,		//1ƣ�Ϳ�תȵ�Ž���

	SVT_RELATION_RECOMMAN_COOLTIEM = 402,					//ʦ��ͽ���Ƽ���ȴʱ��
	SVT_RELATION_RECOMMAN_PLAYERNUM = 403,					//ʦ��ͽ���Ƽ�����
	SVT_RELATION_FINISHLEVEL = 404,							//ͽ�ܳ�ʦ�ȼ�
	SVT_RELATION_MASTERDAILYTASK_NUM = 405,					//ʦ���ճ��������
	SVT_RELATION_REQMASTER_PUNISHTIME = 406,				//��ʦ�ͷ�ʱ��
	SVT_RELATION_REQDISCIP_PUNISHTIME = 407,				//��ͽ�ͷ�ʱ��
	SVT_RELATION_NOPUNISH_OFFLINETIME = 408,				//���ʦͽ��ϵ���ܳͷ�����ʱ��
	SVT_MASTERSYS_AUTOFINISHSCH_OFFLINETIEM = 409,			//�Զ���ʦ��Ҫ�Է�����ʱ��
	SVT_SCORE_WAR_BATTLE_COUNT = 410,						//������ս������
	SVT_CLEARDUNGEN_ADDGROWTH_MAX = 411,				    //ʦͽ���ͨ�ص��³ǳɳ�ֵÿ����������ֵ
	SVT_MASTER_GIVEEQ_ADDGROWTH_MAX = 412,				    //ʦ��װ�����ͳɳ�ֵÿ����������ֵ
	SVT_TAP_SYSTEM_APPRENT_LEVEL_MIN = 413,					//��ʦ��͵ȼ�
	SVT_TAP_SYSTEM_APPRENT_LEVEL_MAX = 414,					//��ʦ��ߵȼ�
	SVT_VETERANT_DEFINE_SERVICE_DAYS = 415,					//�ϱ����忪������
	SVT_VETERANT_DEFINE_ROLE_LEVEL = 416,					//�ϱ������ɫ�ȼ�
	SVT_VETERANT_DEFINE_OFFLINE_DAYS = 417,					//�ϱ�������������
	SVT_VETERANT_DEFINE_RETURN_COOLDOWN = 418,				//�ϱ�����ع�cd
	SVT_VETERANT_RETURN_DAYS = 419,							//�ϱ��ع�����

	SVT_TPSKYFASHION_PART_NOTCREATE_TIMES = 420,			//ͬһ��λ�ϳ�͸�������ʱ,�������ò�λ�����ٳ�͸������״���
	SVT_TPSKYFASHION_BASICS_RATE = 421,                     //͸������׵ĺϳ��������ʣ��ٷֱ�
	SVT_TPSKYFASHION_REDUCE_RATE = 422,						//�ϳ�һ��͸�������,͸������׵Ĵ������ʽ���ֵ
	SVT_TPSKYFASHION_INCRE_RATE = 423,						//�ϳ�һ�����������,͸������׵Ĵ�����������ֵ
	SVT_TPSKYFASHION_RATE_LOWERLIMIT = 424,				    //�ϳ�һ��͸�������,��������
	SVT_TPSKYFASHION_INCRE_UPPERLIMIT = 425,				//�ϳ�һ�����������,��������

	SVT_DIG_GRID_NUM = 430,									//�ڱ�������
	SVT_DIG_GOLD_ITEM_ID = 431,								//�������ƷID
	SVT_DIG_SILVER_ITEM_ID = 432,							//��������ƷID
	SVT_DIG_GOLD_BUY_ITEM_ID = 433,							//���ڱ����͵���ƷID
	SVT_DIG_GOLD_BUY_ITEM_NUM = 434,						//���ڱ����͵���Ʒ����
	SVT_DIG_SILVER_BUY_ITEM_ID = 435,						//���ڱ����͵���ƷID
	SVT_DIG_SILVER_BUY_ITEM_NUM = 436,						//���ڱ����͵���Ʒ����
	SVT_DIG_REFRESH_START_HOUR = 437,						//�ڱ�ˢ�¿�ʼʱ��
	SVT_DIG_REFRESH_END_HOUR = 438,							//�ڱ�ˢ�½���ʱ��


	SVT_MASTERSYS_ANNOUNCE_SPAN = 440,						//ʦͽ��������Ƶ��ʱ����
	SVT_MASTERSYS_DISCIPLE_TITLEID = 441,					//ʦͽ��ʦͽ�ܻ�óƺ�
	SVT_MASTERSYS_FINSCH_TITLEID = 442,						//ʦͽͽ�ܳ�ʦ��óƺ�


	SVT_SCORE_WAR_LEVEL = 450,								//33��ȼ�

	SVT_BIND_DEVICE_MAX_NUM	= 455,							//���豸�������
	SVT_SECURITY_LOCK_FORCE_TIME = 456,						//��ȫ��ǿ�ƽ���ʱ��
	SVT_SECURITY_LOCK_PASSWD_ERROR_NUM = 457,				//��ȫ������������

	SVT_CHFASH_ACTSCORE_SHOPMALL = 460,                     //��װ���� �̳ǹ���ʱװ��ȡ1����ȡ���� �ٷְ�
	SVT_CHFASH_ACTSCORE_FASHCOMP = 461,                     //��װ���� ʱװ�ϳ�1������׻�ȡ����
	SVT_CHFASH_ACTSCORE_ACTFASHCOMP = 462,					//��װ���� �ʱװ�ϳɣ��ϳɽ�������ʿ��ʱ��ȡ���ϳ�1�λ�ȡ����
	SVT_3V3_TUMBLE_PLAYER_NUM	= 463,						// 3V3�Ҷ�ÿ����������
	SVT_STRENGTHEN_TICKET_FUSE_STREN_LEVEL_LIMIT = 466,		// ǿ��ȯ�ں�ǿ��ȯ�ȼ�����
	SVT_STRENGTHEN_TICKET_FUSE_STREN_LEVEL_LOWER_LIMIT = 467,//ǿ��ȯ�ں�ǿ��ȯ�ȼ�����
	SVT_STRENGTHEN_TICKET_FUSE_TOTAL_VALUE_FIX = 468,		//ǿ��ȯ�ں��ܼ�ֵ����ֵ
	
	SVT_GUILD_DUNGEON_GUILD_LEVEL_LIMIT = 469,				// ���ḱ����ͼ��������ȼ�����
	SVT_GUILD_DUNGEON_PLAYER_LEVEL_LIMIT = 470,				// ���ḱ����ͼ��ս��ҵȼ�����
	
	SVT_SECURITY_LOCK_PASSWD_MIN_LENGTH	= 471,				//��ȫ��������С����
	SVT_SECURITY_LOCK_PASSWD_MAX_LENGTH = 472,				//��ȫ��������󳤶�
	SVT_ROLE_BASE_FIELD = 473,								//��ɫ������λ
	SVT_ROLE_EXTENSIBLE_BASE_FIELD = 474,					//��ɫ����չ��λ��������
	SVT_ROLE_EXTEND_PER_INC_NUM = 475,						//��ɫ��չ������������

	SVT_GUILD_DUNGEON_VICTORY_CHAT_INTERVAL = 476,			// ������³�ʤ������Ƶ����Ϣ���(��)
	SVT_GUILD_DUNGEON_VICTORY_CHAT_CNT = 477,				// ������³�ʤ������Ƶ����Ϣ����
	SVT_GUILD_DUNGEON_LOTTERY_CHAT_COLOR = 478,				// ������³ǳ齱�߼�������Ϣ

	SVT_WEAPON_LEASE_TICKETS_MAXBUN = 482,				// �������˷��������
	SVT_BLESS_CRYSTAL_OWN_MAX_NUM = 483,					// �͸�ˮ��ӵ���������ֵ
	SVT_BLESS_CRYSTAL_OWN_MAX_EXP = 484,					// �͸�ˮ��ӵ�о������ֵ
	SVT_INHERIT_BLESS_OWN_MAX_NUM = 485,					// ����ף��ӵ���������ֵ
	SVT_INHERIT_BLESS_OWN_MAX_EXP = 486,					// ����ף��ӵ�о������ֵ
	SVT_INHERIT_BLESS_UNIT_EXP_FOR_USE = 487,				// ����ף����Ԫ����ʹ��ֵ
	SVT_INHERIT_BLESS_USE_REWARD_PLAYER_EXP = 488,			// ����ף��ʹ�ý�����Ҿ���ֵ
	SVT_INHERIT_BLESS_USE_UNIT_EXP_REWARD_PLAYER_EXP = 489, // ����ף��ʹ�õ�Ԫ���齱����Ҿ���ֵ
	SVT_BLESS_CRYSTAL_ITEM_DATA_ID = 496,					// �͸�ˮ������ID
	SVT_INHERIT_BLESS_ITEM_DATA_ID = 497,					// ����ף������ID
	SVT_GNOME_COIN_ITEM_DATA_ID = 501,						// �ؾ�����ҵ���ID

	SVT_AUCTION_TREAS_PUBLIC_TIME = 490,				// ��������Ʒ��ʾʱ�䳤��(��λСʱ)
	SVT_AUCTION_ON_SALE_WAIT_SEC_LOWER = 491,			// �����з���Ʒ�ϼܵȴ�ʱ������(��λ��)
	SVT_AUCTION_ON_SALE_WAIT_SEC_UPPER = 492,			// �����з���Ʒ�ϼܵȴ�ʱ������(��λ��)
	SVT_AUCTION_RUSH_BUY_TIME = 493,					// ��������Ʒ����ʱ�䳤��(��λ��)
	SVT_AUCTION_QUERY_PAGE_MAXNUM = 498,				// ������һҳ�����ʾ����
	SVT_AUCTION_TREAS_COOLTIME = 499,				// ��������Ʒ������ȴʱ��(��λ��)
	SVT_AUCTION_NOT_TREAS_COOLTIME = 500,				// �����з���Ʒ������ȴʱ��(��λ��)
	SVT_AUCTION_NOT_TREAS_ONSALE_TIME = 502,		//�����з���Ʒ�ϼ�ʱ�䳤��(��λСʱ)
	SVT_AUCTION_TREAS_ONSALE_TIME = 503,			//��������Ʒ�ϼ�ʱ�䳤��(��λСʱ)
	SVT_AUCTION_CALC_AVERPRICE_MINTIMES = 504,			//��������Ʒ����ƽ�����׼۸���ͽ��״���
	SVT_AUCTION_NOTTREAN_CALC_AVERPRICE_MAXTIMES = 505,			//�����з���Ʒ��Ʒ����ƽ�����׼۸�����״���
	SVT_AUCTION_TREAN_CALC_AVERPRICE_MAXTIMES = 506,		//��������Ʒ��Ʒ����ƽ�����׼۸�����״���
	SVT_AUCTION_QUERY_TIME_INTERVAL = 507,				//�����в�ѯ���ʱ��
	SVT_AUCTION_DEPOSIT_COST_UPLIMIT = 508,				//�������ϼܷ�����
	SVT_CHARGE_UN_FREEZE_MONEY_RATE_TWO = 509,			//��ֵ�ⶳ����2
	SVT_TUIJIANJIAGE_BEISHU = 510,						//��Ʒ�����ϼܼ۸���

	SVT_ARTIFACT_JAR_COUNT_20 = 512,					//20���������ɽ���Ҫ����
	SVT_ARTIFACT_JAR_COUNT_30 = 513,					//30���������ɽ���Ҫ����
	SVT_ARTIFACT_JAR_COUNT_40 = 514,					//40���������ɽ���Ҫ����
	SVT_ARTIFACT_JAR_COUNT_50 = 515,					//50���������ɽ���Ҫ����
	SVT_ARTIFACT_JAR_COUNT_60 = 516,					//60���������ɽ���Ҫ����
	SVT_ARTIFACT_JAR_COUNT_65 = 517,					//65���������ɽ���Ҫ����

	SVT_RELATION_REMOVEDISP_TIME = 519,					//ʦͽʦ����ͽ�ܱ���ʱ��

	SVT_GUILD_DUNGEON_COLD_TME = 520,					//������³���ȴʱ��
	SVT_GUILD_DUNGEON_SEC_MAX_DAMAGE = 521,				//������³�ÿ������˺�
	SVT_GUILD_DUNGEON_VERIFY_TIME = 522,				//������³���֤ʱ��

	SVT_AUCTION_ATTENT_MAXNUM = 523,					//�����й�ע������
	SVT_GUILD_SET_MAX_JOIN_LEVEL = 524,					//��������������ȼ�
	SVT_CTT_DUNGEON_TIMES_MAX = 525,					//ѭ�����񸱱����������
	SVT_CTT_SET_ITEM_TIMES_MAX = 526,					//ѭ�������Ͻ�����������
	SVT_CTT_STORY_TIMES_MAX = 527,						//ѭ������Ի�������

	SVT_RETURN_CONTINUE_LOGIN_DAY =528,					// �ع��������Ϊ��¼����
	SVT_RETURN_NEED_ROLE_LEVEL = 529,					// �ع����Ҫ���ɫ�ȼ�
	SVT_ENERGY_COIN_ITEM_DATA_ID = 530,					// �����̵����Ļ��ҵ���ID
	SVT_BLACKMARKET_FIXEDPRICE_VIP_RATE = 531,			//�������˹̶��۸��չ�vip������
	SVT_BLACKMARKET_FIXEDPRICE_NOTVIP_RATE = 532,		//�������˹̶��۸��չ���vip������
	SVT_BLACKMARKET_BACKBUY_GOLD_UPPER_BOUND = 533,     //���������չ���������Ͻ�
	SVT_BLACKMARKET_BACKBUY_GOLD_LOWER_BOUND = 534,     //���������չ���������½�
	SVT_RETURN_HELL_DROP_GIFT = 535,					// �ع���Ԩ�������
	SVT_RETURN_HELL_DROP_DAILY_MAX_NUM = 536,			// �ع���Ԩ�������
	SVT_MAX_QUALITY_LEVEL_SCORE_COEFFICIENT = 537,		// ���Ʒ������ϵ��

	SVT_BLACKMARKET_BUYLIMIT_PROP	= 538,				// ���������չ������Ȩ��(ռ������˰��)
	SVT_BLACKMARKET_PLAYER_LEV  = 539,					// ��������������뽻����͵ȼ�
	SVT_MERCENARY_FOUND_GIVE_RENAME_CARD = 545,			// Ӷ���Ŵ���������ʱ������

	SVT_USE_INHERIT_BLESS_MIN_LV = 546,					// ����ף����С��ɫʹ�õȼ�
	SVT_USE_INHERIT_BLESS_MAX_LV = 547,					// ����ף������ɫʹ�õȼ�
	SVT_MERGE_SERVER_GIVE_RENAME_CARD = 548,			// �Ϸ�����������ʱӶ���Ÿ�����

	SVT_GUILD_AUCTION_TIME = 549,						// ��������ʱ��(��)
	SVT_GUILD_WORLD_AUCTION_TIME = 550,					// ������������ʱ��(��)
	SVT_GUILD_AUCTION_BID_TIME = 551,					// ������������ʱ��(��)

	SVT_BLACKMARKET_PROP_GREEN = 552,					// ����������װ�չ����
	SVT_BLACKMARKET_PROP_PINK = 553,					// �������˷�ɫ�չ����

	SVT_GUILD_EMBLEM_ROLE_LEVEL = 556,					// ����ռǽ�ɫ�ȼ�
	SVT_GUILD_EMBLEM_GUILD_LEVEL = 557,					// ����ռǹ���ȼ�
	SVT_GUILD_EMBLEM_HONOUR_LEVEL = 558,				// ����ռ���ҫ���õȼ�
	SVT_GUILD_EMBLEM_SKILL_PVP = 559,					// ����ռǼ���pvp

	SVT_AUCTION_PRICESLIST_NUM = 561,					// �������ۼ��б�����
	SVT_GUILD_AUCTION_PREPARE_TIME = 562,				// ��������׼��ʱ��(��)
	SVT_GUILD_WORLD_AUCTION_LVL = 563,					// �������������ȼ�
	SVT_GUILD_EMBLEM_SKILL_PVE = 565,					// ����ռǼ���pve

	SVT_AUCTION_RECOVE_TREANS_NUM = 566,				// ��������Ʒɨ��ÿ������
	SVT_AUCTION_RECOVE_NOTTREANS_NUM = 567,				// �����з���Ʒɨ��ÿ������

	SVT_WEEK_HELL_FAIL_RETURN_TICKETS = 568,			//�ܳ���Ԩ����ʧ�ܷ�����Ʊ����

	SVT_GUILD_AUCTION_BONUS_NUM = 569,					// ���������ֳ�����
	SVT_MONTH_CARD_DEPOSIT_TIME = 572,					// �¿��ݴ�����Чʱ��(��)
	SVT_MONTH_CARD_DEPOSIT_GIRD = 573,					// �¿��ݴ������

	SVT_MAGIC_COMP_ONES_MAXTIMES = 575,					// ��ħ��һ���ϳ�������

	SVT_ENHANCE_PROTECT_ID = 579,                       //��������ȯID

	SVT_EQUAL_PVP_LIMIT_LEVEL = 580,                    //��ƽ�������ȼ�����

	SVT_STRONG_TEMP = 581,                              //ǿ�ҵĺۼ�ID

	SVT_SIGN_NEW_NOFREE = 582,							// �շѲ�ǩ��������
	SVT_SIGN_NEW_FREE = 583,							// ��Ѳ�ǩ��������
	SVT_SIGN_NEW_ACTIVITE = 584,						// ��Ծ�Ȳ�ǩ��������
	SVT_ADVENT_TEAM_SKILL_PVP = 585,					// ð���ż���pvp
	SVT_ADVENT_TEAM_SKILL_PVE = 586,					// ð���ż���pve

	SVT_ACTIVITE_SIGN_LIMIT = 587,						// ��Ծ�ȿɲ�ǩ����
	SVT_TRIGGER_DUNGEON_HIDDEN_ROOM_PROB = 591,         // �������ط������

	SVT_BATTLE_NPC_EXCHANGE_EQUIP_NUM = 589,	// �Լ�npc�һ�װ����
	SVT_BATTLE_NPC_SURVIVAL_TIME = 590,			// �Լ�npc���ʱ��

	SVT_BATTLE_NPC_TRADER_NUM = 592,			// �Լ�npc��������
	SVT_BATTLE_NPC_MONSTER_NUM = 593,			// �Լ�npc��������

	SVT_GUILD_EMBLEM_SCORE_RATE = 594,			//����ռ�����ϵ��
	SVT_ADVENT_EMBLEM_SCORE_RATE = 595,			//Ӷ���Żռ�����ϵ��	SVT_MAX,

	SVT_EQUAL_PVP_OPEN_ACTIVITE_ID1 = 598,       //��ƽ����������1�������ƻID
	SVT_EQUAL_PVP_OPEN_ACTIVITE_ID2 = 599,       //��ƽ����������2�������ƻID
	SVT_EQUAL_PVP_OPEN_ACTIVITE_ID3 = 600,       //��ƽ����������3�������ƻID
	SVT_EQUAL_PVP_OPEN_ACTIVITE_ID4 = 601,       //��ƽ����������4�������ƻID
	SVT_EQUAL_PVP_OPEN_ACTIVITE_ID5 = 602,       //��ƽ����������5�������ƻID
	SVT_EQUAL_PVP_OPEN_ACTIVITE_ID6 = 603,       //��ƽ����������6�������ƻID
	SVT_EQUAL_PVP_OPEN_ACTIVITE_ID7 = 604,       //��ƽ����������7�������ƻID

	SVT_AUCTION_ONSALE_DUETIME_REMAIN = 605,      //���������޷���Ʒ��Ʒ�ϼ�����ʣ��ʱ��   
	SVT_STRENGTH_ASSIST_COST_MOD = 606,			  //����װ��ǿ������ϵ��

	SVT_GUILD_REDPACKET_ACC_RECV_DAY_MAX = 607,	  //�������˺�����ȡ����	 

	SVT_GUILD_MERGER_SPACE_DAY = 608,             //����沢�����������
	SVT_ZJSL_TOWER_FLOOR_OPEN_DAILY	= 609,		//�ռ���������ÿ�쿪�Ų���
	SVT_ZJSL_DUNGEON_TIMES_DAILY	= 610,		//�ռ�����ÿ���ͬһ�ص��³ǵĴ�������

	SVT_MONTH_CARD_DEPOSIT_TIPS_TIME = 611,       //�¿��ݴ���ʾʱ��(Сʱ)

	SVT_MALL_INTEGRAL_NUM = 612,                  //�̳ǻ�����������
	SVT_TICKET_CONVERT_MALL_INTEGRAL_RATE = 613,  //1��ȯת���̳ǻ���ϵ��

	SVT_12_SKILL_SLOT_TASK_ID = 614,			//��12���ܲ���ɵ�����
	SVT_ADVENTURE_PASS_COIN = 616,              //ð��ͨ��֤����ID

	SVT_ADVENTURE_PASS_UNLOCK = 617,            //ð��ͨ��֤��ֹǰN�첻��������(����)

	SVT_ADVENTURE_PASS_EXP = 624,               //ð��ͨ��֤���⾭�������ֵ

	SVT_RETURN_ANNIVERSARY_TITLE = 625,         //�ع�����ƺ�
	SVT_MALL_POINT_ITEM_ID = 626,               //�̳ǻ��ֵ���ID

	SVT_REPORT_CONSUME = 628,					//�ٱ����ĵĻ�Ծ��ֵ

	SVT_SKILL_PAGE_OPEN_CONSUME_NUM = 629,      //�¼��ܱ�ǩҳ������������
	SVT_SKILL_PAGE_OPEN_CONSUME_CURRENCY = 630, // �¼��ܱ�ǩҳ�������ĵ���id

	SVT_DUNGEON_ROLL_TIME = 631,				//���³�roll����rollʱ��
	SVT_DUNGEON_ROLL_RESULT_TIME = 632,			//���³�roll������չʾʱ��


	SVT_CHALLENGER_INTEGRAL_ITEM_ID = 634,      //��ս�߻�����ƷID

	SVT_ADVENTURE_PASS_EXP_ITME = 635,          //ð��ͨ��֤�������ID
	SVT_ADVENTURE_PASS_EXP_COIN = 636,          //ð��ͨ��֤����ת�����ұ���
	SVT_INDPEND_ATK_PVP_RATIO = 637,			//����������PVPϵ��

	SVT_2V2_SCORE_WAR_LEVEL = 638,           //2v2�Ҷ��������ȼ�����
	SVT_2V2_SCORE_WAR_PARTAKE_NUM = 639,        //2v2�Ҷ��������������
	SVT_2V2_SCORE_WAR_INITIAL_MATCH_TIME = 640, //2v2�Ҷ���������ʼ�׶�ƥ��ʱ�䣨�룩
	SVT_2V2_SCORE_WAR_MIDDLE_MATCH_TIME = 641,  //2v2�Ҷ��������м�׶�ƥ��ʱ�䣨�룩
	SVT_2V2_SCORE_WAR_START_SCORE = 642,        //2v2�Ҷ���������ʼ��
	SVT_2V2_SCORE_WAR_WIN_SCORE = 643,        //2v2�Ҷ�������ʤ����
	SVT_2V2_SCORE_WAR_LOSE_SCORE = 644,        //2v2�Ҷ�������ʧ�ܷ�
	SVT_2V2_SCORE_WAR_MATCH_READY_TIME = 645,  // 2v2�Ҷ�������ƥ�������
	SVT_2V2_SCORE_WAR_TITLE_ID = 646,        //2v2�Ҷ�������ͷ��id
	SVT_2V2_SCORE_WAR_TITLE_TIME = 647,  // 2v2�Ҷ�������ͷ�γ���ʱ��

	SVT_PK_SEASON_1V1_HONOR_MAX = 648,  // ����������������

	SVT_CHALLENGER_SPRING_ITEM_ID = 649,      //������ս������ƷID

	SVT_WEEK_SIGN_SPRING_DUNGEON_LV = 651,      //2020������ǩ�����³ǵȼ���Χ
	
	SVT_PLANT_PROF_MAX_VALUE = 652, //ֲ�� ���������ֵ

	SVT_HONOR_EXP_RATE_1 = 653,		//��ҫ�������1
	SVT_HONOR_EXP_RATE_2 = 654,		//��ҫ�������2

	SVT_PK_CHIJI_HONOR_MAX = 659,  // �Լ�����������

	SVT_ADVENTURE_PASS_KING_ITEM = 660,  // ð��ͨ��֤���߰��������
	
	SVT_DL_MYSTERIOUS_SHOP_PROBABILITY = 661, //���������̵����

	SVT_ROLE_STORAGE_UNLOCK_BEGIN	= 662,	//��ɫ�ֿ������أ���ʼ
	SVT_ROLE_STORAGE_UNLOCK_END		= 678,	//��ɫ�ֿ������أ�����

	SVT_LOSTDUNG_DUNG_TEAM_MAX = 679,  // �������³ǹؿ���������
	SVT_LOSTDUNG_DUNG_TEAM_PLAYER_MAX = 680,  // �������³ǹؿ������Ա����

	SVT_CAPTAIN_LEAVE_TEAM_TIME = 681, //�ӳ��Զ����ʱ��(��λ:��)
	
	SVT_LOSTDUNG_DUNG_BATTLE_PROTECT_TIME = 682, //�������³�ս�����ر���ʱ��
	SVT_LOSTDUNG_DUNG_BIRTH_PROTECT_TIME = 684,  //�������³ǳ�������ʱ��
	SVT_LOSTDUNG_DUNG_DEDUCTING_LIFE = 689,		//������;�˳��ؿ��۳�Ѫ��(��ǰѪ���ٷֱ�)
	SVT_LOSTDUNG_DUNG_TASK_PROTECT_TIME = 690, //�������³��������񱣻�ʱ��
	SVT_LOSTDUNG_DUNG_FADIAN_ITEM = 692, //�����Ϲŷ������id
	SVT_LOSTDUNG_DUNG_FADIAN_SCORE_ADD = 693, //�����Ϲŷ�����ּӳɱ���
	SVT_LOSTDUNG_DUNG_REVIVE_STONE_ITEM = 694, //�����������ʯid
	SVT_LOSTDUNG_DUNG_REVIVE_LUNHUI_ITEM = 695, //���������ֻ�֮ʯid

	SVT_CHAMPION_RELIVE_ITEM_ID = 696, //�ھ����������id
	SVT_CHAMPION_RELIVE_DIANQUAN_NUM = 697, //�ھ��������ȯ����

	SVT_CHAMPION_RELIVE_BETLIMIT_NUM = 698, //�ھ�����������
	
	SVT_LOSTDUNG_CANT_ENTER_TIME = 699, //��������ǰ���벻���ٽ���ս������
	SVT_LOSTDUNG_DUNG_DELAY_TIME = 700, //����ս���ӳٽ���ʱ��
	SVT_LOSTDUNG_DUNG_DESTRUCTION_TIME = 701, //����ս��ǿ��������ʱ��
	SVT_LOSTDUNG_DUNG_QIBAO_ITEM = 702, //���������鿴�鱨id

	SVT_ADVENTURE_TEAM_RENAME_COST = 704, //Ӷ���Ÿ�������
	SVT_WEAPON_BUILD_UNIQUE_NOTIFY_SCORE = 705, //�������켫Ʒװ�����������

	SVT_PREFERENCES_ROLE_NUM = 706,//ƫ�ý�ɫ��������

	SVT_ABYSS_TICKET_WAIVER = 707, //��ԨƱ������У���ǰ��ԨƱ�ٷֱȣ�
};

struct SystemValueDataEntry : public Avalon::DataEntry
{
public:
	SystemValueDataEntry();
	~SystemValueDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//ID
	UInt32 id;

	//��������
	SystemValueType type;

	//��������2
	SystemValueType type2;

	//��������3
	SystemValueType type3;

	//��ֵ
	UInt32 value;

};

class SystemValueDataEntryMgr : public Avalon::DataEntryMgrBase<SystemValueDataEntry>, public Avalon::Singleton<SystemValueDataEntryMgr>
{
public:
	typedef std::map<SystemValueType, SystemValueDataEntry*> SystemValueMap;
public:
	SystemValueDataEntryMgr();
	~SystemValueDataEntryMgr();

public:
	bool AddEntry(SystemValueDataEntry* entry);

	void OnReload();

	void OnAddEntry(SystemValueDataEntry* entry);

	UInt32 GetSystemValue(SystemValueType type);

private:
	SystemValueMap	m_SystemValues;
};

// ϵͳ���ر�
struct SystemSwitchData : public Avalon::DataEntry
{
public:
	SystemSwitchData();
	~SystemSwitchData();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//ϵͳö��
	UInt32 id;

	//����ֵ
	UInt8 value;


};

class SystemSwitchDataMgr : public Avalon::DataEntryMgrBase<SystemSwitchData>
	, public Avalon::Singleton<SystemSwitchDataMgr >
{
public:
	SystemSwitchDataMgr();
	~SystemSwitchDataMgr();

	void GetCloseSysInfo(std::vector<SystemSwitchData*>& infos);
};

#endif //_CL_SYSTEM_VALUE_DATAENTRY_H_

