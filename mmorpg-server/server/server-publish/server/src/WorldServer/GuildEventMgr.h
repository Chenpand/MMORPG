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
	 *@brief  ����ID
	 */
	inline void SetGuildID(ObjID_t guildID) { m_GuildID = guildID; }
	inline ObjID_t GetGuildID() { return m_GuildID; }
	inline UInt16 GetGuildEventSize() { return (UInt16)eventVec.size(); }

	/**
	 *@brief  ���ع�������
	 */
	void LoadGuildEvent(GuildEvent* guildEvent);

	/**
	 *@brief  ���һ�������¼�
	 */
	bool addGuildEvent(std::string data);

	/**
	 *@brief  ��ȡ���й����¼�����
	 */
	void GetGuildEvent(std::vector<GuildEvent>& guildEvents, UInt32 upTime);

	/**
	 *@brief  ����һ�������¼������ݿ�
	 */
	void InsertDBData(GuildEvent* guildEvent);

	/**
	 *@brief  ɾ�����й����¼�
	 */
	void DelDBData();

	/**
	 *@brief  ɾ��һ�������¼�
	 */
	void DelOneDBData(UInt64 guid);

private:
	//����ID
	ObjID_t				m_GuildID;
	
	//���й����¼�
	EventVec			eventVec;
};

#endif  //_GUILDEVENTMGR_H_
