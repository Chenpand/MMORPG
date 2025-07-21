#ifndef __PLAYER_LOAD_CALLBACK_H__
#define __PLAYER_LOAD_CALLBACK_H__

#include <CLDefine.h>

class Player;
class PlayerLoadingCallback
{
public:
    /**
    *@brief 默认超时时间
    */
    static const UInt32 DEFAULT_TIMEOUT_TIME = 30;

    PlayerLoadingCallback(UInt32 timeout = DEFAULT_TIMEOUT_TIME) 
        : m_StartTime((UInt32)CURRENT_TIME.Sec()), m_Timeout(timeout) {}
    virtual ~PlayerLoadingCallback() {}

    /**
    *@brief 是否超时
    */
    bool IsTimeout(const Avalon::Time& now) { return (UInt32)now.Sec() > m_StartTime + m_Timeout; }
    
public:
    /**
    *@brief 加载成功
    */
    virtual void OnFinish(Player* player) = 0;

    /**
    *@brief 加载失败
    */
    virtual void OnFailure(Player* player) = 0;

    /**
    *@brief 加载超时
    */
    virtual void OnTimeout(Player* player) = 0;

protected:
    // 开始时间
    UInt32      m_StartTime;
    // 超时时间(s)
    UInt32      m_Timeout;
};

#endif