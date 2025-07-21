#ifndef _SCENE_GM_SYSTEM_H_
#define _SCENE_GM_SYSTEM_H_

#include <AvalonSingleton.h>
#include <string>
#include <map>
#include <CLParameter.h>

class Player;


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
	*@brief 获取帮助信息
	*/
	virtual const char* GetHelp() const { return ""; };

	/**
	 *@brief 执行一个指令
	 */
	virtual bool Execute(Player* player, const CLParameter& params) = 0;

	/**
	 *@brief 执行一个指令
	 */
	virtual bool Execute(ObjID_t playerId, const CLParameter& params) { return false; };

protected:
	/**
	 *@brief 转发到world
	 */
	void TransmitToWorld(Player* player, const CLParameter& params);

	/**
	 *@brief 转发到其他场景
	 */
	void BroadcastToScenes(Player* player, const CLParameter& params);
};

/**
 *@brief gm系统
 */
class GMSystem : public Avalon::Singleton<GMSystem>
{
	typedef std::map<std::string,std::pair<UInt8,GMCmdHandler*> > HandlerMap; 

public:
	GMSystem();
	~GMSystem();

public:
	/**
	 *@prief 解析gm指令
	 */
	bool ExecuteCommand(Player* player,const std::string& cmd);

	/**
	 *@brief 执行指令
	 */
	bool ExecuteCommand(Player* player, const std::string& cmd, const CLParameter& params);

	/**
	 *@brief 执行指令
	 */
	bool ExecuteCommand(ObjID_t playerId, const std::string& cmd, const CLParameter& params);

public:
	/**
	 *@brief 注册所有gm指令处理器
	 */
	void RegisterHandlers();

	/**
	 *@brief 注册一个gm指令处理器
	 */
	void RegisterHandler(UInt8 level,GMCmdHandler* handler);

	/**
	*@brief 获取所有GM指令
	*/
	std::vector<std::string> GetCmdHelp(std::string str);

private://网络相关

private:
	/**
	 *@brief 处理器列表
	 */
	HandlerMap	m_Handlers;
};

#endif
