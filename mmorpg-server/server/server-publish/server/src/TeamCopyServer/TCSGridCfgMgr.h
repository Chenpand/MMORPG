#ifndef _TCS_GRID_CFG_MGR_H_
#define _TCS_GRID_CFG_MGR_H_

#include <CLTeamCopyDefine.h>

/**
 *@brief 团本格子配置
 */
struct TCSGridCfg
{
	TCSGridCfg() : gridId(0), x(0), y(0), fieldId(0), type(TC_GRID_OBJ_NULL), block(false) {}

	UInt32 gridId;
	Int32 x;
	Int32 y;
	UInt32 fieldId;
	// 格子类型
	TCGridObjType type;
	// 是否阻挡
	bool block;
	// 参数
	std::vector<UInt32> paramVec;
	std::vector<UInt32> paramVec2;
};

typedef std::map<UInt32, TCSGridCfg*> TCSGridCfgMap;
typedef std::map<TCGridPos, TCSGridCfg*> TCSGridCfgPosMap;

/**
 *@brief 团本格子配置管理
 */
class TCSGridCfgMgr : public Avalon::Singleton<TCSGridCfgMgr>
{
public:
	TCSGridCfgMgr();
	~TCSGridCfgMgr();

	/**
	 *@brief 初始化地图
	 */
	void InitCfg();

	/**
	 *@brief 查找格子
	 */
	TCSGridCfg* FindGrid(UInt32 grade, UInt32 gridId);
	TCSGridCfg* FindGrid(UInt32 grade, const TCGridPos& pos);

	/**
	 *@brief 获取有据点的列表
	 */
	TCSGridCfgMap& GetFieldMap(UInt32 grade) { return m_FieldMap[grade]; }

	/**
	 *@brief 寻路
	 */
	std::list<UInt32> SearchWay(UInt32 grade, UInt32 srcId, UInt32 destId, std::set<UInt32>& blockSet);
	std::list<UInt32> SearchWay(UInt32 grade, UInt32 srcX, UInt32 srcY, UInt32 destX, UInt32 destY, std::set<UInt32>& blockSet);
	
	/**
	 *@brief 获取格子周围的格子
	 */
	std::vector<UInt32> GetRoundGrid(UInt32 grade, UInt32 gridId, UInt32 roundNum = 1);

	/**
	 *@brief 两个格子是否相邻
	 */
	bool GridIsNear(UInt32 grade, UInt32 gridId1, UInt32 gridId2);

	/**
	 *@brief 获取怪物出生的格子
	 */
	void GetMonsterBornGrid(UInt32 grade, std::vector<UInt32>& monsterIndexVec, std::vector<UInt32>& bornVec);

private:

	// 难度->(key->格子id)
	std::map<UInt32, TCSGridCfgMap> m_GridMap;

	//难度->(格子坐标->格子)
	std::map<UInt32, TCSGridCfgPosMap> m_PosMap;

	// 有据点的格子 难度->(key->格子id)
	std::map<UInt32, TCSGridCfgMap> m_FieldMap;
};

#endif // !_TCS_GRID_CFG_MGR_H_

