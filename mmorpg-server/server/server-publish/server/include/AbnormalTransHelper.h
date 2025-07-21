#ifndef _ABNORMAL_TRANS_HELPER_H_
#define _ABNORMAL_TRANS_HELPER_H_

#include "CLItemTypeDefine.h"
#include "CLAbnormalTransDefine.h"


namespace AbnormalTransHelper
{
	/**
	*@brief 保存异常交易信息
	*/
	bool SaveAbnormalTransactionInfo(const AbnormalTransDBInfo& info);
}

#endif