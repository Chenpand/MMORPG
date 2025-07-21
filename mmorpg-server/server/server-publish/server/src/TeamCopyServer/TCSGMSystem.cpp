#include "TCSGMSystem.h"
#include "TCSTeamMgr.h"

#include "TCSPlayerMgr.h"

class StartChallengeCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "sc"; }

	void Execute(ObjID_t playerId, const CLParameter& params)
	{
		UInt32 fieldId = params.GetParam("id");

		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(playerId);
		if (player != NULL)
		{
			TCSTeamMgr::Instance()->HandleStartChallengeReq(player, fieldId);
		}
	}
};

class ClearFieldCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "cf"; }

	void Execute(ObjID_t playerId, const CLParameter& params)
	{
		UInt32 fieldId = params.GetParam("id");
		TCSTeamMgr::Instance()->GMClearField(playerId, fieldId);
	}
};

class ClearSquadStatusCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "css"; }

	void Execute(ObjID_t playerId, const CLParameter& params)
	{
		TCSTeamMgr::Instance()->GMClearSquadStatus(playerId);
	}
};

class ClearChallengeNumCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "ccn"; }

	void Execute(ObjID_t playerId, const CLParameter& params)
	{
		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(playerId);
		if (player != NULL)
		{
			player->GMClearChallengeNum();
		}
	}
};

class OpenGoldTeamCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "ogt"; }

	void Execute(ObjID_t playerId, const CLParameter& params)
	{
		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(playerId);
		if (player != NULL)
		{
			player->GMOpenGoldTeam();
		}
	}
};

class TeamCopyFaildCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "tcf"; }

	void Execute(ObjID_t playerId, const CLParameter& params)
	{
		TCSTeamMgr::Instance()->GMTeamCopyFailed(playerId);
	}
};

class TeamCopyBossBloodRateCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "bbr"; }

	void Execute(ObjID_t playerId, const CLParameter& params)
	{
		UInt32 phase = params.GetParam("p");
		UInt32 frame = params.GetParam("f");
		UInt32 bossBloodRate = params.GetParam("r");
		TCSTeamMgr::Instance()->HandlerPhaseBoss(playerId, phase, frame, bossBloodRate);
	}
};

class TeamCopyEnergyReviveRateCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "err"; }

	void Execute(ObjID_t playerId, const CLParameter& params)
	{
		UInt32 rate = params.GetParam("rate");
		TCSTeamMgr::Instance()->GMEnergyReviveRate(playerId, rate);
	}
};

class TeamCopyOpenDiffGradeCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "odg"; }

	void Execute(ObjID_t playerId, const CLParameter& params)
	{
		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(playerId);
		if (player != NULL)
		{
			player->GMOpenDiffGrade();
		}
	}
};

class TeamCopyOneKeyClearCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "okc"; }

	void Execute(ObjID_t playerId, const CLParameter& params)
	{
		TCSTeamMgr::Instance()->GMClearAllField(playerId);
	}
};

class TeamCopySetSquadProCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "squad"; }

	void Execute(ObjID_t playerId, const CLParameter& params)
	{
		UInt32 grid = params.GetParam("grid");
		UInt32 cd = params.GetParam("cd");
		TCSTeamMgr::Instance()->GMSetSquadPro(playerId, grid, cd);
	}
};

class TeamCopyClearGridObjCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "cgo"; }

	void Execute(ObjID_t playerId, const CLParameter& params)
	{
		UInt32 type = params.GetParam("type");
		TCSTeamMgr::Instance()->GMClearGridObj(playerId, type);
	}
};


TCSGMSystem::TCSGMSystem()
{
	RegisterHandlers();
}

TCSGMSystem::~TCSGMSystem()
{
	for (HandlerMap::iterator iter = m_Handlers.begin();
		iter != m_Handlers.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	m_Handlers.clear();
}

void TCSGMSystem::ExecuteCommand(ObjID_t playerId, const std::string& cmd, const CLParameter& params)
{
	std::string subCmd = params.GetParam("cmd").GetString();

	Avalon::StringUtil::Trim(subCmd);

	HandlerMap::iterator iter = m_Handlers.find(subCmd);
	if (iter != m_Handlers.end())
	{
		iter->second->Execute(playerId, params);
	}
}

void TCSGMSystem::RegisterHandlers()
{
	RegisterHandler(new StartChallengeCmdHandler());
	RegisterHandler(new ClearFieldCmdHandler());
	RegisterHandler(new ClearSquadStatusCmdHandler());
	RegisterHandler(new ClearChallengeNumCmdHandler());
	RegisterHandler(new OpenGoldTeamCmdHandler());
	RegisterHandler(new TeamCopyFaildCmdHandler());
	RegisterHandler(new TeamCopyBossBloodRateCmdHandler());
	RegisterHandler(new TeamCopyEnergyReviveRateCmdHandler());
	RegisterHandler(new TeamCopyOpenDiffGradeCmdHandler());
	RegisterHandler(new TeamCopyOneKeyClearCmdHandler());
	RegisterHandler(new TeamCopySetSquadProCmdHandler());
	RegisterHandler(new TeamCopyClearGridObjCmdHandler());
}

void TCSGMSystem::RegisterHandler(GMCmdHandler* handler)
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