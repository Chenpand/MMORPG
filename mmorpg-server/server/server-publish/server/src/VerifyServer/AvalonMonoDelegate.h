#ifndef __AVALON_MONO_DELEGATE_H__
#define __AVALON_MONO_DELEGATE_H__

#include <AvalonSingleton.h>
#include <CLDefine.h>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/object.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/environment.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/mono-config.h>
#include <string>
#include <fcntl.h>
#include <stdio.h>
#pragma comment(lib,"mono-2.0-boehm.lib")

struct MonoClassDesc
{
	std::string namespace_;
	std::string class_;
};

struct _MonoException {
	MonoObject object;
	MonoString *class_name;
	MonoString *message;
	MonoObject *_data;
	MonoObject *inner_ex;
	MonoString *help_link;
	/* Stores the IPs and the generic sharing infos
	(vtable/MRGCTX) of the frames. */
	MonoArray  *trace_ips;
	MonoString *stack_trace;
	MonoString *remote_stack_trace;
	int      remote_stack_index;
	/* Dynamic methods referenced by the stack trace */
	MonoObject *dynamic_methods;
	int      hresult;
	MonoString *source;
	MonoObject *serialization_manager;
	MonoObject *captured_traces;
	MonoArray  *native_trace_ips;
};

class AvalonMonoDelegate : public Avalon::Singleton<AvalonMonoDelegate>
{
public:
	AvalonMonoDelegate() {}
	~AvalonMonoDelegate() {}

	/**
	*@brief 初始化mono环境
	*/
	bool Init(const char* dll);

	/**
	*@brief 获取mono函数
	*/
	template<typename Func>
	Func GetFunc(const char* namespace_, const char* class_, const char* func_);

	/**
	*@brief 创建mono对象
	*/
	MonoObject* CreateObject(const char* namespace_, const char* class_);

	/**
	*@brief 创建mono的string
	*/
	MonoString* MakeString(const char* str);

	/**
	*@brief 打印异常处理
	*/
	static void HandleException(MonoException* excp)
	{
		if (!excp)
		{
			return;
		}

		auto e = (_MonoException*)excp;

		ErrorLogStream << mono_string_to_utf8(e->message) << LogStream::eos;
	}

public:
	template<typename Func, typename... P1>
	static void Call(Func func, P1... arg)
	{
		MonoException* excp;
		try
		{
			func(arg..., &excp);
			HandleException(excp);
		}
		catch (...)
		{
			HandleException(excp);
		}
	}

	template<typename RT, typename Func, typename... P1>
	static RT Call(Func func, P1... arg)
	{
		MonoException* excp;
		RT ret = func(arg..., &excp);
		if (excp)
		{
			//...
		}

		return ret;
	}

private:
	// Mono虚拟机
	MonoDomain*								m_MonoDomain;

	// 加载的dll
	MonoAssembly*							m_MonoAssembly;

	// 类列表
	MonoImage*								m_MonoClassLibraryImage;
};

template<typename Func>
Func AvalonMonoDelegate::GetFunc(const char* namespace_, const char* class_name, const char* func_name)
{
	auto mono_class = mono_class_from_name(m_MonoClassLibraryImage, namespace_, class_name);
	if (!mono_class) {
		return NULL;
	}

	char func_full_name[256];
	sprintf(func_full_name, "%s:%s", class_name, func_name);
	auto desc = mono_method_desc_new(func_full_name, 1);
	if (!desc) {
		return NULL;
	}

	MonoMethod* m = mono_method_desc_search_in_class(desc, mono_class);
	if (!m) {
		return NULL;
	}

	return (Func)mono_method_get_unmanaged_thunk(m);
}

#endif