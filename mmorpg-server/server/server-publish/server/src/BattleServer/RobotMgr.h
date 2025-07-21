#ifndef __ROBOT_MGR_H__
#define __ROBOT_MGR_H__

#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <CLMatchDefine.h>

// �����������Ѷ�
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
	*@brief �������װ��ǿ���ȼ�
	*/
	void GenRobotEquipStrengthen(Robot* robot);

	/**
	*@brief ְҵ
	*/
	UInt8 GetSepcifyRobotOccu() const { return m_SpecifyRobotOccu; }
	void SetSepcifyRobotOccu(UInt8 occu) { m_SpecifyRobotOccu = occu; }

	/**
	*@brief �����Ѷ�
	*/
	RobotAttrHardType GetSepcifyRobotHard() const { return m_SpecifyRobotHard; }
	void SetSepcifyRobotHard(RobotAttrHardType hard) { m_SpecifyRobotHard = hard; }

	/**
	*@brief ai�Ѷ�
	*/
	RobotAIHardType GetSepcifyRobotAI() const { return m_SpecifyRobotAI; }
	void SetSepcifyRobotAI(RobotAIHardType ai) { m_SpecifyRobotAI = ai; }

private:
	/**
	*@brief ����װ����Ϣ
	*/
	RaceEquip _GenEquip(UInt32 itemId);

private:
	// ���ȼ��Ļ�������Ϣ
	//std::vector<Robot*>			m_Robots[100];
	std::map<RobotKey, std::vector<Robot*>>		m_Robots;

	// ָ������������
	// ְҵ
	UInt8										m_SpecifyRobotOccu;
	// �����Ѷ�
	RobotAttrHardType							m_SpecifyRobotHard;
	// AI�Ѷ�
	RobotAIHardType								m_SpecifyRobotAI;
};

#endif