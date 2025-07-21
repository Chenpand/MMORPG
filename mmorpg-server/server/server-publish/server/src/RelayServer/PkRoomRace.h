#ifndef __PK_ROOM_RACE_H__
#define __PK_ROOM_RACE_H__

#include <CLRoomDefine.h>
#include "PvPRace.h"

class PkRoomRace : public PvPRace
{
public:
	AVALON_DEFINE_OBJECTPOOL(PvPRace, Avalon::NullMutex)
public:
	PkRoomRace(RaceType raceType);
	virtual ~PkRoomRace();

	/**
	*@brief ����������ʱ��(s)
	*/
	virtual UInt32 GetRaceMaxTime() { return 40 * 60; }

protected:
	/**
	*@brief ������Ϸ���
	*/
	virtual void CheckRaceEnd(EGameSessionDismissType reason);

	/**
	*@brief ��Ҷ���������ʱ
	*/
	virtual void OnGamerReconnectTimeout(ObjID_t id);

protected:
	/**
	*@brief �Ƿ���Ҫ��֤
	*/
	virtual bool _IsNeedVerify() { return true; }

	/**
	*@brief �Ƿ���Ҫ��ɢ
	*/
	virtual bool _IsNeedDismiss();

	/**
	*@brief ������λ�Ż�ȡ��Ӫ
	*/
	RoomSlotGroup _GetCampBySeat(UInt8 seat);

	/**
	*@brief ��ȡָ����Ӫ�������
	*/
	UInt32 _GetGamerNumByCamp(RoomSlotGroup camp, bool mustOnline = true);

	/**
	*@brief ��ȡ¼����Ϣ
	*/
	virtual bool _GetReplayInfo(ReplayInfo& info);

	/**
	*@brief ��⵽��ͬ��ʱ�Ƿ����ս��
	*/
	virtual bool _IsDismissWhenUnsync() { return true; }

private:
	
};

#endif