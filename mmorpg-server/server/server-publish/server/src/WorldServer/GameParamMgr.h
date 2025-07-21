#ifndef _GAMEPARAM_MGR_H_
#define _GAMEPARAM_MGR_H_

#include <AvalonSingleton.h>
#include <CLRecordCallback.h>
#include <map>

/**
 *@brief ��Ϸ����
 */
struct GameParam
{
	GameParam():id(0), value(0){}

	//����
	ObjID_t	id;
	//ֵ
	UInt32	value;
};

/**
 *@brief ��Ϸ����������
 */
class GameParamMgr : public Avalon::Singleton<GameParamMgr>
{
	friend class SelectGameParamsCallback;
	typedef std::map<std::string, GameParam>	GameParamMap;

public:
	GameParamMgr();
	~GameParamMgr();

public:
	void Init();
	void Final();

	/**
	 *@brief ��ȡ����ʱ��
	 */
	UInt32 GetGameStartTime() const;

	/**
	*@brief ��������ȼ�
	*/
	void CalcWorldLevel(UInt64 now);

	void OnAdminConnected(UInt32 id);
	void OnSceneConnected(UInt32 id);
	void OnGateConnected(UInt32 id);

public:
	/**
	 *@brief ���û�ȡֵ
	 */
	void SetValue(const char* name, UInt32 value);
	UInt32 GetValue(const char* name) const;

	/**
	*@brief �Ƿ��һ�ο���
	*/
	bool IsFirstService() const { return m_FirstService; };

	/**
	*@brief ͬ����scene
	*/
	void GameParamSyncScene();

private:
	bool OnSelectDataRet(CLRecordCallback* callback);

private:
	//��Ϸ����
	GameParamMap	m_GameParams;

	// �Ƿ�load
	bool			m_IsLoad;

	//��һ�ο���
	bool			m_FirstService;
};

#endif
