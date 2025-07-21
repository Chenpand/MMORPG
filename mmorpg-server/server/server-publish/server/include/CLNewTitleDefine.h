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
*@brief ��ν��������
*/
enum NewTitleType
{
	TLETP_NONE = 0,
	TLETP_FIXED = 1, //�̶��۸�
	TLETP_JOINT = 2, //������ƴ��
	TLETP_MAX,
};

/**
*@brief ��νӵ������
*/
enum NewTitleOwnerType
{
	NTOTP_NONE = 0,
	NTOTP_ROLE = 1,	 //��ɫӵ��
	NTOTP_ACCT = 2,	 //�˺�ӵ��
	NTOTP_MAX,
};

/**
*@brief ͷ�δ����������� 
*/
enum NewTitleOpType
{
	NTOPP_NONE = 0,
	NTOPP_UP = 1,	//����
	NTOPP_OFF = 2,  //�ѵ�
	NTOPP_SYNC = 3, //ͬ������
	NTOPP_MAX,
};

/**
*@brief ͷ�����
*/
enum NewTitleCategory
{
	NWCGRY_NONE    = 0,
	NWCGRY_GUILD   = 1,	 //����
	NWCGRY_YBT     = 2,	 //Ӷ����
	NWCGRY_MASTER  = 3,  //ʦͽ	
	NWCGRY_ACHIEVE = 4,  //�ɾ�
	NWCGRY_ABATT   = 5,  //�Ƕ���
	NWCGRY_HONOR   = 6,  //����
	NWCGRY_EMBLEM  = 7,  //�ռ�
	NWCGRY_REWARD  = 8,  //����
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
	@brief ��������
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

//ͷ��
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
	//����ͷ������
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
	//�����
	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("owner", m_owner)
	CL_DBPROPERTY("create_time", m_create_time)
	CL_DBPROPERTY("title_id", m_title_id)
	CL_DBPROPERTY("duetime", m_duetime)
	CL_DBPROPERTY_END()

	ObjUInt64		m_owner;		 //ӵ����
	ObjUInt32		m_create_time;	 //����ʱ��(���ʱ��)
	ObjUInt32		m_title_id;	     //ͷ�α�id
	ObjUInt8		m_nameType;	     //����
	ObjUInt32		m_duetime;	 //����ʱ��,0����
	ObjString		m_name;	     //ͷ������
	ObjUInt8		m_style;	//���
};

#endif