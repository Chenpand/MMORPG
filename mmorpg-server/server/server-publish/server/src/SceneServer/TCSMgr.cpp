#include "TCSMgr.h"

#include <AvalonTimer.h>
#include <CLTeamCopyProtocol.h>
#include "SSRouter.h"

TCSMgr::TCSMgr()
{

}
TCSMgr::~TCSMgr()
{

}

void TCSMgr::Init()
{
	class CfgValTimer : public Avalon::TimerHandler
	{
	public:
		CfgValTimer() {}

		void OnTimeout(const Avalon::Time& now)
		{
			InfoLogStream << "start query team copy cfg!" << LogStream::eos;

			CLProtocol::TeamCopyCfgValReq req;
			Router::SendToTeamCopy(req);
		}
	};

	// 启动服务器一分钟后去查询团本配置
	Avalon::TimerReactor::Instance()->ScheduleOnce(new CfgValTimer(), 60 * 1000);
}

UInt32 TCSMgr::GetCfgval(UInt32 type)
{
	auto iter = m_ValCfgMap.find(type);
	return iter != m_ValCfgMap.end() ? iter->second : 0;
}

void TCSMgr::SetCfgval(UInt32 type, UInt32 val)
{
	m_ValCfgMap[type] = val;
}