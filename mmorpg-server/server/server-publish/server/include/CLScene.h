#ifndef _CL_SCENE_H_
#define _CL_SCENE_H_

#include "CLGrid.h"


/**
 *@brief ����
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
	 *@brief ���û�ȡid
	 */
	void SetID(UInt32 id){ m_Id = id; }
	UInt32 GetID() const { return m_Id; }

	/**
	 *@brief ���û�ȡ������
	 */
	void SetName(const std::string& name){ m_Name = name; }
	std::string GetName() const { return m_Name; }
	void ScriptSetName(const char* name){ 	if(name == NULL) return; m_Name = name;}
	const char* ScriptGetName() const{ return m_Name.c_str(); }

	/**
	 *@brief ����obj���ڸ���
	 *@return �����ı䷵��true�����򷵻�false
	 */
	bool UpdatePosition(CLSceneObject* obj);

	/**
	 *@brief ��������
	 */
	bool UpdateGrid(CLSceneObject* obj, UInt32 grid);

	/**
	 *@brief ��ȡ��ͼ��
	 */
	UInt32 GetLength() const{ return m_Length; }

	/**
	 *@brief ��ȡ��ͼ��
	 */
	UInt32 GetWidth() const{ return m_Width; }

	/**
	 *@brief ��ȡͬ����Χ
	 */
	UInt32 GetSyncRange() const{ return m_SyncRange; }

	/**
	 *@brief �жϵ��Ƿ�Ϸ�
	 */
	bool IsValidPos(const CLPosition& pos);

protected:
	/**
	 *@brief  ע��һ��obj
	 */
	void RegisterObject(CLSceneObject* obj);

	/**
	 *@brief  ע��һ��obj
	 */
	void RegisterObject(UInt32 gridIndex,CLSceneObject* obj);

	/**
	 *@brief ע��һ��obj
	 */
	void UnRegisterObject(CLSceneObject* obj);

	/**
	 *@brief ������ͼ�ϵ��������
	 */
	bool VisitPlayers(CLSceneObjectVisitor& visitor);

	/**
	 *@brief ����ָ�������ڵ����
	 */
	bool VisitPlayers(const GridIndexVector& indexs,CLSceneObjectVisitor& visitor);

	/**
	 *@brief ����ָ�������ڵ�npc
	 */
	bool VisitNpcs(const GridIndexVector& indexs,CLSceneObjectVisitor& visitor);

	/**
	 *@brief ����ָ�������ڳ���Һ�npc������
	 */
	bool VisitOthers(const GridIndexVector& indexs,CLSceneObjectVisitor& visitor);

	/**
	 *@brief ����ָ�����������г���object
	 */
	bool VisitObjects(const GridIndexVector& indexs,CLSceneObjectVisitor& visitor);

	/**
	 *@brief ��ȡָ�������ڵ������
	 */
	UInt32 GetPlayerNum(const GridIndexVector& indexs) const;

	/**
	 *@brief ����һ��λ�ö�Ӧ�ĸ�������
	 */
	UInt32 CalculateGridIndex(const CLPosition& pos) const;
	
	/**
	 *@brief �����¸�����Ⱦɸ�����Χ�³��ĸ���
	 */
	void GetGridsAroundExcept(UInt32 index1,UInt32 index2,GridIndexVector& indexs) const;

	/**
	 *@brief ��ȡ��Χ�ľŸ�����
	 */
	const GridIndexVector& GetGridsAround(UInt32 index) const;

private:
	//����id
	UInt32		m_Id;
	//������
	std::string	m_Name;

	//�����б�
	GridVector				m_Grids;
	//1��������Χ9�����ӵ��б�  ÿ���߼��������Ӧ����Χ�Ź��񣨱��ϵĸ�����Χ����û��9�����ӣ������Ϳ��Ժܿ�õ�һ��SceneObject����Ұ��Ҳ���������ڸ�����Χ�ĸ��ӣ�
	std::vector<GridIndexVector>	m_AroundGrids;
	
	//x���������(�߼��ϵĸ���)
	UInt32		m_GridNumX;	
	//y���������(�߼��ϵĸ���)
	UInt32		m_GridNumY;

	//�� ��λ��������ӵ�����
	UInt32		m_Length;
	//�� ��λ��������ӵ�����
	UInt32		m_Width;
	//ͬ�����С  noted by aprilliu �߼��ϵĸ���grid�������ٸ��������, ��ҵ���Ұ�����߼���������ʾ�ģ�������ǾŹ���
	UInt32		m_SyncRange;
};

#endif
