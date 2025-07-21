#ifndef _CL_ABNORMAL_TRANS_DEFINE_H_
#define _CL_ABNORMAL_TRANS_DEFINE_H_

#include "CLItemTypeDefine.h"

/*
*@brief �쳣���״���״̬
*/
enum AbnormalTransHandleStatus
{
	// �Ƿ�
	ATHS_INVALID = 0,
	// �ȴ�����
	ATHS_WAIT_FREEZE,
	// ���ᱣ����
	ATHS_FROZEN_TAIL,
	// ���ٷ��������
	ATHS_NEVER_BACK_MONEY,
	// ��ʱ
	ATHS_TIME_OUT,
	// �������
	ATHS_END_HANDLE,
	// �Ѻϲ�
	ATHS_MERGE_OVER,
	// �ֶ�����
	ATHS_MANUAL_END_HANDLE,
};

/*
*@brief �쳣���׶���𷵻�;��
*/
enum AbnormalTransFrozenMoneyBackWay
{
	ATFROZEN_MONEY_BACK_WAY_NONE = 0,
	// ͨ����ֵ����
	ATFROZEN_MONEY_BACK_WAY_CHARGE = 1,
	// ͨ��ÿ�ջ�Ծ�ȷ���
	ATFROZEN_MONEY_BACK_WAY_DAILY_TASK_SCORE = 1 << 1,
};

/*
*@brief �쳣���׶������
*/
enum AbnormalTransFreezeObject
{
	// ��������
	AT_FREEZE_OBJECT_SELLER = 0,
	// �������
	AT_FREEZE_OBJECT_BUYER,
};

/*
*@brief �쳣�������ݿ�洢��Ϣ
*/
struct AbnormalTransDBInfo
{
	AbnormalTransDBInfo()
	{
		buyer = 0;
		seller = 0;
		transTime = 0;
		transId = 0;
		itemUId = 0;
		itemDataId = 0;
		itemNum = 0;
		moneyType = MONEY_INVALID;
		transAmount = 0;
		freezeObj = AT_FREEZE_OBJECT_SELLER;
		backWay = ATFROZEN_MONEY_BACK_WAY_NONE;
		isFrozenPermanet = false;
		isTreasure = true;
		mailNotifyType = 0;
		unfrozenAmount = 0;
		isNeedRemovePlayerTransMoney = false;
	}

	const std::string LogStr() const
	{
		std::ostringstream oss;
		oss << "Abnormal trans DB info(" << transId << ")";
		return oss.str();
	}

	// ���
	ObjID_t buyer;
	// ����
	ObjID_t seller;
	// ����ʱ��
	UInt32 transTime;
	// ����id
	ObjID_t transId;
	// ���׵���uid
	ObjID_t itemUId;
	// ���׵��߱�id
	UInt32 itemDataId;
	// ���׵�������
	UInt32 itemNum;
	// ���׽�����
	MoneyType moneyType;
	// ���׽��
	UInt32 transAmount;
	// �ͷ��������
	AbnormalTransFreezeObject freezeObj;
	// ����𷵻�;��
	UInt8 backWay;
	// �Ƿ����ö���
	bool isFrozenPermanet;
	// �Ƿ���Ʒ
	bool isTreasure;
	// �ʼ�֪ͨ����(0:��֪ͨ,1:��֪ͨ,Ŀǰֻ�н���ȡ���Ĳ���1)
	UInt8 mailNotifyType;
	// δ������
	UInt32 unfrozenAmount;
	// �Ƿ���Ҫ׷�ɿ۳���ҽ��׽�
	bool isNeedRemovePlayerTransMoney;
};

#endif