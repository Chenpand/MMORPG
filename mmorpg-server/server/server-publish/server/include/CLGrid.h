#ifndef _CL_GRID_H_
#define _CL_GRID_H_

#include "CLDefine.h"
#include "CLSceneObject.h"

class CLSceneObject;

/**
 *@brief 同步域计算格子
 */
class CLGrid
{
public:
	const static UInt32 MAX_VISIT_DEPTH = 5;

public:
	CLGrid();
	CLGrid(const CLGrid& grid);
	~CLGrid();

	void SetIndex(UInt32 index);

	/**
	 *@brief 添加一个
	 */
	void AddSceneObject(CLSceneObject* obj);

	/**
	 *@brief 删除一个
	 */
	void RemoveSceneObject(CLSceneObject* obj);

	/**
	 *@brief 访问所有npc
	 */
	bool VisitNpcs(CLSceneObjectVisitor& visitor);

	/**
	 *@brief 访问所有玩家
	 */
	bool VisitPlayers(CLSceneObjectVisitor& visitor);

	/**
	 *@brief 访问所有除npc和玩家外的其他object
	 */
	bool VisitOthers(CLSceneObjectVisitor& visitor);

	/**
	 *@brief 访问所有obj
	 */
	bool VisitObjects(CLSceneObjectVisitor& visitor);

	/**
	 *@brief 获取格子中玩家数
	 */
	UInt32 GetPlayerNum() const{ return m_PlayerNum; }

private:
	void AddToList(CLSceneObject*& list,CLSceneObject* obj);

	void RemoveFromList(CLSceneObject*& list,CLSceneObject* obj);

private:
	//npc列表
	CLSceneObject* m_pNpcs;
	//玩家列表
	CLSceneObject* m_pPlayers;
	//其他列表
	CLSceneObject* m_pOthers;

	//备份指针列表
	CLSceneObject* m_ObjIters[MAX_VISIT_DEPTH];
	//遍历深度
	UInt32		   m_VisitDepth;

	//玩家数
	UInt32	m_PlayerNum;

	//索引
	UInt32	m_Index;
};

#endif
