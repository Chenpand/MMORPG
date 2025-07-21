#ifndef __DAILAY_TODO_H__
#define __DAILAY_TODO_H__

#include <AvalonObjectPool.h>
#include <CLSceneObject.h>
#include "CLDungeonDefine.h"
#include "CLDailyTodoDefine.h"

class DungeonDataEntry;
class DailyTodoItemDataEntry;
class WSPlayer;

class  DailyTodo : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(DailyTodo, Avalon::NullMutex)
public:

	DailyTodo();
	~DailyTodo();

	bool Init(WSPlayer* player, DailyTodoItemDataEntry* dataEntry);

	void Update();
	void Insert();
    void Delete();
	bool LoadFromDb(CLRecordCallback *pCallback, DailyTodoItemDataEntry* dataEntry);
	DailyTodoInfo ToClient();

	UInt8	SubType() { return m_subType; }

	ObjID_t Owner() { return m_OwnerId; }

	UInt32  DataId() { return m_DataId; }

	bool IncDayProgress(UInt32 inc = 1);

	bool UpdateDayProgress(UInt32 progress);

	bool IncWeekProgress(UInt32 inc = 1);

	bool UpdateWeekProgress(UInt32 progress);

	bool CheckDayCompeleted();

	bool CheckWeekCompeleted();

	UInt32 GetDayProgMax() {	return m_DayProgMax; }
public:
	//�����
	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("owner_id", m_OwnerId)
	CL_DBPROPERTY("data_id", m_DataId)
	CL_DBPROPERTY("day_progress", m_DayProgress)
	CL_DBPROPERTY("day_prog_time", m_DayProgTime)
	CL_DBPROPERTY("week_progress", m_WeekProgress)
	CL_DBPROPERTY("week_prog_time", m_WeekProgTime)
	CL_DBPROPERTY_END()
private:
	//ӵ����
	ObjUInt64	m_OwnerId;
	//��id
	ObjUInt32	m_DataId;
	//���Ƽ�����
	ObjUInt32	m_DayProgress;
	//�ս���ʱ���
	ObjUInt32	m_DayProgTime;
	//���Ƽ�����
	ObjUInt32	m_WeekProgress;
	//�ܽ���ʱ���
	ObjUInt32	m_WeekProgTime;

	//����
	UInt8		m_subType;
	//�ս���������
	UInt32		m_DayProgMax;
	//�ܽ���������
	UInt32		m_WeekProgMax;
};

class DailyTodoMgr 
{
	typedef std::map<UInt8, DailyTodo*>	DailyTodoMap;
public:

	DailyTodoMgr();
	~DailyTodoMgr();
	
	//���û�ȡӵ����
	WSPlayer*	GetOwner();

	void	SetOwner(WSPlayer* owner) { m_pOwner = owner; };
	/**
	*@brief ����
	*/
	void LoadDailyTodos();

	/**
	*@brief �������ݷ���
	*/
	void OnLoadDataRet(CLRecordCallback* callback);

	void GetDailyTodoList(std::vector<DailyTodoInfo>& list);

	/**
	*@brief ���ͨ��
	*/
	void OnPlayerClearDungeon(const DungeonDataEntry* dungeonData, LevelScore score, UInt32 usedTime);

	/**
	*@brief �������
	*/
	void OnPlayerTaskUpdate(UInt8 subType, UInt32 value, bool isOver = false);

	void CompleteByGm(UInt8 subType);
protected:
	bool _AddOneDailyTodo(DailyTodo* pTitle);

	DailyTodo* _GetAndCreateDailyTodo(DailyTodoSubType subType);

	DailyTodo* _FindDailyTodo(DailyTodoSubType subType);

private:
	DailyTodoMap	m_DailyTodos;
	//ӵ����
	WSPlayer*		m_pOwner;
};

#endif