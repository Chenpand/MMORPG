#include "AbnormalTransHelper.h"

#include <CLRecordClient.h>


namespace AbnormalTransHelper
{
	bool SaveAbnormalTransactionInfo(const AbnormalTransDBInfo& info)
	{
		switch (info.freezeObj)
		{
		case AT_FREEZE_OBJECT_SELLER:
			if (info.seller == 0)
			{
				ErrorLogStream << info.LogStr() << " seller is zero!" << LogStream::eos;
				return false;
			}
			break;

		case AT_FREEZE_OBJECT_BUYER:
			if (info.buyer == 0)
			{
				ErrorLogStream << info.LogStr() << " buyer is zero!" << LogStream::eos;
				return false;
			}
			break;

		default:
			ErrorLogStream << info.LogStr() << " error freeze object(" << (UInt32)info.freezeObj << ")!" << LogStream::eos;
			return false;
		}

		if (info.transAmount == 0)
		{
			ErrorLogStream << info.LogStr() << " trans amount is zero!" << LogStream::eos;
			return false;
		}

		if (info.backWay == ATFROZEN_MONEY_BACK_WAY_NONE)
		{
			ErrorLogStream << info.LogStr() << " trans back way is none!" << LogStream::eos;
			return false;
		}

		auto cmd = CLRecordClient::Instance()->PrepareAutoGuidInsertCommand("t_abnormal_transaction");
		if (!cmd)
		{
			return false;
		}

		cmd->PutData("handle_status", (UInt8)ATHS_WAIT_FREEZE);
		cmd->PutData("create_time", (UInt32)CURRENT_TIME.Sec());
		cmd->PutData("buyer", info.buyer);
		cmd->PutData("seller", info.seller);
		cmd->PutData("transaction_time", info.transTime);
		cmd->PutData("transaction_id", Avalon::StringUtil::ConvertToString<ObjID_t>(info.transId));
		cmd->PutData("item_id", info.itemUId);
		cmd->PutData("item_data_id", info.itemDataId);
		cmd->PutData("item_num", info.itemNum);
		cmd->PutData("money_type", (UInt32)info.moneyType);
		cmd->PutData("transaction_amount", info.transAmount);
		cmd->PutData("freeze_object", (UInt8)info.freezeObj);
		cmd->PutData("back_way", (UInt8)info.backWay);
		cmd->PutData("frozen_permanent", (UInt8)(info.isFrozenPermanet ? 1 : 0));
		cmd->PutData("treasure_flag", (UInt8)(info.isTreasure ? 1 : 0));
		cmd->PutData("mail_notify_type", info.mailNotifyType);
		cmd->PutData("unfrozen_amount", info.unfrozenAmount);
		cmd->PutData("rmplayer_transmoney_flag", (UInt8)(info.isNeedRemovePlayerTransMoney ? 0 : 1));
		CLRecordClient::Instance()->SendCommand(cmd);

		InfoLogStream << "Save abnormal transaction(" << info.transId << ")." << LogStream::eos;
		return true;
	}
}