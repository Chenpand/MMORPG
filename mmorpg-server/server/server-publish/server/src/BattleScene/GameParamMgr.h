#ifndef _GAME_PARAM_MGR_H_
#define _GAME_PARAM_MGR_H_

#include <AvalonSingleton.h>
#include <CLDefine.h>

/**
 *@brief ��Ϸ������������������ԴWorld�������������¶���Ҫͬ����World
 */
class GameParamMgr : public Avalon::Singleton<GameParamMgr>
{
	typedef std::map<std::string, UInt32>	GameParamMap;

public:
	GameParamMgr();
	~GameParamMgr();

public:
	/**
	 *@brief ���û�ȡֵ
	 */
	void SetValue(const std::string& name, UInt32 value, bool isNotifyWorld = true);
	UInt32 GetValue(const std::string& name) const;

private:
	//��Ϸ����
	GameParamMap	m_GameParams;
};

#endif
