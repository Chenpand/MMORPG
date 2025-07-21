#ifndef _WHOLE_BARGAIN_MGR_H_
#define _WHOLE_BARGAIN_MGR_H_

#include <CLDefine.h>
#include <AvalonSimpleTimer.h>

class GASPlayer;
class CLRecordCallback;

/**
*@brief ȫ�񿳼ۻ
*/
class WholeBargainMgr : public Avalon::Singleton<WholeBargainMgr>
{
public:
	WholeBargainMgr();
	~WholeBargainMgr();

	void OnInit();

	/**
	*@brief ��������
	*/
	bool LoadWholeBargain(CLRecordCallback* callback);
	bool LoadWholeBargainPlayerJoin(CLRecordCallback* callback);

	/**
	*@brief ���ʼ
	*/
	void OnActivityStart(UInt32 actId);

	/**
	*@brief �����
	*/
	void OnActivityEnd();

	/**
	*@brief ���ӹ������
	*/
	void HandleAddJoinNum(const std::string& uid, UInt32 num);

	/**
	*@brief ����ȫ�񿳼���Ϣ
	*/
	void HandleWholeBargainReq(GASPlayer* player);

	/**
	*@brief ��ȡ�ۿ�
	*/
	UInt32 GetDiscount(UInt32 actId);

	/**
	*@brief ������ѯ�ۿ�
	*/
	void ZoneQueryDiscount(UInt32 zoneId, UInt32 actId);

private:
	
	/**
	*@brief ��ȡ���������
	*/
	UInt32 _GetActJoinNum(UInt32 actId);

	/**
	*@brief �ۿ��·�
	*/
	void _DiscountSync(UInt32 discount);

	/**
	*@brief ��ȡ��Ҳ������
	*/
	UInt32 _GetPlayerJoinNum(const std::string& uid);

	/**
	*@brief �����Ҳ������
	*/
	void _ClearPlayerJoinNum();

	/**
	*@brief ������Ҳ������DB
	*/
	void _UpdatePlayerJoinNumDB(const std::string& uid, UInt32 num, bool isInsert);

	void _InsertDB(UInt32 actId, UInt32 joinNum);
	void _UpdateDB(UInt32 actId, UInt32 joinNum);

private:

	// ��ǰ�״̬
	UInt32 m_CurActStatus;
	// ��ǰ�id
	UInt32 m_CurActId;

	// ��ǰ���Ҳ������(key->playerid)
	std::map<std::string, UInt32> m_PlayerJoinMap;

	// ��ʷ���������(key->�id)
	std::map<UInt32, UInt32> m_HistoryActJoinNumMap;
};

#endif
