#ifndef _CL_LOGIN_DATAENTRY_H_
#define _CL_LOGIN_DATAENTRY_H_

#include "CLDefine.h"
#include <CLItemDefine.h>

#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief 登录推送数据项
*/
struct LoginPushDataEntry : public Avalon::DataEntry
{
public:
	LoginPushDataEntry();
	~LoginPushDataEntry();
	
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 15; };

public:
	// id
	UInt8				  id;
	// 名称
	std::string			  name;
	// 解锁等级
	UInt16				  unlockLevel;
	// 图标路径
	std::string			  iconPath;
	// 链接位置
	std::string			  linkInfo;
	// 生效开始时间
	UInt32				  startTime;
	// 生效结束时间
	UInt32				  endTime;
	// 开服后第几天(0无效)
	UInt32				  daysAfterService;
	// 持续天数
	UInt32				  continueDays;
	// 开启间隔
	std::string			  openInterval;
	// 关闭间隔
	std::string			  closeInterval;
	// loading预制体路径
	std::string			  loadingIconPath;
	// 排序序号
	UInt8				  sortNum;
	// 是否显示时间
	UInt8				  isShowTime;
	// 是否设置背景图片原比例大小
	UInt8				  isSetNative;
};

/**
*@brief 登录推送管理器
*/
class LoginPushDataEntryMgr : public Avalon::DataEntryMgrBase<LoginPushDataEntry>
	, public Avalon::Singleton<LoginPushDataEntryMgr>
{
public:
	LoginPushDataEntryMgr();
	~LoginPushDataEntryMgr();
};

#endif