
#include <sstream>

#include "Common/common_base.h"

SERVER_BEGIN

	static inline uint8_t toHex(const uint8_t &x)
	{
		return x > 9 ? x + 55: x + 48;
	}

	std::string URLEncode(const char* pszInput, size_t nInputLength)
	{
		std::stringstream ssOutput;
		
		for(int32_t i = 0; i < (int32_t)nInputLength; ++i, ++pszInput)
		{
			if(isalnum((uint8_t)(*pszInput)))
			{
				ssOutput<< *pszInput;
			}
			else
			{
				if(*pszInput==' ')
				{
					ssOutput<< '+';
				}
				else
				{
					ssOutput<< '%'<< toHex( (uint8_t)(*pszInput) >> 4 )<< toHex( (uint8_t)(*pszInput) % 16 );
				}
			}
		}

		return ssOutput.str();
	}


SERVER_END

