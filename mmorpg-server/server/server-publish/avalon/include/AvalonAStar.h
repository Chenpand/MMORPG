/**
 *@author	 
 *@email	 
 *@date		2011-04-25		
 *@brief	a��Ѱ·
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
	 *   A*Ѱ·
	 */
	class AStar
	{
		/**
		 *@brief �ڵ���Ϣ
		 */
		struct GridInfo
		{
			AVALON_DEFINE_OBJECTPOOL(GridInfo, Mutex)

			GridInfo(UInt32 _index, UInt32 _score, UInt32 _cost, GridInfo* _preGrid)
				:index(_index), score(_score), cost(_cost), preGrid(_preGrid){
				bOpen = true;
			}

			UInt32  index;		//��������
			UInt32	score;		//����
			UInt32	cost;		//·������
			bool	bOpen;		//�Ƿ���open�б���
			GridInfo*	preGrid;	//·�����ϸ�����
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
		 *@brief ��ʼ��
		 *@param xSize x�����С
		 *@param ySize y�����С
		 *@param grids �ַ�����ʾ�Ľڵ� ��ʽ0,3,1,F,...
		 */
		bool Init(UInt32 xSize, UInt32 ySize, const UInt8* grids);

		/**
		 *@brief Ѱ·
		 *@param startPos ������λ��
		 *@param endPos   �յ����λ��
		 *@param Pos2D ·��
		 *@param blockFlag �赲��ʶ
		 *@return true �ɹ� false ʧ��
		 */
		bool FindPath(const Pos2D& startPos, const Pos2D& endPos, std::vector<Pos2D>& path, UInt8 blockMask);


		/**
		 *@brief �����ж��赲
		 */
		void SetBlock(const Pos2D& pos, UInt8 mask);
		void UnsetBlock(const Pos2D& pos, UInt8 mask);
		bool CheckBlock(const Pos2D& pos, UInt8 mask) const;

		/**
		 *@brief �ж��Ƿ�Ϸ�λ��
		 */
		bool IsValidGrid(const Pos2D& pos) const{ return pos.x < m_xSize && pos.y < m_ySize; }

		/**
		 *@brief ��x,y ��ȡ����
		 */
		UInt32 GetIndex(UInt32 x, UInt32 y) const { return x * m_ySize + y; }

		/**
		 *@brief ��ȡx,y��С
		 */
		UInt32 GetXSize() const { return m_xSize; }
		UInt32 GetYSize() const { return m_ySize; }

	private:
		/**
		 *@brief ��������ȡx,y
		 */
		UInt32 GetIndexX(UInt32 index) const{ return index / m_ySize; }
		UInt32 GetIndexY(UInt32 index) const{ return index % m_ySize; }

	private:
		//x��С
		UInt32	m_xSize;
		//y��С
		UInt32	m_ySize;
		//�赲����Ϣ
		UInt8*	m_Grids;

		//���Žڵ��б�
		GridInfoVec m_OpenList;
	};
}

#endif
