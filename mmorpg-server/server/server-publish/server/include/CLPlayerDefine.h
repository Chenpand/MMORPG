#ifndef _CL_PLAYER_DEFINE_H_
#define _CL_PLAYER_DEFINE_H_

#include "CLTaskDefine.h"

class PlayerSenceShareData
{
public:
	PlayerSenceShareData()
	{
	}
	void clear()
	{
		m_MasterTaskShareData.clear();
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_MasterTaskShareData;
	}

	MasterTaskShareData m_MasterTaskShareData; //师门任务离线数据
};

#endif