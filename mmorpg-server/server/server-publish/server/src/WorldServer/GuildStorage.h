#ifndef __GUILD_STORAGE_H__
#define __GUILD_STORAGE_H__

#include <CLGuildDefine.h>
#include "WSItem.h"
#include "WSPlayer.h"

class Guild;

#define PRE_TASK_OT		5		// 放仓库任务处理超时时间(s)
#define OP_RECORD_BATCH_NUM 5	// 批量物品操作记录最大个数
#define INIT_GUILD_STORAGE_SZ	80	// 初始化公会仓库大小
#define INIT_GUILD_STORAGE_RECORD_SZ	100	// 初始化公会仓库操作记录大小

enum GuildStorageOpType
{
	GSOT_NONE = 0,
	GSOT_GET = 1,		// 获得
	GSOT_PUT = 2,		// 存入
	GSOT_BUYPUT = 3		// 购买并存入
};

enum ProcTaskState
{
	PALS_UNPROC = 0,		// 未处理
	PALS_WAITING = 1,		// 等待处理
	PALS_DONE = 2,			// 完成处理
};

enum ProcTaskType
{
	PTT_ADD = 0,		// 存入
	PTT_GET = 1,		// 取出
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

	ProcTaskType						type;			// 任务类型
	UInt64								playerUid;		// 玩家uid
	UInt32								time;			// 添加的时间
	ProcTaskState						state;			// 处理状态
	UInt32								code;			// 结果码
	ProcTaskCb*							cb;				// 完成回掉对象
};

class AddItemTask : public GuildStorageTaskInfo
{
public:
	AddItemTask()
	{
		type = PTT_ADD;
	}

	std::vector<GuildStorageItemInfo>	items;			// 添加的道具
};

struct GetItemTask : public GuildStorageTaskInfo
{
public:
	GetItemTask()
	{
		type = PTT_GET;
	}

	UInt32								index;			// 获得的序号
	UInt16								num;			// 获得的数量
};

class GuildStorage
{
public:
	GuildStorage();
	~GuildStorage();

public:
	bool Init(Guild* guild);

	void LoadData();

	// 加载仓库道具
	void LoadItems(CLRecordCallback* cb);

	// 加载操作记录
	void LoadOpRecords(CLRecordCallback* cb);

	// 存仓库
	void AddItem(WSPlayer* player, std::vector<GuildStorageItemInfo>& inItems, AddGuildStorageItemCb* cb);

	// 删除仓库道具
	bool DelItem(UInt32 index, UInt16 num);

	// 删除仓库道具by uid
	bool DelItem(UInt64 uid, UInt16 num, GuildStorageItemInfo& info);

	// 取仓库（抽奖）
	void GetItem(WSPlayer* player, UInt32 index, UInt16 num, GetGuildStorageItemCb* cb);

	// 查询仓库
	std::vector<GuildStorageItemInfo> QueryItems();

	// 获得总空间
	UInt32 GetTotalSize();

	// 获得当前物品数量
	UInt32 GetSize();

	// 获得剩余空间
	UInt32 GetRestSize();

	// 获得所属公会id
	UInt64 GetGuildId();

	// 查询操作记录
	std::vector<GuildStorageOpRecord>& QueryOpRecord();

	void OnTick();

	// 实际放仓库
	void RealAddItem(UInt64 playerUid, UInt32 code, std::vector<WSItem*>& items);

	// 取出完成
	void RealGetItem(UInt64 playerUid, WSItem* item, UInt64 oldUid);

	// 插入操作记录
	void InsertOpRecord(std::string name, GuildStorageOpType type, std::vector<GuildStorageItemInfo>& items, std::vector<GuildStorageOpRecord>& records);

	// 查询仓库道具详情
	WSItem* QueryItemInfo(UInt64 uid);

	//scene->world存道具入仓库 
	bool ShopBuyAddItem(WSPlayer* player, WSItem* item);

	// 检查容量
	bool CheckCapacity(WSItem* item);
private:
	// 处理任务队列
	void _ProcTask();

	void _ProcAddTask(WSPlayer* player, AddItemTask* task);
	void _ProcGetTask(WSPlayer* player, GetItemTask* task);

	// 删除过期的任务
	void _DelOverTimePreAddTask();

	// 检查容量,过滤出可以放入的道具
	bool _CheckCapacity(std::vector<GuildStorageItemInfo>& inItems, std::vector<GuildStorageItemInfo>& outItems);

	// 添加到DB
	void _AddItem(WSItem* item);

	// 从DB删除
	void _DeleteItem(WSItem* item);

	// 更新DB
	void _UpdateItem(WSItem* item);

	// wsitem 2 GuildStorageItemInfo
	GuildStorageItemInfo _TransItemInfo(WSItem* item);

	void _InsertOpRecord(std::string name, GuildStorageOpType type, std::vector<GuildStorageItemInfo>& items, std::vector<GuildStorageOpRecord>& records);
private:

	UInt64					m_guildId;

	// 仓库道具
	std::vector<WSItem*>	m_items;

	// 处理任务列表
	std::vector<GuildStorageTaskInfo*>	m_procTasks;

	// 仓库大小
	UInt32					m_size;

	// 操作记录
	std::vector<GuildStorageOpRecord>	m_opRecords;
};


#endif