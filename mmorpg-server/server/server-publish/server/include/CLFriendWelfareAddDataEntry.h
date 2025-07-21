#ifndef  _CL_FRIEND_WELFARE_ADD_DATAENTRY_H_
#define  _CL_FRIEND_WELFARE_ADD_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief ���Ѹ����ӳ�ģ���
*/
class FriendWelfareAddDataEntry : public Avalon::DataEntry
{
public:
	FriendWelfareAddDataEntry(){};
	~FriendWelfareAddDataEntry(){};

public:
	virtual UInt32 GetKey() const { return id; };

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 3; };
public:
	//id
	UInt32 id;
	//���Ѷ�����
	std::vector<Int32>  intimacySpans;
	//����ӳɱ���
	UInt32 expAddProb;
};

typedef Avalon::DataEntryVisitor<FriendWelfareAddDataEntry> FriendWelfareAddDataEntryVisitor;

class FriendWelfareAddDataEntryMgr : public Avalon::DataEntryMgrBase<FriendWelfareAddDataEntry>
	, public Avalon::Singleton<FriendWelfareAddDataEntryMgr>
{
public:
	UInt32 GetExpAddProb(UInt32 intimacy);
};


#endif 