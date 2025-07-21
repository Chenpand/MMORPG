#ifndef _CENTER_HANDLER_H_
#define _CENTER_HANDLER_H_

#include <CLCenterProtocol.h>
#include "CLOpActiveProtocol.h"
#include <CLMallProtocol.h>
#include <CLRelationProtocol.h>

void HandleProtocol(CLProtocol::CenterForbidTalkListReq& protocol);

void HandleProtocol(CLProtocol::CenterForbidTalkReq& protocol);

void HandleProtocol(CLProtocol::CenterUnForbidTalkReq& protocol);

void HandleProtocol(CLProtocol::CenterTransmitToZone& protocol);

void HandleProtocol(CLProtocol::CenterBroadcastToZones& protocol);

void HandleProtocol(CLProtocol::CenterPushNotify& protocol);

void HandleProtocol(CLProtocol::CenterForbidAccountReq& protocol);

//��Ӫ�
void HandleProtocol(CLProtocol::WCCDKVerifyReq& req);

//��Ӫ�
void HandleProtocol(CLProtocol::CenterCustomServiceSignReq& req);

void HandleProtocol(CLProtocol::CenterReportPlayerRoleList& protocol);

void HandleProtocol(CLProtocol::CenterReportPlayerCheat& protocol);

void HandleProtocol(CLProtocol::CenterGlobalServerInfoReq& protocol);

void HandleProtocol(CLProtocol::CenterPlayerAddictTimeSync& protocol);

void HandleProtocol(CLProtocol::WorldGmOpActivtyReq& protocol);

//����а�
void HandleProtocol(CLProtocol::CenterActivityYearSortListSync& protocol);

void HandleProtocol(CLProtocol::CenterQueryConvertAccountReq& protocol);

void HandleProtocol(CLProtocol::CenterPunishInfoReq& protocol);

//���
void HandleProtocol(CLProtocol::CenterSyncRedPacketInfo& protocol);

void HandleProtocol(CLProtocol::CenterDeleteRedPacketInfo& protocol);

void HandleProtocol(CLProtocol::WorldCheckRechargePushReq& protocol);

void HandleProtocol(CLProtocol::CenterAccountLoginTimeSync& protocol);

void HandleProtocol(CLProtocol::GASCheckHireBindRet& protocol);

void HandleProtocol(CLProtocol::WorldQueryHireAlreadyBindRes& protocol);


#endif
