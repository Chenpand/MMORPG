#include "AvalonPropertySet.h"
#include <fstream>

namespace Avalon
{

	bool PropertySet::LoadFromFile(const std::string &path)
	{
		std::ifstream stream(path.c_str());
		if(!stream) return false;

		PROPERTY_MAP* pSection = NULL;

		std::string pair;
		while(std::getline(stream, pair))
		{
			size_t len = pair.length();
			if(len > 0 && pair.at(len - 1) == '\r')
			{
				pair = pair.substr(0, len-1);
			}

			size_t pos = pair.find_first_of("#;");
			if(pos != std::string::npos)
			{
				pair = pair.substr(0,pos);
			}
			StringUtil::Trim(pair);

			if(pair.empty()) continue;
			if(pair.size() >= 3 && '[' == pair[0] && ']' == pair[pair.length() - 1])
			{
				pair = pair.substr(1, pair.length() - 2);
				StringUtil::Trim(pair);
				if(pair.empty()) continue;
				StringUtil::ToLowerCase(pair);

				pSection = &m_Properties[pair];
			}
			else
			{
				if(NULL == pSection)
				{
					pSection = &m_Properties["global"];
				}
				LoadPair(pSection,pair);
			}

			pair.clear();
		}

		return true;
	}

	bool PropertySet::LoadFromCmdline(const std::string& section,int argc,char* argv[])
	{
		std::string sec = section;
		StringUtil::Trim(sec);
		if(sec.empty()) return false;

		StringUtil::ToLowerCase(sec);

		PROPERTY_MAP* pSection = &m_Properties[sec];
		for(int i = 1;i < argc;++i)
		{
			if(argv[i][0] != '-') continue;

			std::string pair = argv[i];
			size_t pos = pair.find_first_not_of('-');
			if(pos == std::string::npos) continue;
			pair = pair.substr(pos);

			LoadPair(pSection,pair);
		}

		return true;
	}

	bool PropertySet::SaveToFile(const std::string& path)
	{
		std::ofstream stream(path.c_str(), std::ios::out | std::ios::trunc);
		if(!stream) return false;

		for(SECTION_MAP::iterator iter = m_Properties.begin(); iter != m_Properties.end(); ++iter)
		{
			stream << '[' << iter->first << ']' << std::endl;
			for(PROPERTY_MAP::iterator proiter = iter->second.begin(); proiter != iter->second.end();++proiter)
			{
				stream << proiter->first << '=' << proiter->second << std::endl;
			}
			stream << std::endl;
		}
		return true;
	}

	bool PropertySet::LoadPair(PROPERTY_MAP* pSection,const std::string &pair)
	{
		std::vector<std::string> strs;
		
		StringUtil::Split(pair,strs,"=");
		
		if(0 == strs.size())
		{
			return false;
		}
		else if(1 == strs.size())
		{
			strs.push_back("true");
		}

		StringUtil::Trim(strs[0]);
		if(strs[0].empty()) return false;
		StringUtil::ToLowerCase(strs[0]);
		StringUtil::Trim(strs[1]);

		(*pSection)[strs[0]] = strs[1];
		return true;
	}

	const PropertySet::Property PropertySet::GetProperty(const std::string& _section,const std::string &_key)
	{
		static Property nullProperty("");

		std::string section = _section;
		std::string key = _key;
		StringUtil::Trim(section);
		StringUtil::ToLowerCase(section);
		StringUtil::Trim(key);
		StringUtil::ToLowerCase(key);

		SECTION_MAP::iterator seciter = m_Properties.find(section);
		if(seciter != m_Properties.end())
		{
			PROPERTY_MAP::iterator iter = seciter->second.find(key);
			if(iter != seciter->second.end())
			{
				return iter->second;
			}
		}
		return nullProperty;
	}

	void PropertySet::SetProperty(const std::string& _section, const std::string& _key, const std::string& _val)
	{
		std::string section = _section;
		std::string key = _key;
		std::string val = _val;
		StringUtil::Trim(section);
		StringUtil::ToLowerCase(section);
		StringUtil::Trim(key);
		StringUtil::ToLowerCase(key);
		StringUtil::Trim(val);

		if(section.empty() || key.empty()) return;
		m_Properties[section][key] = val;
	}

	void PropertySet::Visit(const std::string& section, PropertyVisitor& visitor)
	{
		SECTION_MAP::iterator seciter = m_Properties.find(section);
		if(seciter == m_Properties.end()) return;

		for(PROPERTY_MAP::iterator iter = seciter->second.begin();
			iter != seciter->second.end(); ++iter)
		{
			if(!visitor(iter->first, iter->second)) return;
		}
	}
}

