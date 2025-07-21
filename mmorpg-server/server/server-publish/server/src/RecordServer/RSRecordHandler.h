#ifndef _RS_RECORD_HANDLER_H_
#define _RS_RECORD_HANDLER_H_

#include <CLPacketExecutor.h>
#include <CLRecordProtocol.h>

class RecordFunction : public CLPacketFunction
{
public:
	virtual void operator()(Avalon::Packet* packet) const{}
	virtual void Handle(UInt32 srcZoneId, UInt32 srcNodeId, Avalon::Packet* packet) const{}
};

/**
 *@brief �����������
 */
class InsertFunction : public RecordFunction
{
public:
	void operator()(Avalon::Packet* packet) const;

	void Handle(UInt32 srcZoneId, UInt32 srcNodeId, Avalon::Packet* packet) const;
};

/**
 *@brief �����������
 */
class UpdateFunction : public RecordFunction
{
public:
	void operator()(Avalon::Packet* packet) const;

	void Handle(UInt32 srcZoneId, UInt32 srcNodeId, Avalon::Packet* packet) const;
};

/**
 *@brief ɾ���������
 */
class DeleteFunction : public RecordFunction
{
public:
	void operator()(Avalon::Packet* packet) const;

	void Handle(UInt32 srcZoneId, UInt32 srcNodeId, Avalon::Packet* packet) const;
};

/**
 *@brief ��ѯ�������
 */
class SelectFunction : public RecordFunction
{
public:
	void operator()(Avalon::Packet* packet) const;

	void Handle(UInt32 srcZoneId, UInt32 srcNodeId, Avalon::Packet* packet) const;
};

/**
 *@brief ��д�������
 */
class FlushFunction : public RecordFunction
{
public:
	void operator()(Avalon::Packet* packet) const;

	void Handle(UInt32 srcZoneId, UInt32 srcNodeId, Avalon::Packet* packet) const;
};

void HandleProtocol(CLProtocol::RecordGuidseedReq& protocol);

/**
 *@brief ����������
 */
class ChangeNameFunction : public RecordFunction
{
public:
	void operator()(Avalon::Packet* packet) const;

	void Handle(UInt32 srcZoneId, UInt32 srcNodeId, Avalon::Packet* packet) const;
};

/**
*@brief �������������������
*/
class AutoGuidInsertFunction : public RecordFunction
{
public:
	void operator()(Avalon::Packet* packet) const;

	void Handle(UInt32 srcZoneId, UInt32 srcNodeId, Avalon::Packet* packet) const;
};

void HandleProtocol(CLProtocol::RecordTransmitRequest& protocol);

void HandleProtocol(CLProtocol::RecordTransmitResponse& protocol);

void HandleProtocol(CLProtocol::RecordNotifyOtherDisconnected& protocol);

#endif
