#ifndef _CL_OPERATE_DEFINE_H_
#define _CL_OPERATE_DEFINE_H_

#include "CLDefine.h"

//��Ӫ��Ϣ
struct OperateInfo
{
	bool IsValide()
	{
		if (chatMonitorURL.empty() || chatMonitorMD5Key.empty() || 
			cdkVerifyURL.empty() || cdkVerifyMD5Key.empty())
		{
			return false;
		}

		return true;
	}

	std::string chatMonitorURL;
	std::string chatMonitorMD5Key;
	std::string cdkVerifyURL;
	std::string cdkVerifyMD5Key;
};


#endif
