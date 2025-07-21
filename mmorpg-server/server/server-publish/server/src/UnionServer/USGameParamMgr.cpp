#include "USGameParamMgr.h"

#include <CLRecordClient.h>
#include <CLGameDefine.h>
#include <CLSyncObjectProtocol.h>

#include "UnionServiceMgr.h"
#include "ChampionService.h"
#include "USApplication.h"

class SelectGameParamsCallback : public CLRecordCallback
{
public:
	virtual void OnFinished()
	{
		USGameParamMgr::Instance()->OnSelectDataRet(this);
	}

	virtual void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "Load ChampionPlayer Faild,ErrorCode is" << errorcode << LogStream::eos;
		USApplication::Instance()->Exit();
	}
};

USGameParamMgr::USGameParamMgr()
{
	m_IsLoad = false;
}

USGameParamMgr::~USGameParamMgr()
{
}

bool USGameParamMgr::Init()
{
	m_IsLoad = false;

	CLSelectCommand* selectcmd = CLRecordClient::Instance()->PrepareSelectCommand("t_gameparam");
	CLRecordClient::Instance()->SendCommand(selectcmd, new SelectGameParamsCallback());
	return true;
}

void USGameParamMgr::Final()
{
}

void USGameParamMgr::SetValue(const char* name, UInt32 value)
{
	if(name == NULL) return;

	GameParamMap::iterator iter = m_GameParams.find(name);
	if(iter != m_GameParams.end())
	{
		if(iter->second.value == value) return;

		iter->second.value = value;

		CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_gameparam", iter->second.id, true);
		updateCmd->PutData("value", value);
		CLRecordClient::Instance()->SendCommand(updateCmd);
	}
	else if(value != 0)
	{
		USGameParam param;
		param.id = CLRecordClient::Instance()->GenGuid();
		param.value = value;
		m_GameParams.insert(std::make_pair(name, param));

		CLInsertCommand* insertCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_gameparam", param.id);
		insertCmd->PutData("name", name);
		insertCmd->PutData("value", value);
		CLRecordClient::Instance()->SendCommand(insertCmd);
	}
}

UInt32 USGameParamMgr::GetValue(const char* name) const
{
	GameParamMap::const_iterator iter = m_GameParams.find(name);
	if(iter != m_GameParams.end()) return iter->second.value;
	return 0;
}

bool USGameParamMgr::OnSelectDataRet(CLRecordCallback* callback)
{
	while(callback->NextRow())
	{
		USGameParam param;
		param.id = callback->GetKey();
		param.value = callback->GetData("value");
		m_GameParams.insert(std::make_pair(callback->GetData("name").GetString(), param));
	}

	m_IsLoad = true;

	InfoLogStream << "game param load finish!" << LogStream::eos;

	OnLoadEnd();

	return true;
}

void USGameParamMgr::OnLoadEnd()
{
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{
		return;
	}
	service->SetDBStatus(static_cast<ChampionStatus>(GetValue(CHAMPION_STATUS_PARAM)));
}

