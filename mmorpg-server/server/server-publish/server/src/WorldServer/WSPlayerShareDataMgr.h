#ifndef _WS_PLAYER_SHARE_DATA_MGR_H_
#define _WS_PLAYER_SHARE_DATA_MGR_H_

#include <CLObjectIndex.h>

#include "WSPlayerShareData.h"

class WSShareDataLoadCallback;

class WSPlayerShareDataMgr : public Avalon::Singleton<WSPlayerShareDataMgr>
{
public:
	WSPlayerShareDataMgr();
	~WSPlayerShareDataMgr();

public:
	/**
	*@brief ���һ�����
	*/
	bool AddPlayer(WSPlayerShareData *player);

	/**
	*@brief �жϻ��������������������һ��ֵ���ͳ���ɾ���������
	*/
	void TryRelasePlayerOfflineData();

	/**
	*@brief ����һ�����
	*/
	WSPlayerShareData* FindPlayer(ObjID_t id);

	/**
	*@brief ����һ����ң�û���򴩼�
	*/
	WSPlayerShareData* FindOrCreatePlayer(ObjID_t id);

	/**
	*@brief ���߼����������
	*/ 
	void LoadOfflinePlayerCb(ObjID_t roleid, UInt32 accid, WSShareDataLoadCallback* cb);

	/**
	*@brief ���������������
	*/
	void OnLoadPlayerTaskInfo(CLRecordCallback* callback);
	
	/**
	*@brief �������
	*/
	void OnOffline(WSPlayer* player);
public:
	/**
	*@brief �����������ͬ������
	*/
	void HandleScenePlayerSyncShareData(ObjID_t guid, const PlayerSenceShareData& sharedata);

	/**
	*@brief ���������Ҽ�ʦͽ��ϵ������ʦ
	*/
	void HandleRelieveMaster(ObjID_t discipleId);

private:
	//id�������
	CLIDObjIndex<WSPlayerShareData>	m_Players;
};

#endif