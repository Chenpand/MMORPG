#ifndef __PLAYER_LOAD_CALLBACK_H__
#define __PLAYER_LOAD_CALLBACK_H__

#include <CLDefine.h>

class Player;
class PlayerLoadingCallback
{
public:
    /**
    *@brief Ĭ�ϳ�ʱʱ��
    */
    static const UInt32 DEFAULT_TIMEOUT_TIME = 30;

    PlayerLoadingCallback(UInt32 timeout = DEFAULT_TIMEOUT_TIME) 
        : m_StartTime((UInt32)CURRENT_TIME.Sec()), m_Timeout(timeout) {}
    virtual ~PlayerLoadingCallback() {}

    /**
    *@brief �Ƿ�ʱ
    */
    bool IsTimeout(const Avalon::Time& now) { return (UInt32)now.Sec() > m_StartTime + m_Timeout; }
    
public:
    /**
    *@brief ���سɹ�
    */
    virtual void OnFinish(Player* player) = 0;

    /**
    *@brief ����ʧ��
    */
    virtual void OnFailure(Player* player) = 0;

    /**
    *@brief ���س�ʱ
    */
    virtual void OnTimeout(Player* player) = 0;

protected:
    // ��ʼʱ��
    UInt32      m_StartTime;
    // ��ʱʱ��(s)
    UInt32      m_Timeout;
};

#endif