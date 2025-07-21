#ifndef _CL_NEW_TITLE_DEFINE_H_
#define _CL_NEW_TITLE_DEFINE_H_
#include <CLDefine.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <format.h>
#include <regex>
#include <CLOwnerObjectProperty.h>

class NewTitleItemDataEntry;

/**
*@brief 称谓名称类型
*/
enum NewTitleType
{
	TLETP_NONE = 0,
	TLETP_FIXED = 1, //固定价格
	TLETP_JOINT = 2, //服务器拼接
	TLETP_MAX,
};

/**
*@brief 称谓拥有类型
*/
enum NewTitleOwnerType
{
	NTOTP_NONE = 0,
	NTOTP_ROLE = 1,	 //角色拥有
	NTOTP_ACCT = 2,	 //账号拥有
	NTOTP_MAX,
};

/**
*@brief 头衔穿戴动作类型 
*/
enum NewTitleOpType
{
	NTOPP_NONE = 0,
	NTOPP_UP = 1,	//穿戴
	NTOPP_OFF = 2,  //脱掉
	NTOPP_SYNC = 3, //同步数据
	NTOPP_MAX,
};

/**
*@brief 头衔类别
*/
enum NewTitleCategory
{
	NWCGRY_NONE    = 0,
	NWCGRY_GUILD   = 1,	 //公会
	NWCGRY_YBT     = 2,	 //佣兵团
	NWCGRY_MASTER  = 3,  //师徒	
	NWCGRY_ACHIEVE = 4,  //成就
	NWCGRY_ABATT   = 5,  //角斗场
	NWCGRY_HONOR   = 6,  //荣誉
	NWCGRY_EMBLEM  = 7,  //徽记
	NWCGRY_REWARD  = 8,  //奖励
	NWCGRY_MAX,	
};

struct PlayerWearedTitleInfo : public CLOwnerComplexProperty
{
public:
	PlayerWearedTitleInfo()
	{
		titleId = 0;
		style = 0;
	}
	~PlayerWearedTitleInfo()
	{}
	/**
	@brief 编解码相关
	*/
	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
	{
		stream & titleId & style & name;
	}
	void Decode(Avalon::NetInputStream& stream)
	{
		stream & titleId & style & name;
	}
	
	void SetData(UInt32 titleId, UInt8 style, const std::string& name)
	{
		this->titleId = titleId;
		this->style = style;
		this->name = name;
		SetDirty();
	}
	
	const std::string& Name() { return name; }

private:
	UInt32	titleId;
	UInt8	style;
	std::string   name;
};

//头衔
class PlayerTitle : public CLSyncObject
{
public:
	PlayerTitle();
	~PlayerTitle();
public:
	AVALON_DEFINE_NETSERIALIZATION()
	{
		ObjID_t id = GetID();
		stream & id & m_create_time & m_title_id & m_nameType & m_duetime & m_name & m_style;
	}

public:
	virtual std::string GetTableName(){ return ""; };
	virtual void Update();
	virtual void Insert();
	virtual void Delete();
	virtual bool LoadFromDb(CLRecordCallback *pCallback, NewTitleItemDataEntry* dataEntry);
	virtual void InitDataOnCreate(UInt64 owenerId, NewTitleItemDataEntry& entry);

	bool IsValid();
	void SetDuTime(UInt32 dueTime) { m_duetime = dueTime; }
	//生成头衔名字
	std::string GenerateTitleName();
	virtual std::string JoinTitleName(NewTitleItemDataEntry& dataEntry) { return ""; };
	std::string GetName() { return m_name; }
	UInt8 Style() { return m_style; }
	UInt32 DataId() { return m_title_id; }
	template <typename... Args>
	std::string MakeName(const char* srcBuf, const Args & ... args)
	{
		std::string result = "";
		try {
			result = fmt::format(srcBuf, args...);
		}
		catch (fmt::FormatError e)
		{
			ErrorLogStream << " MakeName: srcBuf = " << srcBuf << " error : " << e.what() << LogStream::eos;
			return "";
		}

		char buf[256];
		memset(buf, 0, sizeof(buf));
		size_t size = sizeof(buf)-1;
		std::regex regexStr("\\|([^\n]*)\\|");
		std::string fmt("{$1}");
		std::string format = std::regex_replace(result, regexStr, fmt);
		strncpy(buf, format.c_str(), size);
		buf[size - 1] = '\0';
		result = buf;
		return result;
	}
	//编解码
	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("owner", m_owner)
	CL_DBPROPERTY("create_time", m_create_time)
	CL_DBPROPERTY("title_id", m_title_id)
	CL_DBPROPERTY("duetime", m_duetime)
	CL_DBPROPERTY_END()

	ObjUInt64		m_owner;		 //拥有者
	ObjUInt32		m_create_time;	 //创建时间(获得时间)
	ObjUInt32		m_title_id;	     //头衔表id
	ObjUInt8		m_nameType;	     //类型
	ObjUInt32		m_duetime;	 //到期时间,0永久
	ObjString		m_name;	     //头衔名称
	ObjUInt8		m_style;	//风格
};

#endif