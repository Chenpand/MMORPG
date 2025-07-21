#include "TCSGridCfgMgr.h"

#include <queue>
#include <CLTeamCopyDefine.h>
#include <CLTeamCopyMapDataEntry.h>
#include <CLTeamCopyGridParamDataEntry.h>
#include <CLTeamCopyGridMonsterDataEntry.h>
#include "TCSApplication.h"

TCSGridCfgMgr::TCSGridCfgMgr()
{

}

TCSGridCfgMgr::~TCSGridCfgMgr()
{

}

void TCSGridCfgMgr::InitCfg()
{
	// ��ͼ�������ݳ�ʼ��
	class TeamCopyMapEntryVisitor : public Avalon::DataEntryVisitor<TeamCopyMapDataEntry>
	{
	public:
		TeamCopyMapEntryVisitor(UInt32 grade, TCSGridCfgMap* gridMap, TCSGridCfgPosMap* posMap, TCSGridCfgMap* fieldMap)
			: m_Grade(grade), m_GridMap(gridMap), m_PosMap(posMap), m_FieldMap(fieldMap) {}

		bool operator()(TeamCopyMapDataEntry* entry)
		{
			TeamCopyGridDataEntry* gridEntry = TeamCopyGridDataEntryMgr::Instance()->FindEntry(entry->sourceId);
			if (NULL == gridEntry)
			{
				ErrorLogStream << "team copy grid table not find id:" << entry->gridId << LogStream::eos;
				TCSApplication::Instance()->Exit();
			}

			TCSGridCfg* gridCfg = new TCSGridCfg();
			gridCfg->gridId = entry->gridId;
			gridCfg->x = entry->x;
			gridCfg->y = entry->y;
			gridCfg->fieldId = entry->fieldId;
			gridCfg->type = TCGridObjType(gridEntry->gridType);
			gridCfg->block = (gridEntry->IsLink != 0);
			gridCfg->paramVec = TeamCopyGridParamDataEntryMgr::Instance()->GetParam(m_Grade, entry->sourceId);
			gridCfg->paramVec2 = TeamCopyGridParamDataEntryMgr::Instance()->GetParam2(m_Grade, entry->sourceId);
			(*m_GridMap)[gridCfg->gridId] = gridCfg;

			TCGridPos pos(entry->x, entry->y);
			(*m_PosMap)[pos] = gridCfg;

			if (gridEntry->gridType != TC_GRID_OBJ_NULL)
			{
				(*m_FieldMap)[entry->gridId] = gridCfg;
			}

			return true;
		}

	private:
		UInt32 m_Grade;
		TCSGridCfgMap* m_GridMap;
		TCSGridCfgPosMap* m_PosMap;
		TCSGridCfgMap* m_FieldMap;
	};

	// ��ͨ�Ѷȵ�ͼ
	UInt32 grade = TEAM_COPY_TEAM_GRADE_COMMON;
	TeamCopyMapEntryVisitor mapVisitor(grade, &m_GridMap[grade], &m_PosMap[grade], &m_FieldMap[grade]);
	TeamCopyMapDataEntryMgr::Instance()->Visit(mapVisitor);

	// ج���Ѷȵ�ͼ
	grade = TEAM_COPY_TEAM_GRADE_DIFF;
	TeamCopyMapEntryVisitor mapVisitor2(grade, &m_GridMap[grade], &m_PosMap[grade], &m_FieldMap[grade]);
	TeamCopyMapDataEntryMgr2::Instance()->Visit(mapVisitor2);
}

TCSGridCfg* TCSGridCfgMgr::FindGrid(UInt32 grade, UInt32 gridId)
{
	auto iter = m_GridMap[grade].find(gridId);
	return iter != m_GridMap[grade].end() ? iter->second : NULL;
}

TCSGridCfg* TCSGridCfgMgr::FindGrid(UInt32 grade, const TCGridPos& pos)
{
	auto iter = m_PosMap[grade].find(pos);
	return iter != m_PosMap[grade].end() ? iter->second : NULL;
}

std::list<UInt32> TCSGridCfgMgr::SearchWay(UInt32 grade, UInt32 srcId, UInt32 destId, std::set<UInt32>& blockSet)
{
	std::list<UInt32> pathList, emptyList;

	auto sGrid = FindGrid(grade, srcId);
	auto eGrid = FindGrid(grade, destId);

	if (sGrid == NULL || eGrid == NULL)
	{
		if (sGrid == NULL)ErrorLogStream << "can't find a TCSGrid with id = " << srcId << LogStream::eos;
		if (eGrid == NULL)ErrorLogStream << "can't find a TCSGrid with id = " << destId << LogStream::eos;
		return emptyList;
	}

	return SearchWay(grade, sGrid->x, sGrid->y, eGrid->x, eGrid->y, blockSet);
}

std::list<UInt32> TCSGridCfgMgr::SearchWay(UInt32 grade, UInt32 srcX, UInt32 srcY, UInt32 destX, UInt32 destY, std::set<UInt32>& blockSet)
{
	std::list<UInt32> pathList, emptyList;
	TCGridPos sGridPos(srcX, srcY);
	TCGridPos eGridPos(destX, destY);
	
	if (m_PosMap[grade].count(sGridPos)==0 || m_PosMap[grade][sGridPos]==NULL)
	{
		ErrorLogStream << "x = " << sGridPos.x << "y = " << sGridPos.y << " ,TCSGridCfg*  is NULL" << LogStream::eos;
		return emptyList;
	}
	if (m_PosMap[grade].count(eGridPos) == 0 || m_PosMap[grade][eGridPos] == NULL)
	{
		ErrorLogStream << "x = " << eGridPos.x << "y = " << eGridPos.y << " ,TCSGridCfg*  is NULL" << LogStream::eos;
		return emptyList;
	}
	if (sGridPos == eGridPos)
	{
		if (m_PosMap[grade].count(sGridPos))pathList.push_back(m_PosMap[grade][sGridPos]->gridId);

		return pathList;
	}

	std::set<TCGridPos> blockPos;
	for (auto& tt : blockSet)
	{
		TCSGridCfg* cfg = FindGrid(grade, tt);
		if (NULL == cfg)
			continue;
		
		blockPos.insert(TCGridPos(cfg->x, cfg->y));
	}

	std::queue<TCGridPos>que;
	que.push(eGridPos);
	//��¼·��
	std::map<TCGridPos, TCGridPos>preGridPos;
	//��¼�Ƿ��߹��ø���
	std::map<TCGridPos, bool>gridPosFlag;
	gridPosFlag[eGridPos] = true;
	while (!que.empty())
	{
		TCGridPos curGrid = que.front();
		que.pop();
		for (int i = TC_GRID_DIR_INVALID + 1; i < TC_GRID_DIR_MAX; i++)
		{
			TCGridPos nextGrid = GetGridDirPos((TCGridDir)i, curGrid);
			//��������߹��˻��߲�����
			if (gridPosFlag.count(nextGrid) == 1 || m_PosMap[grade].count(nextGrid) == 0)continue;
			TCSGridCfg* tsg = m_PosMap[grade][nextGrid];
			if (tsg == NULL)
			{
				ErrorLogStream <<"x = "<<nextGrid.x <<"y = "<< nextGrid.y << " ,TCSGridCfg*  is NULL" << LogStream::eos;
				return emptyList;
			}
			//������Ӳ���Ŀ�꣬���Ҳ�����
			if (nextGrid != sGridPos && (tsg->block || blockPos.count(nextGrid)))continue;

			preGridPos[nextGrid] = curGrid;
			gridPosFlag[nextGrid] = true;
			if (nextGrid == sGridPos)break;
			que.push(nextGrid);
		}
	}

	if (preGridPos.count(sGridPos) == 0) return emptyList;

	//pathList.push_back(m_PosMap[sGridPos]->gridId);

	while (preGridPos.count(sGridPos))
	{
		sGridPos = preGridPos[sGridPos];
		if (m_PosMap[grade][sGridPos] == NULL)
		{
			ErrorLogStream << "x = " << sGridPos.x << "y = " << sGridPos.y << " ,TCSGridCfg*  is NULL" << LogStream::eos;
			return emptyList;
		}
		pathList.push_back(m_PosMap[grade][sGridPos]->gridId);
	}

	return pathList;
}

std::vector<UInt32> TCSGridCfgMgr::GetRoundGrid(UInt32 grade, UInt32 gridId, UInt32 roundNum)
{
	std::vector<UInt32> tempVec;
	TCSGridCfg* gridCfg = FindGrid(grade, gridId);
	if (NULL == gridCfg)
	{
		return tempVec;
	}

	for (int i = TC_GRID_DIR_INVALID + 1; i < TC_GRID_DIR_MAX; i++)
	{
		//������Ӳ����ڻ������赲
		TCGridPos dirPos = GetGridDirPos(TCGridDir(i), gridCfg->x, gridCfg->y, roundNum);
		TCSGridCfg* dirGridCfg = FindGrid(grade, dirPos);
		if (NULL == dirGridCfg || dirGridCfg->block)
		{
			continue;
		}

		tempVec.push_back(dirGridCfg->gridId);
	}

	return tempVec;
}

bool TCSGridCfgMgr::GridIsNear(UInt32 grade, UInt32 gridId1, UInt32 gridId2)
{
	std::vector<UInt32> grid1RoundVec = GetRoundGrid(grade, gridId1);
	
	for (UInt32 iter : grid1RoundVec)
	{
		if (iter == gridId2)
			return true;
	}

	return false;
}

void TCSGridCfgMgr::GetMonsterBornGrid(UInt32 grade, std::vector<UInt32>& monsterIndexVec, std::vector<UInt32>& bornVec)
{
	for (auto& iter : monsterIndexVec)
	{
		TeamCopyGridMonsterDataEntry* gridMonsterCfg = TeamCopyGridMonsterDataEntryMgr::Instance()->FindEntry(iter);
		if (NULL == gridMonsterCfg)
		{
			ErrorLogStream << " not find grid monster cfg id:" << iter << LogStream::eos;
			continue;
		}

		// ��������㲻����
		if (!TCSGridCfgMgr::Instance()->FindGrid(grade, gridMonsterCfg->bornPos))
		{
			ErrorLogStream << " monster born grid error id:" << gridMonsterCfg->bornPos << LogStream::eos;
			continue;
		}

		bornVec.push_back(gridMonsterCfg->bornPos);
	}
}
