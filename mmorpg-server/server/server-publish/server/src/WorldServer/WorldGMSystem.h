#ifndef _WORLD_GM_SYSTEM_H_
#define _WORLD_GM_SYSTEM_H_

#include <AvalonSingleton.h>
#include <string>
#include <map>
#include <CLParameter.h>

class WSPlayer;

/**
 *@brief gmָ�����
 */
class GMCmdHandler
{
public:
	virtual ~GMCmdHandler(){}

	/**
	 *@brief ��ȡָ����
	 */
	virtual const char* GetCmdName() const = 0;

	/**
	 *@brief ִ��һ��ָ��
	 */
	virtual void Execute(WSPlayer* player,const CLParameter& params) = 0;
};

/**
 *@brief gmϵͳ
 */
class GMSystem : public Avalon::Singleton<GMSystem>
{
	typedef std::map<std::string, GMCmdHandler*> HandlerMap; 

public:
	GMSystem();
	~GMSystem();

public:
	/**
	 *@brief ����ָ��
	 */
	void ExecuteCommand(WSPlayer* player,const std::string& cmd,const CLParameter& params);

public:
	/**
	 *@brief ע������gmָ�����
	 */
	void RegisterHandlers();

	/**
	 *@brief ע��һ��gmָ�����
	 */
	void RegisterHandler(GMCmdHandler* handler);

private://�������

private:
	/**
	 *@brief �������б�
	 */
	HandlerMap	m_Handlers;
};

#endif
