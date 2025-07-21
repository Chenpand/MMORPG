#include "GuildDonate.h"

void GuildDonateLogMgr::PushLog(ObjID_t id, std::string name, GuildDonateType type, UInt8 num, UInt32 contri)
{
	GuildDonateLog log;
	log.id = id;
	log.name = name;
	log.type = type;
	log.num = num;
	log.contri = contri;

	if (m_Logs.size() >= DONATE_LOG_MAX_NUM)
	{
		m_Logs.pop_front();
	}

	m_Logs.push_back(log);
	SetDirty();
}

void GuildDonateLogMgr::DecodeString(std::istringstream& is)
{
	GuildDonateLog log;
	UInt8 split;
	UInt32 type;
	UInt32 num;
	UInt32 contri;

	while (is >> log.id)
	{
		
		is >> split >> type >> split >> num >> split >> contri >> split;
		log.type = type;
		log.num = num;
		log.contri = contri;

		if (m_Logs.size() >= DONATE_LOG_MAX_NUM)
		{
			break;
		}

		m_Logs.push_back(log);
	}
}

void GuildDonateLogMgr::EncodeString(std::ostringstream& os)
{
	for (const auto& log : m_Logs)
	{
		os << log.id << ":" << (UInt32)log.type << ":" << (UInt32)log.num << ":" << (UInt32)log.contri << "|";
	}
}