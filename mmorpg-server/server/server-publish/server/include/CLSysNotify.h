#ifndef _CL_SYSNOTIFY_H_
#define _CL_SYSNOTIFY_H_

#include <AvalonSingleton.h>
#include <AvalonScriptState.h>
#include <map>
#include <CLSysNotifyDataEntry.h>
#include <format.h>
#include <regex>
#include "CLDefine.h"


/**
 *颜色定义:
 * 黑-1
 * 灰-2
 * 白-3
 * 绿-4
 * 蓝-5
 * 紫-6
 * 金-7
 * 红-8
 * 黄-9
 */

/**
 *@brief 系统提示类型
 */

enum SysNotifyType
{
	SYS_NOTIFY_INVALID = 0,
	/*
	SYS_NOTIFY_CHAT	= 1,		//聊天框显示提示
	SYS_NOTIFY_OPERATION = 2,	//右下角操作提示
	SYS_NOTIFY_SHOW = 3,		//屏幕正中醒目提示
	SYS_NOTIFY_UNKNOW = 4,		//待定
	SYS_NOTIFY_ROLL	= 5,		//上方滚动提示
	SYS_NOTIFY_ALARM = 6,		//弹出框提示
	SYS_NOTIFY_ASIDE = 7,		//旁白
	SYS_NOTIFY_TASK	= 8,		//任务提示区域
	SYS_NOTIFY_ALARM1 = 9,		//中间弹出框提示
	SYS_NOTIFY_HUAWAI = 10,		//话外音
	SYS_NOTIFY_
	*/

	SYS_NOTIFY_SCROLL = 7,		//跑马灯
};


/**
 *@brief 系统提示
 */
struct SysNotify
{
	SysNotify():type(0), color(0), format(NULL){}
	
	UInt16	type;	//类型，即显示区域
	UInt8	color;	//颜色
	char*	format;	//格式化字符串
};

/**
 *@brief 系统提示管理器
 */
class SysNotifyMgr
	:public Avalon::Singleton<SysNotifyMgr>
{
	typedef std::map<UInt32, SysNotify> NotifyMap;

public:
	const static UInt32 DAY_STR_ID = 2001;
	const static UInt32 HOUR_STR_ID = 2002;
	const static UInt32 MIN_STR_ID = 2003;
	const static UInt32 SEC_STR_ID = 2004;

public:
	SysNotifyMgr();
	~SysNotifyMgr();

public:
	/**
	 *@初始化
	 */
	bool Load(Avalon::ScriptState* state, const std::string& filename);

	/**
	*@brief 清空
	*/
	void Clear();

	/**
	 *@brief 添加一条提示
	 */
	void AddNotify(UInt32 id, UInt16 type, UInt8 color, const char* format);
	const SysNotify* FindNotify(UInt32 id) const;

	/**
	 *@brief 创建提示内容
	 */
	//by huchenhui
	//bool MakeNotify(UInt32 id, char* buf, size_t size, ...);
	//bool MakeNotify(UInt32 id, UInt16& type, UInt8& color, char* buf, size_t size, ...);
	//bool MakeNotify(UInt32 id, UInt16& type, UInt8& color, char* buf, size_t size, va_list args);

	// added by huchenhui
	template <typename... Args>
	bool MakeNotify(const char* srcBuf, char* destBuf, size_t size, const Args & ... args)
	{
		std::string result = "";
		try {
			result = fmt::format(srcBuf, args...);
		}
		catch (fmt::FormatError e)
		{
			ErrorLogStream << " MakeNotify: srcBuf = " << srcBuf << " error : " << e.what() << LogStream::eos;
			return false;
		}

		std::regex regexStr("\\|([^\n]*)\\|");
		std::string fmt("{$1}");
		std::string format = std::regex_replace(result, regexStr, fmt);

		strncpy(destBuf, format.c_str(), size);
		destBuf[size - 1] = '\0';

		return true;
	}

	template <typename... Args>
	bool MakeNotify(UInt32 id, UInt16& type, UInt8& color, char* buf, size_t size, const Args & ... args)
	{
		if (buf == NULL) return false;
		buf[0] = 0;
		const SysNotify* notify = FindNotify(id);
		if (notify == NULL) return false;
		type = notify->type;
		color = notify->color;
		return MakeNotify(notify->format, buf, size, args...);
	}

	template <typename... Args>
	bool MakeNotify(UInt32 id, char* buf, size_t size, const Args & ... args)
	{
		UInt16 type = 0;
		UInt8 color = 0;
		bool ret = MakeNotify(id, type, color, buf, size, args...);
		return ret;
	}

	template <typename... Args>
	std::string MakeString(UInt32 id, const Args & ... args)
	{
		char buf[2048];
		memset(buf, 0, sizeof(buf));
		UInt16 type = 0;
		UInt8 color = 0;
		bool ret = MakeNotify(id, type, color, buf, sizeof(buf) - 1, args...);
		if (ret)
		{
			return buf;
		}
		return "";
	}

	/**
	 *@brief 时间值转化为字符串
	 */
	std::string TimeToStr(UInt32 secs);

	/**
	 *@brief 根据品质获取颜色名                                                                     
	 */
	const char* GetColorNameByQuality(UInt32 quality);

private:
	//消息提示格式化字符串
	NotifyMap	m_Notifies;
};

#endif
