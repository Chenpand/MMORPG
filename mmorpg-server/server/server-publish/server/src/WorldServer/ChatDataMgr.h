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
 *@brief 聊天数据
 */
struct ChatData
{
	ChatData():createtime(0), len(0), data(NULL){}

	UInt32	createtime;	//创建时间
	UInt16	len;	//数据长度
	char*	data;	//数据
};

/**
 *@brief 聊天数据缓存
 */
class ChatDataMgr : public Avalon::Singleton<ChatDataMgr>
{
	//聊天数据超时时间
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
	 *@brief 添加数据
	 */
	void AddData(ObjID_t id, const char* data, size_t len);
	void SendLinkData(WSPlayer* player, UInt8 type, ObjID_t id);
	void RemoveData(ObjID_t id);

	/**
	 *@brief 判断是否存在数据
	 */
	bool IsExistData(ObjID_t id) const;

	/**
	 *@brief 跨服查询tips
	 */
	bool OtherZoneQueryTips(UInt8 type, UInt64 id, Avalon::Packet* packet);

private:
	//数据
	ChatDataMap	m_ChatDatas;
};

#endif

