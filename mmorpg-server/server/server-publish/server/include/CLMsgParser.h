#ifndef _CL_MSGPARSER_H_
#define _CL_MSGPARSER_H_

#include <AvalonObjectPool.h>
#include <AvalonMutex.h>
#include <AvalonStringUtil.h>
#include "CLDefine.h"
#include "CLChatDefine.h"

//�޶����ǩ
#define MSGTAG_NONE  0

/**----------------����--------------------------------------------
 *��ǩ�б�
 *�ı�		T	����:��ɫ + ����
 *����		L	����:��ַ + ��ɫ
 *����		I	����:Ψһid(base64) + ����id + Ʒ�� + �Ƿ�� + ����id
 *����		F	����:����id
 *Ѱ·		W	����:��ͼid + npcid + x + y + С��Ь����(0��ʾû��С��Ь 1��ʾ����С��Ь  2��ʾ����С��Ь ��Ĭ��Ϊ1)
 *����		N	����:��
 *������ļ	C	����:����id(UInt32) + �Ƿ���Ҫ����(1��Ҫ 0����Ҫ)
 *����		E	����:Ψһid(base64) + �Ǽ�
 *��������	M	����:����id + Ʒ��
 *����		O	����:Ψһid(base64) + ����id
 *�������	P	����:Ψһid(base64) + ���� + �Ա� + ְҵ + �ȼ�|name
 *����		B	����:�ͻ�����Դid������Ĭ��Ϊ0

 const static char  MSG_TAGS[] = {'T','L','I','F','W','N','C','E','M','O','P','B'};
 ------------------����--------------------------------------------*/

/*
*��ǩ�б�
*����		I	����:Ψһid(UInt64) + ����id 
*�̳ǵ���	A	����:Ψһid(UInt64) + ����id
*�������	P	����:Ψһid(UInt64) + ����
*�������	T	����:����id(UInt32)
*����		F	����:����ID
*���		R	����:Ψһid(UInt64) + ���id
*��Ԫʯ		W	����:��Ԫʯid
*���		O	����:���id(UInt64)
*�̵�		S	����:�̵�id
*Բ������	H	����:
*�����̵�	D	����:
*/

enum ChatLinkType
{
	CLT_ITEM = 0,
	CLT_PLAYER = 1,
	CLT_SHOP_ITEM = 2,
	CLT_TEAM = 3,
	CLT_FACE = 4,
	CLT_RETINUE = 5,
	CLT_WARP_STONE = 6,
	CLT_RED_PACKET = 7,
	CLT_SHOP = 8,
	CLT_GUILD_TABLE = 9,
	CLT_GUILD_SHOP = 10,
	CLT_SUFFIXES = 11,

};

enum SystemSuffixesType
{
	SYSTEM_SUFFIXES_TYPE_INVALID = 0,
	SYSTEM_SUFFIXES_TYPE_LEGENT = 1,
	SYSTEM_SUFFIXES_TYPE_TASK,
	SYSTEM_SUFFIXES_TYPE_SHOP,
	SYSTEM_SUFFIXES_TYPE_DUNGEON,
	SYSTEM_SUFFIXES_TYPE_STRENGTHEN,
	SYSTEM_SUFFIXES_TYPE_JAR,
	SYSTEM_SUFFIXES_TYPE_PVP,
	SYSTEM_SUFFIXES_TYPE_GUID_BATTLE,
	SYSTEM_SUFFIXES_TYPE_TIME_JAR,
	SYSTEM_SUFFIXES_TYPE_FIRST_RECHARGE,
	SYSTEM_SUFFIXES_TYPE_RECHARGE,
	SYSTEM_SUFFIXES_TYPE_RELATION,
	SYSTEM_SUFFIXES_TYPE_GUILD,
	SYSTEM_SUFFIXES_TYPE_ACTIVITY,
	SYSTEM_SUFFIXES_TYPE_PREMIUM_LEAGUE,	//�ͽ�
	SYSTEM_SUFFIXES_TYPE_ROOM_INVITE,		//����
	SYSTEM_SUFFIXES_TYPE_FASHION_COMPOSE,	//ʱװ�ϳ�
	SYSTEM_SUFFIXES_TYPE_GAMBING_LOTTERY = 21, //�ᱦ�н�
	SYSTEM_SUFFIXES_TYPE_CROSS_GUID_BATTLE = 22,	//�������ս
	SYSTEM_SUFFIXES_TYPE_EQUIPRECO_JAR = 23, //װ������
	SYSTEM_SUFFIXES_TYPE_MONEY_MANAGE = 24,		//��Ƽƻ�
	SYSTEM_SUFFIXES_TYPE_MAGIC_SHOP = 25, //ħ���̵�
	SYSTEM_SUFFIXES_TYPE_MASTER = 26, //��ʦ
	SYSTEM_SUFFIXES_TYPE_DISCIPLE = 27, //��ͽ
	SYSTEM_SUFFIXES_TYPE_SCORE_WAR_INVITE = 28, //����������
	SYSTEM_SUFFIXES_TYPE_DIG = 29, //�������
	SYSTEM_SUFFIXES_TYPE_THREE_TUMBLE = 30,	//3v3�Ҷ�
	SYSTEM_SUFFIXES_TYPE_ADD_FRIEND = 31,	//��Ӻ���
	SYSTEM_SUFFIXES_TYPE_EQUIE_UPDATE = 32,	//װ������
	SYSTEM_SUFFIXES_TYPE_BLACKMARKET = 33,	//��������
	SYSTEM_SUFFIXES_TYPE_WEEK_SIGN_ACTIVITY = 34,	//���ǩ��
	SYSTEM_SUFFIXES_TYPE_WEEK_SIGN_NEW_PLAYER = 35,	//������ǩ��
	SYSTEM_SUFFIXES_TYPE_TEAM_COPY = 36,	//�ű�
	SYSTEM_SUFFIXES_TYPE_ENHANCE = 37,		//����
};

const static char  MSG_TAGS[] = { 'I', 'P', 'A', 'T', 'F', 'R', 'W', 'O', 'S', 'H', 'D', 'X' };

//��ǩ��ֽ��
#define MSGTAG_LEFT_SEPARATOR		'{'
//��ǩ�ҷֽڷ�
#define MSGTAG_RIGHT_SEPARATOR		'}'
//��ǩ�����ֽڷ�
#define MSGTAG_CONTENT_SEPARATOR	'|'
//�����ָ��
#define MSGTAG_PARAM_SEPARATOR		' '


//��ǩ��������
#define MAX_TAGPARAM_NUM	6
//������󳤶�
#define MAX_TAGPARAM_LENGTH 128
//��Ϣ����ǩ��
#define MAX_TAG_NUM			100
	
/**
 *@brief ��ǩ
 */
class MsgTag
{
	friend class MsgParser;

	AVALON_DEFINE_OBJECTPOOL(MsgTag, Avalon::Mutex)

public:
	MsgTag();
	~MsgTag();

public:
	/**
	 *@brief ��ȡ��ǩ����
	 */
	int GetType() const { return type; }

	void SetType(char _type) { type = _type; }

	/**
	 *@brief ������ز���
	 */
	int GetParamNum() const { return paramNum; }
	void ClearParams();

	/**
	 *@brief ��ȡָ��λ�ò���
	 */
	template<typename T>
	inline T GetParam(int index) const
	{
		if(index >= paramNum) return 0;
		return Avalon::StringUtil::ConvertToValue<T>(params[index]);
	}
	
	/**
	 *@brief ��Ӳ���
	 */
	template<typename T>
	bool AddParam(T val)
	{
		if(paramNum >= MAX_TAGPARAM_NUM) return false;
		Avalon::StringUtil::ConvertToString(val, params[paramNum++]);
		return true;
	}

	bool GetParam(int index, char* buf, size_t size) const
	{
		if(index >= paramNum || buf == NULL || size < 1) return false;
		avalon_strncpy(buf, params[index], size-1);
		return true;
	}
	bool AddParam(char* str){ return AddParam((const char*)str); }
	bool AddParam(const char* str)
	{
		if(paramNum >= MAX_TAGPARAM_NUM || str == NULL) return false;
		avalon_strncpy(params[paramNum++], str, MAX_TAGPARAM_LENGTH-1);
		return true;
	}

	/**
	*@brief �޸�ԭ�еĲ���
	*/
	bool ChangeParam(int index, const char* str)
	{
		if (index >= MAX_TAGPARAM_NUM || index >= paramNum || str == NULL) return false;
		avalon_strncpy(params[index], str, MAX_TAGPARAM_LENGTH - 1);
		return true;
	}

	/**
	 *@brief ��ȡ����
	 */
	const char* GetContent() const{ return content; }
	char* GetContent() { return content;}

	/**
	*@brief ��������
	*/
	void SetContent(char* ptr) { content = ptr; }
	void SetContent(const char* ptr) { content = (char*)ptr; }

	int Dump(char* buf, int size);

private:
	char	type;											//����
	int		paramNum;										//��������
	char	params[MAX_TAGPARAM_NUM][MAX_TAGPARAM_LENGTH];	//����
	char*	content;										//����
};


/**
 *@brief ��ǩ������
 */
class TagVisitor
{
public:
	virtual ~TagVisitor(){}

	virtual bool operator()(MsgTag* tag) = 0;
};

/**
 *@brief ��Ϣ������
 */
class MsgParser
{
public:
	MsgParser();
	~MsgParser();

public:
	/**
	 *@brief ����
	 */
	bool Parse(const char* str);

	/**
	 *@brief �ж��Ƿ�Ϊ��Ч��ǩ
	 */
	static bool IsValidTag(char tag);

	/**
	 *@brief ��ȡĳ�����ͱ�ǩ��
	 */
	int GetTagNum() const{ return m_TagNum; }
	int GetTagNum(int type);
	bool Empty() const { return GetTagNum() == 0; }

	/**
	 *@brief ������ǩ
	 */
	bool Visit(TagVisitor& visitor);

	/**
	 *@brief dump����
	 */
	int Dump(char* buf, int size);

private:
	//������ǩ
	char* ParseTag(char* str);
	//��������
	char* ParseParams(MsgTag* tag, char* str);

	void Clear();

private:
	//��ǩ��
	int		m_TagNum;
	//��ǩ�б�
	MsgTag*	m_Tags[MAX_TAG_NUM];
	//�ַ�����
	char*	m_Buffer;
};

class MsgPackage
{
public:
	MsgPackage(){};
	~MsgPackage(){};

public:
	static bool GetItemMsgTag(std::string& linkStr, UInt64 id, UInt32 dataId, UInt32 strengthen)
	{
		char link[CL_MAX_CHAT_LEN] = { 0 };
		MsgTag tag;
		tag.AddParam(id);
		tag.AddParam(dataId);
		tag.AddParam(strengthen);
		tag.SetType(MSG_TAGS[CLT_ITEM]);
		if (tag.Dump(link, CL_MAX_CHAT_LEN) > 0)
		{
			linkStr = link;
			return true;
		}
		return false;
	}

	static bool GetRetinueMsgTag(std::string& linkStr, UInt64 id, UInt32 dataId, std::string content)
	{
		return GetMsgTag(linkStr, id, dataId, content, CLT_RETINUE);
	}

	static bool GetWarpStoneMsgTag(std::string& linkStr, UInt32 dataId)
	{
		return GetMsgTag(linkStr, dataId, std::string(), CLT_WARP_STONE);
	}

	static bool GetRedPacketMsgTag(std::string& linkStr, UInt64 id)
	{
		return GetMsgTag(linkStr, id, 0, std::string(), CLT_RED_PACKET);
	}

	static bool GetShopMsgTag(std::string& linkStr, UInt32 dataId)
	{
		return GetMsgTag(linkStr, dataId, std::string(), CLT_SHOP);
	}

	static bool GetGuildTable(std::string& linkStr)
	{
		return GetMsgTag(linkStr, std::string(), CLT_GUILD_TABLE);
	}

	static bool GetPlayerMsgTag(std::string& linkStr, UInt64 id, std::string name, UInt8 occu, UInt16 level)
	{
		char link[CL_MAX_CHAT_LEN] = { 0 };
		MsgTag tag;
		tag.AddParam(id);
		tag.AddParam(name.c_str());
		tag.AddParam(occu);
		tag.AddParam(level);
		tag.SetType(MSG_TAGS[CLT_PLAYER]);
		if (tag.Dump(link, CL_MAX_CHAT_LEN) > 0)
		{
			linkStr = link;
			return true;
		}
		return false;
	}

	static bool GetSystemSuffixesMsgTag(std::string& linkStr, UInt32 systemId, UInt32 dataId, UInt32 param = 0)
	{
		char link[CL_MAX_CHAT_LEN] = { 0 };
		MsgTag tag;
		tag.AddParam(systemId);
		tag.AddParam(dataId);
		tag.AddParam(param);
		tag.SetType(MSG_TAGS[CLT_SUFFIXES]);
		if (tag.Dump(link, CL_MAX_CHAT_LEN) > 0)
		{
			linkStr = link;
			return true;
		}
		return false;
	}

	static bool GetSystemSuffixesMsgTag64(std::string& linkStr, UInt32 systemId, UInt64 dataId, UInt32 param = 0)
	{
		char link[CL_MAX_CHAT_LEN] = { 0 };
		MsgTag tag;
		tag.AddParam(systemId);
		tag.AddParam(dataId);
		tag.AddParam(param);
		tag.SetType(MSG_TAGS[CLT_SUFFIXES]);
		if (tag.Dump(link, CL_MAX_CHAT_LEN) > 0)
		{
			linkStr = link;
			return true;
		}
		return false;
	}

private:
	static bool GetMsgTag(std::string& linkStr, UInt64 id, UInt32 dataId, std::string content, ChatLinkType type)
	{
		char link[CL_MAX_CHAT_LEN] = { 0 };
		MsgTag tag;
		tag.AddParam(id);
		if (dataId > 0)
		{
			tag.AddParam(dataId);
		}
		tag.SetType(MSG_TAGS[type]);
		if (!content.empty())
		{
			tag.SetContent(content.c_str());
		}
		if (tag.Dump(link, CL_MAX_CHAT_LEN) > 0)
		{
			linkStr = link;
			return true;
		}
		return false;
	}

	static bool GetMsgTag(std::string& linkStr, UInt32 dataId, std::string content, ChatLinkType type)
	{
		char link[CL_MAX_CHAT_LEN] = { 0 };
		MsgTag tag;
		tag.AddParam(dataId);
		tag.SetType(MSG_TAGS[type]);
		if (!content.empty())
		{
			tag.SetContent(content.c_str());
		}
		if (tag.Dump(link, CL_MAX_CHAT_LEN) > 0)
		{
			linkStr = link;
			return true;
		}
		return false;
	}

	static bool GetMsgTag(std::string& linkStr, std::string content, ChatLinkType type)
	{
		char link[CL_MAX_CHAT_LEN] = { 0 };
		MsgTag tag;
		tag.SetType(MSG_TAGS[type]);
		if (!content.empty())
		{
			tag.SetContent(content.c_str());
		}
		if (tag.Dump(link, CL_MAX_CHAT_LEN) > 0)
		{
			linkStr = link;
			return true;
		}
		return false;
	}

};

#endif
