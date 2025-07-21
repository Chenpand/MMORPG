/**
 *@author	 
 *@email	 
 *@date		2011-04-25		
 *@brief	a星寻路
 */
#ifndef _AVALON_ASTAR_H_
#define _AVALON_ASTAR_H_

#include "AvalonDefine.h"
#include "AvalonObjectPool.h"
#include "AvalonMutex.h"
#include "AvalonPos2D.h"
#include <vector>

namespace Avalon
{
	/**
	 *   A*寻路
	 */
	class AStar
	{
		/**
		 *@brief 节点信息
		 */
		struct GridInfo
		{
			AVALON_DEFINE_OBJECTPOOL(GridInfo, Mutex)

			GridInfo(UInt32 _index, UInt32 _score, UInt32 _cost, GridInfo* _preGrid)
				:index(_index), score(_score), cost(_cost), preGrid(_preGrid){
				bOpen = true;
			}

			UInt32  index;		//格子索引
			UInt32	score;		//估分
			UInt32	cost;		//路径消耗
			bool	bOpen;		//是否在open列表中
			GridInfo*	preGrid;	//路径上上个格子
		};

		typedef std::vector<GridInfo*> GridInfoVec;

		class GridSortFunction
		{
		public:
			bool operator()(const GridInfo* left, const GridInfo* right) const
			{
				return left->score > right->score;
			}
		};

	public:
		AStar();
		AStar(const AStar& astar);
		~AStar();

		/**
		 *@brief 初始化
		 *@param xSize x方向大小
		 *@param ySize y方向大小
		 *@param grids 字符串表示的节点 格式0,3,1,F,...
		 */
		bool Init(UInt32 xSize, UInt32 ySize, const UInt8* grids);

		/**
		 *@brief 寻路
		 *@param startPos 起点格子位置
		 *@param endPos   终点格子位置
		 *@param Pos2D 路径
		 *@param blockFlag 阻挡标识
		 *@return true 成功 false 失败
		 */
		bool FindPath(const Pos2D& startPos, const Pos2D& endPos, std::vector<Pos2D>& path, UInt8 blockMask);


		/**
		 *@brief 设置判断阻挡
		 */
		void SetBlock(const Pos2D& pos, UInt8 mask);
		void UnsetBlock(const Pos2D& pos, UInt8 mask);
		bool CheckBlock(const Pos2D& pos, UInt8 mask) const;

		/**
		 *@brief 判断是否合法位置
		 */
		bool IsValidGrid(const Pos2D& pos) const{ return pos.x < m_xSize && pos.y < m_ySize; }

		/**
		 *@brief 从x,y 获取索引
		 */
		UInt32 GetIndex(UInt32 x, UInt32 y) const { return x * m_ySize + y; }

		/**
		 *@brief 获取x,y大小
		 */
		UInt32 GetXSize() const { return m_xSize; }
		UInt32 GetYSize() const { return m_ySize; }

	private:
		/**
		 *@brief 从索引获取x,y
		 */
		UInt32 GetIndexX(UInt32 index) const{ return index / m_ySize; }
		UInt32 GetIndexY(UInt32 index) const{ return index % m_ySize; }

	private:
		//x大小
		UInt32	m_xSize;
		//y大小
		UInt32	m_ySize;
		//阻挡点信息
		UInt8*	m_Grids;

		//开放节点列表
		GridInfoVec m_OpenList;
	};
}

#endif
