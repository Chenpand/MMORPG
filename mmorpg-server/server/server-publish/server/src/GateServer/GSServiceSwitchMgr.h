#ifndef __GS_SERVICE_SWITCH_MGR_H__
#define __GS_SERVICE_SWITCH_MGR_H__

#include "ServiceSwitchMgr.h"
#include <AvalonSingleton.h>


class GSServiceSwitchMgr : public ServiceSwitchMgr, public Avalon::Singleton<GSServiceSwitchMgr>
{
public:
	virtual void OnOpenService(ServiceType type);
	virtual void OnCloseService(ServiceType type);
};

#endif