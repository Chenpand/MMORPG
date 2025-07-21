#ifndef __GUILD_DONATE_H__
#define __GUILD_DONATE_H__

#include <CLGuildDefine.h>
#include <CLObjectProperty.h>

class GuildDonateLogMgr : public CLComplexProperty
{
	const UInt32 DONATE_LOG_MAX_NUM = 11;
public:
	void PushLog(ObjID_t id, std::string name, GuildDonateType type, UInt8 num, UInt32 contri);

	std::list<GuildDonateLog>& GetLogs() { return m_Logs; }

public:
	// ±‡¬Îœ‡πÿ
	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

private:
	std::list<GuildDonateLog>		m_Logs;
};

#endif