#include "AvalonMonoDelegate.h"
#include "LogicClientInterface.h"

bool AvalonMonoDelegate::Init(const char* dll)
{
#ifdef _MONO_DEBUG_
	const char* options[] = {
		"--soft-breakpoints",
		"--debugger-agent=transport=dt_socket,embedding=1,server=y,suspend=n,address=0.0.0.0:55555"
	};
	mono_jit_parse_options(sizeof(options) / sizeof(options[0]), (char**)options);
	mono_debug_init(MONO_DEBUG_FORMAT_MONO);
#endif

	m_MonoDomain = mono_jit_init_version("MonoApplication", "v4.0.30319");
	if (!m_MonoDomain)
	{
		return false;
	}

	mono_thread_attach(m_MonoDomain);


	mono_thread_set_main(mono_thread_current());
#ifdef _MONO_DEBUG_
	mono_debug_domain_create(m_MonoDomain);
#endif

	mono_add_internal_call("LogicServer::OnServerCheckSum", (const void*)LogicClentInterface::OnReportCheckSum);
	mono_add_internal_call("LogicServer::LogConsole", (const void*)LogicClentInterface::OnRecordLog);
	mono_add_internal_call("LogicServer::OnReportRaceEnd", (const void*)LogicClentInterface::OnReportRaceEnd);


	m_MonoAssembly = mono_domain_assembly_open(mono_domain_get(), dll);
	if (!m_MonoAssembly)
	{
		return false;
	}

	m_MonoClassLibraryImage = mono_assembly_get_image(m_MonoAssembly);
	if (!m_MonoClassLibraryImage)
	{
		return false;
	}

	

	return true;
}

MonoObject* AvalonMonoDelegate::CreateObject(const char* namespace_, const char* class_)
{
	auto class_desc = mono_class_from_name(m_MonoClassLibraryImage, namespace_, class_);
	if (!class_desc)
	{
		return NULL;
	}

	// 创建对象
	auto obj = mono_object_new(m_MonoDomain, class_desc);
	if (!obj)
	{
		return NULL;
	}

	// 调用默认构造函数
	mono_runtime_object_init(obj);

	return obj;
}

MonoString* AvalonMonoDelegate::MakeString(const char* str)
{
	return mono_string_new(m_MonoDomain, str);
}
