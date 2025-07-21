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
	*@brief �ܴ�����roleIdΪ0�ʹ��������˺ŵĴ�����
	*/
	UInt32 GetChargeTimes(ChargeMallType mallType, UInt32 goodsId, ObjID_t roleId);

	/**
	*@brief ���մ�����roleIdΪ0�ʹ��������˺ŵĴ�����
	*/
	UInt32 GetTodayChargeTimes(ChargeMallType mallType, UInt32 goodsId, ObjID_t roleId);

	/**
	*@brief ���ӳ�ֵ����
	*/
	void IncChargeTimes(UInt32 accid, ObjID_t roleId, ChargeMallType mallType, UInt32 goodsId, UInt32 times);

	/**
	*@brief ��ȡ��¼
	*/
	AccountChargeElem* FindRecord(ChargeMallType mallType, UInt32 goodsId, ObjID_t roleId);

	/**
	*@brief ��ȡ��¼
	*/
	void AddRecord(AccountChargeElem* record);

private:
	std::vector<AccountChargeElem*>		m_Records;
};

#endif