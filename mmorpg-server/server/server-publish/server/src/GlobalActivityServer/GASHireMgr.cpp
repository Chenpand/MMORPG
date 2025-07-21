#include "GASHireMgr.h"
#include "GASApplication.h"
#include <CLRecordClient.h>

struct SelectHireAccidCallBack : public CLRecordCallback
{
	SelectHireAccidCallBack() {}

	bool OnLoadFinished()
	{
		return GASHireMgr::Instance()->LoadHireAccidCallBack(this);
	}
};

bool GASHireMgr::OnInit()
{
	CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_g_hire_accid");
	CLRecordClient::Instance()->SendCommand(cmd, new SelectHireAccidCallBack());
	return true;
}

bool GASHireMgr::IsAlreadyBind(UInt32 accid, std::string platform)
{
	auto iter = m_BindedAccidSet.find({ accid, platform });
	if (iter != m_BindedAccidSet.end())
	{
		return true;
	}
	return false;
}

void GASHireMgr::AddToBindedSet(UInt32 accid, std::string platform)
{
	if (IsAlreadyBind(accid, platform))
	{
		return;
	}

	m_BindedAccidSet.insert({ accid, platform });

	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_g_hire_accid", CLRecordClient::Instance()->GenGuid());
	cmd->PutData("accid", accid);
	cmd->PutData("platform", platform);
	CLRecordClient::Instance()->SendCommand(cmd);
}

bool GASHireMgr::LoadHireAccidCallBack(CLRecordCallback * callback)
{
	if (!callback) return false;

	while (callback->NextRow())
	{
		BindedKey key;
		key.accid = callback->GetData("accid");
		key.platform = callback->GetData("platform");
		m_BindedAccidSet.insert(key);
	}

	return true;
}
