#ifndef _GUILDEVENTMGR_H_
#define _GUILDEVENTMGR_H_

#include <CLDefine.h>
#include <CLGuildDefine.h>

class GuildEventMgr
{
	using EventVec = std::list<GuildEvent*>;
public:
	GuildEventMgr();
	~GuildEventMgr();

	/**
	 *@brief  公会ID
	 */
	inline void SetGuildID(ObjID_t guildID) { m_GuildID = guildID; }
	inline ObjID_t GetGuildID() { return m_GuildID; }
	inline UInt16 GetGuildEventSize() { return (UInt16)eventVec.size(); }

	/**
	 *@brief  加载公会数据
	 */
	void LoadGuildEvent(GuildEvent* guildEvent);

	/**
	 *@brief  添加一条公会事件
	 */
	bool addGuildEvent(std::string data);

	/**
	 *@brief  获取所有公会事件数据
	 */
	void GetGuildEvent(std::vector<GuildEvent>& guildEvents, UInt32 upTime);

	/**
	 *@brief  插入一条公会事件到数据库
	 */
	void InsertDBData(GuildEvent* guildEvent);

	/**
	 *@brief  删除所有公会事件
	 */
	void DelDBData();

	/**
	 *@brief  删除一条工会事件
	 */
	void DelOneDBData(UInt64 guid);

private:
	//公会ID
	ObjID_t				m_GuildID;
	
	//所有公会事件
	EventVec			eventVec;
};

#endif  //_GUILDEVENTMGR_H_
