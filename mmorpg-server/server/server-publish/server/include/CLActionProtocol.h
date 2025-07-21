#ifndef _CL_ACTION_PROTOCOL_H_
#define _CL_ACTION_PROTOCOL_H_

#include "CLProtocolDefine.h"

namespace Protocol
{
	/**
	 *@brief server->client ֪ͨ��ʼһ��������������������
	 */
	class SceneBeginAction : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BEGIN_ACTION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & actionType & msg & secs;
		}

	public:
		//������������ 0���� 1���ɼ� 2������
		UInt8		actionType;
		//��ʾ
		std::string	msg;
		//ʣ������
		UInt16		secs;
	};

	/**
	 *@brief server->client �жϵ�ǰ����
	 */
	class SceneSuspendAction : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SUSPEND_ACTION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};
};

#endif
