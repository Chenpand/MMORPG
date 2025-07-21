#include "CLScene.h"

CLScene::CLScene()
{
	m_GridNumX = m_GridNumY = 0;
	m_Length = m_Width = 0;
	m_Id = 0;
	m_SyncRange = 3;
}

CLScene::CLScene(const CLScene& scene)
	:m_Id(scene.m_Id), m_Name(scene.m_Name), m_Grids(scene.m_Grids), m_AroundGrids(scene.m_AroundGrids)
	, m_GridNumX(scene.m_GridNumX), m_GridNumY(scene.m_GridNumY), m_Length(scene.m_Length), m_Width(scene.m_Width), m_SyncRange(scene.m_SyncRange)
{
}

CLScene::~CLScene()
{
}

#define CL_GET_RANGE_RECT(Index,MinX,MaxX,MinY,MaxY) \
	UInt32 indexX = Index % m_GridNumX; \
	UInt32 indexY = Index / m_GridNumX; \
	MinX = indexX > 0 ? indexX - 1 : 0; \
	MaxX = indexX < m_GridNumX - 1 ? indexX + 1 : indexX; \
	MinY = indexY > 0 ? indexY - 1 : 0; \
	MaxY = indexY < m_GridNumY - 1 ? indexY + 1 : indexY;


void CLScene::Init(UInt32 length, UInt32 width, UInt32 syncRange)
{

	m_SyncRange = syncRange;
	if(m_SyncRange < 3) m_SyncRange = 3;
	m_GridNumX = (length - 1) / m_SyncRange + 1;
	m_GridNumY = (width - 1) / m_SyncRange + 1;

	UInt32 gridNum = m_GridNumX * m_GridNumY;

	m_Grids.resize(gridNum);
	m_AroundGrids.resize(gridNum);
	UInt32 minX = 0,maxX = 0,minY = 0,maxY = 0;
	for(UInt32 i = 0 ; i < gridNum ; ++i)
	{
		m_Grids[i].SetIndex(i);

		m_AroundGrids[i].reserve(9);
		CL_GET_RANGE_RECT(i,minX,maxX,minY,maxY)
		for(UInt32 indexY = minY ; indexY <= maxY ; ++indexY)
		{
			for(UInt32 indexX = minX ; indexX <= maxX ; ++indexX)
			{
				m_AroundGrids[i].push_back(indexX + indexY * m_GridNumX);
			}
		}
	}

	m_Length = length;
	m_Width = width;
}

bool CLScene::IsValidPos(const CLPosition& pos)
{
	if(pos.x >= m_Length || pos.y >= m_Width) return false;
	return true;
}

void CLScene::RegisterObject(CLSceneObject *obj)
{
	if(obj == NULL || obj->IsInScene()) return;

	UInt32 index = CalculateGridIndex(obj->GetPos());
	m_Grids[index].AddSceneObject(obj);
	obj->SetSceneID(GetID());
}

void CLScene::RegisterObject(UInt32 gridIndex,CLSceneObject* obj)
{
	if(obj == NULL || obj->IsInScene()) return;

	if(gridIndex >= m_Grids.size()) gridIndex = 0;
	m_Grids[gridIndex].AddSceneObject(obj);
	obj->SetSceneID(GetID());
}

void CLScene::UnRegisterObject(CLSceneObject* obj)
{
	if(obj == NULL || obj->GetSceneID() != GetID()) return;
	if(obj->GetGridIndex() >= m_Grids.size()) return;

	m_Grids[obj->GetGridIndex()].RemoveSceneObject(obj);
	obj->SetGridIndex(INVALID_GRID_INDEX);
}

bool CLScene::UpdatePosition(CLSceneObject *obj)
{
	if(obj == NULL || obj->GetSceneID() != GetID()) return false;

	UInt32 newGrid = CalculateGridIndex(obj->GetPos());
	if(obj->GetGridIndex() < m_Grids.size() && newGrid != obj->GetGridIndex())
	{
		m_Grids[obj->GetGridIndex()].RemoveSceneObject(obj);
		m_Grids[newGrid].AddSceneObject(obj);
		return true;
	}
	return false;
}

bool CLScene::UpdateGrid(CLSceneObject* obj, UInt32 newGrid)
{
	if(obj == NULL || obj->GetSceneID() != GetID()) return false;
	if(newGrid >= m_Grids.size()) return false;

	if(obj->GetGridIndex() < m_Grids.size() && newGrid != obj->GetGridIndex())
	{
		m_Grids[obj->GetGridIndex()].RemoveSceneObject(obj);
		m_Grids[newGrid].AddSceneObject(obj);
		return true;
	}
	return false;
}

bool CLScene::VisitPlayers(CLSceneObjectVisitor& visitor)
{
	for(GridVector::iterator iter = m_Grids.begin();iter != m_Grids.end();++iter)
	{
		if(!iter->VisitPlayers(visitor)) return false;
	}
	return true;
}

bool CLScene::VisitPlayers(const CLScene::GridIndexVector &indexs,CLSceneObjectVisitor& visitor)
{
	for(GridIndexVector::const_iterator iter = indexs.begin();iter != indexs.end();++iter)
	{
		if(*iter < m_Grids.size())
		{
			if(!m_Grids[*iter].VisitPlayers(visitor)) return false;
		}
	}
	return true;
}

bool CLScene::VisitNpcs(const CLScene::GridIndexVector &indexs, CLSceneObjectVisitor &visitor)
{
	for(GridIndexVector::const_iterator iter = indexs.begin();iter != indexs.end();++iter)
	{
		if(*iter < m_Grids.size())
		{
			if(!m_Grids[*iter].VisitNpcs(visitor)) return false;
		}
	}
	return true;
}

bool CLScene::VisitOthers(const CLScene::GridIndexVector &indexs, CLSceneObjectVisitor &visitor)
{
	for(GridIndexVector::const_iterator iter = indexs.begin();iter != indexs.end();++iter)
	{
		if(*iter < m_Grids.size())
		{
			if(!m_Grids[*iter].VisitOthers(visitor)) return false;
		}
	}
	return true;
}

bool CLScene::VisitObjects(const CLScene::GridIndexVector &indexs, CLSceneObjectVisitor &visitor)
{
	for(GridIndexVector::const_iterator iter = indexs.begin();iter != indexs.end();++iter)
	{
		if(*iter < m_Grids.size())
		{
			if(!m_Grids[*iter].VisitObjects(visitor)) return false;
		}
	}
	return true;
}

UInt32 CLScene::GetPlayerNum(const CLScene::GridIndexVector &indexs) const
{
	UInt32 num = 0;
	for(GridIndexVector::const_iterator iter = indexs.begin();iter != indexs.end();++iter)
	{
		if(*iter < m_Grids.size())
		{
			num += m_Grids[*iter].GetPlayerNum();
		}
	}
	return num;
}

UInt32 CLScene::CalculateGridIndex(const CLPosition &pos) const
{
	UInt32 indexX = pos.x / m_SyncRange;
	UInt32 indexY = pos.y / m_SyncRange;

	if(indexX >= m_GridNumX) indexX = 0;
	if(indexY >= m_GridNumY) indexY = 0;

	return indexY * m_GridNumX + indexX;
}

void CLScene::GetGridsAroundExcept(UInt32 index1, UInt32 index2, GridIndexVector& indexs) const
{
	if(index1 >= m_AroundGrids.size()) return;
	if(index2 >= m_AroundGrids.size())
	{
		indexs = m_AroundGrids[index1];
		return;
	}

	const GridIndexVector& indexs1 = m_AroundGrids[index1];
	const GridIndexVector& indexs2 = m_AroundGrids[index2];

	GridIndexVector::const_iterator iter1 = indexs1.begin();
	GridIndexVector::const_iterator iter2 = indexs2.begin();

	for(;iter1 != indexs1.end();++iter1)
	{	
		bool bAdd = true;
		for(;iter2 != indexs2.end();++iter2)
		{
			if(*iter1 > *iter2) continue;
			else if(*iter1 == *iter2) bAdd = false;
			break;
		}
		if(bAdd) indexs.push_back(*iter1);
	}
}

const CLScene::GridIndexVector& CLScene::GetGridsAround(UInt32 index) const
{
	static GridIndexVector null;
	if(index >= m_AroundGrids.size()) return null;

	return m_AroundGrids[index];
}







