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
	*@brief 比赛最多持续时间(s)
	*/
	virtual UInt32 GetRaceMaxTime() { return 40 * 60; }

protected:
	/**
	*@brief 检测结算合法性
	*/
	virtual void CheckRaceEnd(EGameSessionDismissType reason);

	/**
	*@brief 玩家断线重连超时
	*/
	virtual void OnGamerReconnectTimeout(ObjID_t id);

protected:
	/**
	*@brief 是否需要验证
	*/
	virtual bool _IsNeedVerify() { return true; }

	/**
	*@brief 是否需要解散
	*/
	virtual bool _IsNeedDismiss();

	/**
	*@brief 根据座位号获取阵营
	*/
	RoomSlotGroup _GetCampBySeat(UInt8 seat);

	/**
	*@brief 获取指定阵营的玩家数
	*/
	UInt32 _GetGamerNumByCamp(RoomSlotGroup camp, bool mustOnline = true);

	/**
	*@brief 获取录像信息
	*/
	virtual bool _GetReplayInfo(ReplayInfo& info);

	/**
	*@brief 检测到不同步时是否结束战斗
	*/
	virtual bool _IsDismissWhenUnsync() { return true; }

private:
	
};

#endif