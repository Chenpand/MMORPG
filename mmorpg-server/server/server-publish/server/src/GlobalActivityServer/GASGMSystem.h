#ifndef _GAS_GM_SYSTEM_H_
#define _GAS_GM_SYSTEM_H_


#include <AvalonSingleton.h>
#include <string>
#include <map>
#include <CLParameter.h>

/**
*@brief gm指令处理器
*/
class GMCmdHandler
{
public:
	virtual ~GMCmdHandler(){}

	/**
	*@brief 获取指令名
	*/
	virtual const char* GetCmdName() const = 0;

	/**
	*@brief 执行一个指令
	*/
	virtual void Execute(const CLParameter& params) = 0;
};

/**
*@brief gm系统
*/
class GASGMSystem : public Avalon::Singleton<GASGMSystem>
{
	typedef std::map<std::string, GMCmdHandler*> HandlerMap;

public:
	GASGMSystem();
	~GASGMSystem();

public:
	/**
	*@brief 解析指令
	*/
	void ExecuteCommand(const std::string& cmd, const CLParameter& params);

public:
	/**
	*@brief 注册所有gm指令处理器
	*/
	void RegisterHandlers();

	/**
	*@brief 注册一个gm指令处理器
	*/
	void RegisterHandler(GMCmdHandler* handler);

private:
	/**
	*@brief 处理器列表
	*/
	HandlerMap	m_Handlers;
};

#endif // !_GAS_GM_SYSTEM_H_
