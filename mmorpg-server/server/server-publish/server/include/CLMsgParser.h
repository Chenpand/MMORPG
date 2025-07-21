#ifndef _CL_MSGPARSER_H_
#define _CL_MSGPARSER_H_

#include <AvalonObjectPool.h>
#include <AvalonMutex.h>
#include <AvalonStringUtil.h>
#include "CLDefine.h"
#include "CLChatDefine.h"

//无定义标签
#define MSGTAG_NONE  0

/**----------------废弃--------------------------------------------
 *标签列表
 *文本		T	参数:颜色 + 字形
 *链接		L	参数:网址 + 颜色
 *道具		I	参数:唯一id(base64) + 类型id + 品质 + 是否绑定 + 宠物id
 *表情		F	参数:表情id
 *寻路		W	参数:地图id + npcid + x + y + 小飞鞋类型(0表示没有小飞鞋 1表示任务小飞鞋  2表示护送小飞鞋 ，默认为1)
 *换行		N	参数:无
 *副本招募	C	参数:队伍id(UInt32) + 是否需要密码(1需要 0不需要)
 *宠物		E	参数:唯一id(base64) + 星级
 *道具名称	M	参数:类型id + 品质
 *法宝		O	参数:唯一id(base64) + 类型id
 *玩家链接	P	参数:唯一id(base64) + 名字 + 性别 + 职业 + 等级|name
 *标题		B	参数:客户端资源id，不填默认为0

 const static char  MSG_TAGS[] = {'T','L','I','F','W','N','C','E','M','O','P','B'};
 ------------------废弃--------------------------------------------*/

/*
*标签列表
*道具		I	参数:唯一id(UInt64) + 道具id 
*商城道具	A	参数:唯一id(UInt64) + 道具id
*玩家链接	P	参数:唯一id(UInt64) + 名字
*组队链接	T	参数:队伍id(UInt32)
*表情		F	参数:表情ID
*随从		R	参数:唯一id(UInt64) + 随从id
*次元石		W	参数:次元石id
*红包		O	参数:红包id(UInt64)
*商店		S	参数:商店id
*圆桌会议	H	参数:
*公会商店	D	参数:
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
	SYSTEM_SUFFIXES_TYPE_PREMIUM_LEAGUE,	//赏金
	SYSTEM_SUFFIXES_TYPE_ROOM_INVITE,		//房间
	SYSTEM_SUFFIXES_TYPE_FASHION_COMPOSE,	//时装合成
	SYSTEM_SUFFIXES_TYPE_GAMBING_LOTTERY = 21, //夺宝中奖
	SYSTEM_SUFFIXES_TYPE_CROSS_GUID_BATTLE = 22,	//跨服工会战
	SYSTEM_SUFFIXES_TYPE_EQUIPRECO_JAR = 23, //装备回收
	SYSTEM_SUFFIXES_TYPE_MONEY_MANAGE = 24,		//理财计划
	SYSTEM_SUFFIXES_TYPE_MAGIC_SHOP = 25, //魔罐商店
	SYSTEM_SUFFIXES_TYPE_MASTER = 26, //拜师
	SYSTEM_SUFFIXES_TYPE_DISCIPLE = 27, //收徒
	SYSTEM_SUFFIXES_TYPE_SCORE_WAR_INVITE = 28, //积分赛邀请
	SYSTEM_SUFFIXES_TYPE_DIG = 29, //随机宝箱
	SYSTEM_SUFFIXES_TYPE_THREE_TUMBLE = 30,	//3v3乱斗
	SYSTEM_SUFFIXES_TYPE_ADD_FRIEND = 31,	//添加好友
	SYSTEM_SUFFIXES_TYPE_EQUIE_UPDATE = 32,	//装备升级
	SYSTEM_SUFFIXES_TYPE_BLACKMARKET = 33,	//黑市商人
	SYSTEM_SUFFIXES_TYPE_WEEK_SIGN_ACTIVITY = 34,	//活动周签到
	SYSTEM_SUFFIXES_TYPE_WEEK_SIGN_NEW_PLAYER = 35,	//新人周签到
	SYSTEM_SUFFIXES_TYPE_TEAM_COPY = 36,	//团本
	SYSTEM_SUFFIXES_TYPE_ENHANCE = 37,		//增幅
};

const static char  MSG_TAGS[] = { 'I', 'P', 'A', 'T', 'F', 'R', 'W', 'O', 'S', 'H', 'D', 'X' };

//标签左分界符
#define MSGTAG_LEFT_SEPARATOR		'{'
//标签右分节符
#define MSGTAG_RIGHT_SEPARATOR		'}'
//标签结束分节符
#define MSGTAG_CONTENT_SEPARATOR	'|'
//参数分割符
#define MSGTAG_PARAM_SEPARATOR		' '


//标签最大参数数
#define MAX_TAGPARAM_NUM	6
//参数最大长度
#define MAX_TAGPARAM_LENGTH 128
//消息最大标签数
#define MAX_TAG_NUM			100
	
/**
 *@brief 标签
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
	 *@brief 获取标签类型
	 */
	int GetType() const { return type; }

	void SetType(char _type) { type = _type; }

	/**
	 *@brief 参数相关操作
	 */
	int GetParamNum() const { return paramNum; }
	void ClearParams();

	/**
	 *@brief 获取指定位置参数
	 */
	template<typename T>
	inline T GetParam(int index) const
	{
		if(index >= paramNum) return 0;
		return Avalon::StringUtil::ConvertToValue<T>(params[index]);
	}
	
	/**
	 *@brief 添加参数
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
	*@brief 修改原有的参数
	*/
	bool ChangeParam(int index, const char* str)
	{
		if (index >= MAX_TAGPARAM_NUM || index >= paramNum || str == NULL) return false;
		avalon_strncpy(params[index], str, MAX_TAGPARAM_LENGTH - 1);
		return true;
	}

	/**
	 *@brief 获取内容
	 */
	const char* GetContent() const{ return content; }
	char* GetContent() { return content;}

	/**
	*@brief 设置内容
	*/
	void SetContent(char* ptr) { content = ptr; }
	void SetContent(const char* ptr) { content = (char*)ptr; }

	int Dump(char* buf, int size);

private:
	char	type;											//类型
	int		paramNum;										//参数个数
	char	params[MAX_TAGPARAM_NUM][MAX_TAGPARAM_LENGTH];	//参数
	char*	content;										//内容
};


/**
 *@brief 标签访问器
 */
class TagVisitor
{
public:
	virtual ~TagVisitor(){}

	virtual bool operator()(MsgTag* tag) = 0;
};

/**
 *@brief 消息解析类
 */
class MsgParser
{
public:
	MsgParser();
	~MsgParser();

public:
	/**
	 *@brief 解析
	 */
	bool Parse(const char* str);

	/**
	 *@brief 判断是否为有效标签
	 */
	static bool IsValidTag(char tag);

	/**
	 *@brief 获取某种类型标签数
	 */
	int GetTagNum() const{ return m_TagNum; }
	int GetTagNum(int type);
	bool Empty() const { return GetTagNum() == 0; }

	/**
	 *@brief 遍历标签
	 */
	bool Visit(TagVisitor& visitor);

	/**
	 *@brief dump出来
	 */
	int Dump(char* buf, int size);

private:
	//解析标签
	char* ParseTag(char* str);
	//解析参数
	char* ParseParams(MsgTag* tag, char* str);

	void Clear();

private:
	//标签数
	int		m_TagNum;
	//标签列表
	MsgTag*	m_Tags[MAX_TAG_NUM];
	//字符缓冲
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
