#ifndef _US_GM_SYSTEM_H_
#define _US_GM_SYSTEM_H_

#include <AvalonSingleton.h>
#include <string>
#include <map>
#include <CLParameter.h>

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
	virtual void Execute(ObjID_t playerId, const CLParameter& params) = 0;
};

/**
*@brief gmϵͳ
*/
class USGMSystem : public Avalon::Singleton<USGMSystem>
{
	typedef std::map<std::string, GMCmdHandler*> HandlerMap;

public:
	USGMSystem();
	~USGMSystem();

public:
	/**
	*@brief ����ָ��
	*/
	void ExecuteCommand(ObjID_t playerId, const std::string& cmd, const CLParameter& params);

public:
	/**
	*@brief ע������gmָ�����
	*/
	void RegisterHandlers();

	/**
	*@brief ע��һ��gmָ�����
	*/
	void RegisterHandler(GMCmdHandler* handler);

private:
	/**
	*@brief �������б�
	*/
	HandlerMap	m_Handlers;
};

#endif // !_GAS_GM_SYSTEM_H_
