#ifndef __REPLAY_LIST_H__
#define __REPLAY_LIST_H__

#include <CLDefine.h>
#include <CLRecordCallback.h>
#include <CLReplayDefine.h>
#include <AvalonObjectPool.h>

class ReplayList;
// 录像列表加载完成回调
class ReplayListCallback
{
public:
	ReplayListCallback(ReplayListType type, ObjID_t ownerid) : m_Type(type), m_Owner(ownerid) {}
	virtual ~ReplayListCallback() {}

	virtual void OnFinish(ReplayList* replayList) = 0;
	virtual void OnFailure() = 0;

protected:
	ReplayListType	m_Type;
	ObjID_t			m_Owner;
};

// 录像列表的元素
struct ReplayListEntry
{
	AVALON_DEFINE_OBJECTPOOL(ReplayListEntry, Avalon::NullMutex)

	ReplayListEntry(ReplayListType type_);
	~ReplayListEntry();

	void SetReplayInfo(ReplayInfoPtr info);

	/**
	*@brief 获取数据库数据
	*/
	void SaveToDB();
	void SetFromDB(CLRecordCallback* callback);
	void Del();

	ObjID_t			guid;
	ObjID_t			ownerid;
	// 列表类型（对应枚举ReplayListType）
	UInt8			type;
	ObjID_t			raceId;
	ReplayInfoPtr	info;
};

// 录像列表状态
enum ReplayListStatus
{
	// 初始状态
	REPLAY_LIST_STATUS_INIT,
	// 正在加载数据
	REPLAY_LIST_STATUS_LOADING,
	// 加载完成
	REPLAY_LIST_STATUS_FINISH,
};

// 录像列表
class ReplayList
{
	AVALON_DEFINE_OBJECTPOOL(ReplayList, Avalon::NullMutex)
public:
	ReplayList(ObjID_t owner, ReplayListType type);
	~ReplayList();

public:
	/**
	*@brief 获取列表类型
	*/
	ReplayListStatus GetStatus() const { return m_Status; }
	void SetStatus(ReplayListStatus status) { m_Status = status; }

	/**
	*@brief 获取列表类型
	*/
	ReplayListType GetType() const { return m_Type; }

	/**
	*@brief 获取拥有者ID
	*/
	ObjID_t GetOwnerID() const { return m_Owner; }

	/**
	*@brief 获取元素
	*/
	ReplayListEntry* FindEntry(ObjID_t raceid);

	/**
	*@brief 获取数据库数据
	*/
	void AddEntry(ReplayListEntry* entry);

	/**
	*@brief 获取数据库数据
	*/
	void DelEntry(ReplayListEntry* entry);

	/**
	*@brief 设置录像信息
	*/
	void SetReplayInfo(ReplayInfoPtr info);

	const std::list<ReplayListEntry*>& GetEntrys() const { return m_Entrys; }

	/**
	*@brief 数据库数据数量
	*/
	void IncDBQueryNum() { m_DBQueryNum++; }
	void DecDBQueryNum() { m_DBQueryNum--; }
	bool IsLoadFinish() { return m_DBQueryNum == 0; }

	/**
	*@brief 设置删除时间
	*/
	void SetRemoveTime(UInt64 time) { m_RemoveTime = time; }
	UInt64 GetRemoveTime() const { return m_RemoveTime; }

public:
	/**
	*@brief 开始加载数据
	*/
	void StartLoad(ReplayListCallback* cb);

	/**
	*@brief 添加加载回调
	*/
	void AddLoadCallback(ReplayListCallback* cb);

	/**
	*@brief 加载成功
	*/
	void OnLoadFinish();

	/**
	*@brief 加载失败
	*/
	void OnLoadFailure();

	/**
	*@brief 获取列表最大长度
	*/
	UInt32 GetMaxEntryNum();

private:
	ReplayListStatus					m_Status;
	ObjID_t								m_Owner;
	ReplayListType						m_Type;
	std::list<ReplayListEntry*>			m_Entrys;
	UInt64								m_RemoveTime;

	UInt32								m_DBQueryNum;
	std::vector<ReplayListCallback*>	m_Callbacks;
};

#endif