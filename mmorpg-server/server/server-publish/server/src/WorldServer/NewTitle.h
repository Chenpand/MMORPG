#ifndef _CL_NEW_TITLE_H_
#define _CL_NEW_TITLE_H_

#include "CLNewTitleDefine.h"
#include "AvalonSimpleTimer.h"
#include <AvalonObjectPool.h>
#include <functional>

class NewTitleItemDataEntry;
class WSPlayer;
struct AccountInfo;

//���ͷ����
class PlayerTitleRole : public PlayerTitle
{
	AVALON_DEFINE_OBJECTPOOL(PlayerTitle, Avalon::NullMutex)
public:
	PlayerTitleRole() {};
	~PlayerTitleRole() {};

public:
	virtual std::string GetTableName() { return "t_new_title"; }
	std::string JoinTitleName(NewTitleItemDataEntry& dataEntry);
private:
};

//���ͷ����,�˺�ͨ��
class PlayerTitleAccout : public PlayerTitle
{
public:
	PlayerTitleAccout() {};
	~PlayerTitleAccout() {};
public:
	std::string GetTableName() { return "t_new_title_accout"; }
	std::string JoinTitleName(NewTitleItemDataEntry& dataEntry);
private:
};

//���ͷ�ι�����
class PlayerTitleMgr
{
	typedef HashMap<ObjID_t, PlayerTitle*> MapTitle;
public:
	PlayerTitleMgr();
	~PlayerTitleMgr();

public:
	//����ͷ�ζ���
	static PlayerTitle* CreateTitle(UInt64 owenerId, NewTitleItemDataEntry* entry, UInt32 dueTime = 0);

public:
	//��ʱ��
	void OnTick(const Avalon::Time &now);

	//������ҽ�ɫͷ������
	virtual void LoadTitles() = 0;

	//�������ݷ���
	void OnLoadDataRet(CLRecordCallback* callback);

	//���һ��ͷ��
	bool AddOneTitle(PlayerTitle* pTitle);

	//���һ��ͷ��
	virtual void AddOneTitle(NewTitleItemDataEntry& dataEntry);

	//��ȡһ��ͷ��
	PlayerTitle* GetOneTitleByGuid(UInt64 titleGuid);

	//��ȡһ��ͷ��
	PlayerTitle* GetOneTitleByDataId(UInt32 titleId);

	//�Ƴ�һ��ͷ��
	void RemoveOneTitleByDataId(UInt32 titleId);

	//�Ƴ�һ��ͷ��
	void RemoveOneTitleByGuid(UInt64 titleGuid);

	//����ͷ��
	void TraversalTitles(const std::function<bool(PlayerTitle&)>& ff);

	//ͷ����������
	void GenerateAllTitlesName();

	//��ȡownerId
	virtual UInt64 GetOwnerId() { return 0; };

	//��ȡӵ�н�ɫ
	virtual WSPlayer* GetOwnerPlayer() { return NULL; };

	virtual UInt64 GetOwnerPlayerId() { return 0; };

	virtual void CheckNewTitlesOnLoaded() {};

	//��־ǰ׺
	virtual std::string GetOwnerLogStr() { return "";}

	//��ȡָ��ͷ��
	static PlayerTitle* GetOneTitle(WSPlayer* player, UInt64 titleGuid);

	//��ȡָ������ͷ��
	static PlayerTitle* GetOneTitle(WSPlayer* player, UInt64 titleGuid, UInt8 titleOwnerType);

	//������Ҵ���ͷ��
	static void OnPlayerNewTitleTakeUpReq(WSPlayer* player, UInt64 titleGuid, UInt32 titleId);
	
	//��������ѵ�ͷ��
	static void OnPlayerNewTitleTakeOffReq(WSPlayer* player, UInt64 titleGuid, UInt32 titleId);

	//�������ͷ��(ʹ�õ���)
	static void OnPlayerAddNewTitle(WSPlayer* player, UInt32 titleId, UInt32 dueTm);

	//���ͷ�θ���
	static UInt32 CheckTitleTake(WSPlayer* player);

protected:
	//ͷ�ζ���
	MapTitle			m_titles;
	//��ʱ��������ͷ��
	Avalon::SimpleTimer			m_checkDueTitle;
};

//��ҽ�ɫͷ�ι�����
class PlayerTitleRoleMgr : public PlayerTitleMgr
{
public:
	PlayerTitleRoleMgr();
	~PlayerTitleRoleMgr();

public:
	//���û�ȡӵ����
	void SetOwner(WSPlayer* owner){ m_pOwner = owner; }
	WSPlayer* GetOwner() const{ return m_pOwner; }
	UInt64 GetOwnerId();
	std::string GetOwnerLogStr()
	{
		std::stringstream ss;
		ss << "role id : " << GetOwnerId();
		return ss.str();
	}

	//����ͷ������
	void LoadTitles();

	//���һ��ͷ��
	void AddOneTitleByTitleId(UInt32 titleId, UInt32 dueTm = 0);

	WSPlayer* GetOwnerPlayer() { return m_pOwner; }

	UInt64 GetOwnerPlayerId();

	void CheckNewTitlesOnLoaded();

	void OnPlayerGuildPostChange(UInt8 oldPost, UInt8 newPost);

	//�����ʦ�ɹ�
	void OnMakeMaster();

	//������ʦ��
	void OnRemoveMaster();

	//����ʦ�����ָı�
	void OnMasterNameChange();

	//���������ָı�
	void OnGuidNameChange();

	//�����뿪����
	void OnGuidLeave();

	//������Ҵ��Ϲ���ͷ��
	void OnPlayerNewTitleTakeUpGuildPostReq(WSPlayer* player);

public:
	//�������
	void OnOnline();

protected:
	//ӵ����
	WSPlayer*	m_pOwner;
};

//����˺�ͷ�ι�����
class PlayerTitleAccountMgr : public PlayerTitleMgr
{
public:
	PlayerTitleAccountMgr();
	~PlayerTitleAccountMgr();

public:
	//���û�ȡӵ����
	AccountInfo* GetOwner() const { return m_pOwner; }
	void SetOwner(AccountInfo* owner) { m_pOwner = owner; }
	UInt64 GetOwnerId();
	std::string GetOwnerLogStr()
	{
		std::stringstream ss;
		ss << "accid id : " << GetOwnerId();
		return ss.str();
	}

	//����ͷ������
	void LoadTitles();

	//���һ��ͷ��
	void AddOneTitleByTitleId(UInt32 titleId, UInt32 dueTm = 0);

	WSPlayer* GetOwnerPlayer();

	UInt64 GetOwnerPlayerId();

	void CheckNewTitlesOnLoaded();

	//������Ӷ����
	void OnCreateYbt();

	//����Ӷ�������ָı�
	void OnYbtNameChange();
protected:
	//ӵ����
	AccountInfo* m_pOwner;
};
#endif