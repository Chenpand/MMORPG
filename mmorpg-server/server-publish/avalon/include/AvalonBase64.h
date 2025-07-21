#ifndef _AVALON_BASE64_H_
#define _AVALON_BASE64_H_

namespace Avalon
{
	/**
	 *@brief base64 ±àÂë
	 */
	int base64_encode(char* dest, int destsize, const void* src, int size);

	/**
	 *@brief base64 ½âÂë
	 */
	int base64_decode(char* dest, int destsize, const char* src, int size); 
}

#endif
