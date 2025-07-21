#ifndef __LOGIC_CLIENT_INTERFACE_H__
#define __LOGIC_CLIENT_INTERFACE_H__

#include <CLDefine.h>
#include "AvalonMonoDelegate.h"
#include <AvalonPacket.h>

#ifdef _WIN32
#define stdcall __stdcall
#else
#define stdcall 
#endif

enum LogicClientLogType
{
	LC_LOG_DEBUG,
	LC_LOG_INFO,
	LC_LOG_WARN,
	LC_LOG_ERROR,
	LC_LOG_NUM
};

class LogicClentInterface
{
public:
	typedef bool(stdcall *Func_LC_Init)(MonoException**);
	typedef void(stdcall *Func_LC_Update)(int delta, MonoException**);
	typedef bool(stdcall *Func_LC_StartPVE)(UInt64 session, char* buffer, int len, MonoException**);
	typedef bool(stdcall *Func_LC_StartPVP)(UInt64 session, char* buffer, int len, MonoException**);
	typedef bool(stdcall *Func_LC_PushFrameCommand)(UInt64 session, char* buffer, int len, MonoException**);
	typedef void(stdcall *Func_LC_GiveUpVerify)(UInt64 session, MonoException**);
	typedef void(stdcall *Func_LC_SaveRecord)(UInt64 session, MonoException**);
	typedef void(stdcall *Func_LC_DumpMemory)(MonoException**);
	typedef bool(stdcall *Func_LC_IsRunToLastFrame)(UInt64 session, MonoException**);

	/**
	*@brief 初始化
	*/
	bool Init();

public:
	// 函数调用

	/**
	*@brief 逻辑初始化
	*/
	bool LC_Init();

	/**
	*@brief 逻辑更新
	*/
	void LC_Update(int delta);

	/**
	*@brief 开始pve
	*/
	bool LC_StartPVE(ObjID_t session, Avalon::Packet* packet);

	/**
	*@brief 开始pvp
	*/
	bool LC_StartPVP(ObjID_t session, Avalon::Packet* packet);

	/**
	*@brief 压帧
	*/
	bool LC_PushFrameCommand(ObjID_t session, Avalon::Packet* packet);

	/**
	*@brief 放弃验证
	*/
	void LC_GiveUpVerify(ObjID_t session);

	/**
	*@brief 记录日志
	*/
	void LC_SaveRecord(ObjID_t session);

	/**
	*@brief dump内存
	*/
	void LC_DumpMemory();

	/**
	*@brief 有没有跑到最后一帧
	*/
	bool LC_IsRunToLastFrame(ObjID_t session);

public:
	// 导出的接口

	/**
	*@brief 验证逻辑返回随机数
	*/
	static void OnReportCheckSum(ObjID_t session, UInt32 frame, UInt32 checksum);

	/**
	*@brief 验证逻辑返回结算
	*/
	static void OnReportRaceEnd(ObjID_t session, MonoArray* array, int len);

	/**
	*@brief 记录日志
	*/
	static void OnRecordLog(LogicClientLogType type, MonoString* str);

protected:
	// 逻辑整体初始化
	Func_LC_Init					m_LC_Init;
	// 逻辑更新
	Func_LC_Update					m_LC_Update;
	// 逻辑整体初始化
	Func_LC_StartPVE				m_LC_StartPVE;
	// 开始PVP
	Func_LC_StartPVP				m_LC_StartPVP;
	// 压帧
	Func_LC_PushFrameCommand		m_LC_PushFrameCommand;
	// 放弃验证
	Func_LC_GiveUpVerify			m_LC_GiveUpVerify;
	// 记录日志
	Func_LC_SaveRecord				m_LC_SaveRecord;
	// dump内存
	Func_LC_DumpMemory				m_LC_DumpMemory;
	// 有没有跑到最后一帧
	Func_LC_IsRunToLastFrame		m_LC_IsRunToLastFrame;
};

#endif