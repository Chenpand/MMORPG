#ifndef _CL_REQUEST_H_
#define _CL_REQUEST_H_

#include <CLChatDefine.h>

/**
 *@brief ����
 */
struct Request
{
	Request():target(0), leftTime(0), param(0){}

	//Ŀ��
	ObjID_t	target;
	//ʣ����Чʱ��
	UInt16	leftTime;
	//����
	UInt32	param;
};

/**
 *@brief ���������
 */
class RequestMgr
{
	typedef std::map<ObjID_t, Request>	RequestMap;

public:
	RequestMgr();
	~RequestMgr();

public:
	/**
	 *@brief ���һ������
	 */
	void AddRequest(Int32 type, ObjID_t target, UInt32 validTime = REQUEST_VALID_TIME, UInt32 param = 0);
	bool HasRequest(Int32 type, ObjID_t target);
	void RemoveRequest(Int32 tyoe, ObjID_t target);
	UInt32 GetRequestLeftTime(Int32 tyoe, ObjID_t target);

	/**
	 *@brief ����һ������
	 */
	void ClearRequests(Int32 type);

	/**
	*@brief ����������
	*/
	UInt32 GetRequestParam(Int32 type, ObjID_t target);

public://�¼�
	/**
	 *@brief �г���
	 */
	void OnLeaveScene();

	/**
	 *@brief �����¼�
	 */
	void OnTick(const Avalon::Time& now);

private:
	//�������
	RequestMap	m_Requests[REQUEST_MAX];
};

#endif
