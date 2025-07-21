#ifndef _CL_GRID_H_
#define _CL_GRID_H_

#include "CLDefine.h"
#include "CLSceneObject.h"

class CLSceneObject;

/**
 *@brief ͬ����������
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
	 *@brief ���һ��
	 */
	void AddSceneObject(CLSceneObject* obj);

	/**
	 *@brief ɾ��һ��
	 */
	void RemoveSceneObject(CLSceneObject* obj);

	/**
	 *@brief ��������npc
	 */
	bool VisitNpcs(CLSceneObjectVisitor& visitor);

	/**
	 *@brief �����������
	 */
	bool VisitPlayers(CLSceneObjectVisitor& visitor);

	/**
	 *@brief �������г�npc������������object
	 */
	bool VisitOthers(CLSceneObjectVisitor& visitor);

	/**
	 *@brief ��������obj
	 */
	bool VisitObjects(CLSceneObjectVisitor& visitor);

	/**
	 *@brief ��ȡ�����������
	 */
	UInt32 GetPlayerNum() const{ return m_PlayerNum; }

private:
	void AddToList(CLSceneObject*& list,CLSceneObject* obj);

	void RemoveFromList(CLSceneObject*& list,CLSceneObject* obj);

private:
	//npc�б�
	CLSceneObject* m_pNpcs;
	//����б�
	CLSceneObject* m_pPlayers;
	//�����б�
	CLSceneObject* m_pOthers;

	//����ָ���б�
	CLSceneObject* m_ObjIters[MAX_VISIT_DEPTH];
	//�������
	UInt32		   m_VisitDepth;

	//�����
	UInt32	m_PlayerNum;

	//����
	UInt32	m_Index;
};

#endif
