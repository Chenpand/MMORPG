#ifndef __REPLAY_LIST_H__
#define __REPLAY_LIST_H__

#include <CLDefine.h>
#include <CLRecordCallback.h>
#include <CLReplayDefine.h>
#include <AvalonObjectPool.h>

class ReplayList;
// ¼���б������ɻص�
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

// ¼���б��Ԫ��
struct ReplayListEntry
{
	AVALON_DEFINE_OBJECTPOOL(ReplayListEntry, Avalon::NullMutex)

	ReplayListEntry(ReplayListType type_);
	~ReplayListEntry();

	void SetReplayInfo(ReplayInfoPtr info);

	/**
	*@brief ��ȡ���ݿ�����
	*/
	void SaveToDB();
	void SetFromDB(CLRecordCallback* callback);
	void Del();

	ObjID_t			guid;
	ObjID_t			ownerid;
	// �б����ͣ���Ӧö��ReplayListType��
	UInt8			type;
	ObjID_t			raceId;
	ReplayInfoPtr	info;
};

// ¼���б�״̬
enum ReplayListStatus
{
	// ��ʼ״̬
	REPLAY_LIST_STATUS_INIT,
	// ���ڼ�������
	REPLAY_LIST_STATUS_LOADING,
	// �������
	REPLAY_LIST_STATUS_FINISH,
};

// ¼���б�
class ReplayList
{
	AVALON_DEFINE_OBJECTPOOL(ReplayList, Avalon::NullMutex)
public:
	ReplayList(ObjID_t owner, ReplayListType type);
	~ReplayList();

public:
	/**
	*@brief ��ȡ�б�����
	*/
	ReplayListStatus GetStatus() const { return m_Status; }
	void SetStatus(ReplayListStatus status) { m_Status = status; }

	/**
	*@brief ��ȡ�б�����
	*/
	ReplayListType GetType() const { return m_Type; }

	/**
	*@brief ��ȡӵ����ID
	*/
	ObjID_t GetOwnerID() const { return m_Owner; }

	/**
	*@brief ��ȡԪ��
	*/
	ReplayListEntry* FindEntry(ObjID_t raceid);

	/**
	*@brief ��ȡ���ݿ�����
	*/
	void AddEntry(ReplayListEntry* entry);

	/**
	*@brief ��ȡ���ݿ�����
	*/
	void DelEntry(ReplayListEntry* entry);

	/**
	*@brief ����¼����Ϣ
	*/
	void SetReplayInfo(ReplayInfoPtr info);

	const std::list<ReplayListEntry*>& GetEntrys() const { return m_Entrys; }

	/**
	*@brief ���ݿ���������
	*/
	void IncDBQueryNum() { m_DBQueryNum++; }
	void DecDBQueryNum() { m_DBQueryNum--; }
	bool IsLoadFinish() { return m_DBQueryNum == 0; }

	/**
	*@brief ����ɾ��ʱ��
	*/
	void SetRemoveTime(UInt64 time) { m_RemoveTime = time; }
	UInt64 GetRemoveTime() const { return m_RemoveTime; }

public:
	/**
	*@brief ��ʼ��������
	*/
	void StartLoad(ReplayListCallback* cb);

	/**
	*@brief ��Ӽ��ػص�
	*/
	void AddLoadCallback(ReplayListCallback* cb);

	/**
	*@brief ���سɹ�
	*/
	void OnLoadFinish();

	/**
	*@brief ����ʧ��
	*/
	void OnLoadFailure();

	/**
	*@brief ��ȡ�б���󳤶�
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