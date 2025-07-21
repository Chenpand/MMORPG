 
#ifndef _COMMON_TYPEDEF_H_
#define _COMMON_TYPEDEF_H_

#include <AvalonDefine.h>
#include <AvalonStringUtil.h>
#include "CLDefine.h"

#ifdef __GNUC__
#include <stdint.h>
#include <inttypes.h>
#endif

#define MAX_LOG_LENGTH  1024
#define MAX_MESSAGE_LENGTH MAX_LOG_LENGTH*5
#define MAX_OPTION_LENGTH  128
#define UL_OK 0
#define UL_FAIL -1
#define MAX_IPADDR_LENGTH 20
#define MAX_USERMSG_LENGTH 512

#define DEFAULT_CONFIGFILENAME			"../Config/UdpLog.xml"
#define DEFAULT_SERVER_CONFIG_FILE		"../Config/UdpServer.xml"
//#define DEFAULT_CONFIGFILENAME			"D:\\udp_log\\udplog_cpp\\udplog_cpp\\udplog_cpp\\Debug\\config\\udplog.xml"

//#if defined(AVALON_OS_WINDOWS) || defined(_AVALON_OS_WINDOWS)
//#pragma warning(disable:4996)
//#pragma warning(disable:4005)
//#endif
//
//#if defined(AVALON_OS_WINDOWS) || defined(_AVALON_OS_WINDOWS)
//#define OS_AVALON_OS_WINDOWS 1;
//#else
//#define OS_LINUX 1;
//#endif


enum LogSourceType
{
	SOURCE_TYPE_INVALID = 0,
	SOURCE_TYPE_GM = 1,

	//������
	SOURCE_TYPE_PLAYER = 100,
	SOURCE_TYPE_PLAYER_CHANGE_OCCU,
	SOURCE_TYPE_PLAYER_UP_LEVEL,
	SOURCE_TYPE_PLAYER_BIRTH,
	SOURCE_TYPE_PLAYER_ONLINE,
	SOURCE_TYPE_FRIEND_GIVE,
	//ʦ�����ƣ��
	SOURCE_TYPE_MASTER_GET_FATIGUE,
	//ɾ��ʦͽ��ϵ
	SOURCE_TYPE_DEL_MASTER_DISCIPLE,
	//���ˢ��
	SOURCE_TYPE_PLAYER_REFRESH,
	//��������չ��ɫ��λ
	SOURCE_TYPE_UNLOCK_EXTENSIBLE_ROLE_FIELD,
	//ɾ����ɫ
	SOURCE_TYPE_DELETE_ROLE,
	//�ָ���ɫ
	SOURCE_TYPE_RECOVER_ROLE,
	//��������ҳ
	SOURCE_TYPE_UNLOCK_SKILL_PAGE,


	//���³����
	SOURCE_TYPE_DUNGEON = 1000,
	//����
	SOURCE_TYPE_DUNGEON_MONSTER,
	//����
	SOURCE_TYPE_DUNGEON_BOX,
	//�ƽ�����
	SOURCE_TYPE_DUNGEON_GOLD_BOX,
	//���³ǽ���
	SOURCE_TYPE_DUNGEON_END,
	//����֮��
	SOURCE_TYPE_DUNGEON_DEATH_TOWER,
	//ɨ������֮��
	SOURCE_TYPE_DEATH_TOWER_WIPEOUT,
	//����֮����ͨ����
	SOURCE_TYPE_DEATH_TOWER_PASS_FLOOR_AWARD,
	//������Ʒ
	SOURCE_TYPE_DUNGEON_TASK_ITEM,
	//�������
	SOURCE_TYPE_DUNGEON_BUY_TIMES,
	//����
	SOURCE_TYPE_DUNGEON_CHEST,
	//������䷭��
	SOURCE_TYPE_DUNGEON_MULTI_RACE_END_DROP,
	//����
	SOURCE_TYPE_DUNGEON_REVIVE,
	//��ӽ���
	SOURCE_TYPE_DUNGEON_TEAM_REWARD,
	//��Buffҩ
	SOURCE_TYPE_DUNGEON_BUFF_DRUG,
	//����ʹ��Buffҩ
	SOURCE_TYPE_DUNGEON_BUFF_DRUG_QUICK_BUY,
	//�¿������ҽ���
	SOURCE_TYPE_DUNGEON_MONTH_CARD_GOLD_REWARD,
	//��ҳƺŽ�ҽ���
	SOURCE_TYPE_DUNGEON_GOLD_TITLE_GOLD_REWARD,
	//ƣ��ȼ��
	SOURCE_TYPE_DUNGEON_FATIGUE_BURNING,
	//���㽱��
	SOURCE_TYPE_DUNGEON_RACE_END,
	//�ռ�����ˢ��BUFF
	SOURCE_TYPE_ZJSL_REFRESH_BUFF,
	//�ռ�����ȫͨ�ؽ�����ȡ
	SOURCE_TYPE_ZJSL_CLEAR_GET_AWARD,

	//��Ʒ���
	SOURCE_TYPE_ITEM = 2000,
	//����
	SOURCE_TYPE_ITEM_JAR,
	//װ���ֽ�
	SOURCE_TYPE_EQUIP_DECOMPOSE,
	//ǿ��
	SOURCE_TYPE_EQUIP_STRENGTHEN,
	//�ϳɸ�ħ��
	SOURCE_TYPE_ADD_MAGIC_CARD,
	//��ħ
	SOURCE_TYPE_ADD_MAGIC,
	//�ƶ�
	SOURCE_TYPE_MOVE,
	//ϵͳ
	SOURCE_TYPE_SYSTEM,
	//��չ����
	SOURCE_TYPE_ENLARGE_PACK,
	//��չ�ֿ�
	SOURCE_TYPE_ENLARGE_STORAGE,
	//��Ʒ�ƶ�
	SOURCE_TYPE_ITEM_MOVE,
	//������Ʒ
	SOURCE_TYPE_ITEM_TRIM,
	//ʹ����Ʒ
	SOURCE_TYPE_USE_ITEM,
	//װ����װ
	SOURCE_TYPE_SEAL_EQUIP,
	//����Ʒ�ʵȼ�
	SOURCE_TYPE_REMAKE_EQUIP,
	//�����ά��
	SOURCE_TYPE_WUDAO_REWARD,
	//������
	SOURCE_TYPE_WUDAO,
	//ʱװ�ϳ�
	SOURCE_TYPE_FASHION_COMPOSE,
	//װ������
	SOURCE_EQUIP_MAKE,
	//���ߵ���
	SOURCE_TYPE_ITEM_TIMEOUT,
	//ʱװ��������
	SOURCE_TYPE_FASHION_SEL_ATTR,
	//ʹ���¿�����
	SOURCE_TYPE_GM_MONTH_CARD,
	//������û���
	SOURCE_GOLDJAR_RESET_POINT,
	//ħ�����û���
	SOURCE_MAGJAR_RESET_POINT,
	//��������
	SOURCE_SEND_HORN,
	//ʹ�����
	SOURCE_USE_GIFT_PACK,
	//����ʮ����
	SOURCE_TYPE_ITEM_JAR_TEN,
	//����ʱ�޵���
	SOURCE_TYPE_RENEW_TIME_ITEM,
	//ʹ�ø�����
	SOURCE_TYPE_USE_CHANGE_NAME_CARD,
	//�Ϸ��ֿ����
	SOURCE_TYPE_MERGE_SERVER_STORAGE,
	//�Ϸ�����ɫ������
	SOURCE_TYPE_MERGE_SERVER_ROLECHANGENAME,
	//�Ϸ������������
	SOURCE_TYPE_MERGE_SERVER_GUILDCHANGENAME,
	//ʦ������
	SOURCE_TYPE_MASTER_GIVE,
	//ʦ�����ܸ�
	SOURCE_TYPE_MASTER_ADDON_PAY,
	//��ħ��
	SOURCE_TYPE_OPEN_MAG_BOX,
	//��ӱ���
	SOURCE_TYPE_JEWELRY,
	//װ��ת��
	SOURCE_TYPE_TRANSFER,
	//װ�����ջ�������
	SOURCE_TYPE_EQRECO_UPSCORE,
	//װ������װ�����
	SOURCE_TYPE_EQRECO_REDEEM,
	//װ�����ձ���װ���Զ�����(��һ)
	SOURCE_TYPE_EQRECO_DESTORY_SELF,
	//ʹ�����ĵ��ߵĹ��ӹ������
	SOURCE_TYPE_CONSUME_JARGIFT,
	//ǿ��ȯ�ϳ�
	SOURCE_TYPE_STRENGTHEN_TICKET_SYNTHESIS,
	//ǿ��ȯ�ں�
	SOURCE_TYPE_STRENGTHEN_TICKET_FUSE,
	//������Ƕ
	SOURCE_TYPE_PRECBEAD_MOUNT,
	//����ժ��
	SOURCE_TYPE_PRECBEAD_EXTIRPE,
	//�����û�
	SOURCE_TYPE_PRECBEAD_REPLACE,
	//װ������
	SOURCE_EQUIE_UPDATE,
	//�Ϸ���ð�ն�(Ӷ����)������
	SOURCE_TYPE_MERGE_SERVER_ADVENTURETEAMCHANGENAME,
	// ͷ��򲹷�
	SOURCE_TYPE_HEAD_FRAME_COMPENSATE,
	//��ħ������
	SOURCE_TYPE_MAGIC_UP,
	//��ħ��һ���ϳ�
	SOURCE_TYPE_ADD_MAGIC_CARD_ONEKEY,
	//װ������
	SOURCE_TYPE_EQUIP_ENHANCE,
	//�̵����
	SOURCE_TYPE_ENHANCE_MATERIAL_COMBO,
	//��Ϣװ�����
	SOURCE_TYPE_SMELL_EQUIP_CLEAR,
	//��Ϣװ������
	SOURCE_TYPE_SMELL_EQUIP_RED,
	//����װ��ת��
	SOURCE_TYPE_ENHANCE_EQUIP_CHG,
	//����ӳ�
	SOURCE_TYPE_ITEM_ACT_JAR_ONE,
	//�����ʮ����
	SOURCE_TYPE_ITEM_ACT_JAR_TEN,
	//���Ŀ���
	SOURCE_TYPE_INSCRIPTION_OPEN,
	//������Ƕ
	SOURCE_TYPE_INSCRIPTION_MOUNT,
	//����ժȡ
	SOURCE_TYPE_INSCRIPTION_EXTRACT,
	//���ĺϳ�
	SOURCE_TYPE_INSCRIPTION_SYNTHESIS,
	//��������
	SOURCE_TYPE_INSCRIPTION_DESTROY,
	//����ת��
	SOURCE_TYPE_PRECBEAD_CONVERT,
	//װ��ת��
	SOURCE_EQUIE_COVERNT,
	//��չ��ɫ�ֿ�
	SOURCE_TYPE_ENLARGE_ROLE_STORAGE,
	//װ������
	SOURCE_TYPE_EQRECO,
			
	//�̵����
	SOURCE_TYPE_SHOP = 3000,
	//����
	SOURCE_TYPE_SHOP_BUY,
	//����
	SOURCE_TYPE_SHOP_SELL,
	//�̳�
	SOURCE_TYPE_SHOP_MALL,
	//�̵�ˢ��
	SOURCE_TYPE_SHOP_REFRESH,
	//����ֿ⹺��
	SOURCE_TYPE_GUILD_SHOP_BUY,
	//�˺��̵깺��
	SOURCE_TYPE_ACCOUNT_SHOP_BUY,
	//��ֵ���͹���
	SOURCE_TYPE_RECHARGE_PUSH_BUY,

	//�������
	SOURCE_TYPE_TASK = 4000,
	//����
	SOURCE_TYPE_TASK_MAIN,
	//֧��
	SOURCE_TYPE_TASK_BRANCH,
	//ÿ������
	SOURCE_TYPE_TASK_DAILY,
	//�ɾ�
	SOURCE_TYPE_TASK_ACHIEVEMENT,
	//תְ
	SOURCE_TYPE_TASK_CHANGE_OCCU,
	//�����
	SOURCE_TYPE_TASK_ACTIVE,
	//ѭ������
	SOURCE_TYPE_TASK_CYCLE,
	//ѭ������ˢ��
	SOURCE_TYPE_TASK_CYCLE_REFRESH,
	//�������
	SOURCE_TYPE_TASK_RED_PACKET,
	//ÿ��������ֽ���
	SOURCE_TYPE_TASK_SCORE_REWARD,
	//�ƺ�����
	SOURCE_TYPE_TASK_TITLE,
	//����֮·
	SOURCE_TYPE_TASK_LEGEND,
	//��������
	SOURCE_TYPE_TASK_AWAKEN,
	//�ɾͻ��ֽ���
	SOURCE_TYPE_TASK_ACHIEVEMENT_SCORE_REWARD,
	//ʦ���ճ�����
	SOURCE_TYPE_TASK_MASTER_DAILY,
	//ʦ��ѧҵ�ɳ�����
	SOURCE_TYPE_TASK_MASTER_ACADEMIC,
	//�˺ųɾ�
	SOURCE_TYPE_ACCOUNT_TASK_ACHIEVEMENT,
	//�����������
	SOURCE_TYPE_TASK_FINISH_RIGHTNOW,
	//��ǩ���ܽ���
	SOURCE_TYPE_WEEK_SIGN_WEEK_REWARD,
	//ð�ն�(Ӷ����)�˺�ÿ������
	SOURCE_TYPE_ADVENTURE_TEAM_ACCOUNT_WEEKLY_TASK,
	//��Ȼ��ǩ�����
	SOURCE_TYPE_NEW_SIGN,

	//���������
	SOURCE_TYPE_ACTION = 5000,
	//�������ϼ�
	SOURCE_TYPE_ACTION_UP,
	//�������۳�
	SOURCE_TYPE_ACTION_SELL,
	//�����й���
	SOURCE_TYPE_ACTION_BUY,
	//�������¼�
	SOURCE_TYPE_ACTION_CANCEL,
	//�����г�ʱ
	SOURCE_TYPE_ACTION_TIMEOUT,
	//ˢ��������
	SOURCE_TYPE_AUCTION_REFRESH,
	//������λ
	SOURCE_TYPE_AUCTION_BUY_BOOTH,
	//�쳣���׳�ֵ������
	SOURCE_TYPE_ABNORMAL_TRANS_CHARGE_BACK_MONEY,
	//�쳣���״ﵽ��Ծ�ȷ�����
	SOURCE_TYPE_DAILY_TASK_SCORE_BACK_MONEY,
	//�쳣���׶����
	SOURCE_TYPE_ABNORMAL_TRANS_FREEZE_MONEY,
	//�쳣���׶����ʼ�
	SOURCE_TYPE_ABNORMAL_TRANS_FREEZE_MAIL,
	//�쳣���׽ⶳ�ʼ�
	SOURCE_TYPE_ABNORMAL_TRANS_UNFREEZE_MAIL,
	//���������ͨ�����׳ɹ�
	SOURCE_TYPE_ACTION_ADUIT_TRANS_SUCC,
	//����������쳣ȡ������
	SOURCE_TYPE_ACTION_ADUIT_CANCEL_TRANS,
	//����������쳣�������׳ɹ�
	SOURCE_TYPE_ACTION_ADUIT_ABNORAML_TRANS_SUCC,
	//�쳣����δ�����
	SOURCE_TYPE_ABNORMAL_TRANS_UNFREEZE_MONEY,
	//������ɨ��
	SOURCE_TYPE_ACTION_SYS_RECOVERY,

	//��Ԫʯ
	SOURCE_TYPE_WARP_STONE = 6000,
	//����
	SOURCE_TYPE_WARP_STONE_UNLOCK,
	//����
	SOURCE_TYPE_WARP_STONE_CHARGE,

	//���
	SOURCE_TYPE_RETINUE = 7000,
	//��ӽ���
	SOURCE_TYPE_RETINUE_UNLOCK,
	//���ϴ��
	SOURCE_TYPE_RETINUE_CHANGE_SKILL,
	//�������
	SOURCE_TYPE_RETINUE_UPLEVEL,
	//�������
	SOURCE_TYPE_RETINUE_UP_STAR,

	//�ʼ����
	SOURCE_TYPE_MAIL = 8000,
	//��ȡ����
	SOURCE_TYPE_MAIL_COLLECTION_ATTACHMENT,
	//����ϵͳ�۳�
	SOURCE_TYPE_MAIL_SYSTEM_TAKE,

	//VIP
	SOURCE_TYPE_VIP = 9000,
	//����VIP���
	SOURCE_TYPE_VIP_BUY_GIFT = 9001,
	//����֮����ֵ����
	SOURCE_TYPE_VIP_BUY_TOWER_RESET = 9002,

	//���ܽ���
	SOURCE_TYPE_FUNCTION_UNLOCK = 10000,

	//�������
	SOURCE_TYPE_SKILL = 11000,
	//��������
	SOURCE_TYPE_SKILL_UP_LEVEL,
	//���ܽ���
	SOURCE_TYPE_SKILL_DOWN_LEVEL,

	// �������
	SOURCE_TYPE_GUILD = 12000,
	// ��������
	SOURCE_TYPE_GUILD_CREATE,
	// �޸Ĺ�����
	SOURCE_TYPE_GUILD_CHANGE_NAME,
	// �������
	SOURCE_TYPE_GUILD_DONATE,
	// ����һ�
	SOURCE_TYPE_GUILD_EXCHANGE,
	// �������Ἴ��
	SOURCE_TYPE_GUILD_UPGRADE_SKILL,
	// ���͹����ʼ�
	SOURCE_TYPE_GUILD_MAIL,
	// ��ɢ����
	SOURCE_TYPE_GUILD_DISMISS,
	// Ĥ��
	SOURCE_TYPE_GUILD_ORZ,
	// Բ������
	SOURCE_TYPE_GUILD_TABLE,
	// ����ս����
	SOURCE_TYPE_GUILD_BATTLE_ENROLL,
	// ����ս����
	SOURCE_TYPE_GUILD_BATTLE_INSPIRE,
	// ����ս���ֽ���
	SOURCE_TYPE_GUILD_BATTLE_SCORE,
	// ����սƥ��
	SOURCE_TYPE_GUILD_BATTLE_MATCH,
	// ���빫��
	SOURCE_TYPE_GUILD_MEMBER_REPLY_JOIN,
	// ��������
	SOURCE_TYPE_GUILD_MEMBER_KICK,
	// ����սʤ��
	SOURCE_TYPE_GUILD_BATTLE_WIN,
	// ����սʧ��
	SOURCE_TYPE_GUILD_BATTLE_LOSE,
	// ����᳤�Զ�ת��
	SOURCE_TYPE_GUILD_AUTO_CHANGE,
	// ������ײֿ�
	SOURCE_TYPE_ADD_GUILD_STORAGE,
	// ����ս��ս
	SOURCE_TYPE_GUILD_BATTLE_CHALLENGE,
	// ����ս��ս�˻�
	SOURCE_TYPE_GUILD_BATTLE_CHALLENGE_RESULT,
	// ����齱
	SOURCE_TYPE_GUILD_LOTTRY,
	// ������ս����
	SOURCE_TYPE_GUILD_BATTLE_CHALLENGE_INSPIRE,
	// ��������ʽ�
	SOURCE_TYPE_GUILD_POINT_FUND,

	// ���������˿�
	SOURCE_TYPE_GUILD_AUCTION_REFUND,
	// ���������ֳ�
	SOURCE_TYPE_GUILD_AUCTION_BONUS,
	// ������������
	SOURCE_TYPE_GUILD_AUCTION_REWARD,
	// ������������
	SOURCE_TYPE_GUILD_AUCTION_BID,
	// ��������һ�ڼ�
	SOURCE_TYPE_GUILD_AUCTION_FIX,
	// ����ռ�����
	SOURCE_TYPE_GUILD_EMBLEM_UGRADE,
	// �������ÿ�ս���
	SOURCE_TYPE_GUILD_TERR_DAY_REWARD,

	// �������
	SOURCE_TYPE_GUILD_MAX,

	// ���
	SOURCE_TYPE_RED_PACKET = 13000,
	// �����ԷѺ��
	SOURCE_TYPE_RED_PACKET_GUILD_PAY,
	// �ԷѺ��
	SOURCE_TYPE_RED_PACKET_CUSTOM,

	// ��ֵ
	SOURCE_TYPE_BILLING = 14000,
	// ����
	SOURCE_TYPE_BILLING_GOODS,
	//�¿�
	SOURCE_TYPE_MONTH_CARD,

	//���ٹ���
	SOURCE_TYPE_QUICK_BUY = 15000,
	//ͨ�ÿ��ٹ���
	SOURCE_TYPE_QUICK_BUY_COMMON,
	//����
	SOURCE_TYPE_QUICK_BUY_REVIVE,

	//����
	SOURCE_TYPE_ACTIVE = 16000,
	//�ճ��
	SOURCE_TYPE_ROUTINE,
	//���ջ
	SOURCE_TYPE_SEVEN,
	//��ֵ����
	SOURCE_TYPE_CHARGE,
	//CDK
	SOURCE_CDK_GIVE,
	//���߶һ�
	SOURCE_TYPE_ITEM_CHANGE,
	//�·��弶��
	SOURCE_TYPE_NEW_SERVER_FIGHTING,
	// ����ǩ��
	SOURCE_TYPE_SIGN_IN_SEVEN,
	// �����
	SOURCE_TYPE_ACTIVITY_CLOSE,
	// OPPO���
	SOURCE_OPPO_CDK,
	//�ᱦ�������Ʒ
	SOURCE_TYPE_GAMBING_PAYING,
	// �ᱦ�����
	SOURCE_TYPE_GAMBING_LOTTERY,
	// ֲ���
	SOURCE_TYPE_PLANT,

	//�������
	SOURCE_TYPE_SEASON = 17000,
	//��������
	SOURCE_TYPE_SEASON_REWARD = 17001,



	//PVP���
	SOURCE_TYPE_PVP = 18000,
	//pvpս��
	SOURCE_TYPE_PVP_RACE,
	//pvp�
	SOURCE_TYPE_PVP_ACTIVITY,

	//�������
	SOURCE_TYPE_PET = 19000,
	//������
	SOURCE_TYPE_PET_ADD,
	//���۳���
	SOURCE_TYPE_PET_SELL,
	//���ɳ���
	SOURCE_TYPE_PET_DEVOUR,
	//ι������
	SOURCE_TYPE_PET_FEED,
	//��ѡ����
	SOURCE_TYPE_PET_SELECT_SKILL,

	//�ͽ�����
	SOURCE_TYPE_PREMIUM_LEAGUE = 20000,
	// ����
	SOURCE_TYPE_PREMIUM_LEAGUE_ENROLL,
	// ���±���
	SOURCE_TYPE_PREMIUM_LEAGUE_REENROLL,
	// �ͽ���������
	SOURCE_TYPE_PREMIUM_LEAGUE_REWARD,
	// 8ǿ֪ͨ
	SOURCE_TYPE_PREMIUM_LEAGUE_FINAL_EIGHT_NOTIFY,
	// �ͽ�����Ԥѡ������
	SOURCE_TYPE_PREMIUM_LEAGUE_PRELIMINAY_REWARD,

	//�齱
	SOURCE_TYPE_DRAW_PRIZE = 21000,

	//���
	SOURCE_TYPE_MONEY_MANAGE = 22000,

	//������
	SOURCE_TYPE_SCORE_WAR = 23000,
	//������
	SOURCE_TYPE_2V2_SCORE_WAR = 23001,

	//ʦ��ϵͳ���
	//��ȡʦ���ճ�������
	MASTERSYS_RECV_DAILYTASK_RWD = 24000,
	//��ȡʦ�ųɳ�����
	MASTERSYS_RECV_ACADEMIC_RWD = 24001,
	//ʦ�ų�ʦ����
	MASTERSYS_RECV_FINSCH_RWD = 24002,
	//ʦ����ʦ��ȡ��ʦֵ
	MASTERSYS_ADD_GOODTEACH = 24003,
	//ͽ��������ȡ�ɳ�ֵ
	MASTERSYS_ADDGROWTH_UPLEVEL = 24004,
	//ʦͽ���ͨ�ص��³ǻ�ȡ�ɳ�ֵ 
	MASTERSYS_ADDGROWTH_DUNGEON_END = 24005,
	//ʦ�����ͻ�ȡ�ɳ�ֵ
	MASTERSYS_ADDGROWTH_MASTER_GIVE = 24006,
	//ʦͽ��ϵ���
	MASTERSYS_RELIEVED = 24007,
	//��ʦ�ɹ�
	MASTERSYS_APPRENTICE = 24008,

	SOURCE_TYPE_MAX,
	//����
	SOURCE_TYPE_DIG = 25000,
	//���ؽ���
	SOURCE_TYPE_DIG_ADD_REWARD = 25001,


	// ������
	SOURCE_TYPE_BET_HORSE_REWARD = 26000,
	// ����Ѻע
	SOURCE_TYPE_BET_HORSE_STAKE = 26001,

	// ʱװ�����һ�
	SOURCE_TYPE_CHANGE_FASHION_EXCHANGE = 26002,
	// ʱװ���װ����
	SOURCE_TYPE_CHANGE_FASHION_WELFARE = 26003,
	// �ٱ�ʱװ�ʱװ�ϳ�
	SOURCE_TYPE_CHANGE_FASHION_COMP = 26004,

	// ��������
	SOURCE_PRECIOUSBEAD_UPLEVEL = 26005,

	// ������³���սʤ��
	SOURCE_TYPE_GUILD_DUNGEON_VICTORY = 26006,
	// ������³ǲ��뽱��
	SOURCE_TYPE_GUILD_DUNGEON_ATTEND = 26007,
	
	// ���������ɽ�
	SOURCE_TYPE_ARTIFACT_JAR_LOTTERY = 27003,

	//�������˾��ĳɹ�
	SOURCE_TYPE_BLACKMARKET_AUCTION_SUCC = 28001,
	//�������˾���ʧ��
	SOURCE_TYPE_BLACKMARKET_AUCTION_FAIL = 28002,
	//�����������뾺�Ľ���
	SOURCE_TYPE_BLACKMARKET_AUCTION_REQ = 28003,

	// ð�ն�
	SOURCE_TYPE_ADVENTURE_TEAM = 29000,
	// ð�նӸ���
	SOURCE_TYPE_ADVENTURE_TEAM_RENAME = 29001,
	// ʹ�ô���ף��
	SOURCE_TYPE_USE_INHERIT_BLESS = 29002,
	// ð�ն�Զ��
	SOURCE_TYPE_ADVENTURE_TEAM_EXPEDITION = 29003,

	// �ܳ���Ԩʧ�ܷ�����Ʊ
	SOURCE_TYPE_WEEKHELL_FAIL_RETURN_TICKET = 30001,

	// �¿��ݴ���
	SOURCE_TYPE_MONTH_CARD_DEPOSIT = 30002,
	// �¿��ݴ��䵽��
	SOURCE_TYPE_MONTH_CARD_DEPOSIT_EXPIRE = 30003,

	// �ű�����
	SOURCE_TYPE_TEAM_COPY_FLOP = 30010,
	// �ű��۳�
	SOURCE_TYPE_TEAM_COPY_DEDUCT = 30011,
	// �ű�����
	SOURCE_TYPE_TEAM_COPY_FROZEN = 30012,
	// �ű�Ӷ��ֳ�
	SOURCE_TYPE_TEAM_COPY_COMMISSION_BONUS = 30013,
	// �ű����ķ���
	SOURCE_TYPE_TEAM_COPY_CONSUME_RETURN = 30014,
	// ǿ��ͶƱ��������
	SOURCE_TYPE_TEAM_COPY_FORCE_RETURN = 30015,
	// �ű�roll����
	SOURCE_TYPE_TEAM_COPY_ROLL_REWARD= 30016,

	//ð��ͨ��֤
	SOURCE_TYPE_ADVENTURE_PASS = 30020,
	SOURCE_TYPE_ADVENTURE_PASS_BUY = 30021,
	SOURCE_TYPE_ADVENTURE_PASS_BUY_LEVEL = 30022,
	SOURCE_TYPE_ADVENTURE_PASS_REWARD = 30023,
	
	//������ҩ
	SOURCE_TYPE_FLAY_UP = 30030,

	//���³�roll�������佱��
	SOURCE_DUNGEON_ROLL_ADD_REWARD = 30040,

	//��ս�߻
	SOURCE_CHALLENGE_SCORE = 30050,

	//��������
	SOURCE_ITEM_PRESENT_SCORE = 30051,

	//������ս
	SOURCE_SPRING_SCORE = 30060,

	//��ʿ��ļ
	SOURCE_HIRE_TASK = 30070,

	//�Լ�
	SOURCE_CHIJI_REWARD = 30080,

	//�ھ���
	SOURCE_CHAMPION_ENROLL = 30090,

	//��ʧ����
	//������
	SOURCE_LOSTDUNGEON_OPENBOX = 30100,
	//����ʧ��
	SOURCE_LOSTDUNGEON_FAIL = 30101,
	//����������ȡ����
	SOURCE_LOSTDUNGEON_REWARD = 30102,
	//��������
	SOURCE_LOSTDUNGEON_CONS = 30103,

	//������Ȩ
	SOURCE_GIFT_RIGHT_CARD = 30110,

	//��Ԩ��Ȩ
	SOURCE_DUNGEON_RIGHT_CARD = 30111,

	//��������
	SOURCE_WEAPON_BUILD = 30120,

	//��Ҽ���
	//����
	SOURCE_GOLD_CONSIGNMENT_SELL = 30200,
	//���
	SOURCE_GOLD_CONSIGNMENT_BUY = 30201,
	//���ɽ�
	SOURCE_GOLD_CONSIGNMENT_BUY_DEAL = 30202,
	//����ɽ�
	SOURCE_GOLD_CONSIGNMENT_SELL_DEAL = 30203,
	//��Ҷ���ȡ��
	SOURCE_GOLD_CONSIGNMENT_CANCEL_ORDER = 30205,
	//������ʱ
	SOURCE_GOLD_CONSIGNMENT_TIMEOUT_ORDER = 30206,
	//�ر���������
	SOURCE_GOLD_CONSIGNMENT_CLOSE_ZONE = 30207,
	//�����
	SOURCE_GOLD_CONSIGNMENT_ACTIVITY_END = 30208,
	//�¼ܽ�ɫ����
	SOURCE_GOLD_CONSIGNMENT_CLOSE_ROLE = 30209,
	//�ֶ��¼ܶ���
	SOURCE_GOLD_CONSIGNMENT_ADMIN_CLOSE = 30210,
	//���õ�ȯ��ʱת��
	SOURCE_CREDIT_POINT_TRANSFER= 30211,
	//�쳣��������
	SOURCE_GOLD_CONSIGNMENT_ABNORMAL_REWARD = 30212,
	//����
	SOURCE_MONOPOLY = 30300,
	//���̳���ף����
	SOURCE_MONOPOLY_SELL_CARD = 30301,
	//ͨ�����߻��
	SOURCE_MONOPOLY_ITEM = 30302,
};

enum AssetType
{
	ASSET_INVALID = 0,
	ASSET_GOLD = 1,
	ASSET_POINT,
	ASSET_EXP,
	ASSET_LEVEL,
	ASSET_SP,
	ASSET_WARRIOR_SOUL,
	ASSET_PK_COIN,
	ASSET_REVIVE_COIN,
	ASSET_BIND_GOLD,
	ASSET_BIND_POINT,
	ASSET_GUILD_CONTRI,
	ASSET_VIP_EXP,
	ASSET_VIP_LEVEL,
	ASSET_GOLDJAR_POINT,
	ASSET_MAGJAR_POINT,
	ASSET_FATIGUE,
	ASSET_PVP_SP,
	ASSET_APPOINTMENT_COIN,
	ASSET_MASTER_ACADEMIC_GROWTH,
	ASSET_MASTER_GOODTEACH_VALUE,
	ASSET_CHANGEFASHIN_SCORE,
	ASSET_WEAPON_LEASE_TICKETS,
	//�ؾ������
	ASSET_GNOME_COIN,
	//ð�նӾ���
	ASSET_ADVENTURE_TEAM_EXP,
	//ð�նӵȼ�
	ASSET_ADVENTURE_TEAM_LEVEL,
	//ð�ն�����
	ASSET_ADVENTURE_TEAM_GRADE,
	//��ɫ������
	ASSET_TOTAL_ROLE_SCORE,
	//�͸�ˮ����Ӷ��ѫ�£�
	ASSET_BLESS_CRYSTAL,
	//�͸�ˮ����Ӷ��ѫ�£�����
	ASSET_BLESS_CRYSTAL_EXP,
	//����ף�����ɳ�ҩ����
	ASSET_INHERIT_BLESS,
	//����ף�����ɳ�ҩ��������
	ASSET_INHERIT_BLESS_EXP,
	//�ͽ�
	ASSET_BOUNTY,
	//�Լ���ҫ��
	ASSET_CHI_JI_COIN,
	//�ռ���������1
	ASSET_ZJSL_WZHJJJ_COIN,
	//�ռ���������2
	ASSET_ZJSL_WZHGGG_COIN,
	//�̳ǻ���
	ASSET_MALL_POINT,
	//ð�ձ�
	ASSET_ADVENTURE_PASS_COIN,
	//��ļӲ��
	ASSET_HIRE_COIN,
	//����
	ASSET_HONOR,
	//��ս�߻���
	ASSET_CHALLENGE_SCORE,
	//�ھ���Ӳ��
	ASSET_CAHMPION_COIN,
	//���õ�ȯ
	ASSET_CREDIT_POINT,
	//��������Ӳ��
	ASSET_SECRET_COIN,
	//����Ӳ��
	ASSET_MONOPOLY_COIN,
	//��������
	ASSET_MONOPOLY_ROLL,
	//װ�����ջ���
	ASSET_EQUIP_REC_SCORE,
};

// ���ֹ�������
enum FunctionLogType
{
	// װ��ǿ��
	FUNCTION_EQUIP_STRENGTHEN = 1,
	//��ӽ���
	FUNCTION_RETINUE_UNLOCK,
	//���ϴ��
	FUNCTION_RETINUE_CHANGE_SKILL,
	//�������
	FUNCTION_RETINUE_UPLEVEL,
	//�������
	FUNCTION_RETINUE_UPSTAR,
	//��Ԫʯ����
	FUNCTION_WARPSTONE_UNLOCK,
	//��Ԫʯ����
	FUNCTION_WARPSTONE_CHARGE,
	//������
	FUNCTION_OPEN_JAR,
	//��ħ
	FUNCTION_ADD_MAGIC,
	//����
	FUNCTION_EQUIP_MAKE,
	//�������
	FUNCTION_ACTIVATE_GIFT,
	//װ������
	FUNCTION_EQUIP_ENHANCE,
};

#define REASON_PARAM_COUNT 3
#define REASON_TYPE_OFFSET 0
#define REASON_PARAM_ONE_OFFSET 1
#define REASON_PARAM_TWO_OFFSET 2
#define LOG_SOURCE_TYPE_RATIO	1000

inline std::string GetReason(LogSourceType type = SOURCE_TYPE_INVALID, UInt64 p1 = 0, UInt32 p2 = 0)
{
	char reasonBuff[MAX_LOG_LENGTH] = { 0 };

#ifndef AVALON_OS_WINDOWS
	snprintf(reasonBuff, sizeof(reasonBuff), "%u;%llu;%u", type, p1, p2);
#else
	_snprintf_s(reasonBuff, sizeof(reasonBuff), "%u;%llu;%u", type, p1, p2);
#endif
	reasonBuff[MAX_LOG_LENGTH - 1] = '\0';
	return std::string(reasonBuff);
}

struct ReasonInfo
{
	LogSourceType type;
	UInt64 param1;
	UInt32 param2;
};

inline bool ParseReason(const char* reason, ReasonInfo& param)
{
	std::vector<std::string> tmpVec;
	Avalon::StringUtil::Split(reason, tmpVec, ";");
	if (tmpVec.size() != REASON_PARAM_COUNT)
		return false;
	param.type = (LogSourceType)Avalon::StringUtil::ConvertToValue<UInt32>(tmpVec[REASON_TYPE_OFFSET]);
	param.param1 = Avalon::StringUtil::ConvertToValue<UInt64>(tmpVec[REASON_PARAM_ONE_OFFSET]);
	param.param2 = Avalon::StringUtil::ConvertToValue<UInt32>(tmpVec[REASON_PARAM_TWO_OFFSET]);
	return true;
}

// add by huchenhui
//version|openId|zoneId|pf|did|model|deviceVersion|ip|playerId|accid|level|vipLevel|occu|monthCardTime|createTime|date|bornZoneId|isReturn|roleScore|roleTotalCharge|guildLvl
#define LOG_USERMSG_FORMAT "%s|%s|%u|%s|%s|%s|%s|%s|%llu|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u"

//userMsg|reason|objectId|dataId|itemType|itemSubType|itemName|number|oldNumber|nowNumber|itemQuality|strenth|equipType|enhanceType
#define LOG_ITEM_FORMAT "%s|%s|%llu|%u|%u|%u|%s|%d|%u|%u|%u|%u|%u|%u"

//userMsg|objectId|dataId|itemNum|itemType|itemSubType|quality|strengthen|qualityLevel|itemName|preciousId|preciousQuality|cardId|cardQuality|slot|equipType|magicId|magicLv|inscriptionId1|inscriptionQuality1|inscriptionId2|inscriptionQuality2|needLevel|equipScheme
#define LOG_EQUIP_LOGIN_FORMAT "%s|%llu|%u|%u|%u|%u|%u|%u|%u|%s|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u"

//����
//userMsg|jarId|combo|itemId|itemType|itemQuality|itemName|number
#define LOG_OPEN_JAR_FORMAT "%s|%u|%u|%u|%u|%u|%s|%u"

//userMsg|reason|AssetType|oldAsset|assetNum|nowAssetNum|nowAsset
#define LOG_ASSET_FORMAT "%s|%s|%u|%llu|%lld|%lld|%llu"

//userMsg|userName|OfflineTime|point|bindPoint|gold|bindGold|tower|adventureTeamLevel|adventureTeamGrade|roleCreateTime|honorLvl|honorExp
#define LOG_LOGIN_FORMAT "%s|%s|%llu|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u"

//userMsg|roleFileds
#define LOG_ACCOUNT_LOGIN_FORMAT "%s|%u"

//userMsg|deleteTime
#define LOG_DELETE_FORMAT "%s|%u"

//userMsg|userName|occu|roleId|preOccu|isAppointMentOccu
#define LOG_CREATE_FORMAT "%s|%s|%u|%llu|%u|%u"

//userMsg|onlinetime|point|bindpoint|gold|bindgold|masterType|resistMagic
#define LOG_LOGOUT_FORMAT "%s|%llu|%u|%u|%u|%u|%u|%u"

//userMsg|gameVer|zoneid|timeStr|pf|onlineNum|todayMaxOnlineNum|todayMaxOnlineTime|todayMinOnlineNum|todayMinOnlineTime
#define LOG_PCU_FORMAT "%s|%s|%s|%s|%s|%d|%d|%d|%d|%d"

//userMsg|taskId|type|dailyScore
#define LOG_TASK_SUBMIT_FORMAT "%s|%u|%u|%u"

//userMsg|taskId|type|subType
#define LOG_TASK_ACCEPT_FORMAT "%s|%u|%u|%u"

#define LOG_TASK_ABANDON_FORMAT "%s|%u|%u"

#define LOG_LEVEL_FORMAT "%s|%u"

//userMsg|gameVer|zoneId|time|pf|ip|roleId|accid|level|vipLevel|occu|raceId|dungeonId|dungeonType|dungeonName|hard|hellMode|usedTime|score|reviveCount|isAssist|shopId|isTeam|hpItemUsedNum|hellAutoClose
#define LOG_DUNGEON_FORMAT "%s|%s|%d|%d|%s|%s|%llu|%u|%u|%u|%u|%llu|%u|%u|%s|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u"

// userMsg|dungeonId|dungeonType|dungeonName|hellHardMode|isTeam|isHaveFriend|hellAutoClose
#define LOG_DUNGEON_START_FORMAT "%s|%u|%u|%s|%u|%u|%u|%u"

// userMsg|dungeonId|dungeonType|dungeonName|hellHardMode|areaId|usedTime
#define LOG_DUNGEON_CLEAR_AREA_FORMAT "%s|%u|%u|%s|%u|%u|%u"

// userMsg|raceId|dungeonId|areaId|itemTypeId|isTeam
#define LOG_DUNGEON_USE_POTION_FORMAT "%s|%llu|%u|%u|%u|%u"

// userMsg|dungeonId|itemId|color|itemLevel
#define LOG_DUNGEON_DROP_FORMAT "%s|%u|%u|%u|%u"

// userMsg|functionType|value1|value2
#define LOG_FUNCTION_FORMAT "%s|%u|%d|%d"

// userMsg|skillid|skillName|changeLevel|sourceLevel|newLevel
#define LOG_SKILL_CHANGE_FORMAT "%s|%u|%s|%d|%u|%u"

// userMsg|dungeonId|target
#define LOG_REVIVE_FORMAT "%s|%u|%llu|%u"

//userMsg|auctionType|itemId|itemDataId|itemNum|moneyType|price|deposit|duetime
#define LOG_AUCTION_UP_FORMAT "%s|%u|%llu|%u|%u|%u|%u|%u|%u"

//userMsg|playerId|downType|auctionType|itemId|itemDataId|itemNum
#define LOG_AUCTION_DOWN_FORMAT "%s|%llu|%u|%u|%llu|%u|%u"

//userMsg|auctionPlayerId|auctionType|srcItemId|itemId|auctionMainType|itemDataId|itemNum|moneyType|price|tax|abnormalDeal|strengthen|averPrice|onSealTime|itemQuality|sellerIp|sellerDeviceId|auctionPlayerAccId| auctionPlayerViplev |isTreas|sysRecomPrice|minPriceOnSale|transGuid|isAttented|itemAttentNum
//|mountMagicCardId|mountMagicCardAverPrice|mountBeadId|mountBeadAverPrice|eqQualityLv|remainSealCount|minPriceGuid|minPriceOnsalePrice|minPriceQualityLv|minPriceMountBeadId|minPriceMountBeadAverPrice|minPriceMountMagicId|minPriceMountMagicAverPrice|auctionPrice|auctionTransNum|auctionTransMax
#define LOG_AUCTION_BUY_FORMAT "%s|%llu|%u|%llu|%llu|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%s|%s|%u|%u|%u|%u|%u|%llu|%u|%u|%u|%u|%u|%u|%u|%u|%llu|%u|%u|%u|%u|%u|%u|%u|%u|%u"

//userMsg|gameVer|zoneid|timeStr|pf|transGuid|punishAId|punishBId|punishCId|punishedId|minPriceOnsale|extraProfit|verifyEndTime|programAuditResult|state|isSysRecove
#define LOG_AUCTION_TREAS_TRANSACTION_PUNISH "%s|%s|%u|%u|%s|%llu|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u"

//userMsg|gameVer|zoneid|timeStr|pf|isTreas|itemDataid|itemNum|price
#define LOG_AUCTION_SYSRECOVER "%s|%s|%u|%u|%s|%u|%u|%u|%u"

//userMsg|StoneId|money
#define LOG_WARPSTONE_UNLOCK_FORMAT "%s|%u|%u"

//userMsg|StoneId|incEnergy|energy|oldEnergy|nowEnergy|oldLevel|nowLevel
#define LOG_WARPSTONE_CHARGE_FORMAT "%s|%u|%u|%u|%u|%u|%u|%u"

//userMsg|retinueId|dataId|
#define LOG_RETINUE_UNLOCK_FORMAT "%s|%llu|%u"

//userMsg|retinueId|dataId|retinueLevel
#define LOG_RETINUE_UPLEVEL_FORMAT "%s|%llu|%u|%u"

//userMsg|retinueId|dataId|lockbuff|oldbuff|newbuff
#define LOG_RETINUE_CHANGE_SKILL_FORMAT "%s|%llu|%u|%s|%s|%s"

//userMsg|activeId|taskId
#define LOG_ACTIVE_FINISH_FORMAT "%s|%u|%u"

//userMsg|shopId|shopItemId|num|newNum|dungeonId
#define LOG_SHOP_BUY_FORMAT "%s|%u|%u|%u|%u|%u"

//userMsg|shopId|shopItemId|costNum
#define LOG_SHOP_ITEM_FORMAT "%s|%u|%u|%u"

//userMsg|mallItemId|num
#define LOG_MALL_BUY_FORMAT "%s|%u|%u"

//userMsg|bootId
#define LOG_PLAYER_NEW_BOOT "%s|%u"

//userMsg|itemId|itemDataId|oldStrength|newStrength|code|pack|useUnbreak|isTicker|probability
#define LOG_STRENGTH_EQUIP_FORMAT "%s|%llu|%u|%u|%u|%u|%u|%u|%u|%u"

//userMsg|dungeonId|floor|usedSec
#define LOG_ENHANCE_EQUIP_FORMAT "%s|%llu|%u|%u|%u|%u|%u|%u"

//userMsg|type|itemId|itemDataId|itemLvel|sit|quality|enhanceLv|oldEnhanceType|newEnhanceType
#define LOG_ENHANCE_EQUIP_CHG_FORMAT "%s|%u|%llu|%u|%u|%u|%u|%u|%u|%u"

//userMsg|dungeonId|floor|usedSec
#define LOG_FIRST_CLEAR_DEATH_TOWER "%s|%u|%u|%u"

//userMsg|pvpType
#define LOG_MATCH_START_FORMAT "%s|%u"

//userMsg|zoneId|opponentId|type|seasonLevel|matchScore|usedTime
#define LOG_MATCH_SUCCESS_FORMAT "%s|%u|%llu|%u|%u|%u|%u"

//userMsg|type|seasonLevel|matchScore|matchTime
#define LOG_MATCH_CANCEL_FORMAT "%s|%u|%u|%u|%u"

//userMsg|targetZoneId|targetId|type|result|oldMatchScore|newMatchScore|oldPkValue|newPkValue|oldSeasonLevel|newSeasonLevel|oldSeasonStar|newSeasonStar|oldSeasonExp|newSeasonExp|seasonId|dungeonID|usedTime
#define LOG_PVP_END_FORMAT "%s|%u|%llu|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u"

//userMsg|guildId|guildName|targetId|targetName|type|result|oldGuildScore|newGuildScore
#define LOG_GUILD_PVP_END_FORMAT "%s|%llu|%s|%llu|%s|%u|%u|%u|%u|%u"

//userMsg|level|passType
#define LOG_ADVENTURE_PASS_FORMAT "%s|%u|%u"

//userMsg|gameVer|zoneId|channge|time|roleId|accid|level|vipLevel|orderId|mallType|goodsId|itemId|itemNum|vipScore|money|timeoutTime
#define LOG_PAY_START_FORMAT "%s|%s|%u|%s|%u|%llu|%u|%u|%u|%s|%u|%u|%u|%u|%u|%u|%u"

//userMsg|gameVer|zoneId|time|roleId|accid|level|vipLevel|orderId|mallType|goodsId|itemId|itemNum|vipScore|money|timeoutTime|result
//#define LOG_PAY_END_FORMAT "%s|%s|%u|%u|%llu|%u|%u|%u|%s|%u|%u|%u|%u|%u|%u|%u|%u"

//userMsg|orderId|mallType|goodsId|itemId|itemNum|vipScore|money|time|timeoutTime
#define LOG_PAY_END_FORMAT "%s|%s|%u|%u|%u|%u|%u|%u|%u|%u"

//userMsg|itemSubType
#define LOG_WELFARE_CARD_FORMAT "%s|%u"

//�ϳɸ�ħ��
//userMsg|card1|card2|targetCard|code
#define LOG_COMPOSE_MAGIC_CARD_FORMAT "%s|%u|%u|%u|%u"

//�ϳ�ʱװ
//userMsg|fashion1|fashion2|commonFashion|skyFashion
#define LOG_COMPOSE_FASHION_FORMAT "%s|%u|%u|%u|%u"

//Ʒ�׵���
//userMsg|itemId|itemDataId|oldLevel|nowLevel|oldMatNum|newMatNum|oldPoint|newPoint|isFrenzy|eqColor|eqPost
#define LOG_REMAKE_EQUIP_LEVEL_FORMAT "%s|%llu|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u"

//����
//#define 

//תְ�¼�
//userMsg|mainOccu|preOccu|Occu
#define LOG_CHANGE_OCCU_FORMAT "%s|%u|%u|%u"

//�����½
//userMsg|objId|dataId|petLevel|petExp|petType|slot
#define LOG_PET_LOGIN_FORMAT "%s|%llu|%u|%u|%u|%u|%u"

//���ӳ���
//userMsg|objId|dataId|itemId|petType|petQuality
#define LOG_PET_ADD_FORMAT "%s|%llu|%u|%u|%u|%u"

//�������
//userMsg|objId|dataId|oldPetId|oldDataId
#define LOG_PET_BATTLE_FORMAT "%s|%llu|%u|%llu|%u"

//���ɳ���
//userMsg|objId|dataId|oldLevel|newLevel|oldExp|newExp|exp|beDevourObjId|beDevourDataId|beDevourPetLevel|beDevourPetExp
#define LOG_PET_DEVOUR_FORMAT "%s|%llu|%u|%u|%u|%u|%u|%u|%llu|%u|%u|%u"

//����������ѡ
//userMsg|objId|dataId|oldIndex|newIndex
#define LOG_PET_CHANGE_SKILL_FORMAT "%s|%llu|%u|%u|%u"

//��������
//userMsg|operationType|roomId|roomType|reason|createTime|playerSize
#define LOG_ROOM_FORMAT "%s|%u|%u|%u|%s|%u|%u"

//�ϱ�ʵʱ��������
//userMsg|type|voiceRoomId|joinTimeStamp
#define LOG_LIVE_VOICE_DATA_FORMAT "%s|%u|%s|%s"

//�ϱ�������������
//userMsg|type|voiceId
#define LOG_CHAT_VOICE_DATA_FORMAT "%s|%u|%s"

//��ɢ����
//userMsg|roomId|roomType|deleteTime
//#define LOG_ROOM_CREATE_FORMAT "%s|%u|%u|%u"

// ����֮����ʼɨ��
//userMsg|startFloor|endFloor
#define LOG_TOWER_START_WIPEOUT_FORMAT "%s|%u|%u"

// ����֮������
//userMsg|topFloor
#define LOG_TOWER_RESET_FORMAT "%s|%u"

//userMsg|gameVer|zoneid|timeStr|pf|monster_guid|monster_id|type|dungeon_id|scene_id|total_times|owner
#define LOG_CITY_MONSTER_FORMAT "%s|%s|%u|%u|%s|%llu|%u|%u|%u|%u|%u|%llu"

//��ʦ/��ͽ�ɹ�
//userMsg|discipleId|masterId|discipleLevel|masterLevel|discipleViplev|masterViplev|timestamp|eventType|masterOccu|discipleOccu
#define LOG_MASTERSYS_MAKE_MASTERDISCIPLE  "%s|%llu|%llu|%u|%u|%u|%u|%u|%u|%u|%u"

//ʦ�ų�ʦ
//userMsg|type|discipleId|masterId|discipleLevel|masterLevel|discipleGrowth|apprentTimeSec|finishTimeSec
#define LOG_MASTERSYS_FINISH_SCHOOL "%s|%u|%llu|%llu|%u|%u|%u|%u|%u"

//ʦ������ʦ������
//userMsg|discipleId|masterId|timestamp
#define LOG_MASTERSYS_PUBISH_DAILYTASK "%s|%llu|%llu|%u"

//ͽ�����ʦ������
//userMsg|discipleId|masterId|timestamp
#define LOG_MASTERSYS_COMP_DAILYTASK "%s|%llu|%llu|%u"

//��ȡʦ���ճ��������
//userMsg|type|uid|timestamp
#define LOG_MASTERSYS_RECEIVE_DAILYTASKGIFT "%s|%u|%llu|%u"

//ʦͽ��������
//userMsg|discipleId|masterId|dungeonId|timestamp
#define LOG_MASTERSYS_INTERACTION "%s|%llu|%llu|%u|%u"

// �������������
//userMsg|courtId|shooterId|timestamp
#define LOG_BET_HORSE_SHOOTER "%s|%u|%u|%u"

// װ����
//userMsg|opType
#define LOG_SECURITY_LOCK "%s|%u"

// �������
//userMsg|reason|raceId|dungeonId
#define LOG_PLAYER_CHEAT "%s|%u|%llu|%u"

// ���ḱ��
//userMsg|dungeonId|dungeonName|dungeonLvl|score|totalDamage|bossDamage|usetime|reviveCnt|isTeam|guildName|guildLvl|bossOddBlood
#define LOG_GUILD_DUNGEON "%s|%u|%s|%u|%u|%lu|%lu|%u|%u|%u|%s|%u|%lu"

// ����
//userMsg|beadOpType|objectId|itemId|srcBeadId|newBeadId|costItemId|costItemNum|isSuccess|param1|param2
#define LOG_PRECIOUS_BEAD "%s|%u|%llu|%u|%u|%u|%u|%u|%u|%u|%u"

// ��Ʒ����
//userMsg|itemId|oldAdventurerEmblem|oddAdventurerEmblem|costAdventurerEmblem
#define LOG_ITEM_LEASE "%s|%u|%u|%u|%u"

// �����Ƽ�
//userMsg|skillConfigType
#define LOG_SKILL_RECOMMEND "%s|%u"

// ���ܲ�λ
//userMsg|skillConfigType|slotNum
#define LOG_SKILL_SLOT "%s|%u|%u"

// �������˻��յ�����Ϣ
// userMsg|gameVer|zoneid|timeStr|pf|auctId|auctType|itemDataId|recovePrice|recoveNum
#define LOG_BLACKMARKET_RECOVE_AUCTION_INFO "%s|%s|%u|%u|%s|%llu|%u|%u|%u|%u"

// �����������ջ�����Ϣ
// userMsg|auctId|auctType|itemDataId|palyerId|itemId|recovedPrice
#define LOG_BLACKMARKET_RECOVED_INFO "%s|%llu|%u|%u|%llu|%llu|%u"

// �������˽�����Ϣ
//  userMsg|opType|transid|auctId|auctType|auctItemDataId|auctItemId|auctPrice
enum BlackMarketOpType
{
	BMOT_REQ = 1,      //���뽻��
	BMOT_CANCEL = 2,   //ȡ������
};
#define LOG_BLACKMARKET_TRANSCATION "%s|%u|%llu|%llu|%u|%u|%llu|%u"

// ð�նӸ�����־
//  userMsg|oldName|newName
#define LOG_ADVENTURE_TEAM_NAME_CHANGE "%s|%s|%s"

// ð�նӻ�����Ϣ
// userMgr|adventureTeamLevel|adventureTeamName|blessCrystalNum|inheritBlessNum|bountyNum
#define LOG_ADVENTURE_TEAM_INFO "%s|%u|%s|%u|%u|%u|%u|%u"

// ��ǲ(ȡ��)Զ����־(falg: 1:��ǲ��2:ȡ��)
//  userMsg|flag|mapId|duration|memberNum
#define LOG_EXPEDITION_TEAM "%s|%u|%u|%u|%u"

// ��ȡԶ��������־
//  userMsg|mapId|duration|cond1|cond2|cond3|cond4|cond5
#define LOG_GET_EXPEDITION_TEAM_REWARDS "%s|%u|%u|%u|%u|%u|%u|%u"

// �����������ۿ���־
//  userMsg|opActId|discountRate|effectTimes
#define LOG_EXTRACT_ARTIFACT_JAR_DISCOUNT "%s|%u|%u|%u"

// �����Ա��½
//  userMsg|GuildId|GuildName|GuildLevel|GuildMemberNum|EmblemLevel
#define LOG_GUILD_MEMBER_LOGIN "%s|%llu|%s|%u|%u|%u"

enum 
{
	UDP_GUILD_AUCTION_CREATE = 1,	// ������������
	UDP_GUILD_AUCTION_ABORATION = 2,	// ������������
};
// ������������
//  userMsg|gameVer|zoneId|time|pf|opType|auctionType|guildId|guildLvl|guid|rewardGroup|itemId|itemNum
// ������������
//  userMsg|gameVer|zoneId|time|pf|opType|auctionType|guildId|guildLvl|guid|rewardGroup|itemId|itemNum
#define LOG_GUILD_AUCTION "%s|%s|%d|%d|%s|%u|%u|%lu|%u|%lu|%u|%u|%u"

// ������������
//  userMsg|auctionType|guildId|guildLvl|guid|rewardGroup|itemId|itemNum|bidPrice|bidNum
#define LOG_GUILD_AUCTION_BID "%s|%u|%lu|%u|%lu|%u|%u|%u|%u|%u"

// ���������ɽ�(dealType, 0������, 1һ�ڼ�)
//  userMsg|gameVer|zoneId|time|pf|auctionType|guildId|guildLvl|guid|rewardGroup|itemId|itemNum|bidPrice|bidNum|dealType|playerId|vipLvl|level|Occu
#define LOG_GUILD_AUCTION_DEAL "%s|%s|%d|%d|%s|%u|%lu|%u|%lu|%u|%u|%u|%u|%u|%u|%lu|%u|%u|%u"

enum 
{
	UDP_HEAD_FRAME_LOGIN = 1,	// ��¼ͷ���
	UDP_HEAD_FRAME_USE = 2,	// ����ͷ���
};
// ͷ���
//  userMsg|opType|headFrameId
#define LOG_HEAD_FRAME "%s|%u|%u"

// ��ֵ����
//  userMsg|pushId|price
#define LOG_RECHARGE_PUSH "%s|%u|%u"

enum
{
	UDP_ITEM_DEPOSIT_OBTAIN = 1,	// �ݴ�
	UDP_ITEM_DEPOSIT_GET = 2,	// ��ȡ
	UDP_ITEM_DEPOSIT_EXPIRE = 3,	// ����
	UDP_ITEM_DEPOSIT_REPLACE = 4,	// �滻
};
// ��Ʒ�ݴ���
//  userMsg|opType|itemId|itemNum|itemColor|dungeonId
#define LOG_ITEM_DEPOSIT "%s|%u|%u|%u|%u|%u"

//	��ħ������
//	userMsg|isSuccess|oldId|oldLv|curId|curLv|consumeId|consumeNum|oldColor
#define LOG_ITEM_UP_MAGIC "%s|%u|%u|%u|%u|%u|%u|%u|%u"
// �Լ�����
//  userMsg|rank|survivalTime|reason
#define LOG_CHIJI_BALANCE "%s|%u|%u|%u"
// �Լ�����
// userMsg|battleId|playerId|boxId|param
#define LOG_CHIJI_BOX "%s|%u|%llu|%u|%u"
// �Լ�����
// userMsg|battleId|playerId|param
#define LOG_CHIJI_TRAPS "%s|%u|%llu|%u"
// �Լ���ɫ
// userMsg|battleId|playerId|occu
#define LOG_CHIJI_OCCU "%s|%u|%llu|%u"

enum
{
	CHIJI_NPC_CREATE = 1, // ˢnpc
	CHIJI_NPC_EXCHANGE = 2, // �һ�(��ս)
};

// �Լ�npcˢ��
// userMsg|battleId|npcType|opType|createNum
// �Լ�npc�һ�
// userMsg|battleId|npcType|opType|playerId|itemId|param
#define LOG_CHIJI_NPC "%s|%u|%u|%u|%llu|%u|%u"

// �Լ�����
// userMsg|battleId|playerId|skillId|skillLvl
#define LOG_CHIJI_SKILL "%s|%u|%llu|%u|%u"

// �Լ���ս��
// userMsg|battleId|playerId|oddNum|isUse
#define LOG_CHIJI_NO_WAR_CARD "%s|%u|%llu|%u|%u"

enum  GuildRedPacketOpType
{
	GUILD_REDPACKET_SEND = 1,  //�������
	GUILD_REDPACKET_RECV = 2,  //��ȡ���
};

// ������
//  userMsg|redpacketId|opType|redpacketType|money|num|receiveState|senderId
#define LOG_GUILD_REDPACKET "%s|%llu|%u|%u|%u|%u|%u|%llu"

// �ռ���������
// userMsg|floor|topFloor|usedTime|score|dungeonId|dungeonBuff|inspireBuff
#define LOG_ZJSL_TOWER "%s|%u|%u|%u|%u|%u|%u|%u"

// ���³�ͨ��ʱ��
// userMsg|dungeonId|score|time|roleValueScore|dungeonValueScore
#define LOG_DUNGEON_CLEAR_TIME "%s|%u|%u|%u|%u|%u"


enum GuildMergerResult
{
	GUILD_MERGER_RESULT_CANCEL       = 0,
	GUILD_MERGER_RESULT_ACCEPT       = 1,
	GUILD_MERGER_RESULT_REFUSE       = 2,
	GUILD_MERGER_RESULT_SUCCESS      = 3,
	GUILD_MERGER_RESULT_SEND         = 4,
	GUILD_MERGER_RESULT_DISMISS_BY   = 5,
	GUILD_MERGER_RESULT_DISMISS_FAILD= 6,
	GUILD_MERGER_RESULT_DISMISS_YES  = 7,
	GUILD_MERGER_RESULT_DISMISS_MERGER=8,
};

//����沢
// userMsg|gameVer|zoneId|time|pf|guildID|guildName|guileLevel|memberNum|mermberLoginNum|LastWeekFund|result
#define LOG_GUILD_MERGER "%s|%s|%d|%d|%s|%llu|%s|%d|%d|%d|%d|%d"

//����ǩ��
// userMsg|signDays|repairSignDays|AwardRemainReapirNum|ActiviteRemainReapirNum
#define LOG_NEW_SIGN "%s|%u|%u|%u|%u"

//���������˳�
// userMsg|opType
#define LOG_GUILD_JOIN_EXIT "%s|%u"

// �ͻ������
//userMsg|name|param1|param2|param3
#define LOG_CLIENT_LOG "%s|%s|%s|%s|%s"

// ��װ����
// userMsg|suitId|suitNum
#define LOG_SUIT_WEAR "%s|%u|%u"

// �ű���ս
//userMsg|post|isGold|isOpenGold|teamId|model|playerNum|difficulty|preWarCfg|commission|equipScore|teamGrade|teamType
#define LOG_TEAM_COPY_START_BATTLE "%s|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u"

// �ű��ݵ����
// userMsg|teamId|squadId|stageId|fieldId|dungeonId|costTime|result|teamGrade|teamType
#define LOG_TEAM_COPY_FIELD_SETTLE "%s|%u|%u|%u|%u|%u|%u|%u|%u|%u"

// �ű��׶ν���
// userMsg|teamId|stageId|costTime|result|teamGrade|teamType|changeSquadCnt|moveCnt|bossType|mainCityBlood
#define LOG_TEAM_COPY_STAGE_END "%s|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u"

// �ű�����
// userMsg|teamId|stageId|rewardId|rewardNum|limitType|teamGrade|teamType|param1
#define LOG_TEAM_COPY_FLOP "%s|%u|%u|%u|%u|%u|%u|%u|%u"

// �ű�ǿ�ƽ���
// userMsg|isAgree|teamId|forceQuitNum|agreeNum|disAgreeNum|waiverNum|oddNum|isSuccess|oddTime|teamGrade|teamType
#define LOG_TEAM_FORCE_END "%s|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u"

// roll������Ϣ
// userMsg|dungeonId|rollIndex|rollItem|rollItenNum|rollType|point
#define LOG_DUNGEON_ROLL "%s|%u|%u|%u|%u|%u|%u"

//��Ӻ���
//userMsg|guildId|tlevel|tvipLevel|toccu|tguildId
#define LOG_ADD_FRIEND  "%s|%llu|%u|%u|%u|%llu"

//���ҿ��
//userMsg|gold|point|pkCoin|reviveCoin|bindGold|bindPoint|guildContri|vipExp|magjarPoint
#define LOG_MONEY_STOCK "%s|%u|%u|%u|%u|%u|%u|%u|%u|%u"

//���ҿ�棨�˺ţ�
//userMsg|gnomePoint
#define LOG_MONEY_STOCK_ACC "%s|%u"

//װ��ת��
//userMsg|epicEquipId|equipDataId|part|equipType|strLev|beadId|magicId|inscriptId|equipNewDataId|newPart|convertType
#define LOG_EQUIP_CONVERT "%s|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u"

//�ű�����
//userMsg|cheatNum|forbidTime
#define LOG_TEAM_COPY_CHEAT "%s|%u|%u"

enum 
{
	ITEM_THING_START = 1,	// ��ʼ����
	ITEM_THING_CONMIT = 2,	// ��ʼ����
	ITEM_THING_ROLL_BACK= 3,	// ��ʼ����
	ITEM_THING_CANCEL = 4,	// ��ʼȡ��
};
//��������
//userMsg|opType|thingId|serialId|reason|itemCost
#define LOG_ITEM_THING "%s|%u|%llu|%llu|%s|%s"

//userMsg|isPreferences|roleLocation
#define LOG_ROLE_PREFERENCES "%s|%u|%u"
#endif
