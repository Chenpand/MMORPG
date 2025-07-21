#ifndef __ROBOT_MGR_H__
#define __ROBOT_MGR_H__

#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <CLMatchDefine.h>

// 机器人属性难度
enum RobotAttrHardType
{
	ROBOT_ATTR_INVALID,
	ROBOT_ATTR_EASY,
	ROBOT_ATTR_NORMAL,
	ROBOT_ATTR_HARD,
};

struct Robot
{
	RobotAttrHardType	hardType;
	RacePlayerInfo		raceInfo;
};

struct RobotKey
{
	bool operator<(const RobotKey& key) const
	{
		if (hardType != key.hardType)
		{
			return hardType < key.hardType;
		}

		return level < key.level;
	}

	RobotAttrHardType	hardType;
	UInt16				level;
};

class RobotMgr : public Avalon::Singleton<RobotMgr>
{
public:
	RobotMgr();

	bool Init();

	Robot* GenRobot(RobotAttrHardType hardType, UInt16 level);

	/**
	*@brief 随机生成装备强化等级
	*/
	void GenRobotEquipStrengthen(Robot* robot);

	/**
	*@brief 职业
	*/
	UInt8 GetSepcifyRobotOccu() const { return m_SpecifyRobotOccu; }
	void SetSepcifyRobotOccu(UInt8 occu) { m_SpecifyRobotOccu = occu; }

	/**
	*@brief 属性难度
	*/
	RobotAttrHardType GetSepcifyRobotHard() const { return m_SpecifyRobotHard; }
	void SetSepcifyRobotHard(RobotAttrHardType hard) { m_SpecifyRobotHard = hard; }

	/**
	*@brief ai难度
	*/
	RobotAIHardType GetSepcifyRobotAI() const { return m_SpecifyRobotAI; }
	void SetSepcifyRobotAI(RobotAIHardType ai) { m_SpecifyRobotAI = ai; }

private:
	/**
	*@brief 生成装备信息
	*/
	RaceEquip _GenEquip(UInt32 itemId);

private:
	// 各等级的机器人信息
	//std::vector<Robot*>			m_Robots[100];
	std::map<RobotKey, std::vector<Robot*>>		m_Robots;

	// 指定机器人属性
	// 职业
	UInt8										m_SpecifyRobotOccu;
	// 属性难度
	RobotAttrHardType							m_SpecifyRobotHard;
	// AI难度
	RobotAIHardType								m_SpecifyRobotAI;
};

#endif