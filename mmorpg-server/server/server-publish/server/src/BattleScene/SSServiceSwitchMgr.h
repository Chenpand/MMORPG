#ifndef __SS_SERVICE_SWITCH_MGR_H__
#define __SS_SERVICE_SWITCH_MGR_H__

#include "ServiceSwitchMgr.h"
#include <AvalonSingleton.h>

class Player;
class SSServiceSwitchMgr : public ServiceSwitchMgr, public Avalon::Singleton<SSServiceSwitchMgr>
{
public:
	virtual void OnOpenService(ServiceType type);
	virtual void OnCloseService(ServiceType type);

public:
	/**
	*@brief ÕÊº“…œœﬂ
	*/
	void OnPlayerOnline(Player* player);
};

#endif