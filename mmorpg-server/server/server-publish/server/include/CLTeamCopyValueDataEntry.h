/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_TEAMCOPYVALUE_DATA_ENTRY_H__
#define __CL_TEAMCOPYVALUE_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

enum TeamCopyValueType
{
	TC_VALUE_TEAM_CAPACITY = 1,			// 团本团队容量
	TC_VALUE_SQUAD_CAPACITY = 2,		// 团本小队容量
	TC_VALUE_SQUAD_NUM = 3,				// 团本小队数量
	TC_VALUE_PAGE_NUM = 4,				// 每页团队数量
	TC_VALUE_APPLY_LIST_MAX = 5,		// 申请列表数量
	TC_VALUE_PREPARE_SCENE_ID = 6,		// 准备场景ID
	TC_VALUE_ATTACK_SCENE_ID = 7,		// 攻坚场景ID
	TC_VALUE_VOTE_TIME = 8,				// 投票时间
	TC_VALUE_COMMISSION_TAX = 9,		// 佣金税率(千分比)
	TC_VALUE_AUTO_FLOP_TIME = 10,		// 自动翻牌时间
	TC_VALUE_CHECK_COND_TIME = 11,		// 玩家条件检查超时时间
	TC_VALUE_COMMISSION_ID = 12,		// 佣金id
	TC_VALUE_ONE_GAME_TIME = 13,		// 一局游戏时间
	TC_VALUE_PUSH_PLAYER_NUM = 14,		// 推送玩家数
	TC_VALUE_COMMON_TAIL_FIELD_ID = 15,		// 普通关卡尾部据点
	TC_VALUE_FILTER_MONSTER_ID = 16,	// 过滤怪物Id

	TC_VALUE_TICKET_ID_1 = 17,		// 门票1
	TC_VALUE_TICKET_NUM_1 = 18,		// 门票1数量
	TC_VALUE_TICKET_ID_2 = 19,		// 门票2
	TC_VALUE_TICKET_NUM_2 = 20,		// 门票2数量

	TC_VALUE_GOLD_MAX_NUM = 21,			// 金主最大数
	TC_VALUE_DAY_NUM = 22,				// 普通日挑战次数
	TC_VALUE_WEEK_NUM = 23,				// 普通周挑战次数
	TC_VALUE_WEEK_SCORE_LIMIT = 24,		// 周积分限制
	TC_VALUE_GOLD_MODEL_COND_1 = 25,	// 金团开启条件一
	TC_VALUE_GOLD_MODEL_COND_2 = 26,	// 金团开启条件二

	TC_VALUE_DAY_FREE_QUIT_NUM = 28,	// 日免费退队次数
	TC_VALUE_WEEK_FREE_QUIT_NUM = 29,	// 周免费退队次数

	TC_VALUE_RECRUIT_CD = 30,	// 招募CD时间

	TC_VALUE_FORCE_END_QUIT_NUM = 31,	// 强制结束退队人数
	TC_VALUE_FORCE_END_GAME_TIME = 32,	// 强制结束剩余挑战时间(秒)
	TC_VALUE_FORCE_END_VOTE_TIME = 33,	// 强制结束投票时间(秒)
	TC_VALUE_FORCE_END_COOL_TIME = 34,	// 强制结束冷却时间(秒)

	TC_VALUE_DIFF_DAY_NUM = 35,		// 噩梦日挑战次数
	TC_VALUE_DIFF_WEEK_NUM = 36,	// 噩梦周挑战次数
	TC_VALUE_DIFF_UNLOCK_COND = 37,	// 噩梦解锁条件

	TC_VALUE_DIFF_ENERGY_REVIVE_FIELD = 38,	// 噩梦能量恢复据点
	TC_VALUE_DIFF_ENERGY_REVIVE_ENTER_RATE = 39,	// 噩梦能量恢复据点进入比例
	TC_VALUE_DIFF_ENERGY_REVIVE_CLEAR_REDUCE_RATE = 40,	// 噩梦能量恢复据点通关缩减比例
	TC_VALUE_DIFF_ENERGY_REVIVE_BUFF_ONE_RATE = 41,	// 噩梦能量恢复据点Buff一倍比例
	TC_VALUE_DIFF_ENERGY_REVIVE_BUFF_TWO_RATE = 42,	// 噩梦能量恢复据点Buff二倍比例

	TC_VALUE_DIFF_TAIL_FIELD_ID = 43,		// 噩梦关卡尾部据点

	// 团本2

	TC_VALUE_2_TEAM_CAPACITY = 201,		// 团本2团队容量
	TC_VALUE_2_SQUAD_CAPACITY = 202,	// 团本2小队容量
	TC_VALUE_2_SQUAD_NUM = 203,			// 团本2小队数量
	TC_VALUE_2_TICKET_ID_1 = 204,		// 门票1
	TC_VALUE_2_TICKET_NUM_1 = 205,		// 门票1数量
	TC_VALUE_2_TICKET_ID_2 = 206,		// 门票2
	TC_VALUE_2_TICKET_NUM_2 = 207,		// 门票2数量
	TC_VALUE_2_PREPARE_SCENE_ID = 208,		// 准备场景ID
	TC_VALUE_2_ATTACK_SCENE_ID = 209,		// 攻坚场景ID
	TC_VALUE_2_GOLD_MAX_NUM = 210,			// 金主最大数
	TC_VALUE_2_DAY_NUM = 211,				// 普通日挑战次数
	TC_VALUE_2_WEEK_NUM = 212,				// 普通周挑战次数
	TC_VALUE_2_GOLD_MODEL_COND_1 = 213,	// 金团开启条件一
	TC_VALUE_2_GOLD_MODEL_COND_2 = 214,	// 金团开启条件二
	TC_VALUE_2_DAY_FREE_QUIT_NUM = 215,	// 日免费退队次数
	TC_VALUE_2_WEEK_FREE_QUIT_NUM = 216,	// 周免费退队次数
	TC_VALUE_2_MONSTER_FIELD_ID = 217,	// 团本怪物据点
	TC_VALUE_2_SQUAD_MOVE_CD = 218,	// 小队移动CD
	TC_VALUE_2_CHANGE_CAPTAIN_TIME = 219,	// 更换队长时间
	TC_VALUE_2_DIFF_DAY_NUM = 220,		// 噩梦日挑战次数
	TC_VALUE_2_DIFF_WEEK_NUM = 221,	// 噩梦周挑战次数
	TC_VALUE_2_DIFF_UNLOCK_COND = 222,	// 噩梦解锁条件

	TC_VALUE_ROLL_TIME = 225,	// roll时间
};

// __CUSTOM_STRUCT_END__

class TeamCopyValueDataEntry : public Avalon::DataEntry
{
public:
	TeamCopyValueDataEntry();
	virtual ~TeamCopyValueDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 名字
	std::string                                     name;
	// 数值
	UInt32                                          value;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class TeamCopyValueDataEntryMgr : public Avalon::DataEntryMgrBase<TeamCopyValueDataEntry>
	, public Avalon::Singleton<TeamCopyValueDataEntryMgr>
{
public:
	virtual bool AddEntry(TeamCopyValueDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	void OnReload();

	UInt32 GetTeamCopyValue(UInt32 type);

private:

	std::map<UInt32, UInt32> m_ValueMap;
// __CUSTOM_MGR_END__
};

#endif
