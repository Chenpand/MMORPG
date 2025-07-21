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
	*@brief ��ʼ��
	*/
	bool Init();

public:
	// ��������

	/**
	*@brief �߼���ʼ��
	*/
	bool LC_Init();

	/**
	*@brief �߼�����
	*/
	void LC_Update(int delta);

	/**
	*@brief ��ʼpve
	*/
	bool LC_StartPVE(ObjID_t session, Avalon::Packet* packet);

	/**
	*@brief ��ʼpvp
	*/
	bool LC_StartPVP(ObjID_t session, Avalon::Packet* packet);

	/**
	*@brief ѹ֡
	*/
	bool LC_PushFrameCommand(ObjID_t session, Avalon::Packet* packet);

	/**
	*@brief ������֤
	*/
	void LC_GiveUpVerify(ObjID_t session);

	/**
	*@brief ��¼��־
	*/
	void LC_SaveRecord(ObjID_t session);

	/**
	*@brief dump�ڴ�
	*/
	void LC_DumpMemory();

	/**
	*@brief ��û���ܵ����һ֡
	*/
	bool LC_IsRunToLastFrame(ObjID_t session);

public:
	// �����Ľӿ�

	/**
	*@brief ��֤�߼����������
	*/
	static void OnReportCheckSum(ObjID_t session, UInt32 frame, UInt32 checksum);

	/**
	*@brief ��֤�߼����ؽ���
	*/
	static void OnReportRaceEnd(ObjID_t session, MonoArray* array, int len);

	/**
	*@brief ��¼��־
	*/
	static void OnRecordLog(LogicClientLogType type, MonoString* str);

protected:
	// �߼������ʼ��
	Func_LC_Init					m_LC_Init;
	// �߼�����
	Func_LC_Update					m_LC_Update;
	// �߼������ʼ��
	Func_LC_StartPVE				m_LC_StartPVE;
	// ��ʼPVP
	Func_LC_StartPVP				m_LC_StartPVP;
	// ѹ֡
	Func_LC_PushFrameCommand		m_LC_PushFrameCommand;
	// ������֤
	Func_LC_GiveUpVerify			m_LC_GiveUpVerify;
	// ��¼��־
	Func_LC_SaveRecord				m_LC_SaveRecord;
	// dump�ڴ�
	Func_LC_DumpMemory				m_LC_DumpMemory;
	// ��û���ܵ����һ֡
	Func_LC_IsRunToLastFrame		m_LC_IsRunToLastFrame;
};

#endif