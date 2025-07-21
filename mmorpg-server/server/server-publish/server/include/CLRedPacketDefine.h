#ifndef __CL_RED_PACKET_DEFINE_H__
#define __CL_RED_PACKET_DEFINE_H__

#include <CLDefine.h>
#include <AvalonNetStream.h>

typedef PlayerIcon RedPacketPlayerIcon;

// ���״̬
enum RedPacketStatus
{
	// ��ʼ״̬
	RED_PACKET_STATUS_INIT,
	// δ���
	RED_PACKET_STATUS_UNSATISFY,
	// �ȴ�������ȡ���
	RED_PACKET_STATUS_WAIT_RECEIVE,
	// ����
	RED_PACKET_STATUS_RECEIVED,
	// �ѱ�����
	RED_PACKET_STATUS_EMPTY,
	// �ɴݻ�
	RED_PACKET_STATUS_DESTORY,
};

// �������
enum RedPacketType
{
	// ������
	RED_PACKET_TYPE_GUILD = 1,
	// ȫ�����
	RED_PACKET_TYPE_NEW_YEAR = 2,
};

// �����������
enum RedPacketThreeType
{
	// ����ȫ���Ա
	RPT_THR_TYPE_GUILD_ALL = 1,
	// ���빫��ս��Ա
	RPT_THR_TYPE_GUILD_BATTLE = 2,
	// ����������ս��Ա
	RPT_THR_TYPE_GUILD_CROSS_BATTLE = 3,
	// ���빫����³ǳ�Ա
	RPT_THR_TYPE_GUILD_DUNGEON = 4,

	RPT_THR_TYPE_GUIL_MAX,
};

// ���������Ϣ
struct RedPacketBaseEntry
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & ownerId & ownerName & status & opened & type & reason & totalMoney & totalNum & remainNum & guildTimeStamp;
	}

	// ���ID
	ObjID_t		id;
	// ����
	std::string	name;
	// ������ID
	ObjID_t		ownerId;
	// ����������
	std::string	ownerName;
	// ״̬(RedPacketStatus)
	UInt8		status;
	// ��û�д򿪹�
	UInt8		opened;
	// �������(��Ӧö��RedPacketType)
	UInt8		type;
	// �����Դ
	UInt16		reason;
	// ������
	UInt32		totalMoney;
	// �������
	UInt8		totalNum;
	// ���ʣ������
	UInt8		remainNum;
	// ����ϵ��ս����ʱ���
	UInt32		guildTimeStamp;
};

// �����ȡ����Ϣ
struct RedPacketReceiverEntry
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & icon & money;
	}

	// icon
	RedPacketPlayerIcon icon;
	// ��ý��
	UInt32				money;
};

// �����ϸ��Ϣ
struct RedPacketDetail
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & baseEntry & ownerIcon & receivers;
	}

	// ������Ϣ
	RedPacketBaseEntry					baseEntry;
	// ӵ����ͷ��
	RedPacketPlayerIcon					ownerIcon;
	// ������ȡ�����
	std::vector<RedPacketReceiverEntry> receivers;
};

// �����ȡ��¼
struct RedPacketRecord 
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & guid & time & redPackOwnerName & moneyId & moneyNum & isBest;
	}
	
	//Ψһid
	ObjID_t guid;

	//ʱ��
	UInt32  time;

	//�������������
	std::string	redPackOwnerName;

	//����id
	UInt32  moneyId;

	//��������
	UInt32 moneyNum;

	//�Ƿ���Ѻ��
	UInt8  isBest;
};

// �����������Ϣ
struct GuildRedPacketSpecInfo 
{
	GuildRedPacketSpecInfo()
	{
		type = 0;
		lastTime = 0;
		joinNum = 0;
	}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & type & lastTime & joinNum;
	}
	//����
	UInt8		type;
	//ʱ��
	UInt32		lastTime;
	//��������
	UInt32		joinNum;
};

#endif