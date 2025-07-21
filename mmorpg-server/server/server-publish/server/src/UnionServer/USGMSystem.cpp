#include "USGMSystem.h"

#include "USPlayerMgr.h"
#include "UnionServiceMgr.h"
#include "ChampionService.h"
#include "ChampionPlayer.h"
#include "USGameParamMgr.h"

class ChangeChampionStatusCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "cs"; }

	void Execute(ObjID_t playerId, const CLParameter& params)
	{
		UInt8 status = params.GetParam("status");
		auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
		if (service == nullptr)
		{
			return;
		}
		service->GMChangeStatus(static_cast<ChampionStatus>(status));
	}
};

class ChangePlayerChampionStatusCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "ss"; }

	void Execute(ObjID_t playerId, const CLParameter& params)
	{
		auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
		if (service == nullptr)
		{
			return;
		}

		UInt8 status = params.GetParam("status");
		UInt32 score = params.GetParam("score");
		UInt32 lose = params.GetParam("lose");
		UInt32 win = params.GetParam("win");
		UInt32 group = params.GetParam("group");
		UInt32 pos = params.GetParam("pos");
		UInt32 all = 0;
		all = params.GetParam("all");
 
		if (all != 0)
		{
			playerId = 0;
		}

		service->GMChangePlayerStatus(playerId, [&](ChampionPlayer* player)->void {
			if (params.HasParam("status"))
			{
				player->SetStatus(static_cast<ChampionStatus>(status));
			}
			if (params.HasParam("score"))
			{
				player->SetScore(score);
			}
			if (params.HasParam("lose"))
			{
				player->SetLoseCount(lose);
			}
			if (params.HasParam("win"))
			{
				player->SetWinCount(win);
			}
			if (params.HasParam("group"))
			{
				player->SetGroup(group);
			}
			if (params.HasParam("pos"))
			{
				player->SetPos(pos);
			}
		});
	}
};

class GameParamCmdHandler : public GMCmdHandler
{
public:
	const char* GetCmdName() const { return "ugp"; }

	void Execute(ObjID_t playerId, const CLParameter& params)
	{
		std::string name = params.GetParam("name").GetString();
		UInt32 val = params.GetParam("val");

		USGameParamMgr::Instance()->SetValue(name.c_str(), val);
	}
};


USGMSystem::USGMSystem()
{
	RegisterHandlers();
}

USGMSystem::~USGMSystem()
{
	for (HandlerMap::iterator iter = m_Handlers.begin();
		iter != m_Handlers.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	m_Handlers.clear();
}

void USGMSystem::ExecuteCommand(ObjID_t playerId, const std::string& cmd, const CLParameter& params)
{
	std::string subCmd = params.GetParam("cmd").GetString();

	Avalon::StringUtil::Trim(subCmd);

	HandlerMap::iterator iter = m_Handlers.find(subCmd);
	if (iter != m_Handlers.end())
	{
		iter->second->Execute(playerId, params);
	}
}

void USGMSystem::RegisterHandlers()
{
	RegisterHandler(new ChangeChampionStatusCmdHandler());
	RegisterHandler(new ChangePlayerChampionStatusCmdHandler());
	RegisterHandler(new GameParamCmdHandler());
}

void USGMSystem::RegisterHandler(GMCmdHandler* handler)
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