#ifndef _CL_RELAYSERVER_PROTOCOL_H_
#define _CL_RELAYSERVER_PROTOCOL_H_

#include <AvalonPacket.h>
#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"
#include "CLGameSessionDefine.h"
#include "CLDungeonDefine.h"

namespace CLProtocol
{
	//client -> relay server   登录请求
	class RelaySvrLoginReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAYSERVER_LOGIN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream &m_ucPos & m_unAccId & m_unRoleId & m_unGameSessionId;
		}

	public:
		UInt8  m_ucPos;
		UInt32 m_unAccId;
		UInt64 m_unRoleId;

		//
		UInt64 m_unGameSessionId;		

	protected:
	private:
	};

	//登录响应
	enum _relaysvr_cs_resultid
	{
		relaysvr_cs_resultid_login_systemError		=			1,
		relaysvr_cs_resultid_login_invalidSession	=			2,				//无效的game session
		relaysvr_cs_resultid_login_invalidfighter   =			3,				//无效的参战者


	};
	class RelaySvrLoginRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAYSERVER_LOGIN_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & currentTime;
		}
	public:

		UInt32	result;
		UInt64	currentTime;

	protected:
	private:
	};

	//relay server通知client  PK开始    relay server  -> client
	class RelaySvrNotifyGameStart : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAYSERVER_START_GAME_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & gamesessionId & startTime;
		}

	public:
		//
		ObjID_t		gamesessionId;							//game session ID
		UInt64		startTime;								//start time, (ms)
	protected:
	private:
	};

	//RELAYSERVER_INPUT_REQ player上报input信息 client --> relay server
	typedef struct _inputData
	{
		_inputData() { memset(this, 0, sizeof(*this)); }
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sendTime & data1 & data2 & data3;
		}

        UInt32              sendTime;                            //帧发送时间（测试用）
		UInt32				data1;
		UInt32				data2;
		UInt32				data3;
	}InputData;

	typedef	struct _fighterInput
	{
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & pos & input;
		}
		UInt8				pos;				//fighter位置，对战双方统一编码，取值范围：0-（max_fighter_count-1）
		InputData			input;				//fighter具体输入
	}FighterInput;

	//帧
	class Frame
	{
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			//stream & sequence & msec &data;
			stream & sequence & data;
		}
	public:
		Frame()
		{
			Init();
		}

// 		Frame(UInt32 seq, UInt64 now) :sequence(seq),msec(now)
// 		{
// 			data.clear();
// 		}

		Frame(UInt32 seq) :sequence(seq)
		{
			data.clear();
		}

		//copy constructor
		Frame(const Frame& fr) :sequence(fr.sequence),data(fr.data)
		{

		}

// 		Frame(Frame& fr) :sequence(fr.sequence), data(fr.data)
// 		{
// 
// 		}

		~Frame()
		{

		}

		void Init()
		{
			sequence = 0;
			//msec = 0;
			data.clear();
		}

	public:
		UInt32 sequence;							//帧序号
		//UInt64 msec;								//服务器生成帧的时间
		std::vector<FighterInput> data;				//帧数据

	protected:
	private:
	};

	class RelaySvrPlayerInputReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAYSERVER_INPUT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & gamesessionId & pos & roleId & input;
		}

	public:
		ObjID_t gamesessionId;
		UInt8	pos;
		ObjID_t roleId;
		
		//input data
		InputData input;
	protected:
	private:
	};

	//RELAYSERVER_FRAMEDATA_NOTIFY  relay server下发pk 数据(帧数据)  relay server --> client
	class RelaySvrFrameDataNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAYSERVER_FRAMEDATA_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & frames;
		}

	public:
		std::vector<Frame> frames;
	protected:
	private:
	};

	

	//RELAYSERVER_GAMERESULT_NOTIFY  relay server通知client比赛结果(胜负，结算信息) relay server --> client
	class RelaySvrGameResultNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAYSERVER_GAMERESULT_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & endReason & gamesessionID & results;
		}

		UInt8	endReason;						//结束原因：取值EGameSessionDismissType
		ObjID_t gamesessionID;

		std::vector<FightergResult> results;
	protected:
	private:
	};

	//client --> relay server client请求结束游戏

	class RelaySvrEndGameReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAYSERVER_ENDGAME)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

		//
		PkRaceEndInfo		info;

	protected:
	private:
	};

    class RelaySvrDungeonRaceEndReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(RELAYSERVER_DUNGEON_RACE_END_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & raceEndInfo;
        }

        DungeonRaceEndInfo      raceEndInfo;
    };

    class RelaySvrRaceEndNotify : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(RELAYSERVER_RACE_END_NOTIFY)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & endReason;
        }

        UInt8	endReason;						//结束原因：取值EGameSessionDismissType
    };

	class RelaySvrDisconnectNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAYSERVER_DISCONNECT_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & disconnectReason & accId & roleId;
		}

		//
		UInt8		disconnectReason;
		UInt32		accId;
		ObjID_t		roleId;

		const static Avalon::UInt32 _size_ = 16;
	protected:
	private:
	};

	// 通知ping值
	class RelaySvrPingNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAYSERVER_PING_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accId & roleId & ping;
		}

		UInt32		accId;
		ObjID_t		roleId;
		UInt32		ping;
	};

	// 用户请求观战
	class RelaySvrObserveReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAYSERVER_OBSERVE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid & roleId & raceId & startFrame;
		}

		UInt32 accid;
		UInt64 roleId;
		UInt64 raceId;
		UInt32 startFrame;
	};

	// 用户请求观战
	class RelaySvrObserveRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAYSERVER_OBSERVE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32 result;
	};

	//RELAYSERVER_FRAMECHECKSUM_REQ
	class FrameChecksum
	{
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & framesequence & checksum;
		}

	public:
		UInt32		framesequence;			//帧序号
		UInt32		checksum;				//帧校验值

	protected:
	private:
	};

	class RelaySvrFrameChecksumRequest : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAYSERVER_FRAMECHECKSUM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & frameschecksum;
		}

		//
	public:
		FrameChecksum					frameschecksum;
		
	protected:
	private:
	};

	class RelaySvrReconnectReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAYSERVER_RECONNECT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & pos & accid & roleId & gameSessionId & lastFrame;
		}

	public:
		UInt8  pos;
		UInt32 accid;
		UInt64 roleId;
		UInt64 gameSessionId;
		UInt32 lastFrame;
	};

	class RelaySvrReconnectRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAYSERVER_RECONNECT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32 result;
	};

	class RelaySvrReconnectFrameData : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAYSERVER_RECONNECT_FRAME_DATE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & finish & frames;
		}

	public:
		UInt8				finish;
		std::vector<Frame>	frames;
	};


	class RelaySvrReportLoadProgress : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAYSERVER_REPORT_LOAD_PROGRESS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & progress;
		}

	public:
		UInt8		progress;
	};

	class RelaySvrNotifyLoadProgress : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAYSERVER_NOTIFY_LOAD_PROGRESS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & pos & progress;
		}

	public:
		UInt8		pos;
		UInt8		progress;
	};

	class RelaySvrRaceInitPacket : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAYSERVER_RACE_INIT_PACKET)
	public:
		RelaySvrRaceInitPacket()
		{
			packet = Avalon::Packet::Create();
		}

		virtual ~RelaySvrRaceInitPacket()
		{
			if (packet)
			{
				Avalon::Packet::Destroy(packet);
			}
		}

		void Detach()
		{
			packet = NULL;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceId & *packet;
		}

	public:
		ObjID_t			raceId;
		// 初始化包
		Avalon::Packet*	packet;
	};
	
	
}//end of namespace CLProtocol

#endif
