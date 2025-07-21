#ifndef _US_GAMEPARAM_MGR_H_
#define _US_GAMEPARAM_MGR_H_

#include <AvalonSingleton.h>
#include <CLRecordCallback.h>
#include <map>


#define CHAMPION_STATUS_PARAM "champion_status"

/**
 *@brief ��Ϸ����
 */
struct USGameParam
{
	USGameParam():id(0), value(0){}

	//����
	ObjID_t	id;
	//ֵ
	UInt32	value;
};

/**
 *@brief ��Ϸ����������
 */
class USGameParamMgr : public Avalon::Singleton<USGameParamMgr>
{
	friend class SelectGameParamsCallback;
	typedef std::map<std::string, USGameParam>	GameParamMap;

public:
	USGameParamMgr();
	~USGameParamMgr();
	bool IsLoaded() { return m_IsLoad; }

public:
	bool Init();
	void Final();

public:
	/**
	 *@brief ���û�ȡֵ
	 */
	void SetValue(const char* name, UInt32 value);
	UInt32 GetValue(const char* name) const;

private:
	bool OnSelectDataRet(CLRecordCallback* callback);
	void OnLoadEnd();

private:
	//��Ϸ����
	GameParamMap	m_GameParams;

	// �Ƿ�load
	bool			m_IsLoad;
};

#endif
