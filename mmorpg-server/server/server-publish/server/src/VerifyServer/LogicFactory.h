#ifndef __LOGIC_FACTORY_H__
#define __LOGIC_FACTORY_H__

#include <AvalonSingleton.h>
#include <CLMatchDefine.h>

class ILogic;
class LogicFactory : public Avalon::Singleton<LogicFactory>
{
public:
	ILogic* CreateLogic(RaceType type);
};

#endif