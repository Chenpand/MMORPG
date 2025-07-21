#ifndef _CL_RETINUE_MGR_H_
#define _CL_RETINUE_MGR_H_

#include <CLRetinueDefine.h>
#include <CLRetinueDataEntry.h>
#include <CLObject.h>
#include <CLObjectProperty.h>
#include <CLRecordCallback.h>
#include <CLSyncObject.h>
#include "Retinue.h"

class OffRetinueList : public CLComplexProperty
{
public:
	OffRetinueList();
	~OffRetinueList();

public:
	std::vector<UInt64>& GetRetinueIds() { return m_OffRetinueIds; };

	void SetOffRetinueId(UInt32 index, UInt64 value);

	UInt64 GetOffRetinueId(UInt32 index);

	void RemoveOffRetinueId(UInt64 id);

	bool IsUsing(UInt64 id, UInt32& index);

public:
	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

private:
	std::vector<UInt64> m_OffRetinueIds;

};

/**
*@brief ��ӷ�����
*/
typedef CLVisitor<Retinue>	RetinueVisitor;

/**
*@brief ��ӹ�����
*/
class RetinueMgr
{
public:
	typedef std::vector<Retinue*> RetinueVec;
	typedef std::map<ObjID_t, Retinue*> RetinueMap;

public:
	RetinueMgr();
	~RetinueMgr();

public:
	static bool Init();

public:
	void SetOwner(Player* player) { m_pOwner = player; }
	Player* GetOwner() const { return m_pOwner; }

	/**
	*@brief �����������
	*/
	void VisitorRetinue(RetinueVisitor& visitor);

	/**
	*@brief �������ݱ�ID����һ�����
	*/
	Retinue* FindRetinue(UInt32 dataId);

	/**
	*@brief ��ȡ��Ӹ���
	*/
	UInt32 GetRetinueSize();

	/**
	*@brief ����ID����һ�����
	*/
	Retinue* FindRetinue(ObjID_t id);

	/**
	*@brief ����ʹ�õ������
	*/
	ObjUInt64& GetRetinueFighting() { return m_FightingRetinue; }

	/**
	*@brief ����ʹ�õĸ����
	*/
	OffRetinueList& GetOffRetinueList() { return m_OffRetinueList; }

	/**
	*@brief ����ʹ�õĸ����ID
	*/
	std::vector<UInt64>& GetOffRetinueIds() { return m_OffRetinueList.GetRetinueIds(); }

	/**
	*@brief ����ʹ�õ����DataId
	*/
	ObjUInt32& GetFightingDataId();

	/**
	*@brief �ڳ�����ʹ�õ����
	*/
	ObjSceneRetinue& GetSceneRetinueInfo();

	/**
	*@brief ����ʹ�õ����
	*/
	void SetRetinueFighting(UInt64 id, UInt32 dataId, UInt8 level = 0);

	/**
	*@brief �������
	*/
	void GetRaceRetinues(std::vector<RaceRetinue>& raceRetinues);

	/**
	*@brief ��ȡ��ʷϴ������
	*/
	UInt32 GetChangeSkillTotal();

	/**
	*@brief ��ȡ���ܸ���
	*/
	UInt32 GetSkillCount(UInt32 id, UInt8 level);

	/**
	*@brief ��ȡ������ȼ�
	*/
	UInt32 GetMaxRetinueLevel();

	/**
	*@brief ��ȡ�������Ǽ�
	*/
	UInt32 GetMaxRetinueStar();

public:

	/**
	*@brief �������
	*/
	UInt32 AddRetinue(UInt32 dataId, bool isConsume = false);

	/**
	*@brief �������
	*/
	UInt32 AddRetinue(Retinue* retinue);

	/**
	*@brief �������
	*/
	UInt32 UpLevelRetinue(ObjID_t id);

	/**
	*@brief �������
	*/
	UInt32 UpStarRetinue(ObjID_t id);

	/**
	*@brief ϴ����Ӽ���
	*/
	UInt32 ChangeSkill(ObjID_t id, std::vector<UInt32> skillIds);

	/**
	*@brief ϴ��ָ����Ӽ���
	*/
	UInt32 TestChangeSkill(ObjID_t id, std::vector<UInt32> skillIds);

	/**
	*@brief ����ʹ�õ����
	*/
	UInt32 SetRetinue(ObjID_t id, UInt32 index = MAIN_RETINUE_INDEX);

	/**
	*@brief ���������
	*/
	UInt32 CheckAddRetinueByItemId(UInt32 itemId);

	/**
	*@brief ��ȡָ��Ʒ�����ϵ�����
	*/
	UInt32 GetRetinueSizeByQuality(RetinueQuality quality);

	/**
	*@brief ����ϴ������
	*/
	void ResetChangeSkillCount();

	/**
	*@brief ���浽���ݿ�
	*/
	void SaveToDB();

public: //�¼�
	/**
	*@brief �����¼�
	*/
	void OnOnline();

	/**
	*@brief ��ѯ�ص�
	*/
	void OnSelectDataRet(CLRecordCallback* callback);

private:

	/**
	*@brief ���������
	*/
	UInt32 CheckAddRetinue(UInt32 dataId);

	/**
	*@brief ����IDж�¸����
	*/
	void RemoveOffRetinue(UInt64 id);

	/**
	*@brief ��ȡЭ��ʹ�õĽṹ
	*/
	std::vector<RetinueInfo> GetRetinueList();

	/**
	*@brief �·�����б�
	*/
	void SyncRetinueList();

	/**
	*@brief �·������Ϣ
	*/
	void SyncRetinue(Retinue* retinue);

private:
	//ӵ����
	Player*	m_pOwner;

	//����ʹ�õ������ID
	ObjUInt64	m_FightingRetinue;

	//����ʹ�õ������DataId
	ObjUInt32	m_FightingDataId;

	//����ʹ�õ������
	ObjSceneRetinue m_SceneRetinue;

	//�����
	OffRetinueList m_OffRetinueList;

	//����б�	����Retinue*���б�
	RetinueVec	m_RetinueVec;

	//���map, ����ӵ�guidΪkey
	RetinueMap	m_RetinueMap;

	//���ĵ���ϵ��
	UInt32		m_PointRatio;

};

#endif //_CL_RETINUE_MGR_H_
