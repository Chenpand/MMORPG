#ifndef _CL_WORLD_ADVENTURE_PASS_PROTOCOL_H_
#define _CL_WORLD_ADVENTURE_PASS_PROTOCOL_H_

#include "CLProtocolDefine.h"

namespace CLProtocol
{
	/**
	*@brief client->world ��ѯð��ͨ��֤���
	*/
	class WorldAventurePassStatusReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AVENTURE_PASS_STATUS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}

	public:

	};

	/**
	*@brief world -> client ��ѯð��ͨ��֤�������
	*/
	class WorldAventurePassStatusRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AVENTURE_PASS_STATUS_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & lv & startTime & endTime & seasonID & exp & type & activity & normalReward & highReward;
		}

		WorldAventurePassStatusRet()
		{

			lv = 0;
			startTime = 0;
			endTime = 0;
			seasonID = 0;
			normalReward = "";
			highReward = "";
		}
	public:
		//ð��ͨ��֤�ȼ�
		UInt32 lv;
		//��ʼʱ��
		UInt32 startTime;
		//����ʱ��
		UInt32 endTime;
		//��ǰ����id
		UInt32 seasonID;
		//����
		UInt32 exp;
		//ͨ��֤����
		UInt8 type;
		//�˺Ż�Ծ�����
		UInt32 activity;
		//��ͨ������ȡ���
		std::string	normalReward;
		//�߼�������ȡ���
		std::string highReward;
	};

	/**
	*@brief client->world ����ð��ͨ��֤
	*/
	class WorldAventurePassBuyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AVENTURE_PASS_BUY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}

	public:
		//ͨ��֤��������
		UInt8 type;
	};

	/**
	*@brief world -> client ����ð��ͨ��֤����
	*/
	class WorldAventurePassBuyRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AVENTURE_PASS_BUY_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}

	public:
		//ð��ͨ��֤����
		UInt8 type;
	};


	/**
	*@brief client->world ����ͨ��֤�ȼ�
	*/
	class WorldAventurePassBuyLvReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AVENTURE_PASS_BUY_LV_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & lv;
		}

	public:
		//����ĵȼ�����
		UInt32 lv;
	};

	/**
	*@brief world -> client ����ð��ͨ��֤�ȼ�����
	*/
	class WorldAventurePassBuyLvRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AVENTURE_PASS_BUY_LV_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & lv;
		}

	public:
		//����ĵȼ����� 0Ϊʧ��
		UInt32 lv;
	};
	

	/**
	*@brief client->world ��ȡ��ѯ�����
	*/
	class WorldAventurePassExpPackReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AVENTURE_PASS_EXP_PACK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & op;
		}

	public:
		//0 �ǲ�ѯ 1����ȡ
		UInt8 op;
	};

	/**
	*@brief world -> client ��ȡ��ѯ���������
	*/
	class WorldAventurePassExpPackRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AVENTURE_PASS_EXP_PACK_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}

	public:
		//0 δ���� 1�ѽ���δ��ȡ 2����ȡ
		UInt8 type;
	};

	/**
	*@brief client->world ��ȡͨ��֤�ȼ�����
	*/
	class WorldAventurePassRewardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AVENTURE_PASS_REWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & lv;
		}

	public:
		UInt32 lv;
	};

	/**
	*@brief world -> client ��ȡͨ��֤�ȼ���������
	*/
	class WorldAventurePassRewardRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AVENTURE_PASS_REWARD_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & lv & errorCode;
		}

	public:
		//������ȡ�ĵȼ�
		UInt32 lv;
		//��ȡ���
		UInt32 errorCode;
	};

}

#endif