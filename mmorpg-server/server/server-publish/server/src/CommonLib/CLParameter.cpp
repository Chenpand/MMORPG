#include "CLParameter.h"
#include <sstream>

CLParameter::CLParameter()
{
}

CLParameter::~CLParameter()
{
}

bool CLParameter::ParseParams(const std::string& params)
{
	std::vector<std::string> tokens;
	Avalon::StringUtil::Split(params,tokens," \t\n\r");
	if(params.size() == 0) return true;
	
	for(std::vector<std::string>::iterator iter = tokens.begin();
		iter != tokens.end();++iter)
	{
		std::string pair = *iter;
		std::string::size_type index = pair.find_first_of('=');
		if(index == std::string::npos) return false;

		std::string key = pair.substr(0,index);
		std::string value = pair.substr(index + 1);
		if(key.empty()) return false;

		m_Params[key] = value;
	}

	return true;
}

void CLParameter::Clear()
{
	m_Params.clear();
}

bool CLParameter::HasParam(const std::string& name) const
{
	return m_Params.find(name) != m_Params.end();
}

const CLParameter::Value CLParameter::GetParam(const std::string& name) const
{
	ParamMap::const_iterator iter = m_Params.find(name);
	if(iter != m_Params.end()) return iter->second;
	return std::string("");
}

void CLParameter::SetParam(const std::string& name,const std::string& val)
{
	if(name.empty()) return;

	m_Params[name] = val;
}

std::string CLParameter::ToString() const
{
	bool bFirst = true;
	std::ostringstream oss;
	for(ParamMap::const_iterator iter = m_Params.begin();
		iter != m_Params.end(); ++iter)
	{
		if(bFirst) bFirst = false;
		else oss << ',';
		oss << iter->first << '=' << iter->second;
	}
	return oss.str();
}


