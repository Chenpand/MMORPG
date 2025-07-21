#ifndef _WSITEM_CHECK_H_
#define _WSITEM_CHECK_H_
#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <functional>

class WSItem;
class SceneCheckItemRet;

typedef std::function<void(UInt32, UInt32, WSItem*)> OBFUNCTION;
typedef std::map<UInt32, OBFUNCTION> ObFunctionMap;

class WSItemCheck : public Avalon::Singleton<WSItemCheck>
{
public:
	WSItemCheck();
	~WSItemCheck();

public:
	UInt32 RequestOneCheck(UInt8 checkType, ObjID_t owerId, ObjID_t itemId, ObjID_t param, OBFUNCTION ff);
	void HandleReponse(Avalon::NetInputStream &stream);
	void RmoveOneHandler(UInt32 serialId);
	UInt32 IncReqId();
private:
	ObFunctionMap m_ffs;
	UInt32 m_serial_id;
};

#endif