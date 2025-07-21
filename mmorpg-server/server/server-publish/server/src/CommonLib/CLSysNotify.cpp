#include "CLSysNotify.h"

#include <regex>
#include <sstream>
#include <AvalonScriptFile.h>

SysNotifyMgr::SysNotifyMgr()
{
}

SysNotifyMgr::~SysNotifyMgr()
{
	Clear();
}

bool SysNotifyMgr::Load(Avalon::ScriptState* state, const std::string& filename)
{


	// by huchenhui
	/*
	state->DefClass<SysNotifyMgr>("SysNotifyMgr")
		.Def("Set", &SysNotifyMgr::AddNotify);

	state->Set("sysNotify", this);

	Avalon::ScriptFile script(state);
	if(!script.Load(filename.c_str())) return false;
	*/

	// by huchenhui

	Clear();

	class SysNotifyDataEntryVisitor : public Avalon::DataEntryVisitor<SysNotifyDataEntry>
	{
	public:
		SysNotifyDataEntryVisitor(SysNotifyMgr* sysNotifyMgr) : m_pSysNotifyMgr(sysNotifyMgr) {}

		bool operator()(SysNotifyDataEntry* dataEntry)
		{

			m_pSysNotifyMgr->AddNotify(dataEntry->id, dataEntry->type, 0, dataEntry->format.c_str());
			return true;
		}

	private:
		SysNotifyMgr* m_pSysNotifyMgr;
	};

	SysNotifyDataEntryVisitor visitor(this);
	SysNotifyDataEntryMgr::Instance()->Visit(visitor);
	//SysNotifyDataEntryMgr 已经用完了 by huchenhui
	SysNotifyDataEntryMgr::Instance()->Destroy();

	return true;
}

void SysNotifyMgr::Clear()
{
	for (NotifyMap::iterator iter = m_Notifies.begin();
		iter != m_Notifies.end(); ++iter)
	{
		free(iter->second.format);
	}
	m_Notifies.clear();
}

void SysNotifyMgr::AddNotify(UInt32 id, UInt16 type, UInt8 color, const char* format)
{
	if(format == NULL) return;

	SysNotify notify;
	notify.type = type;
	notify.color = color;
	notify.format = avalon_strdup(format);
	
	NotifyMap::iterator iter = m_Notifies.find(id);
	if(iter != m_Notifies.end())
	{
		free(iter->second.format);
		m_Notifies.erase(iter);
	}

	m_Notifies.insert(std::make_pair(id, notify));
}

const SysNotify* SysNotifyMgr::FindNotify(UInt32 id) const
{

	NotifyMap::const_iterator iter = m_Notifies.find(id);
	if(iter != m_Notifies.end()) return &iter->second;
	return NULL;
}

std::string SysNotifyMgr::TimeToStr(UInt32 secs)
{
 	const SysNotify* day = FindNotify(DAY_STR_ID);
 	const SysNotify* hour = FindNotify(HOUR_STR_ID);
 	const SysNotify* min = FindNotify(MIN_STR_ID);
 	const SysNotify* sec = FindNotify(SEC_STR_ID);

 	if(day == NULL || hour == NULL || min == NULL || sec == NULL) return "";

	UInt32 days = secs / DAY_TO_SEC;
	secs = secs % DAY_TO_SEC;
	UInt32 hours = secs / HOUR_TO_SEC;
	secs = secs % HOUR_TO_SEC;
	UInt32 mins = secs / MIN_TO_SEC;
	secs = secs % MIN_TO_SEC;

	std::ostringstream oss;
	if(days != 0)	oss << days << day->format;
	if(hours != 0)	oss << hours << hour->format;
	if(mins != 0)	oss << mins << min->format;
	if(secs != 0)	oss << secs << sec->format;

	return oss.str();
}

const char* SysNotifyMgr::GetColorNameByQuality(UInt32 quality)
{
	if(quality > 5) return "";

	const SysNotify* notify = FindNotify(201 + quality);
	if(notify == NULL) return "";

	return notify->format;
}
