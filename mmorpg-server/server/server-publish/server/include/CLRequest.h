#ifndef _CL_REQUEST_H_
#define _CL_REQUEST_H_

#include <CLChatDefine.h>

/**
 *@brief 请求
 */
struct Request
{
	Request():target(0), leftTime(0), param(0){}

	//目标
	ObjID_t	target;
	//剩余有效时间
	UInt16	leftTime;
	//参数
	UInt32	param;
};

/**
 *@brief 请求管理器
 */
class RequestMgr
{
	typedef std::map<ObjID_t, Request>	RequestMap;

public:
	RequestMgr();
	~RequestMgr();

public:
	/**
	 *@brief 添加一个请求
	 */
	void AddRequest(Int32 type, ObjID_t target, UInt32 validTime = REQUEST_VALID_TIME, UInt32 param = 0);
	bool HasRequest(Int32 type, ObjID_t target);
	void RemoveRequest(Int32 tyoe, ObjID_t target);
	UInt32 GetRequestLeftTime(Int32 tyoe, ObjID_t target);

	/**
	 *@brief 请求一种请求
	 */
	void ClearRequests(Int32 type);

	/**
	*@brief 获得请求参数
	*/
	UInt32 GetRequestParam(Int32 type, ObjID_t target);

public://事件
	/**
	 *@brief 切场景
	 */
	void OnLeaveScene();

	/**
	 *@brief 心跳事件
	 */
	void OnTick(const Avalon::Time& now);

private:
	//请求队列
	RequestMap	m_Requests[REQUEST_MAX];
};

#endif
