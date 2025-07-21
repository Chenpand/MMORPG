#ifndef _CL_SCENE_H_
#define _CL_SCENE_H_

#include "CLGrid.h"


/**
 *@brief 场景
 */
class CLScene
{
public:
	typedef std::vector<CLGrid> GridVector;
	typedef std::vector<UInt32> GridIndexVector;

public:
	CLScene();
	CLScene(const CLScene& scene);
	virtual ~CLScene();

public:
	void Init(UInt32 length, UInt32 width, UInt32 syncRange);

	/**
	 *@brief 设置获取id
	 */
	void SetID(UInt32 id){ m_Id = id; }
	UInt32 GetID() const { return m_Id; }

	/**
	 *@brief 设置获取场景名
	 */
	void SetName(const std::string& name){ m_Name = name; }
	std::string GetName() const { return m_Name; }
	void ScriptSetName(const char* name){ 	if(name == NULL) return; m_Name = name;}
	const char* ScriptGetName() const{ return m_Name.c_str(); }

	/**
	 *@brief 更新obj所在格子
	 *@return 发生改变返回true，否则返回false
	 */
	bool UpdatePosition(CLSceneObject* obj);

	/**
	 *@brief 更新索引
	 */
	bool UpdateGrid(CLSceneObject* obj, UInt32 grid);

	/**
	 *@brief 获取地图长
	 */
	UInt32 GetLength() const{ return m_Length; }

	/**
	 *@brief 获取地图宽
	 */
	UInt32 GetWidth() const{ return m_Width; }

	/**
	 *@brief 获取同步范围
	 */
	UInt32 GetSyncRange() const{ return m_SyncRange; }

	/**
	 *@brief 判断点是否合法
	 */
	bool IsValidPos(const CLPosition& pos);

protected:
	/**
	 *@brief  注册一个obj
	 */
	void RegisterObject(CLSceneObject* obj);

	/**
	 *@brief  注册一个obj
	 */
	void RegisterObject(UInt32 gridIndex,CLSceneObject* obj);

	/**
	 *@brief 注销一个obj
	 */
	void UnRegisterObject(CLSceneObject* obj);

	/**
	 *@brief 遍历地图上的所有玩家
	 */
	bool VisitPlayers(CLSceneObjectVisitor& visitor);

	/**
	 *@brief 遍历指定格子内的玩家
	 */
	bool VisitPlayers(const GridIndexVector& indexs,CLSceneObjectVisitor& visitor);

	/**
	 *@brief 遍历指定格子内的npc
	 */
	bool VisitNpcs(const GridIndexVector& indexs,CLSceneObjectVisitor& visitor);

	/**
	 *@brief 遍历指定格子内除玩家和npc外的物件
	 */
	bool VisitOthers(const GridIndexVector& indexs,CLSceneObjectVisitor& visitor);

	/**
	 *@brief 遍历指定格子内所有场景object
	 */
	bool VisitObjects(const GridIndexVector& indexs,CLSceneObjectVisitor& visitor);

	/**
	 *@brief 获取指定格子内的玩家数
	 */
	UInt32 GetPlayerNum(const GridIndexVector& indexs) const;

	/**
	 *@brief 计算一个位置对应的格子索引
	 */
	UInt32 CalculateGridIndex(const CLPosition& pos) const;
	
	/**
	 *@brief 计算新格子相比旧格子周围新出的格子
	 */
	void GetGridsAroundExcept(UInt32 index1,UInt32 index2,GridIndexVector& indexs) const;

	/**
	 *@brief 获取周围的九个格子
	 */
	const GridIndexVector& GetGridsAround(UInt32 index) const;

private:
	//场景id
	UInt32		m_Id;
	//场景名
	std::string	m_Name;

	//格子列表
	GridVector				m_Grids;
	//1个格子周围9个格子的列表  每个逻辑格子其对应的周围九宫格（边上的格子周围可能没有9个格子）这样就可以很快得到一个SceneObject的视野（也就是其所在格子周围的格子）
	std::vector<GridIndexVector>	m_AroundGrids;
	
	//x方向格子数(逻辑上的格子)
	UInt32		m_GridNumX;	
	//y方向格子数(逻辑上的格子)
	UInt32		m_GridNumY;

	//长 单位是物理格子的数量
	UInt32		m_Length;
	//宽 单位是物理格子的数量
	UInt32		m_Width;
	//同步域大小  noted by aprilliu 逻辑上的格子grid包含多少个物理格子, 玩家的视野是用逻辑格子来表示的（具体就是九宫格）
	UInt32		m_SyncRange;
};

#endif
