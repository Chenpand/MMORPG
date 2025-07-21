#include "AuctionTransRecord.h"
#include <CLRecordClient.h>
#include <AbnormalTransHelper.h>
#include "WSSelectInitDataCallback.h"
#include "WSPlayerMgr.h"
#include "WSItemCheck.h"
#include "MailBox.h"
#include "CLMsgParser.h"
#include "WSNetwork.h"
#include "WorldSysNotify.h"

/**
*@breif 查询未处理订单
*/
class SelectUnHadledAuctionTransCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return AuctionTransRecordMgr::Instance()->OnLoadUnHandledTransRet(this);
	}
};

/**
*@brief 查询该处理订单回调
*/
class SelectAuctionTransCallback : public CLRecordCallback
{
public:
	virtual void OnFinished()
	{
		AuctionTransRecordMgr::Instance()->OnLoadShouldHandleTransRet(this);
	}

	virtual void OnFailed(UInt32 errorcode)
	{	
		GameErrorLogStream("Player") << "SelectAuctionTransCallback, OnFailed  errorcode = " << errorcode << LogStream::eos;
	}
};

/**
*@brief 查询已经处理订单回调
*/
class SelectAuctionTransedsCallback : public CLRecordCallback
{
public:
	virtual void OnFinished()
	{
		AuctionTransRecordMgr::Instance()->OnLoadAlreadyTransRet(this);
	}

	virtual void OnFailed(UInt32 errorcode)
	{
		GameErrorLogStream("Player") << "SelectAuctionTransedsCallback, OnFailed  errorcode = " << errorcode << LogStream::eos;
	}
};

AuctionTransRecord::AuctionTransRecord()
:m_check_item_req_id(0), m_is_in_punishing(false), m_punish_timeout_timestamp(0)
{}


AuctionTransRecord::~AuctionTransRecord()
{}

void AuctionTransRecord::ToClient(ObjID_t playerId, AuctionTransaction& auctionTrans)
{
	if (playerId == m_seller)
	{
		auctionTrans.type = AuctionTransactionType::ATT_SELL;
	}
	else if (playerId == m_buyer)
	{
		auctionTrans.type = AuctionTransactionType::ATT_BUY;
	}
	else
	{
		auctionTrans.type = AuctionTransactionType::ATT_INVAIL;
	}
	auctionTrans.item_id = m_item_data_id;
	auctionTrans.item_num = m_item_num;
	auctionTrans.item_score = m_item_score;
	auctionTrans.unit_price = m_unit_price;
	auctionTrans.verify_end_time = m_verify_end_time;
	auctionTrans.enhance_type = m_enhanceType;
	auctionTrans.strength = m_strengthen;
	auctionTrans.qualitylv = m_qualitylv;
	auctionTrans.trans_time = m_transaction_time;
	auctionTrans.beadbuffId = m_beadbuffId;
	auctionTrans.mountBeadId = m_mountBeadId;
	auctionTrans.mountBeadBuffId = m_mountBeadBuffId;
	auctionTrans.mountMagicCardId = m_mountMagicCardId;
	auctionTrans.mountMagicCardLv = m_mountMagicCardLv;
	auctionTrans.equipType = m_equipType;
	auctionTrans.enhanceNum = m_enhanceNum;
}

void AuctionTransRecord::Update()
{
	if (GetID() == 0)
	{
		Insert();
	}
	else
	{
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_auction_transaction", GetID(), false);
		this->GetDBData(cmd, true);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}

void AuctionTransRecord::Insert()
{
	SetID(CLRecordClient::Instance()->GenGuid());
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_auction_transaction", GetID());
	this->GetDBData(cmd, false);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void AuctionTransRecord::Delete()
{
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_auction_transaction", GetID());
	CLRecordClient::Instance()->SendCommand(cmd);
}

bool AuctionTransRecord::IsShouldHanlde(UInt32 curr_time)
{
	//return curr_time >= this->m_verify_end_time && m_state == THS_NONE && this->m_program_audit_result == 1;
	return curr_time >= this->m_verify_end_time && m_state == THS_NONE;
}

bool AuctionTransRecord::IsNoAbnormal()
{
	/*return (this->m_program_audit_result == 0
			|| (this->m_program_audit_result == 1
			&& this->m_punishA_id == 0
			&& this->m_punishB_id == 0));*/
	return (m_punishA_id == 0 && m_punishB_id == 0) || m_punishC_id == 0 || m_isSysRecove == 1;
}

void AuctionTransRecord::SetState(UInt8 st)
{
	if (st < THS_NONE || st >= THS_MAX) return;
	this->m_state = st;
	this->Update();
}

void AuctionTransRecord::SetReturnedItemMark(UInt8 mark)
{
	this->m_item_returned = mark;
}

void AuctionTransRecord::SetPunishedId(UInt32 punishedId)
{
	this->m_punished_id = punishedId;
}

void AuctionTransRecord::SetSceneCheckItemReqId(UInt32 reqId)
{
	m_check_item_req_id = reqId; 
}

UInt32 AuctionTransRecord::GetSceneCheckItemReqId()
{ 
	return m_check_item_req_id; 
}

void AuctionTransRecord::SetPunishingMark(bool mark)
{ 
	m_is_in_punishing = mark; 
	m_punish_timeout_timestamp = (UInt32)CURRENT_TIME.Sec() + punish_timeout;
}

bool AuctionTransRecord::PunishingMark()
{ 
	return m_is_in_punishing; 
}

bool AuctionTransRecord::IsTimeOut(UInt32 curr_time)
{
	if (m_is_in_punishing && curr_time > m_punish_timeout_timestamp)
	{
		return true;
	}
	return false;
}

void  AuctionTransRecord::LoadPushishDataFromDb(CLRecordCallback *pCallback)
{
	if (!pCallback) return;
	this->m_program_audit_result = pCallback->GetData("program_audit_result");
	this->m_punishA_id = pCallback->GetData("punishA_id");
	this->m_punishB_id = pCallback->GetData("punishB_id");
	this->m_punishC_id = pCallback->GetData("punishC_id");
	this->m_transaction_extra_profit = pCallback->GetData("transaction_extra_profit");
	this->m_isNotRecAveprice = pCallback->GetData("isNotRecAveprice");
}

void AuctionTransRecord::PriteState(){
	InfoLogStream << "data of auction trans record guid:" << GetID() << ", seller:" << m_seller << ", buyer:" << m_buyer 
		<< ", currtime:" << (UInt32)CURRENT_TIME.Sec() << ", transaction_amount:" << m_transaction_amount << ", counter_fee:" << m_counter_fee
		<< ", verify_end_time:" << m_verify_end_time << ", program_audit_result:" << m_program_audit_result << ", punishA_id" << m_punishA_id  
		<< ", punishB_id:" << m_punishB_id << ", state" << m_state << LogStream::eos;
}

void AuctionTransRecord::SendUdpLog()
{
	Sys::SendUdpLog("auction_treas_transaction_punish", LOG_AUCTION_TREAS_TRANSACTION_PUNISH, GAME_VERSION.c_str(), ZONE_ID, UInt32(CURRENT_TIME.Sec()), PLATFORM.c_str(),
		GetID(), (UInt32)m_punishA_id, (UInt32)m_punishB_id, (UInt32)m_punishC_id, (UInt32)m_punished_id, (UInt32)m_minPriceOnSale, (UInt32)m_transaction_extra_profit, 
		(UInt32)m_verify_end_time, (UInt32)m_program_audit_result, (UInt32)m_state, (UInt32)m_isSysRecove);

	InfoLogStream << "send trans record udp log  id : " << GetID() << ", m_punishA_id : " << m_punishA_id << ", m_punishB_id : " << m_punishB_id << ", m_punishC_id : " << m_punishC_id << ", m_punished_id : "
		<< m_punished_id << ", m_minPriceOnSale : " << m_minPriceOnSale << ", m_transaction_extra_profit : " << m_transaction_extra_profit << ", m_verify_end_time : " << m_verify_end_time << ", m_program_audit_result" 
		<< m_program_audit_result << ", m_state : " << m_state << ", m_isSysRecove : " << m_isSysRecove << LogStream::eos;
	/*if (m_isSysRecove)
	{
	Sys::SendUdpLog("auction_sys_recover", LOG_AUCTION_SYSRECOVER, GAME_VERSION.c_str(), ZONE_ID, UInt32(CURRENT_TIME.Sec()), PLATFORM.c_str(),
	1, m_item_id, 1, m_unit_price);
	}*/
}

AuctionTransRecordMgr::AuctionTransRecordMgr()
{}

AuctionTransRecordMgr::~AuctionTransRecordMgr()
{}

void AuctionTransRecordMgr::Init()
{
	this->LoadUnHandledRecords();
	//this->LoadAlreadyTransedRecords();
	m_checkTimer.SetInterval(60 * 1000);
}

void AuctionTransRecordMgr::LoadUnHandledRecords()
{
	//UInt32 curr_time = (UInt32)CURRENT_TIME.Sec();
	CLSelectCommand* selectAuctionTransCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_auction_transaction");
	selectAuctionTransCmd->PutCond("state", 0);

	CLRecordClient::Instance()->SendCommand(selectAuctionTransCmd, new SelectUnHadledAuctionTransCallback());
}

void AuctionTransRecordMgr::LoadShouldHandleRecords()
{
	UInt32 curr_time = (UInt32)CURRENT_TIME.Sec();
	CLSelectCommand* selectAuctionTransCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_auction_transaction");

	selectAuctionTransCmd->PutCond("verify_end_time", curr_time, "<=");
	selectAuctionTransCmd->PutCond("state", Avalon::StringUtil::ConvertToString<UInt32>(THS_NONE));
	//std::string limitStr = "AND `state` = ";
	//limitStr += Avalon::StringUtil::ConvertToString<UInt32>(THS_NONE);
	std::string limitStr = " limit 100";
	selectAuctionTransCmd->SetLimit(limitStr);
	CLRecordClient::Instance()->SendCommand(selectAuctionTransCmd, new SelectAuctionTransCallback());
}

void AuctionTransRecordMgr::LoadAlreadyTransedRecords()
{
	UInt32 curr_time = (UInt32)CURRENT_TIME.Sec();
	CLSelectCommand* selectAuctionTransCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_auction_transaction");
	selectAuctionTransCmd->PutCond("transaction_time", curr_time - 30 * 24 * 3600, ">=");
	selectAuctionTransCmd->PutCond("state", THS_DAOZHUANG, "=");
	selectAuctionTransCmd->PutCond("isSysRecove", 0, "=");
	CLRecordClient::Instance()->SendCommand(selectAuctionTransCmd, new SelectAuctionTransedsCallback());
}

bool AuctionTransRecordMgr::AddOneRecord(AuctionTransRecord* record)
{
	if (!record) return false;
	AuctionTransRecordMap::iterator it = m_records.find(record->GetID());
	if (it != m_records.end()) return false;
	m_records.insert(std::pair<UInt64, AuctionTransRecord*>(record->GetID(), record));
	return true;
}

AuctionTransRecord* AuctionTransRecordMgr::GetRecord(UInt64 uid)
{
	AuctionTransRecordMap::iterator it = m_records.find(uid);
	if (it == m_records.end()) return NULL;

	return it->second;
}

void AuctionTransRecordMgr::OnTick(const Avalon::Time &now)
{
	if (m_checkTimer.IsTimeout(now))
	{
		/*UInt32 currTime = (UInt32)now.Sec();
		for (auto & it : m_records)
		{
			AuctionTransRecord* trans = it.second;
			if (!trans) continue;
			if (trans->IsShouldHanlde(currTime))
			{
				HandleOneTrans(*trans);
			}
		}*/
		this->LoadShouldHandleRecords();
	}
}

void AuctionTransRecordMgr::HandleOneTrans(AuctionTransRecord& trans)
{
	trans.SetState(THS_HANDLING);
	trans.PriteState();
	trans.SetPunishingMark(true);
	if (trans.IsNoAbnormal()) //无异常
	{
		HandleOnNoAbnormal(trans);
	}
	else{	//异常
		HandleOnAbNormal(trans);
	}
}

void AuctionTransRecordMgr::HandleOnNoAbnormal(AuctionTransRecord& trans)
{
	//邮件发送交易金额给卖家
	InfoLogStream << "HandleOnNoAbnormal guid:" << trans.GetID() << LogStream::eos;
	trans.PriteState();
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	
	std::string strSenderName;
	std::string	strTitle;
	std::string	strContent;
	std::string strReason;
	std::string strengthenStr = "";
	//std::string itemlinkStr = "";

	SysNotifyMgr::Instance()->MakeNotify(1047, buffer, sizeof(buffer));
	strSenderName = buffer;
	SysNotifyMgr::Instance()->MakeNotify(2210, buffer, sizeof(buffer));
	strTitle = buffer;
	
	if (trans.m_strengthen != 0)
	{
		strengthenStr = Avalon::StringUtil::ConvertToString<UInt32>(trans.m_strengthen);
	}

	if (trans.m_isSysRecove)
	{
		strReason = GetReason(SOURCE_TYPE_ACTION_SYS_RECOVERY, trans.m_auction_id, trans.m_item_data_id);
	}
	else
	{
	strReason = GetReason(SOURCE_TYPE_ACTION_ADUIT_TRANS_SUCC, trans.m_auction_id, trans.m_item_data_id);
	}
	std::vector<ItemReward>	vecitems;
	UInt32 money = trans.m_transaction_amount;
	UInt32 counterFee = trans.m_counter_fee;
	if (money >= counterFee)
	{
		money -= counterFee;
	}
	else{
		money = 0;
	}

	if (!trans.m_isSysRecove)
	{
 	zjy_auction::AuctionMgr::Instance()->RecordCounterFee(counterFee);
	}
 	
	//MsgPackage::GetItemMsgTag(itemlinkStr, 0, trans.m_item_data_id, trans.m_strengthen);

	//SysNotifyMgr::Instance()->MakeNotify(2211, buffer, sizeof(buffer), strengthenStr.c_str(), trans.m_item_name.c_str(), trans.m_buy_num, trans.m_transaction_amount, counterFee, money);
	UInt32 notifyId = trans.m_strengthen != 0 ? 2224 : 2211;
	SysNotifyMgr::Instance()->MakeNotify(notifyId, buffer, sizeof(buffer), strengthenStr.c_str(), trans.m_item_name.c_str(), trans.m_buy_num, trans.m_transaction_amount, counterFee, money);
	strContent = buffer;

	if (money > 0)
	{
		UInt32 uGetMoneyItemId = zjy_auction::AuctionMgr::Instance()->_GetItemTypeIDByPriceType((MoneyType)((UInt32)trans.m_money_type));
		vecitems.push_back(ItemReward(uGetMoneyItemId, money, false, 0));
	}

	if (MailBox::SendSysMail(trans.m_seller, strSenderName, strTitle, strContent, strReason, vecitems) != ErrorCode::SUCCESS)
	{
		// 发送邮件失败，这里出错就不return了
		ErrorLogStream << "handle no abnormal trans send transaction money to seller mail fail AuctionTransRecord(" << trans.GetID() << "),  auction id(" 
			<< trans.m_auction_id << "), mail to role(" << trans.m_seller << ") title(" << strTitle << ") money(" << money << ") failed." << LogStream::eos;
	}
	InfoLogStream << "trans is no abnormal guid=" << trans.GetID() << ", auction id : " << trans.m_auction_id << ", seller=" << trans.m_seller << ", buyer=" << trans.m_buyer
		<< ", transaction_amount=" << trans.m_transaction_amount << ", counterFee=" << counterFee << ", get_money=" << money << "state : " << (UInt32)THS_DAOZHUANG << LogStream::eos;
	trans.SetState(THS_DAOZHUANG);
	zjy_auction::AuctionMgr::Instance()->RecordTreasDealPrice(trans, true);
	trans.SendUdpLog();
	AddOneTransedRecord(trans);
	//trans.Update();
}

void AuctionTransRecordMgr::HandleOnAbNormal(AuctionTransRecord& trans)
{
	if ((trans.m_punishA_id == 0 && trans.m_punishB_id == 0) || trans.m_punishC_id == 0)
	{
		ErrorLogStream << "Handle AbNormal trans fail  guid(" << trans.GetID() << ") no punish punishA_id : " << trans.m_punishA_id 
			<< ", punishB_id : " << trans.m_punishB_id << ", punishC_id : " << trans.m_punishC_id << LogStream::eos;
		return;
	}

	if (JudgeOvertime(trans))
	{
		return;
	}

	UInt32 punishId = 0;
	UInt8  punishType = TPT_NONE;
	if (trans.m_punishC_id != PUNISHC_DEFAUT_ID)
	{
		punishId = trans.m_punishC_id;
		punishType = TPT_C;
	}
	else if (trans.m_punishA_id != 0)
	{
		punishId = trans.m_punishA_id;
		punishType = TPT_A;
	}
	else{
		punishId = trans.m_punishB_id;
		punishType = TPT_B;
	}

	if (punishId >= TPI_MAX)
	{
		ErrorLogStream << "Handle AbNormal trans fail, punish id err  guid(" << trans.GetID() << ") no punish punishA_id : " << trans.m_punishA_id
			<< ", punishB_id : " << trans.m_punishB_id << ", punishC_id : "<< trans.m_punishC_id << LogStream::eos;
		return;
	}

	if (punishType == TPT_C)
	{
		return HandleOnAbNormalByTypeC(trans, punishId);
	}
	else if (punishType == TPT_B) //B类型惩罚,判断交易成功,不需退还道具
	{
		return HandleOnAbNormalByTypeB(trans, punishId);
	}
	else //A类型处罚,判断可能成功,也可能取消交易
	{
		return HandleOnAbNormalByTypeA(trans, punishId);
	}
}

bool AuctionTransRecordMgr::JudgeOvertime(AuctionTransRecord& trans)
{
	zjy_auction::AuctionRecommendPrice* recommendRrices = zjy_auction::AuctionMgr::Instance()->GetRecommendPrice(
		trans.m_item_data_id, trans.m_strengthen, trans.m_beadbuffId, trans.m_enhanceType);

	if (NULL == recommendRrices)
	{
		return false;
	}

	if (recommendRrices->recentPrice.size() == 0)
	{
		return false;
	}
	zjy_auction::AuctionItemRecentPrice& pirce = recommendRrices->recentPrice[recommendRrices->recentPrice.size() - 1];
	UInt32 limit = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_TREAS_OVERTIME);
	if (pirce.timestamp + limit * HOUR_TO_SEC > (UInt32)CURRENT_TIME.Sec())
	{
		return false;
	}
	InfoLogStream << "Handle AbNormal trans, overtime last  guid(" << trans.GetID() << LogStream::eos;

	recommendRrices->recentPrice.clear();
	recommendRrices->price = 0;
	recommendRrices->price_update_time = 0;
	recommendRrices->visibility_price = 0;
	recommendRrices->vis_price_update_time = 0;
	//zjy_auction::AuctionMgr::Instance()->RecordTreasDealPrice(trans, true);
	HandleOnNoAbnormal(trans);

	trans.SetState(THS_OVERTIME_LAST);
	
	return true;
}

bool AuctionTransRecordMgr::GetItemInMailOfBuyer(AuctionTransRecord& trans, WSPlayer& buyer, WSItem*& item)
{
	item = NULL;
	std::function<bool(Mail*)> ff = [&trans, &item](Mail* mail)
	{
		if (mail->titleInfo.id == trans.m_mail_id)
		{
			for (auto mailitem : mail->detailItems)
			{
				if (mailitem && mailitem->GetID() == trans.m_item_id)
				{
					item = new WSItem(*mailitem);
					item->SetID(mailitem->GetID());
				}
			}
			return false;
		}
		return true;
	};
	buyer.GetMailBox().VisitMails(ff);
	return item != NULL ? true : false;
}

void AuctionTransRecordMgr::DeleteItemInMailOfBuyer(AuctionTransRecord& trans, WSPlayer& buyer)
{
	//删除邮件中道具
	std::function<bool(Mail*)> ff = [&trans, &buyer](Mail* mail)
	{
		if (mail->titleInfo.id == trans.m_mail_id)
		{
			for (UInt32 i = 0; i < mail->detailItems.size(); ++i)
			{
				if (mail->detailItems[i]->GetID() == trans.m_item_id)
				{
					WSItem* deleteItem = mail->detailItems[i];
					if (deleteItem)
					{
						InfoLogStream << "delete mailitem that return to seller of auction, auction id : " << trans.m_auction_id << ", mail id : " << mail->titleInfo.id << ", mailitem id : "
							<< deleteItem->GetID() << ", buyer : " << buyer.GetID() << LogStream::eos;
						CLDeleteCommand* deleteCmd = CLRecordClient::Instance()->PrepareDeleteCommand(buyer.GetSrcZoneId(), "t_mailitem", deleteItem->GetID(), buyer.GetID(), WSApplication::Instance()->GetTableCount());
						CLRecordClient::Instance()->SendCommand(deleteCmd, NULL, buyer.GetID());
						CL_SAFE_DELETE(deleteItem);
					}
					mail->detailItems.erase(mail->detailItems.begin() + i);
				}
			}
			return false;
		}
		return true;
	};
	buyer.GetMailBox().VisitMails(ff);
}

//异常处理,判断交易成功
void AuctionTransRecordMgr::HandleOnAbNormalByTypeB(AuctionTransRecord& trans, UInt32 punishId)
{
	InfoLogStream << "handle abnormal trans of type B guid:" << trans.GetID() << ", auction id : " << trans.m_auction_id
		<< ", punishId is : " << punishId << ", seller=" << trans.m_seller << ", buyer=" << trans.m_buyer << LogStream::eos;
	HandleOnAbnormalTransactionSucc(trans, punishId);
	//trans.Update();
}

//异常处理,取消交易或判断交易成功
void AuctionTransRecordMgr::HandleOnAbNormalByTypeA(AuctionTransRecord& trans, UInt32 punishId)
{
	InfoLogStream << "handle abnormal trans of type A guid :" << trans.GetID() << ", auction id : " << trans.m_auction_id
		<< ", punishId is : " << punishId << ", seller=" << trans.m_seller << ", buyer=" << trans.m_buyer << LogStream::eos;
	if (IsCancelTransactionPunish(punishId))
	{
		HandleOnTryCancelTransaction(trans, punishId);
	}
	else{
		HandleOnAbnormalTransactionSucc(trans, punishId);
	}
}

//人工审核结果处理
void AuctionTransRecordMgr::HandleOnAbNormalByTypeC(AuctionTransRecord& trans, UInt32 punishId)
{
	InfoLogStream << "handle abnormal trans of type C guid ：" << trans.GetID() << ", auction id : " << trans.m_auction_id
		<< ", punishId is : " << punishId << ", seller=" << trans.m_seller << ", buyer=" << trans.m_buyer << LogStream::eos;
	if (IsCancelTransactionPunish(punishId))
	{
		HandleOnTryCancelTransaction(trans, punishId);
	}
	else{
		HandleOnAbnormalTransactionSucc(trans, punishId);
	}
}

void AuctionTransRecordMgr::HandleOnAbnormalTransactionSucc(AuctionTransRecord& trans, UInt32 punishId)
{
	InfoLogStream << "handle abnormal trans that determine success guid(" << trans.GetID() << ") punishId is : " << punishId << LogStream::eos;
	trans.PriteState();
	//ObjID_t forzen_playerid = trans.m_seller;  //冻结金币的玩家
	UInt32 frozen_money_mount = 0; //预冻结金币
	//UInt32 forzen_money_type = 0; //预冻结类型
	UInt32 seller_get_money = 0; //卖家获得金币
	UInt32 transaction_amount = trans.m_transaction_amount; //交易总额
	UInt32 counterFee = trans.m_counter_fee; //手续费
	UInt32 transaction_extra_profit = trans.m_transaction_extra_profit; //交易额外获利
	AbnormalTransFreezeObject freezeObj = AT_FREEZE_OBJECT_SELLER;
	UInt8 backWay = 0;
	bool isFrozenPermanet = false;
	if (punishId == TPI_ONE) //交易成功，卖家交易额外获利金币冻结，可付费解除也可通过每日活跃解除
	{
		backWay = ATFROZEN_MONEY_BACK_WAY_CHARGE | ATFROZEN_MONEY_BACK_WAY_DAILY_TASK_SCORE;
		frozen_money_mount = trans.m_transaction_extra_profit;
		if (transaction_amount >= transaction_extra_profit + counterFee)
		{
			seller_get_money = transaction_amount - transaction_extra_profit - counterFee;
		}
		else
		{
			seller_get_money = 0;
		}
		zjy_auction::AuctionMgr::Instance()->RecordCounterFee(counterFee);
	}
	else if (punishId == TPI_TWO) //交易成功，卖家交易全额金币冻结，可付费解除也可通过每日活跃解除
	{
		backWay = ATFROZEN_MONEY_BACK_WAY_CHARGE | ATFROZEN_MONEY_BACK_WAY_DAILY_TASK_SCORE;
		if (transaction_amount >= counterFee)
		{
			frozen_money_mount = transaction_amount - counterFee;
		}
		else
		{
			frozen_money_mount = 0;
		}
		zjy_auction::AuctionMgr::Instance()->RecordCounterFee(counterFee);
	}
	else if (punishId == TPI_FOUR) //交易成功，交易额外获利金币冻结，只可付费解除（代替直接删除的处理，永久冻结）
	{
		backWay = ATFROZEN_MONEY_BACK_WAY_CHARGE;
		isFrozenPermanet = true;
		frozen_money_mount = trans.m_transaction_extra_profit;
		if (transaction_amount >= transaction_extra_profit + counterFee)
		{
			seller_get_money = transaction_amount - transaction_extra_profit - counterFee;
		}
		else
		{
			seller_get_money = 0;
		}
		zjy_auction::AuctionMgr::Instance()->RecordCounterFee(counterFee);
	}
	else if (punishId == TPI_SIX) //交易成功，交易全额金币冻结，只可付费解除（代替直接删除的处理，永久冻结）
	{
		backWay = ATFROZEN_MONEY_BACK_WAY_CHARGE;
		isFrozenPermanet = true;
		if (transaction_amount >= counterFee)
		{
			frozen_money_mount = transaction_amount - counterFee;
		}
		else
		{
			frozen_money_mount = 0;
		}
		zjy_auction::AuctionMgr::Instance()->RecordCounterFee(counterFee);
	}
	else if (punishId == TPI_NONE)
	{
		trans.SetPunishedId(punishId);
		HandleOnNoAbnormal(trans);
		return;
	}
	else{

	}

	//发放交易获得金额
	if (seller_get_money >= 0)
	{
		//SendGetMoneyToSeller(trans, seller_get_money);
	}
	//金币冻结
	//if (frozen_money_mount > 0)
	//{
	AbnormalTransDBInfo info;
	info.buyer = trans.m_buyer;
	info.seller = trans.m_seller;
	info.transTime = trans.m_transaction_time;
	info.transId = trans.m_auction_id;
	info.itemUId = trans.m_item_id;
	info.itemDataId = trans.m_item_data_id;
	info.itemNum = trans.m_buy_num;
	info.moneyType = (MoneyType)trans.m_money_type.GetDBValue();
	info.transAmount = frozen_money_mount;
	info.freezeObj = freezeObj;
	info.backWay = backWay;
	info.isFrozenPermanet = isFrozenPermanet;
	info.isTreasure = (UInt8)(trans.m_is_treas);
	info.mailNotifyType = 0;
	info.unfrozenAmount = seller_get_money;
	info.isNeedRemovePlayerTransMoney = false;
	AbnormalTransHelper::SaveAbnormalTransactionInfo(info);
	//}
	trans.SetPunishedId(punishId);
	InfoLogStream << "set trans state guid(" << trans.GetID() << ") state : " << (UInt32)THS_WEIGUI_SUCC << LogStream::eos;
	trans.SetState(THS_WEIGUI_SUCC); //违规处理成功
	zjy_auction::AuctionMgr::Instance()->RecordTreasDealPrice(trans);
	trans.SendUdpLog();
	UInt32 playerId = (freezeObj == AT_FREEZE_OBJECT_SELLER) ? trans.m_seller : trans.m_buyer;
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(playerId);
	if (player)
	{
		player->NotifySceneQueryWaitHandleAbnormalTrans();
	}
}

/**
*@brief 尝试取消交易
*/
void AuctionTransRecordMgr::HandleOnTryCancelTransaction(AuctionTransRecord& trans, UInt32 punishId)
{
	InfoLogStream << "try cancel transaction guid(" << trans.GetID() << ") punishId is : " << punishId << LogStream::eos;
	//ObjID_t item_id = trans.m_item_id;
	ObjID_t buyer = trans.m_buyer;

	//secne检查道具callback
	OBFUNCTION sceneCkeckItemCb = [&trans, punishId, this](UInt32 reqId, UInt32 res, WSItem* item)
	{
		InfoLogStream << "secne check item cb guid : " << trans.GetID() << ", auction id : " << trans.m_auction_id << ", punishId is : " << punishId 
			<< ", res : " << res << ", trans state : " << (UInt32)trans.GetState() << ", reqId : " << reqId << ",trans.m_check_item_req_id : " << trans.m_check_item_req_id << LogStream::eos;
		if (reqId != trans.GetSceneCheckItemReqId())
		{
			ErrorLogStream << "err reqid not equal guid : " << trans.GetID() << ", auction id : " << trans.m_auction_id << "punish id:" << punishId << LogStream::eos;
			return;
		}
		if (trans.GetState() != THS_HANDLING)
		{
			ErrorLogStream << "trans not in THS_HANDLING  guid : " << trans.GetID() << ", auction id : " << trans.m_auction_id << ", punish id:" << punishId << LogStream::eos;
			return;
		}
		if (res != ErrorCode::SUCCESS)  //道具不能返还
		{
			//执行B类型惩罚,
			DebugLogStream << "call b type guid : " << trans.GetID() << ", auction id : " << trans.m_auction_id << ",punish id:" << punishId << LogStream::eos;
			HandleOnAbNormalByTypeB(trans, trans.m_punishB_id);
		}
		else if (res == ErrorCode::SUCCESS && item) //道具可以返还
		{
			DebugLogStream << "call cancel transaction guid : " << trans.GetID() << ", auction id : " << trans.m_auction_id << ", punish id:" << punishId << LogStream::eos;
			HandleOnCancelTransaction(trans, punishId, item);
		}
	};

	//离线加载玩家查询道具callback
	PlayerOfflineLoadCallBack playerOfflineLoadCb = [&trans, punishId, sceneCkeckItemCb, this](WSPlayer* player)
	{
		InfoLogStream << "loaded player:"<< trans.m_buyer << "trans id:" << trans.GetID() << ") punishId is : " << punishId << LogStream::eos;
		WSItem* item = NULL;
		if (GetItemInMailOfBuyer(trans, *player, item) && item) //邮件中找到
		{
			InfoLogStream << "finded mailitem:" << trans.m_item_id << LogStream::eos;
			//删除邮件中道具
			DeleteItemInMailOfBuyer(trans, *player);
			//取消交易
			HandleOnCancelTransaction(trans, punishId, item);
			//CL_SAFE_DELETE(item);
		}
		else //邮件中未找到
		{
			InfoLogStream << "not finded mailitem:" << trans.m_item_id << ", start sync request scene to check item" << LogStream::eos;
			UInt32 req_id = WSItemCheck::Instance()->RequestOneCheck(CMRT_AUCTION_PUNISH_CANCEL_TRANS, trans.m_buyer, trans.m_item_id, trans.GetID(), sceneCkeckItemCb);
			if (req_id == 0)
			{
				ErrorLogStream << "request scene check item fail req_id = " << req_id << LogStream::eos;
			}
			else
			{
				InfoLogStream << "req_id = " << req_id << LogStream::eos;
				trans.SetSceneCheckItemReqId(req_id);
			}
		}

	};

	//1.先检查道具是否在邮件中为领取
	WSPlayer* palyer_buy = WSPlayerMgr::Instance()->FindPlayer(buyer);
	if (palyer_buy)
	{
		InfoLogStream << "find player" << trans.GetID() << ") punishId is : " << punishId << LogStream::eos;
		WSItem* item = NULL;
		if (GetItemInMailOfBuyer(trans, *palyer_buy, item) && item) //邮件中找到
		{
			InfoLogStream << "find return item" << trans.GetID() << ") punishId is : " << punishId << LogStream::eos;
			//删除邮件中道具
			DeleteItemInMailOfBuyer(trans, *palyer_buy);
			//取消交易
			HandleOnCancelTransaction(trans, punishId, item);
			//CL_SAFE_DELETE(item);
		}
		else //邮件中未找到
		{
			InfoLogStream << "not find return item" << trans.GetID() << ") punishId is : " << punishId << LogStream::eos;
			UInt32 req_id = WSItemCheck::Instance()->RequestOneCheck(CMRT_AUCTION_PUNISH_CANCEL_TRANS, trans.m_buyer, trans.m_item_id, trans.GetID(), sceneCkeckItemCb);
			if (req_id == 0)
			{
				ErrorLogStream << "request scene check item fail req_id = " << req_id << LogStream::eos;
			}
			else
			{
				InfoLogStream << "request scene check item" << trans.GetID() << ") punishId is : " << punishId << ", req_id : " << req_id << LogStream::eos;
				trans.SetSceneCheckItemReqId(req_id);
			}
		}
	}
	else //离线加载玩家
	{
		InfoLogStream << "not find player, off load player" << trans.GetID() << ") punishId is : " << punishId << LogStream::eos;
		WSPlayer* target_temp = new WSPlayer();
		target_temp->SetOfflineReplyCtx(NULL, LT_AUCTION_QUERY_ITEM, playerOfflineLoadCb);
		target_temp->SetID(buyer);
		WSPlayerMgr::Instance()->AddOfflinePlayerCache(target_temp);
		target_temp->LoadData();
	}
}

void AuctionTransRecordMgr::HandleOnCancelTransaction(AuctionTransRecord& trans, UInt32 punishId, WSItem* item)
{
	InfoLogStream << "CancelTransaction guid: " << trans.GetID() << ", auction id:" << trans.m_auction_id << " punishId is : " << punishId << LogStream::eos;
	if (!item)
	{
		return;
	}
	trans.PriteState();
	std::vector<ItemReward> depositItems;
	std::vector<WSItem*> vecitems;
	item->SetAuctionCoolTimeStamp(0);
	vecitems.push_back(item);

	UInt32 frozen_money_mount = 0; //预冻结金币
	//UInt32 forzen_money_type = 0; //预冻结类型
	//ObjID_t forzen_playerid = trans.m_buyer; //冻结金币的玩家
	UInt32 transaction_amount = trans.m_transaction_amount; //交易总额
	AbnormalTransFreezeObject freezeObj = AT_FREEZE_OBJECT_BUYER;
	UInt8 backWay = 0;
	bool isFrozenPermanet = false;
	if (TPI_THRE == punishId) //交易取消，道具和金币各自返还
	{
		//还道具,押金给卖家
		//UInt32 depositItemTypeId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_DEPOSIT_ITEM_ID);
		//depositItems.push_back(ItemReward(depositItemTypeId, trans.m_deposit, false, 0));

		ReturnPayMoneyToBuyer(trans); //返还交易金币给买家
	}
	else if (TPI_FIRE == punishId) //交易取消，道具返还，金币全部冻结，可付费解除也可通过每日活跃解除
	{
		//金币全部冻结,买家
		frozen_money_mount = transaction_amount;
		backWay = ATFROZEN_MONEY_BACK_WAY_CHARGE | ATFROZEN_MONEY_BACK_WAY_DAILY_TASK_SCORE;
	}
	else if (TPI_SVEN == punishId) //交易取消，道具返还，金币全部冻结，只可付费解除（代替直接删除的处理，永久冻结）
	{
		//金币全部冻结,买家
		frozen_money_mount = transaction_amount;
		backWay = ATFROZEN_MONEY_BACK_WAY_CHARGE;
		isFrozenPermanet = true;
	}
	else if (TPI_EIGHT == punishId) //交易取消，道具返还，金币扣除
	{
		//金币扣除,邮件通知买家
		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));

		std::string strSenderName;
		std::string	strTitle;
		std::string	strContent;
		std::string strReason;

		// 发送者
		SysNotifyMgr::Instance()->MakeNotify(9953, buffer, sizeof(buffer));
		strSenderName = buffer;

		SysNotifyMgr::Instance()->MakeNotify(9852, buffer, sizeof(buffer));
		strTitle = buffer;

		SysNotifyMgr::Instance()->MakeNotify(9957, buffer, sizeof(buffer), 
			TimeUtil::TimestampToStandarFormatNoSpace(trans.m_transaction_time), trans.m_transaction_amount);
		strContent = buffer;

		strReason = GetReason(SOURCE_TYPE_ACTION_ADUIT_CANCEL_TRANS, trans.m_auction_id);

		std::vector<WSItem*> vecitemsTemp;
		if (MailBox::SendSysMail(trans.m_buyer, strSenderName, strTitle, strContent, strReason, depositItems, vecitemsTemp) != ErrorCode::SUCCESS)
		{
			// 发送邮件失败
			ErrorLogStream << "send return item of auction abnormal transaction  (" << trans.GetID() << ") mail to role(" << trans.m_buyer << ") title("
				<< strTitle << ") failed." << LogStream::eos;
		}
		{
			InfoLogStream << "send sys mail to remove money of buyer auction id=" << trans.GetID() << ", m_buyer=" << trans.m_buyer << LogStream::eos;
		}
	}
	else{
		CL_SAFE_DELETE(item);
		return;
	}

	//金币冻结
	if (frozen_money_mount > 0)
	{
		AbnormalTransDBInfo info;
		info.buyer = trans.m_buyer;
		info.seller = trans.m_seller;
		info.transTime = trans.m_transaction_time;
		info.transId = trans.m_auction_id;
		info.itemUId = trans.m_item_id;
		info.itemDataId = trans.m_item_data_id;
		info.itemNum = trans.m_buy_num;
		info.moneyType = (MoneyType)trans.m_money_type.GetDBValue();
		info.transAmount = frozen_money_mount;
		info.freezeObj = freezeObj;
		info.backWay = backWay;
		info.isFrozenPermanet = isFrozenPermanet;
		info.isTreasure = (UInt8)(trans.m_is_treas);
		info.mailNotifyType = 1;
		info.unfrozenAmount = 0;
		info.isNeedRemovePlayerTransMoney = false;
		AbnormalTransHelper::SaveAbnormalTransactionInfo(info);
	}
	ReturnItemsToSeller(trans, depositItems, vecitems); //返还物品给卖家
	trans.SetPunishedId(punishId);
	InfoLogStream << "set trans state guid(" << trans.GetID() << ") state : " << (UInt32)THS_WEIGUI_SUCC << LogStream::eos;
	trans.SetState(THS_WEIGUI_SUCC);
	trans.SendUdpLog();
	//trans.Update();
	UInt32 playerId = (freezeObj == AT_FREEZE_OBJECT_SELLER) ? trans.m_seller : trans.m_buyer;
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(playerId);
	if (player)
	{
		player->NotifySceneQueryWaitHandleAbnormalTrans();
	}
}

void AuctionTransRecordMgr::ReturnItemsToSeller(AuctionTransRecord& trans, std::vector<ItemReward>& depositItems, std::vector<WSItem*> vecitems)
{
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	std::string strSenderName;
	std::string	strTitle;
	std::string	strContent;
	std::string strReason;

	// 发送者
	SysNotifyMgr::Instance()->MakeNotify(9953, buffer, sizeof(buffer));
	strSenderName = buffer;

	SysNotifyMgr::Instance()->MakeNotify(9852, buffer, sizeof(buffer));
	strTitle = buffer;

	SysNotifyMgr::Instance()->MakeNotify(9954, buffer, sizeof(buffer), TimeUtil::TimestampToStandarFormatNoSpace(trans.m_transaction_time));
	strContent = buffer;

	strReason = GetReason(SOURCE_TYPE_ACTION_ADUIT_CANCEL_TRANS, trans.m_auction_id);

	if (MailBox::SendSysMail(trans.m_seller, strSenderName, strTitle, strContent, strReason, depositItems, vecitems) != ErrorCode::SUCCESS)
	{
		// 发送邮件失败
		ErrorLogStream << "send return item mail fail guid : " << trans.GetID() << ", auction id : " << trans.m_auction_id << " mail to role(" << trans.m_seller << ") title("
			<< strTitle << ") failed." << LogStream::eos;
		trans.SetReturnedItemMark(0);
	}
	{
		InfoLogStream << "send return item mail succ guid : " << trans.GetID() << ", auction id : " << trans.m_auction_id << ", seller=" << trans.m_seller 
			<< ", itemid:" << trans.m_item_id << LogStream::eos;
	}
	trans.SetReturnedItemMark(1);
}

void AuctionTransRecordMgr::ReturnPayMoneyToBuyer(AuctionTransRecord& trans)
{
	//邮件发送退还交易金额给买家
	std::vector<ItemReward>	vecitems;
	UInt32 uGetMoneyItemId = zjy_auction::AuctionMgr::Instance()->_GetItemTypeIDByPriceType((MoneyType)((UInt32)trans.m_money_type));
	vecitems.push_back(ItemReward(uGetMoneyItemId, trans.m_transaction_amount, false, 0));
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	std::string strSenderName;
	std::string	strTitle;
	std::string	strContent;
	std::string strReason;
	//std::string strengthenStr = "";

	SysNotifyMgr::Instance()->MakeNotify(9953, buffer, sizeof(buffer));
	strSenderName = buffer;
	SysNotifyMgr::Instance()->MakeNotify(9852, buffer, sizeof(buffer));
	strTitle = buffer;

	SysNotifyMgr::Instance()->MakeNotify(9955, buffer, sizeof(buffer), TimeUtil::TimestampToStandarFormatNoSpace(trans.m_transaction_time));
	strContent = buffer;

	strReason = GetReason(SOURCE_TYPE_ACTION_ADUIT_CANCEL_TRANS, trans.m_auction_id);
	if (MailBox::SendSysMail(trans.m_buyer, strSenderName, strTitle, strContent, strReason, vecitems) != ErrorCode::SUCCESS)
	{
		// 发送邮件失败，这里出错就不return了
		ErrorLogStream << "handle no abnormal trans send pay money to buyer mail fail AuctionTransRecord(" << trans.GetID() << "),  auction id("
			<< trans.m_auction_id << "), mail to role(" << trans.m_buyer << ") title(" << strTitle << ") money(" << trans.m_transaction_amount << ") failed." << LogStream::eos;
	}
}

void AuctionTransRecordMgr::SendGetMoneyToSeller(AuctionTransRecord& trans, UInt32 money)
{
	//邮件发送交易获得金币给卖家
	std::vector<ItemReward>	vecitems;
	UInt32 uGetMoneyItemId = zjy_auction::AuctionMgr::Instance()->_GetItemTypeIDByPriceType((MoneyType)((UInt32)trans.m_money_type));
	vecitems.push_back(ItemReward(uGetMoneyItemId, money, false, 0));
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	std::string strSenderName;
	std::string	strTitle;
	std::string	strContent;
	std::string strReason;
	//std::string strengthenStr = "";

	SysNotifyMgr::Instance()->MakeNotify(9953, buffer, sizeof(buffer));
	strSenderName = buffer;
	SysNotifyMgr::Instance()->MakeNotify(9852, buffer, sizeof(buffer));
	strTitle = buffer;
	//SysNotifyMgr::Instance()->MakeNotify(9853, buffer, sizeof(buffer), strengthenStr.c_str(), trans.m_item_name.c_str(), trans.m_buy_num, trans.m_transaction_amount, counterFee, money);
	strContent = buffer;

	strReason = GetReason(SOURCE_TYPE_ACTION_ADUIT_ABNORAML_TRANS_SUCC, trans.m_auction_id);
	if (MailBox::SendSysMail(trans.m_buyer, strSenderName, strTitle, strContent, strReason, vecitems) != ErrorCode::SUCCESS)
	{
		// 发送邮件失败，这里出错就不return了
		ErrorLogStream << "handle no abnormal trans send pay money to buyer mail fail AuctionTransRecord(" << trans.GetID() << "),  auction id("
			<< trans.m_auction_id << "), mail to role(" << trans.m_buyer << ") title(" << strTitle << ") money(" << trans.m_transaction_amount << ") failed." << LogStream::eos;
	}
}

bool AuctionTransRecordMgr::OnLoadUnHandledTransRet(CLRecordCallback *pCallback)
{
	if (pCallback == NULL) return false;
	while (pCallback->NextRow())
	{
		ObjID_t id = pCallback->GetKey();
		if (GetRecord(id) != NULL) continue;	//重复

		AuctionTransRecord* record = new AuctionTransRecord();
		record->SetDBData(pCallback);
		record->SetID(pCallback->GetKey());
		record->ClearDirty();
		this->AddOneRecord(record);
	}
	return true;
}

bool AuctionTransRecordMgr::OnLoadShouldHandleTransRet(CLRecordCallback *pCallback)
{
	if (pCallback == NULL) return false;

	std::vector<AuctionTransRecord*> records;
	while (pCallback->NextRow())
	{
		ObjID_t id = pCallback->GetKey();
		AuctionTransRecord* record = GetRecord(id);
		if (!record)
		{
			AuctionTransRecord* record = new AuctionTransRecord();
			record->SetDBData(pCallback);
			record->SetID(pCallback->GetKey());
			record->ClearDirty();
			this->AddOneRecord(record);
		}
		else if (record && record->GetState() == THS_NONE)
		{
			//record->SetDBData(pCallback);
			record->LoadPushishDataFromDb(pCallback);
		}

		if (record)
		{
			records.push_back(record);
		}
	}

	auto sceneServerID = WSNetwork::Instance()->SelectRandSceneServer();
	if (sceneServerID == 0)
	{
		return false;
	}

	for (auto& trans : records)
	{
		if (!trans) continue;
		UInt32 CurrTime = (UInt32)CURRENT_TIME.Sec();
		//if (!trans || trans->m_verify_end_time > CurrTime || trans->m_state != 0) continue; //未到审核时间或已经处理完成
		if (trans->IsShouldHanlde(CurrTime))
		{
			this->HandleOneTrans(*trans);
		}
		if (trans->GetState() == THS_HANDLING && trans->IsTimeOut(CurrTime))
		{
			InfoLogStream << "set trans state guid(" << trans->GetID() << ") state : " << (UInt32)THS_WEIGUI_HANDLE_TIMEOUT << LogStream::eos;
			trans->SetState(THS_WEIGUI_HANDLE_TIMEOUT);
			trans->SendUdpLog();
		}	
	}
	return true;
}

bool AuctionTransRecordMgr::OnLoadAlreadyTransRet(CLRecordCallback *pCallback)
{
	if (pCallback == NULL) return false;
	while (pCallback->NextRow())
	{
		//ObjID_t id = pCallback->GetKey();

		AuctionTransRecord record;
		record.SetDBData(pCallback);
		record.SetID(pCallback->GetKey());
		record.ClearDirty();
		this->AddOneTransedRecord(record);
	}
	return true;
}

void AuctionTransRecordMgr::GetPlayerTrans(WSPlayer* player, std::vector<AuctionTransaction>& sales, std::vector<AuctionTransaction>& buys)
{
	if (!player)	return;
	UInt32 curtime = (UInt32)CURRENT_TIME.Sec();
	AuctionTransRecordMap::iterator it = m_records.begin();
	for (; it != m_records.end(); ++it)
	{
		if (it->second)
		{
			AuctionTransRecord* trans = it->second;
			if (trans->m_seller == player->GetID() && trans->m_verify_end_time > curtime && trans->m_state == 0)
			{
				AuctionTransaction auctionTrans;
				trans->ToClient(player->GetID(), auctionTrans);
				sales.push_back(auctionTrans);
			}
			/*else if (trans->m_buyer == player->GetID())
			{
				AuctionTransaction auctionTrans;
				trans->ToClient(player->GetID(), auctionTrans);
				buys.push_back(auctionTrans);
			}*/
		}
	}
}

bool AuctionTransRecordMgr::IsCancelTransactionPunish(UInt32 punishId)
{
	return punishId == TPI_THRE || punishId == TPI_FIRE || punishId == TPI_SVEN || punishId == TPI_EIGHT;
}

UInt64 AuctionTransRecordMgr::AddNewTransaction(ObjID_t buyer, zjy_auction::AuctionInfo *auctionInfo, UInt32 num, 
	const std::string& itemName, UInt32 deposit, UInt64 mailId, UInt32 minPriceOnSale, UInt8 isSysRecove, UInt32 MPrice)
{
	if (!auctionInfo) return 0;

	AuctionTransRecord* record = new AuctionTransRecord;
	if (!record)
	{
		ErrorLogStream << "new AuctionTransRecord fail, buyer=" << buyer << "aution guid" << auctionInfo->guid << LogStream::eos;
		return 0;
	}
	UInt32 curr_time = (UInt32)CURRENT_TIME.Sec();
	record->m_auction_id = auctionInfo->guid;
	record->m_is_treas = auctionInfo->isTreas;
	record->m_seller = auctionInfo->owner;
	record->m_buyer = buyer;
	record->m_transaction_time = curr_time;
	record->m_item_id = auctionInfo->item ? auctionInfo->item->GetID() : 0;
	record->m_item_data_id = auctionInfo->itemTypeId;
	record->m_item_num = auctionInfo->num;
	record->m_strengthen = auctionInfo->strengthen;
	record->m_beadbuffId = auctionInfo->beadBuffId;
	record->m_item_name = itemName;
	record->m_buy_num = num;
	record->m_item_score = auctionInfo->itemScore;
	record->m_money_type = auctionInfo->pricetype;
	record->m_transaction_amount = auctionInfo->price;
	//record->m_counter_fee = auctionInfo->counterFee / auctionInfo->num * num;
	record->m_counter_fee = zjy_auction::AuctionMgr::Instance()->GetCounterFeeNew(auctionInfo->ownerVipLev, AT_ITEM, auctionInfo->price);
	record->m_deposit = deposit;
	record->m_transaction_extra_profit = 0;
	record->m_unit_price = auctionInfo->price / auctionInfo->num;
	record->m_mail_id = mailId;
	UInt32 interval = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_VERIFY_INTERVAL_TIAME);
	//if (interval == 0) interval = 20;
	//测试代码
	//interval = 5;
	record->m_verify_end_time = curr_time + interval* 60;
	//record->m_verify_end_time = curr_time + 2*60;
	record->m_program_audit_result = 0;
	record->m_punishA_id = 0;
	record->m_punishB_id = 0;
	record->m_punishC_id = PUNISHC_DEFAUT_ID;
	record->m_item_returned = 0;
	record->m_state = 0;
	record->m_minPriceOnSale = minPriceOnSale;
	record->m_isSysRecove = isSysRecove;
	record->m_enhanceType = auctionInfo->enhanceType;
	record->m_equipType = auctionInfo->equipType;
	record->m_enhanceNum = auctionInfo->enhanceNum;
	record->m_MPrice = MPrice;

	if (!auctionInfo->item || auctionInfo->itemtype != ITEM_EQUIP)
	{
		record->m_mountBeadId = 0;
		record->m_mountMagicCardId = 0;
		record->m_beadAverPrice = 0;
		record->m_magicCardAverPrice = 0;
		record->m_qualitylv = 0;
	}
	else{
		WSItem* item = auctionInfo->item;
		UInt32 beadAverPrice = 0;
		UInt32 magicCardAverPrice = 0;
		UInt32 mountedBeadId = 0;
		UInt32 mountedMagicId = 0;
		UInt32 mountedBeadBuffId = 0;
		UInt8 mountedMagicLv = 0;
		zjy_auction::AuctionMgr::Instance()->GetBeadAndMagicAverPriceForCalcTransEqMPrice(auctionInfo, beadAverPrice, magicCardAverPrice, 
			mountedBeadId, mountedMagicId, mountedBeadBuffId, mountedMagicLv);

		record->m_mountBeadId = mountedBeadId;
		record->m_mountMagicCardId = mountedMagicId;
		record->m_beadAverPrice = beadAverPrice;
		record->m_magicCardAverPrice = magicCardAverPrice;
		record->m_qualitylv = item->m_QualityLv;
		record->m_mountBeadBuffId = mountedBeadBuffId;
		record->m_mountMagicCardLv = mountedMagicLv;
	}
	

	record->Update();
	if (!this->AddOneRecord(record)) return 0;
	return record->GetID();
}

bool AuctionTransRecordMgr::AddOneTransedRecord(const AuctionTransRecord& record)
{
	return false;
	if (record.m_isSysRecove)
	{
		return false;
	}
	auto itemData = ItemDataEntryMgr::Instance()->FindEntry(record.m_item_data_id);
	if (!itemData)
	{
		return false;
	}

	UInt8 strengthen = zjy_auction::AuctionMgr::Instance()->GetStrengthenForAverPrice(*itemData, record.m_strengthen, record.m_enhanceType);

	UInt8 isReadEquip = (record.m_enhanceType > ENHANCE_INVALID) ? 1 : 0;

	zjy_auction::AuctionPricesKey key(record.m_item_data_id, strengthen, record.m_beadbuffId, isReadEquip, 0);
	auto iter = m_transedRecords.find(key);
	if (iter == m_transedRecords.end())
	{
		std::map<UInt64, AuctionTransRecord> tm;
		tm[record.GetID()] = record;
		m_transedRecords[key] = tm;
	}
	else
	{
		iter->second[record.GetID()] = record;
	}
	return true;
}

UInt32 AuctionTransRecordMgr::GetItemTransedRecords(UInt32 itemTypeId, UInt32 strengthen, UInt32 beadBuffId, UInt8 enhanceType, std::vector<AuctionTransaction>& itemTrans)
{
	return ErrorCode::ITEM_DATA_INVALID;
	auto itemData = ItemDataEntryMgr::Instance()->FindEntry(itemTypeId);
	if (!itemData)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	UInt8 strengthen_ = zjy_auction::AuctionMgr::Instance()->GetStrengthenForAverPrice(*itemData, strengthen, enhanceType);
	UInt8 isReadEquip = (enhanceType > ENHANCE_INVALID) ? 1 : 0;
	zjy_auction::AuctionPricesKey key(itemTypeId, strengthen_, beadBuffId, isReadEquip, 0);


	auto iter = m_transedRecords.find(key);
	if (iter == m_transedRecords.end())
	{
		return ErrorCode::SUCCESS;
	}
	else
	{
		UInt32 max = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_QUERY_RECORD_MAX);
		max = (max == 0) ? 10 : max;
		std::vector<UInt64> delIds;
		std::map<UInt64, AuctionTransRecord>& transMap = iter->second;
		UInt32 count = 0;
		auto iter = transMap.rbegin();
		for (; iter != transMap.rend(); iter++)
		{
			AuctionTransRecord& record = iter->second;
			//检查是否满足条件
			UInt32 curr_time = (UInt32)CURRENT_TIME.Sec();
			if (record.m_state != THS_DAOZHUANG)
			{
				delIds.push_back(record.GetID());
				continue;
			}
			if (record.m_isSysRecove)
			{
				delIds.push_back(record.GetID());
				continue;
			}
			if (record.m_transaction_time < curr_time - 30 * 24 * 3600)
			{
				delIds.push_back(record.GetID());
				continue;
			}
			UInt32 averPrice = zjy_auction::AuctionMgr::Instance()->_GetRealAverPrice(itemTypeId, strengthen, beadBuffId, enhanceType);
			if (averPrice == 0)
			{
				averPrice = itemData->recommendPrice;
			}
			UInt32 upLimit = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_QUERY_RECORD_PRICE_UPLIMIT);
			UInt32 lowLimit = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_QUERY_RECORD_PRICE_LOWLIMIT);
			UInt32 upPrice = averPrice * upLimit / 100;
			UInt32 lowPrice = averPrice * lowLimit / 100;
			UInt32 price = 0;
			if (itemData->type == ITEM_EQUIP)
			{
				price = record.m_MPrice;
			}
			else
			{
				price = record.m_transaction_amount;
			}
			if (price >= upPrice || price <= lowPrice)
			{
				continue;
			}
			++count;
			if (count <= max)
			{
				AuctionTransaction itemTran;
				record.ToClient(0, itemTran);
				itemTrans.push_back(itemTran);
			}
		}

		for (auto id : delIds)
		{
			transMap.erase(id);
		}
		return ErrorCode::SUCCESS;
	}
}