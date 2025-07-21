#ifndef _CL_HONOR_H_
#define _CL_HONOR_H_

#include <CLDefine.h>

/**
 *@brief ÈÙÓş»ùÀà
 */
class CLHonor
{
public:
	CLHonor() {}
	~CLHonor() {}

public:

	/**
	 *@brief pvpÈÙÓş½±Àø
	 */
	static UInt32 PvpHonorReward(UInt32 pvpType, UInt32 result, UInt32 rank = 0, UInt32 kill = 0);

};

#endif