#ifndef __ACCOUNT_CHARGE_RECORD_H__
#define __ACCOUNT_CHARGE_RECORD_H__

#include <CLDefine.h>
#include <sstream>
#include <CLSceneObject.h>
#include <CLObjectProperty.h>
#include "AccountChargeElem.h"
#include "ChargeManager.h"

class AcccountChargeRecord
{
public:
	AcccountChargeRecord();
	virtual ~AcccountChargeRecord();

public:
	
	/**
	*@brief 总次数（roleId为0就代表整个账号的次数）
	*/
	UInt32 GetChargeTimes(ChargeMallType mallType, UInt32 goodsId, ObjID_t roleId);

	/**
	*@brief 今日次数（roleId为0就代表整个账号的次数）
	*/
	UInt32 GetTodayChargeTimes(ChargeMallType mallType, UInt32 goodsId, ObjID_t roleId);

	/**
	*@brief 增加充值次数
	*/
	void IncChargeTimes(UInt32 accid, ObjID_t roleId, ChargeMallType mallType, UInt32 goodsId, UInt32 times);

	/**
	*@brief 获取记录
	*/
	AccountChargeElem* FindRecord(ChargeMallType mallType, UInt32 goodsId, ObjID_t roleId);

	/**
	*@brief 获取记录
	*/
	void AddRecord(AccountChargeElem* record);

private:
	std::vector<AccountChargeElem*>		m_Records;
};

#endif