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
	//编解码
	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("owner_id", m_OwnerId)
	CL_DBPROPERTY("data_id", m_DataId)
	CL_DBPROPERTY("day_progress", m_DayProgress)
	CL_DBPROPERTY("day_prog_time", m_DayProgTime)
	CL_DBPROPERTY("week_progress", m_WeekProgress)
	CL_DBPROPERTY("week_prog_time", m_WeekProgTime)
	CL_DBPROPERTY_END()
private:
	//拥有者
	ObjUInt64	m_OwnerId;
	//表id
	ObjUInt32	m_DataId;
	//日推荐进度
	ObjUInt32	m_DayProgress;
	//日进度时间戳
	ObjUInt32	m_DayProgTime;
	//周推荐进度
	ObjUInt32	m_WeekProgress;
	//周进度时间戳
	ObjUInt32	m_WeekProgTime;

	//类型
	UInt8		m_subType;
	//日进度最大次数
	UInt32		m_DayProgMax;
	//周进度最大次数
	UInt32		m_WeekProgMax;
};

class DailyTodoMgr 
{
	typedef std::map<UInt8, DailyTodo*>	DailyTodoMap;
public:

	DailyTodoMgr();
	~DailyTodoMgr();
	
	//设置获取拥有者
	WSPlayer*	GetOwner();

	void	SetOwner(WSPlayer* owner) { m_pOwner = owner; };
	/**
	*@brief 加载
	*/
	void LoadDailyTodos();

	/**
	*@brief 加载数据返回
	*/
	void OnLoadDataRet(CLRecordCallback* callback);

	void GetDailyTodoList(std::vector<DailyTodoInfo>& list);

	/**
	*@brief 玩家通关
	*/
	void OnPlayerClearDungeon(const DungeonDataEntry* dungeonData, LevelScore score, UInt32 usedTime);

	/**
	*@brief 完成任务
	*/
	void OnPlayerTaskUpdate(UInt8 subType, UInt32 value, bool isOver = false);

	void CompleteByGm(UInt8 subType);
protected:
	bool _AddOneDailyTodo(DailyTodo* pTitle);

	DailyTodo* _GetAndCreateDailyTodo(DailyTodoSubType subType);

	DailyTodo* _FindDailyTodo(DailyTodoSubType subType);

private:
	DailyTodoMap	m_DailyTodos;
	//拥有者
	WSPlayer*		m_pOwner;
};

#endif