#include "AvalonAStar.h"
#include <map>
#include <algorithm>

//横向行走消耗
#define COST_STRAIGHT 10
//斜向行走消耗
#define COST_DIAGONAL 14
//最大尝试次数
#define MAX_TRY	5000

namespace Avalon
{
	AStar::AStar()
	{
		m_xSize = m_ySize = 0;
		m_Grids = NULL;
	}

	AStar::AStar(const AStar& astar)
	{
		m_xSize = astar.m_xSize;
		m_ySize = astar.m_ySize;
		m_Grids = new UInt8[m_xSize * m_ySize];
		memcpy(m_Grids, astar.m_Grids, m_xSize * m_ySize);
	}

	AStar::~AStar()
	{
		delete[] m_Grids;
		m_Grids = NULL;
	}

	bool AStar::Init(UInt32 xSize, UInt32 ySize, const UInt8* grids)
	{
		if(grids == NULL || xSize == 0 || ySize == 0) return false;

		m_xSize = xSize;
		m_ySize = ySize;

		UInt32 num = m_xSize * m_ySize;
		delete[] m_Grids;
		m_Grids = new UInt8[num];
		memcpy(m_Grids, grids, num);
		
		return true;
	}

	bool AStar::FindPath(const Pos2D& startPos, const Pos2D& endPos, std::vector<Pos2D>& path, UInt8 blockMask)
	{
		if(!IsValidGrid(startPos) || !IsValidGrid(endPos)) return false;
		if(CheckBlock(endPos, blockMask)) return false;

		UInt32 startIndex = GetIndex(startPos.x, startPos.y);
		UInt32 endIndex = GetIndex(endPos.x, endPos.y);


		GridInfo* startGrid = new GridInfo(startIndex, 0, 0, NULL);
		
		std::map<UInt32,GridInfo*>	gridMap;
		gridMap[startIndex] = startGrid;

		m_OpenList.clear();
		m_OpenList.push_back(startGrid);

		GridInfo* curGrid = NULL;
		GridSortFunction func;

		UInt32 curTry = 0;
		while(!m_OpenList.empty())
		{
			if(++curTry > MAX_TRY)
			{
				curGrid = NULL;
				break;
			}

			//取出最小估分节点
			std::pop_heap(m_OpenList.begin(), m_OpenList.end(), func);
			std::vector<GridInfo*>::iterator endIter = --m_OpenList.end();
			curGrid = *endIter;
			m_OpenList.erase(endIter);
			curGrid->bOpen = false;

			//到达终点
			if(curGrid->index == endIndex) break;

			UInt32 curX = GetIndexX(curGrid->index);
			UInt32 curY = GetIndexY(curGrid->index);
			//遍历周围的9个点
			for(Int32 offsetX = -1; offsetX <= 1; ++offsetX)
			{
				for(Int32 offsetY = -1; offsetY <= 1; ++offsetY)
				{
					if(Int32(curX) + offsetX < 0 || Int32(curY) + offsetY < 0) continue;

					Pos2D pos;
					pos.x = UInt16(Int32(curX) + offsetX);
					pos.y = UInt16(Int32(curY) + offsetY);
					//位置不合法 或者 有阻挡
					if(!IsValidGrid(pos) || CheckBlock(pos, blockMask)) continue;

					UInt32 cost = curGrid->cost + ((pos.x == curX || pos.y == curY) ? COST_STRAIGHT : COST_DIAGONAL);
					UInt32 score = cost + std::abs(Int32(endPos.x) - Int32(pos.x)) + std::abs(Int32(endPos.y) - Int32(pos.y));

					UInt32 index = GetIndex(pos.x, pos.y);
					std::map<UInt32,GridInfo*>::iterator iter = gridMap.find(index);
					if(iter != gridMap.end())
					{
						GridInfo* grid = iter->second;
						if(!grid->bOpen) continue; //已经关闭

						if(cost < grid->cost)
						{
							grid->cost = cost;
							grid->score = score;
							grid->preGrid = curGrid;
							std::make_heap(m_OpenList.begin(), m_OpenList.end(), func);
						}
					}
					else
					{
						GridInfo* grid = new GridInfo(index, score, cost, curGrid);
						gridMap[index] = grid;
						m_OpenList.push_back(grid);
						std::push_heap(m_OpenList.begin(), m_OpenList.end(), func);
					}
				}
			}
		}

		std::vector<UInt32> indexPath;
		//取路径
		while(curGrid != NULL)
		{
			indexPath.push_back(curGrid->index);
			curGrid = curGrid->preGrid;
		}
		
		bool bFind = !indexPath.empty();
		for(std::vector<UInt32>::reverse_iterator iter = indexPath.rbegin();
			iter != indexPath.rend(); ++iter)
		{
			path.push_back(Pos2D(GetIndexX(*iter), GetIndexY(*iter)));
		}

		for(std::map<UInt32,GridInfo*>::iterator iter = gridMap.begin()
			; iter != gridMap.end(); ++iter)
		{
			delete iter->second;
		}
		gridMap.clear();
		m_OpenList.clear();

		return bFind;
	}

	void AStar::SetBlock(const Pos2D& pos, UInt8 mask)
	{
		if(!IsValidGrid(pos)) return;
		UInt32 index = GetIndex(pos.x, pos.y);
		m_Grids[index] = m_Grids[index] | mask;
	}

	void AStar::UnsetBlock(const Pos2D& pos, UInt8 mask)
	{
		if(!IsValidGrid(pos)) return;
		UInt32 index = GetIndex(pos.x, pos.y);
		m_Grids[index] = m_Grids[index] & (~mask);
	}

	bool AStar::CheckBlock(const Pos2D& pos, UInt8 mask) const
	{
		if(!IsValidGrid(pos)) return true;
		UInt32 index = GetIndex(pos.x, pos.y);
		return ( m_Grids[index] & mask ) != 0;
	}
}

