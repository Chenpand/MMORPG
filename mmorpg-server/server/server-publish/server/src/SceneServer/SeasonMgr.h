#ifndef _SCENE_SEASON_MGR_H_
#define _SCENE_SEASON_MGR_H_

#include <AvalonSingleton.h>
#include <CLGameDefine.h>

class SeasonMgr : public Avalon::Singleton<SeasonMgr>
{
public:
	SeasonMgr() :m_SeasonId(0), m_StartTime(0), m_EndTime(0), m_SeasonAttrStartTime(0), m_SeasonAttrEndTime(0){};

public:
	UInt32 GetSeasonId() { return m_SeasonId; }
	void SetSeasonId(UInt32 seasonId) { m_SeasonId = seasonId; }

	UInt32 GetStartTime() { return m_StartTime; }
	void SetStartTime(UInt32 startTime) { m_StartTime = startTime; }

	UInt32 GetEndTime() { return m_EndTime; }
	void SetEndTime(UInt32 endTime) { m_EndTime = endTime; }

	UInt32 GetSeasonAttrStartTime() { return m_SeasonAttrStartTime; }

	UInt32 GetSeasonAttrEndTime() { return m_SeasonAttrEndTime; }

public:
	bool ChangeSeasonAttr(UInt32 seasonAttrStartTime, UInt32 seasonAttrEndTime, std::vector<UInt64> playerIds);

	void UpdateSeasonInfo();

private:
	//����id
	UInt32 m_SeasonId;

	//��ʼʱ��
	UInt32 m_StartTime;

	//����ʱ��
	UInt32 m_EndTime;

	//�������Կ�ʼʱ��
	UInt32 m_SeasonAttrStartTime;

	//�������Խ���ʱ��
	UInt32 m_SeasonAttrEndTime;
};

#endif //_SCENE_SEASON_MGR_H_