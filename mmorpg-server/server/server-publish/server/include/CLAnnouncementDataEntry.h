#ifndef _CL_ANNOUNCEMENT_DATAENTRY_H_
#define _CL_ANNOUNCEMENT_DATAENTRY_H_

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief ϵͳ���� 
* ԭ����SysNotifyDataEntry,����������.���Ըĵ�
*/

enum AnnouncementSyncEvent
{
	SyncGuildLog = 1 << 1,//ͬ��������־
};
class AnnouncementDataEntry : public Avalon::DataEntry
{
public:
	AnnouncementDataEntry();
	~AnnouncementDataEntry();

	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow &row);

public:
	//ID
	UInt32	id;

	//�ײ�ʱ�䣨�룩
	//UInt32	firstTime;

	//����
	std::string content;

	//���ż�����룩
	UInt32	interval;

	//���Ŵ���
	UInt32	times;

	//����
	UInt32	param;

	//�Ƿ�ͬ����������־
	UInt64	SyncEventBit;

};

typedef Avalon::DataEntryVisitor<AnnouncementDataEntry> AnnouncementDataEntryVisitor;
typedef Avalon::DataEntryMgr<AnnouncementDataEntry> AnnouncementDataEntryMgr;

#endif
