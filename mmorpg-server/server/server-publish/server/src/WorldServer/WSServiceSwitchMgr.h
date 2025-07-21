#ifndef __WS_SERVICE_SWITCH_MGR_H__
#define __WS_SERVICE_SWITCH_MGR_H__

#include "ServiceSwitchMgr.h"
#include <AvalonSingleton.h>

class WSServiceSwitchMgr : public ServiceSwitchMgr, public Avalon::Singleton<WSServiceSwitchMgr>
{
public:
	WSServiceSwitchMgr();

	virtual void OnOpenService(ServiceType type);
	virtual void OnCloseService(ServiceType type);

	void OnSceneConnected(UInt32 id);
	void OnGateConnected(UInt32 id);

	UInt8 GetAutoBattleMode() const { return m_autoBattleMode; }
	void SetAutoBattleMode(UInt8 mode) { m_autoBattleMode = mode; }

private:
	// 自动战斗模式
	UInt8			m_autoBattleMode;
};

#endif