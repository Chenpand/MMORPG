#ifndef _CL_LOGIN_DATAENTRY_H_
#define _CL_LOGIN_DATAENTRY_H_

#include "CLDefine.h"
#include <CLItemDefine.h>

#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief ��¼����������
*/
struct LoginPushDataEntry : public Avalon::DataEntry
{
public:
	LoginPushDataEntry();
	~LoginPushDataEntry();
	
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 15; };

public:
	// id
	UInt8				  id;
	// ����
	std::string			  name;
	// �����ȼ�
	UInt16				  unlockLevel;
	// ͼ��·��
	std::string			  iconPath;
	// ����λ��
	std::string			  linkInfo;
	// ��Ч��ʼʱ��
	UInt32				  startTime;
	// ��Ч����ʱ��
	UInt32				  endTime;
	// ������ڼ���(0��Ч)
	UInt32				  daysAfterService;
	// ��������
	UInt32				  continueDays;
	// �������
	std::string			  openInterval;
	// �رռ��
	std::string			  closeInterval;
	// loadingԤ����·��
	std::string			  loadingIconPath;
	// �������
	UInt8				  sortNum;
	// �Ƿ���ʾʱ��
	UInt8				  isShowTime;
	// �Ƿ����ñ���ͼƬԭ������С
	UInt8				  isSetNative;
};

/**
*@brief ��¼���͹�����
*/
class LoginPushDataEntryMgr : public Avalon::DataEntryMgrBase<LoginPushDataEntry>
	, public Avalon::Singleton<LoginPushDataEntryMgr>
{
public:
	LoginPushDataEntryMgr();
	~LoginPushDataEntryMgr();
};

#endif