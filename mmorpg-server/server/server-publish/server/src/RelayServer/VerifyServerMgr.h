#ifndef __VERIFY_SERVER_MGR_H__
#define __VERIFY_SERVER_MGR_H__

#include <CLVerifyNodeMgr.h>
#include <AvalonSingleton.h>

class VerifyServerMgr : public CLVerifyNodeMgr, public Avalon::Singleton<VerifyServerMgr>
{

};

#endif