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
 *@brief 插入命令处理函数
 */
class InsertFunction : public RecordFunction
{
public:
	void operator()(Avalon::Packet* packet) const;

	void Handle(UInt32 srcZoneId, UInt32 srcNodeId, Avalon::Packet* packet) const;
};

/**
 *@brief 更新命令处理函数
 */
class UpdateFunction : public RecordFunction
{
public:
	void operator()(Avalon::Packet* packet) const;

	void Handle(UInt32 srcZoneId, UInt32 srcNodeId, Avalon::Packet* packet) const;
};

/**
 *@brief 删除命令处理函数
 */
class DeleteFunction : public RecordFunction
{
public:
	void operator()(Avalon::Packet* packet) const;

	void Handle(UInt32 srcZoneId, UInt32 srcNodeId, Avalon::Packet* packet) const;
};

/**
 *@brief 查询命令处理函数
 */
class SelectFunction : public RecordFunction
{
public:
	void operator()(Avalon::Packet* packet) const;

	void Handle(UInt32 srcZoneId, UInt32 srcNodeId, Avalon::Packet* packet) const;
};

/**
 *@brief 回写命令处理函数
 */
class FlushFunction : public RecordFunction
{
public:
	void operator()(Avalon::Packet* packet) const;

	void Handle(UInt32 srcZoneId, UInt32 srcNodeId, Avalon::Packet* packet) const;
};

void HandleProtocol(CLProtocol::RecordGuidseedReq& protocol);

/**
 *@brief 改名处理函数
 */
class ChangeNameFunction : public RecordFunction
{
public:
	void operator()(Avalon::Packet* packet) const;

	void Handle(UInt32 srcZoneId, UInt32 srcNodeId, Avalon::Packet* packet) const;
};

/**
*@brief 主键自增插入命令处理函数
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
