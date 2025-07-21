#ifndef _CL_RLPLAYER_H_
#define _CL_RLPLAYER_H_
#include <AvalonObjectPool.h>
#include "CLObject.h"
#include <bitset>
#include "CLRelayServerProtocol.h"

namespace Avalon
{
	class Packet;
	class Protocol;
}

class RLGameSession;
struct _gamesessionEvent;
typedef struct _gamesessionEvent GameSessionEvent;

typedef enum _rlplayerStatus
{
	rlplayerStatus_offline			=	1,
	rlplayerStatus_normal			=	2,

	max_player_status_count			=	8,
}ERLPlayerStatus;

class RLPlayer : public CLObject
{
	AVALON_DEFINE_OBJECTPOOL(RLPlayer, Avalon::NullMutex)
public:

public:
	RLPlayer(UInt32 accId, RLGameSession* session = NULL);
	~RLPlayer();

	void OnTick(const Avalon::Time& now);

	void set_gamesession(RLGameSession* session);
	RLGameSession* get_gamesession();

    inline void SetRaceID(ObjID_t id) { m_raceId = id; }
    inline ObjID_t GetRaceID() const { return m_raceId; }

	UInt32 get_accId();

    void disconnect();
	bool send_packet(Avalon::Packet& packet, bool disconnect = false);
	bool send_protocol(Avalon::Protocol& protocol, bool disconnect = false);

	bool is_offline();

	void set_offline();
	void set_online();

	//
	void on_gamesession_event(const GameSessionEvent& event);

	CLProtocol::FrameChecksum& get_currentFrameChecksum();
	void					 set_currentFrameChecksum(CLProtocol::FrameChecksum& frameChecksum);

public:
	// ������
	void RecordMsg(Avalon::Packet* packet, bool isRecv);
	void SetMonitored(bool monitored);
	bool IsMonitored() const { return m_IsMonitored; }
	void CloseMonitorFile();

protected:
	//UInt8				m_positon;			//��GameSession�е�λ��, ��ʱֱ�Ӵ�GameSession�в�ѯ
	RLGameSession*							m_Session;		//��ǰ���ڵ�RLGameSession
	
	std::bitset<max_player_status_count>	m_Status;		//

	CLProtocol::FrameChecksum					m_framesChecksum;	//�ͻ����ϱ���֡У��

private:
	UInt32 m_AccId;				//account Id

    ObjID_t m_raceId;

	bool						m_IsMonitored;
	//��¼�����Ϣ�Ļ�����Ϣ���ļ�
	FILE*						m_NetSimpleFile;
	//��¼��Ҿ�����Ϣ���ļ�
	FILE*						m_NetDetailFile;
};

typedef CLVisitor<RLPlayer> RLPlayerVisitor;

#endif
