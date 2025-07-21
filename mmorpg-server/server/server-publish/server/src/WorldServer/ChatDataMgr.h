#ifndef _CHATDATA_MGR_H_
#define _CHATDATA_MGR_H_

#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <map>

class WSPlayer;

namespace Avalon {
	class Packet;
};

/**
 *@brief ��������
 */
struct ChatData
{
	ChatData():createtime(0), len(0), data(NULL){}

	UInt32	createtime;	//����ʱ��
	UInt16	len;	//���ݳ���
	char*	data;	//����
};

/**
 *@brief �������ݻ���
 */
class ChatDataMgr : public Avalon::Singleton<ChatDataMgr>
{
	//�������ݳ�ʱʱ��
	const static UInt32 CHATDATA_TIMEOUT = HOUR_TO_SEC;

	typedef std::map<ObjID_t, ChatData> ChatDataMap; 

public:
	ChatDataMgr();
	~ChatDataMgr();

public:
	void Init();
	void Final();

	void OnTick(const Avalon::Time& now);

public:
	/**
	 *@brief �������
	 */
	void AddData(ObjID_t id, const char* data, size_t len);
	void SendLinkData(WSPlayer* player, UInt8 type, ObjID_t id);
	void RemoveData(ObjID_t id);

	/**
	 *@brief �ж��Ƿ��������
	 */
	bool IsExistData(ObjID_t id) const;

	/**
	 *@brief �����ѯtips
	 */
	bool OtherZoneQueryTips(UInt8 type, UInt64 id, Avalon::Packet* packet);

private:
	//����
	ChatDataMap	m_ChatDatas;
};

#endif

