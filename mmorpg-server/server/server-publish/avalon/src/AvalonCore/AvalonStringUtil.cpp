#include "AvalonStringUtil.h"
#include <algorithm>

namespace Avalon
{
	void StringUtil::Trim(std::string& str)
	{
		static const char* delims = " \t\r\n";
        str.erase(str.find_last_not_of(delims)+1); // trim right
        str.erase(0, str.find_first_not_of(delims)); // trim left
	}

	void StringUtil::ToLowerCase(std::string& str)
	{
		std::transform(
			str.begin(),
            str.end(),
            str.begin(),
			tolower);
	}

	void StringUtil::ToUpperCase(std::string& str)
	{
		std::transform(
			str.begin(),
            str.end(),
            str.begin(),
			toupper);
	}

	void StringUtil::Split(const std::string& str,std::vector<std::string>& strs,const std::string& delims)
	{
		if(str.empty()) return;
		strs.clear();
        size_t start = 0, pos;
        do 
        {
            pos = str.find_first_of(delims, start);
			if(pos == std::string::npos)
            {
                // Copy the rest of the string
                strs.push_back( str.substr(start) );
                break;
            }
            else
            {
                // Copy up to delimiter
                strs.push_back( str.substr(start, pos - start) );
                start = pos + 1;
            }
		} while (start != std::string::npos);
	}
}
