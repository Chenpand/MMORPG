#ifndef _SCENE_GM_SYSTEM_H_
#define _SCENE_GM_SYSTEM_H_

#include <AvalonSingleton.h>
#include <string>
#include <map>
#include <CLParameter.h>

class Player;


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
	*@brief ��ȡ������Ϣ
	*/
	virtual const char* GetHelp() const { return ""; };

	/**
	 *@brief ִ��һ��ָ��
	 */
	virtual bool Execute(Player* player, const CLParameter& params) = 0;

	/**
	 *@brief ִ��һ��ָ��
	 */
	virtual bool Execute(ObjID_t playerId, const CLParameter& params) { return false; };

protected:
	/**
	 *@brief ת����world
	 */
	void TransmitToWorld(Player* player, const CLParameter& params);

	/**
	 *@brief ת������������
	 */
	void BroadcastToScenes(Player* player, const CLParameter& params);
};

/**
 *@brief gmϵͳ
 */
class GMSystem : public Avalon::Singleton<GMSystem>
{
	typedef std::map<std::string,std::pair<UInt8,GMCmdHandler*> > HandlerMap; 

public:
	GMSystem();
	~GMSystem();

public:
	/**
	 *@prief ����gmָ��
	 */
	bool ExecuteCommand(Player* player,const std::string& cmd);

	/**
	 *@brief ִ��ָ��
	 */
	bool ExecuteCommand(Player* player, const std::string& cmd, const CLParameter& params);

	/**
	 *@brief ִ��ָ��
	 */
	bool ExecuteCommand(ObjID_t playerId, const std::string& cmd, const CLParameter& params);

public:
	/**
	 *@brief ע������gmָ�����
	 */
	void RegisterHandlers();

	/**
	 *@brief ע��һ��gmָ�����
	 */
	void RegisterHandler(UInt8 level,GMCmdHandler* handler);

	/**
	*@brief ��ȡ����GMָ��
	*/
	std::vector<std::string> GetCmdHelp(std::string str);

private://�������

private:
	/**
	 *@brief �������б�
	 */
	HandlerMap	m_Handlers;
};

#endif
