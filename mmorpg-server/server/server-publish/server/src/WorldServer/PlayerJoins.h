#ifndef __PLAYER_JOINS_H_
#define __PLAYER_JOINS_H_

#include <AvalonSimpleTimer.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLPlayerJoinEventDefine.h>

//记录保存时间
#define PLAYER_JOINS_EVENT_SAVE_TIME  7*24*3600

//玩家列表
struct  PlayerList : public CLComplexProperty
{
	PlayerList();
	~PlayerList();

public:
	void AddOnePlayer(ObjID_t playerId);
	bool FindPlayer(ObjID_t playerId);
	UInt32 GetNum();
public://编解码相关
	/**
	*@brief 编解码相关
	*/
	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	/**
	*@brief 编解码字符串
	*/
	void EncodeString(std::ostringstream& os);
	void DecodeString(std::istringstream& is);

private:
	std::set<ObjID_t>  m_playerIds;
};

struct PlayerJoinRecordKey
{
	PlayerJoinRecordKey(UInt8 type_, UInt64 param1_, UInt64 param2_)
	: type(type_), param1(param1_), param2(param2_)
	{}

	bool operator<(const PlayerJoinRecordKey& other) const
	{
		if (other.type != type)
		{
			return type < other.type;
		}
		if (other.param1 != param1)
		{
			return param1 < other.param1;
		}
		return param2 < other.param2;
	}

	UInt8 type;
	UInt64 param1; //(公会战,指公会战时间)
	UInt64 param2; //(公会战,指公会id)
};

class PlayerJoinRecord : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(PlayerJoinRecord, Avalon::NullMutex)
public:
	PlayerJoinRecord(UInt8 type, UInt64 param1, UInt64 param2);
	~PlayerJoinRecord();
public:
	void Update();
	void Insert();
	void Delete();

	void OnTick(const Avalon::Time &now);
	void AddOnePlayer(ObjID_t playerId);
	bool IsPlayerJoin(ObjID_t playerId);
	bool IsShouldDeleted(const Avalon::Time& now);
	UInt32 GetJoinNum();
	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("create_time", m_create_time)
	CL_DBPROPERTY("type", m_type)
	CL_DBPROPERTY("param1", m_param1)
	CL_DBPROPERTY("param2", m_param2)
	CL_DBPROPERTY("join_players", m_join_players)
	
	CL_DBPROPERTY_END()

public:
	//类型
	ObjUInt8				m_type;
	//参数1(公会战,指公会战时间)
	ObjUInt64				m_param1;
	//参数2(公会战,指公会id)
	ObjUInt64				m_param2;
	//玩家列表
	PlayerList				m_join_players;
	//创建时间戳
	ObjUInt32				m_create_time;
	// 定时器
	Avalon::SimpleTimer	    m_saveTimer;
};


class PlayerJoinMgr : public Avalon::Singleton<PlayerJoinMgr>
{
public:
	typedef std::map<PlayerJoinRecordKey, PlayerJoinRecord*> PlayerJoinRecordMap;
public:
	PlayerJoinMgr();
	~PlayerJoinMgr();

public:
	void Init();

	bool OnTick(const Avalon::Time& now);

	bool OnLoadRecordsRet(CLRecordCallback *pCallback);

	void AddOnePlayer(UInt8	type, UInt64 param1, UInt64 param2, ObjID_t playerId);

	PlayerJoinRecord* CreateOneRecord(UInt8 type, UInt64 param1, UInt64 param2);
	
	bool IsPlayerJoinEvent(ObjID_t playerId, UInt8 type, UInt64 param1, UInt64 param2);

	void RemoveRecord(PlayerJoinRecordKey& key);

public:
	UInt32 GetPlayerJoinNum(UInt8 type, UInt64 param1, UInt64 param2);
protected:
	PlayerJoinRecord* _FindJoinRecord(PlayerJoinRecordKey& key);

private:
	PlayerJoinRecordMap		m_records;
};

#endif