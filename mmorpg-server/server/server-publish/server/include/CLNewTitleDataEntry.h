#ifndef _CL_NEW_TITLE_DATAENTRY_H_
#define _CL_NEW_TITLE_DATAENTRY_H_
#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief ͷ�α�������
*/
class NewTitleItemDataEntry : public Avalon::DataEntry
{
public:
	NewTitleItemDataEntry() {};
	~NewTitleItemDataEntry() {};

public:
	virtual UInt32 GetKey() const { return iD; };
	
	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 7; };

public:
	//ΨһID
	UInt32				iD;
	//ͷ��ƴ������
	UInt8				nameType;
	//ӵ��������
	UInt8				ownerType;
	//��Ŀ
	UInt8				category;
	//��Ŀ����
	UInt32				categoryParam;
	//����
	std::string			name;
	//���
	UInt8				style;
};

typedef Avalon::DataEntryVisitor<NewTitleItemDataEntry> NewTitleDataEntryVisitor;

class NewTitleItemDataEntryMgr : public Avalon::DataEntryMgrBase<NewTitleItemDataEntry>
	, public Avalon::Singleton<NewTitleItemDataEntryMgr>
{
public:
	UInt32 GetMasterTitleId();

	UInt32 GetGuidTitleId(UInt8 guidPost);

	UInt32 GetYbtTitleId();

	UInt32 GetYbtTitleIdForSortList(UInt32 sortRanking);
};


#endif