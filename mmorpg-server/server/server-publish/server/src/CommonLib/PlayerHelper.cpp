#include "PlayerHelper.h"

#include "CLSystemValueDataEntry.h"
#include "CLJobDataEntry.h"

namespace PlayerHelper
{
	bool CalculateSystemOpenedExtensibleRoleNum(UInt32& num)
	{
		// 角色基础栏位
		UInt32 baseNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ROLE_BASE_FIELD);
		// 单次增加栏位
		UInt32 perIncNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ROLE_EXTEND_PER_INC_NUM);
		// 当前开放的转职职业数量
		UInt32 openChangeOccuNum = JobDataEntryMgr::Instance()->GetOpenChangeOccuNum();

		if (baseNum == 0 || perIncNum == 0)
		{
			ErrorLogStream << "Systemvalue data is zero!" << LogStream::eos;
			return false;
		}

		if (openChangeOccuNum == 0)
		{
			ErrorLogStream << "Open change occu num is zero!" << LogStream::eos;
			return false;
		}

		if (openChangeOccuNum <= baseNum)
		{
			num = baseNum;
			InfoLogStream << "change occu num(" << openChangeOccuNum << ") is less than increase threshhold(" << baseNum
				<< "), now extensible role num=" << num << LogStream::eos;
			return true;
		}

		num = (openChangeOccuNum - baseNum) * perIncNum;

		InfoLogStream << "After calculate open change occu num(" << openChangeOccuNum << ") and base num(" << baseNum << ") and per increase num(" 
			<< perIncNum << "), extensible role num=" << num << LogStream::eos;
		return true;
	}
}