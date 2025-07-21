#ifndef __PK_ROOM_MATCH_RACE_H__
#define __PK_ROOM_MATCH_RACE_H__

#include "PvPMatchRace.h"

/*
	PK����
*/

class PkRoomMatchRace : public PvPMatchRace
{
public:
	PkRoomMatchRace(PkType pkType);
	virtual ~PkRoomMatchRace();

public:
	virtual bool OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd);

protected:
	/**
	*@brief ��ȡ�������
	*/
	virtual UInt32 _GetRoomScore(std::vector<UInt32> scores);

	/**
	*@brief ����ս�����������ֱ仯
	*/
	Int32 _CountMatchScoreChange(UInt32 score, UInt32 opponeScore, MatchRaceResult result);

protected:
	PkType				m_PkType;
};


#endif