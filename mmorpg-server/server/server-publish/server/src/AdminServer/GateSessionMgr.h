#ifndef _GATESESSION_MGR_H_
#define _GATESESSION_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonTime.h>
#include <CLObjectIndex.h>
#include "GateSession.h"

typedef CLVisitor<GateSession> GateSessionVisitor;

/**
 *@brief 网关会话管理器
 */
class GateSessionMgr : public Avalon::Singleton<GateSessionMgr>
{
	typedef std::map<UInt32,UInt32> AccountMap;

public:
	GateSessionMgr();
	~GateSessionMgr();

	void Init();
	void Final();

public:
	/**
	 *@brief 查找一个网关
	 */
	GateSession* FindSession(UInt32 id){ return m_Sessions.FindObject(id);}

	/**
	 *@brief 选出一个登录的网关
	 */
	GateSession* SelectGate();

	/**
	 *@brief 判断一个玩家是否在线
	 */
	bool IsPlayerOnline(UInt32 id);

	/**
	 *@brief 踢掉一个玩家
	 */
	void KickOffPlayer(UInt32 id, GateSession* newSession = NULL);

	/**
	 *@brief 遍历gate会话                                                                     
	 */
	void VisitGateSession(GateSessionVisitor& visitor){ m_Sessions.VisitObjects(visitor); }

public://事件

	void OnTick(const Avalon::Time& now);

	void OnGateConnected(UInt32 connid);

	void OnGateDisconnected(UInt32 connid);

private:
	//网关会话map
	CLIDObjIndex<GateSession>  m_Sessions;
	//在线账号列表，用于唯一登录验证
	AccountMap					m_Accounts;
};

#endif
