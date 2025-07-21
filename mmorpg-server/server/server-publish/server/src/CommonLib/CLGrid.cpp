#include "CLGrid.h"


CLGrid::CLGrid()
{
	m_Index = 0;
	m_pNpcs = m_pPlayers = m_pOthers = NULL;
	m_PlayerNum = 0;
	memset(m_ObjIters, 0, sizeof(m_ObjIters));
	m_VisitDepth = 0;
}

CLGrid::CLGrid(const CLGrid& grid)
{
	m_Index = grid.m_Index;
	m_pNpcs = m_pPlayers = m_pOthers = NULL;
	m_PlayerNum = 0;
	memset(m_ObjIters, 0, sizeof(m_ObjIters));
	m_VisitDepth = 0;
}

CLGrid::~CLGrid()
{
}

void CLGrid::SetIndex(Avalon::UInt32 index)
{
	m_Index = index;
}

void CLGrid::AddToList(CLSceneObject*& list, CLSceneObject *obj)
{	
	if(list != NULL)
	{
		list->m_Prev = obj;
	}
	obj->m_Next = list;
	obj->m_Prev = NULL;

	list = obj;
}

void CLGrid::RemoveFromList(CLSceneObject *&list, CLSceneObject *obj)
{
	if(obj == NULL) return;

	for(UInt32 i = 0; i < m_VisitDepth; ++i)
	{
		if(m_ObjIters[i] == obj)
		{
			m_ObjIters[i] = m_ObjIters[i]->m_Next;
		}
	}

	if(list == obj)
	{
		list = list->m_Next;
		if(list != NULL) list->m_Prev = NULL;
		obj->m_Prev = obj->m_Next = NULL;
		return;
	}
	
	if(obj->m_Prev != NULL)
	{
		obj->m_Prev->m_Next = obj->m_Next;
	}
	if(obj->m_Next != NULL)
	{
		obj->m_Next->m_Prev = obj->m_Prev;
	}
	obj->m_Prev = obj->m_Next = NULL;
}

void CLGrid::AddSceneObject(CLSceneObject *obj)
{
	switch(obj->GetType())
	{
	case SOT_NPC:
		{
			AddToList(m_pNpcs,obj);
		}
		break;
	case SOT_PLAYER:
		{
			AddToList(m_pPlayers,obj);
			++m_PlayerNum;
		}
		break;
	default:
		{
			AddToList(m_pOthers,obj);
		}
		break;
	}

	obj->SetGridIndex(m_Index);
}

void CLGrid::RemoveSceneObject(CLSceneObject *obj)
{
	switch(obj->GetType())
	{
	case SOT_NPC:
		{
			RemoveFromList(m_pNpcs,obj);
		}
		break;
	case SOT_PLAYER:
		{
			RemoveFromList(m_pPlayers,obj);
			--m_PlayerNum;
		}
		break;
	default:
		{
			RemoveFromList(m_pOthers,obj);
		}
		break;
	}
}

bool CLGrid::VisitPlayers(CLSceneObjectVisitor &visitor)
{
	if(m_VisitDepth >= MAX_VISIT_DEPTH) ++(*(UInt32*)NULL);

	UInt32 depth = m_VisitDepth++;
	m_ObjIters[depth] = m_pPlayers;
	CLSceneObject* ptr = NULL;
	bool ret = true;
	while(m_ObjIters[depth] != NULL)
	{
		ptr = m_ObjIters[depth];
		m_ObjIters[depth] = m_ObjIters[depth]->m_Next;

		if(!visitor(ptr))
		{
			ret = false;
			break;
		}
	}
	m_ObjIters[--m_VisitDepth] = NULL;

	return ret;
}

bool CLGrid::VisitNpcs(CLSceneObjectVisitor &visitor)
{
	if(m_VisitDepth >= MAX_VISIT_DEPTH) ++(*(UInt32*)NULL);

	UInt32 depth = m_VisitDepth++;
	m_ObjIters[depth] = m_pNpcs;
	CLSceneObject* ptr = NULL;
	bool ret = true;
	while(m_ObjIters[depth] != NULL)
	{
		ptr = m_ObjIters[depth];
		m_ObjIters[depth] = m_ObjIters[depth]->m_Next;

		if(!visitor(ptr))
		{
			ret = false;
			break;
		}
	}
	m_ObjIters[--m_VisitDepth] = NULL;

	return ret;
}

bool CLGrid::VisitOthers(CLSceneObjectVisitor &visitor)
{
	if(m_VisitDepth >= MAX_VISIT_DEPTH) ++(*(UInt32*)NULL);

	UInt32 depth = m_VisitDepth++;
	m_ObjIters[depth] = m_pOthers;
	CLSceneObject* ptr = NULL;
	bool ret = true;
	while(m_ObjIters[depth] != NULL)
	{
		ptr = m_ObjIters[depth];
		m_ObjIters[depth] = m_ObjIters[depth]->m_Next;

		if(!visitor(ptr))
		{
			ret = false;
			break;
		}
	}
	m_ObjIters[--m_VisitDepth] = NULL;

	return ret;
}

bool CLGrid::VisitObjects(CLSceneObjectVisitor &visitor)
{
	if(!VisitPlayers(visitor)) return false;
	if(!VisitNpcs(visitor)) return false;
	if(!VisitOthers(visitor)) return false;

	return true;
}
