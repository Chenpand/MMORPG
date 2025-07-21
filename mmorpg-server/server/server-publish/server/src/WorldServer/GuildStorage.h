#ifndef __GUILD_STORAGE_H__
#define __GUILD_STORAGE_H__

#include <CLGuildDefine.h>
#include "WSItem.h"
#include "WSPlayer.h"

class Guild;

#define PRE_TASK_OT		5		// �Ųֿ�������ʱʱ��(s)
#define OP_RECORD_BATCH_NUM 5	// ������Ʒ������¼������
#define INIT_GUILD_STORAGE_SZ	80	// ��ʼ������ֿ��С
#define INIT_GUILD_STORAGE_RECORD_SZ	100	// ��ʼ������ֿ������¼��С

enum GuildStorageOpType
{
	GSOT_NONE = 0,
	GSOT_GET = 1,		// ���
	GSOT_PUT = 2,		// ����
	GSOT_BUYPUT = 3		// ���򲢴���
};

enum ProcTaskState
{
	PALS_UNPROC = 0,		// δ����
	PALS_WAITING = 1,		// �ȴ�����
	PALS_DONE = 2,			// ��ɴ���
};

enum ProcTaskType
{
	PTT_ADD = 0,		// ����
	PTT_GET = 1,		// ȡ��
};

class ProcTaskCb
{
public:
	ProcTaskCb(){}
	virtual ~ProcTaskCb(){}
};

class AddGuildStorageItemCb : public ProcTaskCb
{
public:
	AddGuildStorageItemCb(){}
	~AddGuildStorageItemCb(){}

	virtual void OnSuccess(std::vector<GuildStorageItemInfo>& items, std::vector<GuildStorageItemInfo>& recordItems){};
	virtual void OnFailed(UInt32 code){};
};

class GetGuildStorageItemCb : public ProcTaskCb
{
public:
	GetGuildStorageItemCb(){}
	~GetGuildStorageItemCb(){}

	virtual void OnSuccess(WSItem* item, GuildStorageItemInfo& itemInfo){};
	virtual void OnFailed(UInt32 code){};
};

class GuildStorageTaskInfo
{
public:
	~GuildStorageTaskInfo()
	{
		CL_SAFE_DELETE(cb);
	}

	ProcTaskType						type;			// ��������
	UInt64								playerUid;		// ���uid
	UInt32								time;			// ��ӵ�ʱ��
	ProcTaskState						state;			// ����״̬
	UInt32								code;			// �����
	ProcTaskCb*							cb;				// ��ɻص�����
};

class AddItemTask : public GuildStorageTaskInfo
{
public:
	AddItemTask()
	{
		type = PTT_ADD;
	}

	std::vector<GuildStorageItemInfo>	items;			// ��ӵĵ���
};

struct GetItemTask : public GuildStorageTaskInfo
{
public:
	GetItemTask()
	{
		type = PTT_GET;
	}

	UInt32								index;			// ��õ����
	UInt16								num;			// ��õ�����
};

class GuildStorage
{
public:
	GuildStorage();
	~GuildStorage();

public:
	bool Init(Guild* guild);

	void LoadData();

	// ���زֿ����
	void LoadItems(CLRecordCallback* cb);

	// ���ز�����¼
	void LoadOpRecords(CLRecordCallback* cb);

	// ��ֿ�
	void AddItem(WSPlayer* player, std::vector<GuildStorageItemInfo>& inItems, AddGuildStorageItemCb* cb);

	// ɾ���ֿ����
	bool DelItem(UInt32 index, UInt16 num);

	// ɾ���ֿ����by uid
	bool DelItem(UInt64 uid, UInt16 num, GuildStorageItemInfo& info);

	// ȡ�ֿ⣨�齱��
	void GetItem(WSPlayer* player, UInt32 index, UInt16 num, GetGuildStorageItemCb* cb);

	// ��ѯ�ֿ�
	std::vector<GuildStorageItemInfo> QueryItems();

	// ����ܿռ�
	UInt32 GetTotalSize();

	// ��õ�ǰ��Ʒ����
	UInt32 GetSize();

	// ���ʣ��ռ�
	UInt32 GetRestSize();

	// �����������id
	UInt64 GetGuildId();

	// ��ѯ������¼
	std::vector<GuildStorageOpRecord>& QueryOpRecord();

	void OnTick();

	// ʵ�ʷŲֿ�
	void RealAddItem(UInt64 playerUid, UInt32 code, std::vector<WSItem*>& items);

	// ȡ�����
	void RealGetItem(UInt64 playerUid, WSItem* item, UInt64 oldUid);

	// ���������¼
	void InsertOpRecord(std::string name, GuildStorageOpType type, std::vector<GuildStorageItemInfo>& items, std::vector<GuildStorageOpRecord>& records);

	// ��ѯ�ֿ��������
	WSItem* QueryItemInfo(UInt64 uid);

	//scene->world�������ֿ� 
	bool ShopBuyAddItem(WSPlayer* player, WSItem* item);

	// �������
	bool CheckCapacity(WSItem* item);
private:
	// �����������
	void _ProcTask();

	void _ProcAddTask(WSPlayer* player, AddItemTask* task);
	void _ProcGetTask(WSPlayer* player, GetItemTask* task);

	// ɾ�����ڵ�����
	void _DelOverTimePreAddTask();

	// �������,���˳����Է���ĵ���
	bool _CheckCapacity(std::vector<GuildStorageItemInfo>& inItems, std::vector<GuildStorageItemInfo>& outItems);

	// ��ӵ�DB
	void _AddItem(WSItem* item);

	// ��DBɾ��
	void _DeleteItem(WSItem* item);

	// ����DB
	void _UpdateItem(WSItem* item);

	// wsitem 2 GuildStorageItemInfo
	GuildStorageItemInfo _TransItemInfo(WSItem* item);

	void _InsertOpRecord(std::string name, GuildStorageOpType type, std::vector<GuildStorageItemInfo>& items, std::vector<GuildStorageOpRecord>& records);
private:

	UInt64					m_guildId;

	// �ֿ����
	std::vector<WSItem*>	m_items;

	// ���������б�
	std::vector<GuildStorageTaskInfo*>	m_procTasks;

	// �ֿ��С
	UInt32					m_size;

	// ������¼
	std::vector<GuildStorageOpRecord>	m_opRecords;
};


#endif