#ifndef _CL_OFFLINE_EVENT_DEFINE_H_
#define _CL_OFFLINE_EVENT_DEFINE_H_

#include <CLDefine.h>

/**
*@brief 全局离线事件查询返回类型
*/
enum GlobalOfflineEventQueryType
{
	GOEQT_NO_CACHE = 0,		// 没有缓存
	GOEQT_NO_EVENT,			// 有缓存但没有离线事件	
	GOEQT_HAS_EVENT,		// 有离线事件
};

/**
 *@brief 离线事件类型
 */
enum OfflineEventType
{
	OFFLINE_EVENT_INVALID = 0,
	OFFLINE_EVENT_IMPEACHFAILED_FORLEAVE = 1,		//离开部落弹劾失败
	OFFLINE_EVENT_IMPEACHFAILED_FORONLINE = 2,		//首领上限弹劾失败
	OFFLINE_EVENT_BEIMPEACHED = 3,					//被弹劾
	OFFLINE_EVENT_BETRIBELEADER = 4,				//成为部落首领
	OFFLINE_EVENT_BACKYARDOP = 5,					//后院操作  ---废弃
	OFFLINE_EVENT_FIRETRIBE = 6,					//开除出部落
	OFFLINE_EVENT_INC_INTIMACY = 7,					//增加好感度
	OFFLINE_EVENT_ESCORTTARGET_KILLED = 8,			//护送目标被杀
	OFFLINE_EVENT_ESCORTTARGET_DISAPPEAR = 9,		//远离
	OFFLINE_EVENT_GIVETITLE = 10,					//授予一个称号
	OFFLINE_EVENT_GIVETRANSAWARD = 11,				//给运送经验和贡献
	OFFLINE_EVENT_ADD_MALLITEM_BUYCOUNT = 12,		//商城限量商品购买计数
	OFFLINE_EVENT_DIVORCE = 13,						//结缘

	OFFLINE_INVITE_FRIEND = 14,						//邀请好友
	OFFLINE_SELL_AUCTION = 15,						//拍卖行 售出
	OFFLINE_REPLY_FRIEND = 16,						//接受好友

	OFFLINE_PRIVATE_CHAT = 17,						//离线私聊
	OFFLINE_REQUEST_MASTER = 18,					//离线请求拜师
	OFFLINE_REQUEST_DISCILPE = 19,					//离线请求收徒

	OFFLINE_JOIN_GUILD = 20,						//加入工会
	OFFLINE_GET_TITLE = 21,							//获得头衔
	OFFLINE_GOLD_CONSIGNMENT_POINT = 22,			//金币寄售红点
};

#endif // !_CL_OFFLINE_EVENT_DEFINE_H_

