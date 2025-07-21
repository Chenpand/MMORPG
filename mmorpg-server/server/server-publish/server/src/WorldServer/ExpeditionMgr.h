#ifndef _EXPEDITION_MGR_H_
#define _EXPEDITION_MGR_H_

#include <functional>
#include <CLDefine.h>

class CLRecordCallback;
class WSPlayer;
class ExpeditionMap;
class ExpeditionMember;


class ExpeditionMgr
{
public:
	ExpeditionMgr();
	~ExpeditionMgr();

public:
	/**
	*@brief ����Զ����ͼ
	*/
	void LoadExpeditionMapCallback(CLRecordCallback* callback);

	/**
	*@brief ����Զ����ͼ
	*/
	ExpeditionMap* FindExpeditionMap(UInt8 mapId);

	/**
	*@brief ���Զ����ͼ
	*/
	void AddExpeditionMap(ExpeditionMap* map);

	/**
	*@brief �Ƴ�Զ����ͼ
	*/
	bool RemoveExpeditionMap(UInt8 mapId);

	/**
	*@brief ����Զ����ͼ
	*/
	void VisitExpeditionMaps(const std::function<bool(ExpeditionMap*)>& F);

public:
	/**
	*@brief ����Զ���ӳ�Ա
	*/
	void LoadExpeditionMemberCallback(CLRecordCallback* callback);

	/*
	*@brief ����Զ���ӳ�Ա
	*/
	ExpeditionMember* FindExpeditionMember(ObjID_t roleid);

	/*
	*@brief ���Զ���ӳ�Ա
	*/
	void AddExpeditionMember(ExpeditionMember* member);

	/*
	*@brief ����Զ���ӳ�Ա
	*/
	void VisitExpeditionMember(UInt8 mapid, const std::function<bool(ExpeditionMember*)>& F);

	/*
	*@brief ���Զ���ӳ�Ա
	*/
	void ClearExpeditionMember(UInt8 mapid);

	/*
	*@brief ��ȡԶ���ӳ�Ա��
	*/
	UInt16 GetExpeditionMemberNum(UInt8 mapid);

public:
	void OnTick(const Avalon::Time& now);

	void OnPlayerOnline(WSPlayer* player);

public:
	/**
	*@brief ��Ҳ�ѯԶ����ͼ
	*/
	void OnPlayerQueryExpeditionMap(WSPlayer* player, UInt8 mapId);

	/**
	*@brief ��Ҳ�ѯ��ѡԶ����ɫ
	*/
	void OnPlayerQueryOptionalExpeditionRoles(WSPlayer* player);

	/**
	*@brief �����ǲԶ����
	*/
	void OnPlayerDispatchExpeditionTeam(WSPlayer* player, UInt8 mapId, const std::vector<ObjID_t>& members, UInt32 duration);

	/**
	*@brief ���ȡ��Զ��
	*/
	void OnPlayerCancelExpedition(WSPlayer* player, UInt8 mapId);

	/**
	*@brief ��һ�ȡԶ������
	*/
	void OnPlayerGetExpeditionRewards(WSPlayer* player, UInt8 mapId);

	/**
	*@brief ��Ҳ�ѯȫ��Զ����ͼ
	*/
	void OnPlayerQueryAllExpeditionMaps(WSPlayer* player, const std::vector<UInt8>& mapIds);

private:
	/**
	*@brief ͬ��ȫ��Զ����ͼ
	*/
	void _SyncAllExpeditionMaps();

private:
	// Զ����ͼ��
	std::vector<ExpeditionMap*> m_ExpeditionMaps;

	// Զ���ӳ�Ա
	std::vector<ExpeditionMember*> m_ExpeditionMembers;
};

#endif