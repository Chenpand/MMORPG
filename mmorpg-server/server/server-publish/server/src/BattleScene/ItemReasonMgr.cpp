#include "ItemReasonMgr.h"

#include <AvalonStringUtil.h>
#include <fstream>
#include <string.h>

ItemReasonMgr::ItemReasonMgr()
{
}

ItemReasonMgr::~ItemReasonMgr()
{
}

bool ItemReasonMgr::Init(const std::string& config)
{
	std::ifstream stream(config.c_str(), std::ios_base::in);
	if(!stream) return false;
	
	char linebuf[256];
	memset(linebuf, 0, sizeof(linebuf));
	while(stream)
	{
		stream.getline(linebuf, 255);
		linebuf[255] = 0;

		std::vector<std::string> tokens;
		Avalon::StringUtil::Split(linebuf, tokens, "\t\r\n");
		if(tokens.size() >= 2)
		{
			m_Reasons.insert(std::make_pair(tokens[0], tokens[1]));
		}
	}
	
	stream.close();
	
	return true;
}

bool ItemReasonMgr::FindReason(const std::string& key, std::string& reason)
{
	ReasonMap::iterator iter = m_Reasons.find(key);
	if(iter != m_Reasons.end())
	{
		reason = iter->second;
		return true;
	}
	return false;
}
