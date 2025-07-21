#include "GASGMSystem.h"
#include "BetHorseMgr.h"
#include "ArtifactJarMgr.h"
#include "WholeBargainMgr.h"

class BetHorsePhaseCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "bethorse"; }

	void Execute(const CLParameter& params)
	{
		std::string str_phase = params.GetParam("id").GetString();
		UInt32 phase = 0;
		try
		{
			phase = std::atoi(str_phase.c_str());
		}
		catch (...)
		{
			ErrorLogStream << "bet horse phase gm param error!!! id" << phase << LogStream::eos;
			return;
		}

		BetHorseMgr::Instance()->GMUpdatePhase(phase);
	}
};

class BetHorseCourtNumCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "betcourt"; }

	void Execute(const CLParameter& params)
	{
		std::string str_num = params.GetParam("num").GetString();
		UInt32 num = 0;
		try
		{
			num = std::atoi(str_num.c_str());
		}
		catch (...)
		{
			ErrorLogStream << "bet horse court num gm param error!!! id" << num << LogStream::eos;
			return;
		}

		BetHorseMgr::Instance()->ExecuteCourtNum(num);
	}
};

class ArtfactJarCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "ajar"; }

	void Execute(const CLParameter& params)
	{
		ArtifactJarMgr::Instance()->LotteryToRecord();
	}
};

GASGMSystem::GASGMSystem()
{
	RegisterHandlers();
}

GASGMSystem::~GASGMSystem()
{
	for (HandlerMap::iterator iter = m_Handlers.begin();
		iter != m_Handlers.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	m_Handlers.clear();
}

void GASGMSystem::ExecuteCommand(const std::string& cmd, const CLParameter& params)
{
	HandlerMap::iterator iter = m_Handlers.find(cmd);
	if (iter != m_Handlers.end())
	{
		iter->second->Execute(params);
	}
}

void GASGMSystem::RegisterHandlers()
{
	RegisterHandler(new BetHorsePhaseCmdHandler());
	RegisterHandler(new BetHorseCourtNumCmdHandler());
	RegisterHandler(new ArtfactJarCmdHandler());
}

void GASGMSystem::RegisterHandler(GMCmdHandler* handler)
{
	if (handler == NULL) return;
	HandlerMap::iterator iter = m_Handlers.find(handler->GetCmdName());
	if (iter != m_Handlers.end())
	{
		CL_SAFE_DELETE(iter->second);
		iter->second = handler;
	}
	else
	{
		m_Handlers.insert(std::make_pair(handler->GetCmdName(), handler));
	}
}