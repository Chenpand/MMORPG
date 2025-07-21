#ifndef __GAMEPLATFORM_MESSAGE_H__
#define __GAMEPLATFORM_MESSAGE_H__

// #include "gameplatform_types.h"
#include "CSMessages_i.h"

//////////////////////////////////////////////////////////////////////////
// SS协议  特别注意: ID范围: [0x4000 -- 0x7FFD]
//////////////////////////////////////////////////////////////////////////
//Logic server  VS Game DB server
#define SS_MSG_GETGAMEDATA		                    0x4000			//(must)
#define SS_MSG_LOCKMONEY		                    0x4001
#define SS_MSG_UPDATEPOINT		                    0x4002          //must
#define SS_MSG_REFRESH_GAMEPOINT	                0x4003			//notify game db -> logic server，Point与Money的变化都通过此值命令来实现.(must)
#define SS_MSG_NOTIFY_LOCKMONEY		                0x4004			//game db -> logic server notify 通知其他服务器游戏币加锁/解锁
#define SS_MSG_UPDATE_GAME_CONFIG	                0x4005			//
#define SS_MSG_LOGICSERVER_INIT		                0x4006			//notify logic server -> game db 通知gamedb logic server进行了初始化启动(must)
#define SS_MSG_NOTIFY_DELETE_PLAYER_REFRENCE	    0x4007			//notify logic server -> game db 通知game db 删除玩家的游戏数据索引
#define SS_MSG_REFRESH_GAMECONFIG	                0x4008          //game db -> logic server notify 通知其它服务器游戏该游戏的配置已经更新 add by lz 2008.04.08
#define SS_MSG_CHANGEMONEY			                0x4009			//更改游戏币(must)
#define SS_MSG_UPDATE_PLAY_COMMON_DATA		  	    0x4010			//更新player的基本数据(除了coin之外的) add by lz 2008.08.16
#define SS_MSG_REFRESH_PLAY_COMMON_DATA			    0x4011			//通知player的基本数据变化 add by lz 2008.08.16
#define SS_MSG_RECORD_SHARED_MONEY				    0x4012			//logic server --> game db. 不需要response
#define SS_MSG_GET_PLAYER_ALL_GAME_DATA		        0x4013			//profile server -->game db,拉取指定玩家的游戏数据.
#define SS_MSG_UPDATE_EXT_GAME_INFO					0x4014			//service server -->game db,更新玩家的某个游戏的配置数据
#define SS_MSG_REFRESH_EXT_GAME_INFO				0x4015			//game db--->service server,通知前端游戏服务器某个玩家的游戏的配置数据的刷新
#define SS_MSG_UPDATE_PLAYER_CARDID                 0x4016          //更新用户身份证号
#define SS_MSG_PLAYER_CHARGE                        0x4017          //用户充值
#define SS_MSG_PLYAER_EXCHANGE                      0x4018          //用户回兑
#define SS_MSG_GET_PLAYER_ENTHRALMENTDATA           0x4019          //拉取玩家的防沉迷资料.
#define SS_MSG_NOTIFY_FORBIDDEN_TALK				0x401A			//禁止发言	
#define SS_MSG_PRESENT_HAPPY_BEAN					0x401B			//赠送欢乐豆
#define SS_MSG_NOTIFY_PRESENT_HAPPY_BEAN          0x401C			//赠送欢乐豆的通知消息
#define SS_MSG_LOCK_HAPPY_BEAN                      0x401D          //锁欢乐豆
#define SS_MSG_NOTIFY_LOCK_HAPPY_BEAN			    0x401E          //刷新欢乐豆的锁
#define SS_MSG_CHANGE_HAPPY_BEAN                    0x401F          //变更欢乐豆
#define SS_MSG_NOTIFY_REFRESH_HAPPY_BEAN            0x4020          //变更欢乐豆的通知.

//#define SS_MSG_SET_PLAYER_ACCOUNT                 0x4020          //设置某人帐户
#define SS_MSG_UPDATE_PLAYER_ENTHRALMENTDATA        0X4021          //更新玩家的防沉迷资料
#define SS_MSG_NOTIFY_PLAYER_CONSUME_DATA           0x4022          //通知玩家的消费情况(GameDB-->监控服务器)
#define SS_MSG_QUERY_PLATFORM_MONEY_LOCK	    0x4023          //查询平台的金币锁...
#define SS_MSG_UPDATE_PLATFORM_MONEY_LOCK           0x4024			//更新平台的金币锁...
#define SS_MSG_REFRESH_PLATFORM_MONEY_LOCK          0x4025          //反向通知游戏逻辑服务器,平台金币锁的变化....
#define SS_MSG_REGISTER_SERVICE                     0x4026          //注册服务
#define SS_MSG_REFRESH_SERVICE                      0x4027          //反向服务刷新通知....
#define SS_MSG_RENEW_SERVICE                        0x4030          //服务续费
#define SS_MSG_GET_USER_DETAIL_INFO                 0x4028          //拉取用户的详细信息...(目前仅供后台管理平台使用,如gm)
#define SS_MSG_CHECK_USER_IS_EXIST                  0x4029          //检查用户存在(目前仅供hall server使用)
#define SS_MSG_NOTIFY_GAME_RESULT                   0x4030          //通知游戏结果(目前仅供Action Server使用)
#define SS_MSG_NOTIFY_QUN_PLAYER_DATA               0x4031          //群用户的数据变更通知
#define SS_MSG_NOTIFY_GAMEDATA_HEARTBEAT	    0x4032			//通知GameDB游戏数据心跳包
#define	SS_MSG_NOTIFY_PLAYER_ACTIONDATA             0x4033          //通知玩家活动消息包.
#define SS_MSG_GET_PLAY_COMMON_DATA                 0x4034          //拉取玩家基本资料
#define SS_MSG_NOTIFY_PLAYER_COMMON_DATA            0x4035          //用户上线通知hall server该用户的common data数据
#define SS_MSG_GET_USER_PROFILE                     0x4036          //拉取用户的个人资料(profile专用)
#define SS_MSG_KICK_PLAYER			    0x4100			//踢人
#define SS_MSG_BROADCAST_SYSTEM_MESSAGE		    0x4101			//广播系统消息
#define SS_MSG_NOTIFY_BROADCAST_NOTICE_INFO	    0x4102			//发布公告消息

#define SS_MSG_REFRESH_VIP_DATA                         0x4201          //刷新用户的VIP数据 vipserver-->gamedb server
#define SS_MSG_VIP_NOTIFY_MESSAGE                       0x4202          //通知用户的VIP相关的消息
#define SS_MSG_UPDATE_VIP_DATA                          0x4203          //(GM)手动更新用户的VIP数据.


//logic server vs antibot server

#define SS_MSG_ANTIBOT_DATA						    0x4151          //反外挂数据
#define SS_MSG_ANTIBOT_PUNISH						0x4152          //反外挂惩罚

//logic server vs compete db
#define SS_MSG_GET_COMPETE_GAME_DATA				0x4300			//获取比赛积分
#define SS_MSG_UPDATE_COMPETE_GAME_POINT			0x4301			//更新比赛积分
#define SS_MSG_REFRESH_COMPETE_GAME_POINT			0x4302			//通知更新比赛积分
#define SS_MSG_NOTIFY_PLAYER_LEAVE_COMPETE			0x4303			//通知玩家离开游戏

//其他服务器与hotnews server之间的协议
#define SS_MSG_NOTIFY_HOTNEWS			            0x4E00          //好友动态

//其他服务器与router server之间的协议
#define SS_MSG_ROUTER_HEARTBEAT			            0x4F00			//与router server之间的heart beat
#define SS_MSG_ROUTER_REGISTER			            0x4F01			//其他服务器向router server自己


//vip server


//Logic server VS Catalog server
#define SS_MSG_HELLO			                    0x5000
#define SS_MSG_REGISTER			                    0x5001			// s<->s 注册服务器，拉取配置信息
#define SS_MSG_REPORTSTATICS	                    0x5002			//上报统计信息 notify



//logic server VS State Server （should be deleted）
#define SS_MSG_NOTIFY_LOGICSERVER_INIT		        0x5100			//logic server 通知state server 自身初始化
//#define SS_MSG_NOTIFY_PLAYER_STATUS			        0x5101			//logic server上报玩家状态给state server
//#define SS_MSG_NOTIFY_HEARTBEAT_LOGIC_STATE	        0x5102			//logic server与state server之间的心跳包


//Profile server VS Game DB server
#define SS_MSG_GETUSERINFO                          0x6000          //profile server -> game db server add by lz 2008.04.08
#define SS_MSG_GETOTHERUSERINFO                     0x6001          //profile server -> game db server add by lz 2008.04.08


#define SS_MSG_AD_URL								0x6200			//magic server <--> ad server，获取广告URL

#define SS_MSG_PUBLISH_AD							0x6201    // gm server <--> ad server 发布广告
#define SS_MSG_DELETE_AD							0x6202    //  gm server <--> ad server 删除广告
#define SS_MSG_GET_AD_LIST							0x6203    //  gm server <--> ad server 查询广告
#define SS_MSG_SEND_MAC_LIST						0x6204     //  gm server <--> ad server 发送mac地址列表
//hall server vs score server
#define SS_MSG_NOTIFY_51_SCORE_DELTA				0x7300			//积分服务器通知用户所在的hall，ta的51积分增量
#define SS_MSG_NOTIFY_MESSAGEBOX				    0x7301
#define SS_MSG_NOTIFY_LOGIN     				    0x7302
//HallServer、LogicServer、StateServer等 VS Friend DB server 
#define SS_MSG_GET_POSITIVE_FRIEND_LIST             0x7400          //获取自己的正向玩伴列表
#define SS_MSG_GET_STATISTICAL_TAG                  0x7401          //获取某人的统计标签，目前最多10项
#define SS_MSG_ADD_TAG_TO_FRIEND                    0x7402          //给人贴标签，并自动添加为玩伴
#define SS_MSG_DELETE_FRIEND                        0x7403          //删除玩伴
#define SS_MSG_GET_REVERSE_FRIEND_LIST              0x7404          //获取自己的逆向玩伴列表
#define SS_MSG_ADD_REVERSE_TAG                      0x7405          //贴逆向标签
#define SS_MSG_NOTIFY_FRIEND_INFO                   0x7406          //全量通知某个玩伴的标签信息
#define SS_MSG_GET_RECOMMEND_TAG                    0x7407          //获取玩家的推荐标签，目前从统计标签中取6个，不足6个取全部
#define SS_MSG_DELETE_REVERSE_FRIEND				0x7408			//删除逆向玩伴
#define SS_MSG_GET_SIMPLE_FRIEND_LIST				0x7409			//获取简单的玩伴列表（只有玩伴的UIN）
#define SS_MSG_ADD_FRIEND_GROUP						0x740A			//玩伴列表中添加自定义分组
#define SS_MSG_DEL_FRIEND_GROUP						0x740B			//删除玩伴列表中的自定义分组
#define SS_MSG_MOVE_FRIEND_TO_GROUP					0x740C			//把玩伴移到指定的分组中
#define SS_MSG_RENAME_FRIEND_GROUP					0x740D			//自定义玩伴分组重命名
#define SS_MSG_GET_FRIEND_GROUPS					0x740E			//拉去玩家的玩伴分组信息
#define SS_MSG_RENAME_FRIEND_REMARKS				0x740F			//更新玩伴的备注
#define SS_MSG_GET_PLAYED_WITH_INFO					0x7410			//拉去和谁一起玩过的信息
#define SS_MSG_ADD_PLAYED_WITH_INFO					0x7411			//添加和谁一起玩过的信息
#define SS_MSG_NOTIFY_DEL_PLAYED_WITH_INFO			0x7412			//通知删除某个和谁一起玩过的信息
#define SS_MSG_ADD_FRIEND							0x7413			//加玩伴(贴标签+分组+备注)
#define SS_MSG_GET_FRIEND_BASE_INFO					0x7414			//拉取玩伴的基本信息（UIN+账号）
#define SS_MSG_GET_BLACK_LIST_INFO					0x7415			//获取黑名单
#define SS_MSG_ADD_BLACK_LIST_INFO					0x7416			//添加黑名单
#define SS_MSG_DEL_BLACK_LIST_INFO					0x7417			//删除黑名单
#define SS_MSG_ADD_NOT_VERIFIED_FRIEND_INFO			0x7418			//添加未验证的好友信息
#define SS_MSG_CHECK_NOT_VERIFIED_FRIEND_INFO		0x7419			//核实未验证的好友信息
#define SS_MSG_NOTIFY_MAKE_FRIEND_COUNT_INFO		0x741A			//通知加好友的数量信息
#define SS_MSG_NOTIFY_GET_CHAT_WITH_INFO			0x741B			//通知加载和谁聊过的信息
#define SS_MSG_NOTIFY_ADD_CHAT_WITH_INFO			0x741C			//通知添加和谁聊过的信息
#define SS_MSG_NOTIFY_DEL_CHAT_WITH_INFO			0x741D			//通知删除和谁聊过的信息



//hall server与其它后端服务器公共的协议，比如说初始化、上报玩家状态等
//hall server vs state、game db、item db、friend db server
#define SS_HALL_NOTIFY_INIT						    0x7500		    //hall启动时，向state、friend db、game db、item db server注册
#define SS_HALL_NOTIFY_PLAYER_STATE 			    0x7501		    //hall向state、friend db、game db、item db server上报玩家状态，上线、下线等
#define SS_MSG_BROADCAST_TRANSFRE_MESSAGE			0x7502			//请求向指定类型的所有目的服务器转发透明数据
#define SS_MSG_NOTIFY_BROADCAST_TRANSFRE_MESSAGE	0x7503			//向指定类型的所有目的服务器转发透明数据

//gm server和state server
#define SS_MSG_BROADCAST_GM_SYSTEM_MESSAGE          0x7600          //GM发送系统消息

//GM Server VS(Logic&Hall Server)
#define SS_MSG_RELAY_SYSTEM_MESSAGE					0x7601			//GM通过logic server 和(或)hall server发生系统消息
#define	SS_MSG_SUBMIT_SYSTEM_MESSAGE				0x7602			//GM 提交（设置）系统消息到logic & hall server
#define SS_MSG_CANCEL_SYSTEM_MESSAGE				0x7603			//GM 取消(清除)原先提交（设置）给logic & hall Server的系统消息

//GM Server VS State Server
#define SS_GM_GET_PLAYER_STATE						0x7604			//GM请求获取玩家状态
#define SS_GM_GET_ALL_PLAYER_STATE					0x7605			//GM请求查看State Server中的所有内存数据

#define SS_MSG_CANCEL_MESSAGE				        0x7606			//GM HallServer删除原先设定的系统消息，并且通知客户端删除指定的已收到的消息 
#define SS_MSG_SEND_SYSTEM_MESSAGE_TO_PLAYER        0x7607          //GM <-> HallServer 给指定用户发送系统消息

//State Server
#define SS_MSG_REGISTER_TO_STATESERVER				0x7610			//向StateServer注册服务
#define SS_MSG_STATESERVER_NOTIFY_PLAYER_STATE		0x7611			//StateServer通知玩家状态变化到已经注册服务的服务器
#define SS_MSG_STATE_NOTIFY_BROADCAST_MSG_INFO		0x7612			//向logic server的房间或hall server下发广播消息

//hall server vs state server
#define SS_HALL_STATE_REPORT_GAME_STATE				0x7700		    //客户端向hall上报当前游戏状态，hall通知给所以state
#define SS_HALL_STATE_UPDATE_PLAYER_PROFILE			0x7701			//客户端更新基本资料，hall通知给所以state
#define SS_HALL_STATE_REQUEST_PLAYER_STATE			0x7703		    //cliet向hall请求获取玩家状态，hall转发给state
#define SS_HALL_STATE_REQUEST_PLAYERCOUNT_IN_HALL	0x7704		    //hall向state请求大厅在线人数
#define SS_HALL_STATE_HEARTBEAT						0x7705		    //心跳，可能跟请求在线人数在一起处理
#define SS_HALL_STATE_NOTIFY_FRIEND_ONLINE			0x7706		    //通知好友上线                 state -> hall
#define SS_HALL_STATE_NOTIFY_KICKPLAYER				0x7707		    //已在其它地方登陆，通知玩家关闭游戏
#define SS_HALL_STATE_REQUEST_TRANSFER_MESSAGE		0x7708		    //请求state中转一个消息
#define SS_HALL_STATE_NOTIFY_TRANSFER_MESSAGE		0x7709		    //通知hall有消息要转给client   state -> hall
#define SS_STATE_HALL_GET_REVERSE_FRIEND_UINS		0x770A			//state server拉取逆向玩伴的UIN
#define SS_HALL_STATE_NOTIFY_BROADCAST_MESSAGE		0x770B			//通知state server广播消息
#define SS_STATE_HALL_NOTIFY_STATUS_TOFRIEND		0x7710		    //State Server向Hall主动push用户的状态信息给用户反向好友

//hall server vs item db
//#define SS_HALL_ITEM_NOTIFY_PLAYER_STATUS         0x7800          //hall向item db上报玩家状态，login和logout del by lz 2008.08.21 SS_HALL_NOTIFY_PLAYER_STATE
#define SS_ITEMDB_UPDATE_PLAYER_ITEMINFO			0x7801	        //其他服务器与Item DB之间更新玩家物品信息的协议	
#define SS_ITEM_HALL_NOTIFY_PLAYER_INFO             0x7802          //item db下发玩家物品更新信息给hall
#define SS_MSG_ITEMDB_GET_USER_ITEM_INFO		    0x7803	        //业务服务器(如hall server,...)查询某个用户的道具或魅力物品信息...
#define SS_MSG_ITEMDB_TRANSFER_MESSAGE				0x7804          //业务服务器与ItemDB之间传递消息(此消息为通知类消息)
#define SS_MSG_DELETE_ITEM                          0x7805          //删除指定的道具，包括所有用户
#define SS_MSG_CLEAN_EXPIRE_ITEM                    0x7806          //清除过期道具,包括所有用户
#define SS_MSG_QUERY_PLAYER_ITEMDATA                0x7807          //查询用户的指定类型的道具
#define SS_MSG_UPDATE_PLAYER_EQUIPMENT_INFO         0x7808          //更新用户的装备信息
#define SS_MSG_REFRESH_PLAYER_EQUIPMENT_INFO        0x7809          //刷新用户的装备信息

//state server VS Catalog server
#define SS_MSG_STATE_SERVER_HELLO					0x7900
#define SS_MSG_STATE_SERVER_REGISTER				0x7901						// s<->s 注册服务器，拉取配置信息
#define SS_MSG_STATE_SERVER_REPORTSTATICS			0x7902						//上报统计信息 notify
#define WS_MSG_WEB_REPORTSTATICS					0x7903						//web节点上报统计信息 notify


//Misc server VS Group server
#define SS_MSG_GET_GROUP_RANK						0x7A01         //拉取指定群排名
#define SS_MSG_GET_GROUP_TOP_RANK					0x7A02         //拉取群top排名


//offline server vs hall server
#define SS_MSG_PUSH_OFFLINE_MSG					0x7A10


//pay.51.com  VS proxy server
#define CS_MSG_PAY_TO_GAME							0x7B01
#define CS_MSG_PAY_FROM_GAME						0x7B02
#define CS_MSG_SEARCH_GAME_MONEY					0x7B03
#define CS_MSG_PAY_TO_VIP					        0x7B04        //PayServer直接注册用户VIP信息
#define CS_MSG_SEARCH_VIP_INFO                      0x7B05        //PayServer查询用户VIP信息
#define CS_MSG_PAY_MOBILE_TO_VIP					0x7B06        //手机包月VIP
#define CS_MSG_PAY_UNSUBSCRIBE_VIP					0x7B07        //手机退订包月VIP

//game.51.com  VS proxy server
#define CS_MSG_UPDATE_CARD_ID						0x7C01
#define CS_MSG_GET_USER_INFO						0x7C02
#define CS_MSG_ITEM_EXCHANGE						0x7C03
#define CS_MSG_GET_ITEM_INFO						0x7C04
#define CS_MSG_BUY_GOODS							0x7C05		//web道具商城与Market Server之间的购买/赠送协议 .双向
#define CS_MSG_RENEW_SERVICE						0x7C06
#define CS_MSG_LUCKY_DRAW						    0x7C07
#define CS_MSG_GET_ACTION_INFO						0x7C08
#define CS_MSG_MULTI_EXCHANGE   				    0x7C09
#define CS_MSG_QUERY_DRAW   				        0x7C0A
#define CS_MSG_BALANCE   				            0x7C0B
#define CS_MSG_ACTION_HIT                           0x7C0C
#define CS_MSG_QUERY_ITEM_INFO						0x7C0D
#define CS_MSG_UPDATE_EQUIPMENT						0x7C0E
#define CS_MSG_CONSIGN                              0x7C0F
#define CS_MSG_GET_ONLINE_INFO                      0x7C10
#define CS_MSG_GET_ONLINE_RANK                      0x7C11
#define CS_MSG_GET_PAIR_INFO						0x7C12
#define CS_MSG_PAIRING								0x7C13
#define CS_MSG_GET_LATEST_PAIR						0x7C14
#define CS_MSG_SEND_MESSAGE_TO_CLIENT				0x7C15

//gameadm.51.com  VS GMServer
#define CS_MSG_GM_PUNISH							0x7D01
#define CS_MSG_GM_SEND_MESSAGE						0x7D02

#define CS_MSG_GM_RELAY_SYSTEM_MESSAGE_TO_HALL      0x7D03
#define CS_MSG_GM_RELAY_SYSTEM_MESSAGE_TO_LOGIC     0x7D04
#define CS_MSG_GM_SUBMIT_SYSTEM_MESSAGE_TO_HALL     0x7D05
#define CS_MSG_GM_SUBMIT_SYSTEM_MESSAGE_TO_LOGIC    0x7D06
#define CS_MSG_GM_CANCEL_SYSTEM_MESSAGE_TO_HALL     0x7D07
#define CS_MSG_GM_CANCEL_SYSTEM_MESSAGE_TO_LOGIC    0x7D08

#define CS_MSG_GM_GET_USER_DETAIL_INFO				0x7D09 //查看用户状态，禁言，封号，解锁&加锁游戏币
#define CS_MSG_CHANGE_MONEY				            0x7D0A //修改玩家游戏币
#define CS_MSG_LOCKMONEY							0x7D0B //解锁&加锁玩家游戏币
#define CS_MSG_QUERY_PLATFORM_MONEYLOCK_TO_GAMEDB   0x7D0C //查看gamedb平台游戏币状态
#define CS_MSG_UPDATE_PLATFORM_MONEYLOCK_TO_GAMEDB  0x7D0D //更新gamedb平台游戏币状态
#define CS_MSG_GM_UPDATE_GAME_PROP                  0x7D0E //更新用户游戏道具
#define CS_MSG_GM_UPDATE_GAME_POINT                 0x7D0F //更新用户游戏积分
#define CS_MSG_GM_UPDATE_Withery_Value              0x7D10 //更新用户魅力值
#define CS_MSG_GM_UPDATE_Achievement_Value          0x7D11 //更新用户成就值
#define CS_MSG_GM_DELETE_ITEM                       0x7D12 //删除指定道具
#define CS_MSG_GM_CLEAN_EXPIRED_ITEM                0x7D13 //清除过期道具
#define CS_MSG_QUERY_PLATFORM_MONEYLOCK_TO_ITEMDB   0x7D14 //查看itemdb平台游戏币状态
#define CS_MSG_UPDATE_PLATFORM_MONEYLOCK_TO_ITEMDB  0x7D15 //更新itemdb平台游戏币状态
#define CS_MSG_GM_CHANGE_HAPPY_BEAN                 0x7D16 //修改玩家快乐豆
#define CS_MSG_GM_LOCK_HAPPY_BEAN                   0x7D17 //解锁&加锁玩家快乐豆
#define CS_MSG_GET_PLAYER_ALL_GAME_DATA             0x7D18 //获取玩家所有游戏信息
#define CS_MSG_GET_PLAYER_ITEM_DATA                 0x7D19 //获取玩家所有道具信息
#define CS_MSG_GET_PLAYER_STATE_FROM_STATE_SERVER   0x7D20 //从StateServer获取玩家所有状态信息
#define CS_MSG_GET_ALL_PLAYER_STATE                 0x7D21 //获取所有玩家状态信息
#define CS_MSG_GM_CANCEL_MESSAGE                    0x7D22 //删除系统消息
#define CS_MSG_GM_SEND_SYSTEM_MESSAGE_TO_PLAYER     0x7D23 //发送系统消息给指定用户
#define CS_MSG_GM_REGISTER_GAME_SERVICE             0x7D24 //Register游戏服务
#define CS_MSG_GM_UPDATE_VIP_DATA                   0x7D25 //更新用户VIP相关信息
#define CS_MSG_GM_REGISTER_SERVICE                  0x7D26 //给用户加服务，包括VIP服务
#define CS_MSG_GM_PUBLISH_AD                        0x7D27 //GM发布广告信息
#define CS_MSG_GM_DELETE_AD                         0x7D28 //GM取消广告
#define CS_MSG_GM_GET_AD_LIST                       0x7D29 //GM拉取广告列表
#define CS_MSG_GM_SEND_MAC_LIST                     0x7D30 //GM发送MAC地址列表

//client.51.com VS misc Server
#define CS_MSG_MISC_GET_PROFILE_FRIEND_LIST			0x7E01 //拉取简单玩伴列表
#define CS_MSG_GET_PLAYER_CARD_ID					0x7E02 //查询用户身份证信息
#define CS_MSG_GET_GROUP_RANK						0x7E03 //拉取指定群排名
#define CS_MSG_GET_GROUP_TOP_RANK					0x7E04 //拉取群top排名
#define CS_MSG_GET_FRIEND_GROUP_INFO				0x7E05 //拉取分组信息
#define CS_MSG_GET_PLAYER_GROUP_DATA				0x7E06 //拉取用户群权限
#define WS_MSG_GET_SIMPLE_FRIEND_LIST				0x7E08 //获得玩伴列表
#define WS_MSG_GET_SIMPLE_GAME_ID_LIST				0x7E09 //获取最近常玩的游戏ID


//client.51.com VS Recommend Server

#define CS_MSG_GET_RECOMMEND_USER					0x7E07  //拉取推荐用户
#define WS_MSG_GET_RECOMMEND_USER_LIST				0x7E0A  //拉取推荐用户列表
//特别注意:SS协议的ID范围: [0x4000 -- 0x7FFD]


//chat server 

#define MSG_CHAT_NOTIFY_CLEAR_PLAYER				0x5000			//清理player 
namespace KingNet 
{
	enum enmCSResultID
	{
		result_id_success						= 0,
		result_id_fail							= 1,		//未知错误，请重试！
		result_id_invalid_param					= 2,		//参数不匹配，请重试！
		result_id_acion_deny					= 3,		//被系统拒绝：不满足系统要求！您是不是想干坏事?
		result_id_action_timeout				= 4,		//系统超时啦！怎么搞的?
		result_id_action_enterroom_deny			= 5,		//进房权限不足,不允许进房！
		result_id_sequence_invalid				= 6,		//客户端的sequence错误

		result_id_action_invalid				= 10,		//无效的操作，请检查后重试！您是不是想干坏事?小心警察找您！
		result_id_sitdown_noseat				= 11,		//哦啊，座位上已经有人了！	
		result_id_sitdown_deney					= 12,		//系统拒绝您了，您做错了什么?
		result_id_lock_fail						= 13,		//锁定游戏币失败！
		result_id_unlock_fail					= 14,		//解锁游戏币失败！
		result_id_lack_resource					= 15,		//游戏币不足！
		result_id_room_deny_observe				= 16,		//该房间内不允许旁观！
		result_id_not_in_room					= 17,		//不在房间内不能旁观！
		result_id_on_other_table				= 18,		//已在其他桌游戏中，不能站起！
		result_id_table_not_gaming				= 19,		//游戏还没开局，不能旁观！

		result_id_seat_no_player				= 20,		//不能在空座位上旁观！
		result_id_game_deny_observer			= 21,		//拒绝旁观该游戏！
		result_id_table_full					= 22,		//桌上人爆满，不能再挤啦！
		result_id_invalid_table_key				= 23,		//游戏桌已上锁，请输入正确的密码！
		result_id_deny_kick						= 24,		//没有足够的权限踢人！
		result_id_sitdown_deny_lack_money		= 25,		//游戏币不足！
		result_id_enter_so_many_room			= 26,		//进入的房间太多了！
		result_id_enter_room_is_full			= 27,		//该房间人数爆满，别挤啦，选个稍微不挤的房间吧！
		result_id_replay_not_in_room			= 28,		//回不去了，您不在这个房间内！
		result_id_replay_invalid_status			= 29,		//无法续玩了，您是正在游戏中吗?

		result_id_open_so_many_game				= 30,		//不能同时玩太多的游戏，您玩这么多游戏，您行吗?
		result_id_not_be_observing				= 31,		//不在旁观状态！
		result_id_not_on_seat					= 32,		//不在指定的位置上！
		result_id_table_alread_in_gameing		= 33,		//游戏桌已经开局！
		result_id_player_not_on_table			= 34,		//不在游戏桌上！
		result_id_cant_locktable_in_compete		= 35,		//比赛房间不能锁桌！
		result_id_cant_locktable_someelse		= 36,       //桌上还有其他人,不能锁桌！
		result_id_cant_obsv_alread_sitted		= 37,		//已坐在该桌,不能旁观其他位置！
		result_id_cant_kickplayer_in_gaming		= 38,		//已在游戏中,不能踢人！ 
		result_id_target_not_in_room			= 39,		//目标不在房间内！

		result_id_buy_no_goods					= 40,		//没有指定的商品！您从哪看到有这些商品的?
		result_id_buy_not_enough_money			= 41,		//钱不够啦，赶紧去充点！
		result_id_buy_chang_money_timeout		= 42,		//无法扣费，请重试！
		result_id_must_on_table_to_kick			= 43,		//必须在游戏桌上才能踢人！
		result_id_cant_standup_in_gameing		= 44,		//已在游戏中无法站起！
		result_id_enter_room_less_charm			= 45,		//进房失败：魅力不够,至少需要%d点魅力！
		result_id_enter_room_less_yxb			= 46,		//进房失败：游戏币不足,至少需要%d游戏币！
		result_id_cant_talk_not_on_table		= 47,		//不在游戏桌上不能聊天！
		result_id_observor_cant_talk			= 48,		//本房间旁观者禁止聊天！
		result_id_cant_talk_not_in_room			= 49,		//不在房间内不能聊天！

		result_id_cant_talk_prohibit			= 50,		//系统不允许您聊天！	
		result_id_cant_talk_illegal_words		= 51,		//51游戏祝您快乐游戏，健康娱乐！
		result_id_anti_addict_tired_notify		= 52,		//由于您已经进入疲劳游戏时间,本局积分由原来的(%d)减少为(%d),51游戏温馨提醒您:欢乐51,健康游戏！
		result_id_anti_addict_abnormal_notify	= 53,	    //由于您已经进入不健康游戏时间,本局积分由原来的(%d)减少为(%d),51游戏温馨提醒您:欢乐51,健康游戏！
		result_id_client_version_so_old			= 54,		//大厅版本太低，请您先升级大厅后，重新登陆！
		result_id_zero_fail_no_gamedata			= 55,		//您还没有玩过该游戏，无法使用负分清零道具！
		result_id_zero_fail_isnot_negative		= 56,		//您该游戏的积分非负，请不要使用负分清零道具！
		result_id_kick_player_not_exist			= 57,		//踢人失败:目标玩家不存在！
		result_id_kick_player_not_inroom		= 58,		//踢人失败：目标玩家不在房间内！
		result_id_replay_game_deny				= 59,		//断线重回失败：游戏拒绝重回！
		result_id_locktable_not_master			= 60,		//锁桌失败,只有桌主才能锁桌!
		result_id_kick_player_target_is_vip		= 61,		//对方是VIP玩家，不能踢人!点击|我也成为VIP玩家!|
		result_id_kick_player_not_table_master  = 62,		//不能踢人,只有VIP玩家或是游戏桌桌主才能踢人,点击|我要成为VIP玩家!|
		result_id_kick_player_not_ahead_vip_target = 63,	//对方也是VIP玩家,您只有先于对方坐下才能踢对方,这次暂且饶了ta!
		result_id_enter_room_less_happybean		= 64,		//
		result_id_enter_room_somany_happybean	= 65,		//
		result_id_lock_table_invalid_key		= 66,		//锁桌失败，无效的密码
		result_id_lock_enter_room_fail_default  = 67,		//很遗憾，您没有达到进入该房间的条件,请选择其他房间！
		result_id_action_deny_service_halting	= 68,		//系统正在停机
		result_id_not_selectable_seat			= 69,		//找不到合适的游戏桌,请稍后重试!(应该有再试一次的按钮)
		result_id_chat_51score_not_enough		= 70,		//51积分等级不够，不能私聊
		result_id_chat_illegal_words			= 71,		//说了非法的词，不能私聊
		result_id_chat_refused_not_friend		= 72,		//不是玩伴，拒绝私聊
		result_id_assign_table_failed			= 73,		//
		result_id_wait_to_assign_table			= 74,		//等待系统为您分配游戏桌
		result_id_dogfight_no_idle_table		= 75,		//系统配桌时没有空闲游戏桌
		result_id_talk_inroom_so_fast			= 76,		//发言频率过快
		result_id_talk_too_many_words			= 77,		//一句话说了太多的字
		result_id_cant_talk_to_many_players		= 78,		//不能同时跟N多人聊天

		//added by aprilliu, 2010-01-07
		result_id_painting_enter_room_fail		= 80,		//


		string_code_be_silenced					= 100,		//您已经被禁言,在%s前您将无法发言！
		string_code_be_forbidden				= 101,		//您的帐号已被冻结, 将在%s后解封！
		string_code_first_enter_game			= 102,		//正在51游戏平台玩[%s]游戏！
		string_code_first_login_platform		= 103,		//您第一次登陆了51游戏平台！
		string_code_gameplayer_name				= 104,		//游戏玩家
		string_code_gameobserver_name			= 105,		//旁观玩家
		string_code_gamemaster_name				= 106,		//游戏网管
		string_code_talk_inroom_be_delayed		= 107,		//还有等一会您才能在房间内聊天！
		string_code_talk_inroom_less_gameround  = 108,		//要玩游戏后您才能在房间内聊天！
		string_code_talk_inroom_less_day		= 109,		//刚登录51游戏您还不能在房间内聊天！

		string_code_first_login_platform_score0 = 200,		//首次登陆游戏平台赠送51积分，发送好友动态到网站
		string_code_first_login_platform_score1 = 201,		//首次登陆游戏平台赠送51积分，发送好友动态到网站
		string_code_first_login_platform_score2 = 202,		//首次登陆游戏平台赠送51积分，发送好友动态到网站
		string_code_first_login_platform_score3 = 203,		//首次登陆游戏平台赠送51积分，发送好友动态到网站
		string_code_first_login_platform_score4 = 204,		//首次登陆游戏平台赠送51积分，发送好友动态到网站
		string_code_recv_many_flowers0			= 210,		//收到好多鲜花，发送好友动态到网站
		string_code_recv_many_flowers1			= 211,		//收到好多鲜花，发送好友动态到网站
		string_code_recv_many_flowers2			= 212,		//收到好多鲜花，发送好友动态到网站
		string_code_recv_many_flowers3			= 213,		//收到好多鲜花，发送好友动态到网站
		string_code_recv_many_flowers4			= 214,		//收到好多鲜花，发送好友动态到网站


		//大家别抢哦！已经占用的不会被删除，放心！
		//范围[300-400)的result id为client and hall的
		result_id_friend_list_full				= 300,      //不能再添加了,玩伴列表的人数已经到上限了！
		result_id_add_friend_fail				= 301,		//添加玩伴失败！
		result_id_message_not_send				= 302,	    //私聊失败！
		result_id_friend_list_null				= 304,		//没有取得玩伴列表！
		result_id_add_tag_fail					= 305,		//贴标签失败！
		result_id_delete_friend_fail			= 306,		//删除玩伴失败！
		result_id_get_stat_tags_fail			= 307,		//没有取得别人眼中的Ta！
		result_id_get_recomm_tags_fail			= 308,		//没有取得推荐标签！
		result_id_use_bomb_less_money			= 309,		//使用炸弹失败，游戏币不够啦，赶紧去充点吧！
		result_id_present_charm_less_money		= 310,		//送花失败，游戏币不够啦，赶紧去充点吧！
		result_id_use_speaker_less_money		= 311,		//使用喇叭失败，游戏币不够啦，赶紧去充点吧！
		result_id_cumulative_time_one_hour		= 312,		//您累计在线时间已满1小时！
		result_id_cumulative_time_two_hour		= 313,		//您累计在线时间已满2小时！
		result_id_cumulative_time_three_hour	= 314,		//您累计在线时间已满3小时，请您下线休息，做适当身体活动！
		result_id_cumulative_time_four_hour		= 315,		//您已经进入疲劳游戏时间，您的游戏收益将降为正常值的50%，为了您的健康，请尽快下线休息，做适当身体活动，合理安排学习生活！
		result_id_cumulative_time_five_hour		= 316,		//您已进入不健康游戏时间，为了您的健康，请您立即下线休息。如不下线，您的身体将受到损害，您的收益已降为零，直到您的累计下线时间满5小时后，才能恢复正常！
		result_id_add_51_score					= 317,		//您通过51Game的累积在线时间，获得%d个51积分！
		result_id_custom_group_full				= 318,		//您不能再添加新分组了,分组数已经到上限了！
		result_id_make_friend_another_day		= 319,		//提示用户今天已经加了够多的好友了，明天才能继续加好友
		result_id_add_group_fail				= 320,		//添加分组失败！
		result_id_del_group_fail				= 321,		//删除分组失败！
		result_id_move_friend_fail				= 322,		//把玩伴移到某个指定分组中失败！
		result_id_rename_group_fail				= 323,		//重命名分组失败！
		result_id_get_groups_fail				= 324,		//拉取分组失败！
		result_id_rename_remarks_fail			= 325,		//更新玩伴备注失败！
		result_id_first_login_hall_present_bean = 326,		//欢迎进51游戏，系统赠送您%d欢乐豆
		result_id_use_bomb_lock_money			= 327,		//使用炸弹失败，游戏币被锁定了！
		result_id_present_charm_lock_money		= 328,		//送花失败，游戏币被锁定了！
		result_id_use_speaker_lock_money		= 329,		//使用喇叭失败，游戏币被锁定了！
		result_id_use_zero_point_less_money		= 330,		//使用负分清零，游戏币不足
		result_id_use_zero_point_lock_money		= 331,		//使用负分清零，游戏币锁定
		result_id_use_small_speaker_success		= 332,		//使用小喇叭成功
		result_id_use_zero_point_success		= 333,		//使用负分清零成功
		result_id_refuse_make_friends			= 334,		//对方拒绝加玩伴
		result_id_refuse_private_chat			= 335,		//对方拒绝聊天
		result_id_refuse_play_with				= 336,		//对方拒绝一起玩
		result_id_flower_present_info_0			= 337,		//赠送鲜花的广播信息
		result_id_flower_present_info_1			= 338,		//赠送鲜花的广播信息
		result_id_flower_present_info_2			= 339,		//赠送鲜花的广播信息
		result_id_flower_present_info_3			= 340,		//赠送鲜花的广播信息
		result_id_flower_present_info_4			= 341,		//赠送鲜花的广播信息
		result_id_anonymous_flower_present_info_0	= 342,		//匿名赠送鲜花的广播信息
		result_id_anonymous_flower_present_info_1	= 343,		//匿名赠送鲜花的广播信息
		result_id_anonymous_flower_present_info_2	= 344,		//匿名赠送鲜花的广播信息
		result_id_anonymous_flower_present_info_3	= 345,		//匿名赠送鲜花的广播信息
		result_id_anonymous_flower_present_info_4	= 346,		//匿名赠送鲜花的广播信息
		result_id_ff_message_box				= 347,		//翻番的提示框
		//范围[300-400)的result id为client and hall的


		result_id_has_no_compete_game_data		= 400,		//没有比赛积分或资格
		result_id_compete_already_started		= 401,		//比赛已经开始，别重复开游戏窗口（一个比赛只允许开一个游戏窗口）
		result_id_has_no_such_compete_id		= 402,		//没有这个比赛

		//Server端的返回结果ID
		result_id_system						= 501,		//系统错误！
		result_id_toomany_openroom				= 502,		//进入的房间太多！
		result_id_invalid_uin					= 503,		//无效的用户UIN！
		//游戏币加锁&解锁
		result_id_already_lock					= 504,      //游戏币已经被锁！
		result_id_lock_lack_money				= 505,		//游戏币不足！
		result_id_lock_permission_deny			= 506,		//拒绝锁定，游戏帐户处于保护状态！
		result_id_lock_failed					= 507,		//游戏币加锁失败！
		result_id_unlock_failed					= 508,		//游戏币解锁失败！
		result_id_lock_timeout					= 509,		//进房失败:锁游戏币超时！

		result_id_consume_many_money			= 510,		//您的总消费额已经超过当天的上限！
		result_id_getgamedata_timeout			= 511,		//获取游戏数据超时！
		result_id_updategamedata_timeout		= 512,		//更新游戏数据超时！
		result_id_updateitemdb_timeout			= 513,		//更新道具超时！
		result_id_updategamedb_timeout			= 514,		//更新游戏数据超时！
		result_id_changemoney_useitem_timeout	= 515,		//扣费超时,无法使用，请重试！
		result_id_freeze_account				= 516,		//帐号已被冻结！
		result_id_close_service_money_change	= 517,		//更新游戏币失败,因已关闭该业务的金币更新！
		result_id_not_exist_service				= 518,		//不存在该有效的服务！
		result_id_lockhappybean_timeout			= 519,		//进房失败:锁欢乐豆超时!请重试

		result_id_more_than_present_count       = 520,      //超过赠送(欢乐豆)的次数
		result_id_cost_happybean_round_fare		= 521,		//每局收取欢乐豆台费
		result_id_cost_yxb_round_fare			= 522,		//每局收取游戏币台费
		result_id_lock_happybean_fail			= 523,		//加锁欢乐豆失败
		result_id_unlock_happybean_fail			= 524,		//解锁欢乐豆失败
		result_id_enter_room_required_camera	= 525,		//该房间要求必须开启摄像头才能进入,请确认您的摄像头已经开启！
		result_id_enter_room_required_videoauth = 526,		//该房间只有通过视频认证的才能进入,点击立即进行视频认证
		result_id_enter_room_qun_member_only	= 527,		//
		result_id_happybean_already_lock        = 528,
		result_id_happybean_lack				= 529,
		result_id_not_player_data               = 530,      //平台无此用户数据...
		result_id_qun_room_string				= 531,		//进入群组房间时的通知消息

		result_id_present_happybean_timeout     = 532,		//赠送欢乐豆超时
		result_id_in_process_of_present_happybean = 533,	//向gamedb请求赠送欢乐豆的过程中
		result_id_game_change_51point_timeout	= 534,		//游戏SO更新51点超时
		result_id_get_iteminfo_from_itemdb_timeout = 535,	//从item db获取道具信息超时
		result_id_game_purchase_goods_from_market_timeout = 536,	//从market购买道具信息超时


		//踢人协议中的踢人原因
		result_id_kick_player_reason_start		= 700,		//您的帐号在其他地方登录，导致您现在已经与服务器断开连接，请注意自己的帐号安全！
		result_id_kick_player_multilogin		= result_id_kick_player_reason_start,		//重复登陆
		result_id_kick_player_system_halt		= 701,		//系统停机维护,请您暂时离开平台！
		result_id_kick_player_dull				= 702,		//太长时间不准备开始游戏,系统请您暂时离开！
		result_id_kick_player_gameover			= 703,		//本局游戏结束,系统清理游戏桌,暂时请您离开！
		result_id_kick_player_by_other			= 704,		//别人请您离开！
		result_id_kick_player_observed_leave	= 705,		//您所旁观的玩家已经离开！
		result_id_kick_player_off				= 706,		//清除断线玩家！
		result_id_kick_player_not_enough_money	= 707,		//游戏币不足,系统请您暂时离开！
		result_id_kick_player_lock_money_fail	= 708,		//锁定51点失败，系统请你暂时离开!
		result_id_kick_player_not_enough_happybean = 709,	//很遗憾，由于您没有足够的欢乐豆，无法继续进行游戏，请您充值后继续游戏
		result_id_kick_player_lock_happybean_fail = 710,	//无法锁定欢乐豆
		result_id_kick_player_for_by_other		= 711,		//别人请您离开！
		result_id_kick_player_toomuch_score     = 712,		//积分过高
		result_id_kick_player_toomuch_happybean = 713,		//欢乐豆过高
		result_id_kick_player_toomuch_51dian    = 714,		//51点过高
		result_id_kick_player_unknown			= 715,		//系统请你暂时离开
		result_id_kick_player_not_enough_score  = 716,		//积分不足
		result_id_kick_player_system_clear		= 717,		//系统清理游戏桌

		//720以前的id不能被使用，作为保留
		result_id_kick_player_be_killed			= 720,		//您已被系统踢出51游戏,在未来%d小时内您将无法登录！
		result_id_kick_player_be_freezed		= 721,      //您的帐号已被冻结, 将在%s后解封！
		result_id_kick_player_old_version		= 722,		//您的版本过低,请更新版本后重新登录！
		result_id_present_happybean_first_play  = 723,		//第一次赠送欢乐豆
		result_id_present_happybean_daily		= 724,		//每日赠送欢乐豆

		//高魅力值用户进入房间通知
		result_id_high_charming_notify			= 750,		//高魅力值玩家进房通知起始ID
		//...................保留32个ID，即结束ID为782

		result_id_player_charge_money_timeout   = 800,		//充值失败: 超时
		result_id_player_exchange_money_timeout = 801,		//回兑失败: 超时
		result_id_player_search_money_timeout	= 802,		//查询游戏币失败: 超时
		result_id_player_charge_money_error		= 803,		//充值失败
		result_id_player_exchange_money_error	= 804,      //回兑失败
		result_id_player_search_money_error		= 805,      //查询失败

		result_id_player_update_card_id_timeout = 810,		//更新身份证失败: 超时
		result_id_player_get_user_info_timeout	= 811,		//得到用户信息失败: 超时
		result_id_player_update_card_id_error   = 812,		//更新身份证失败
		result_id_player_get_user_info_error	= 813,		//得到用户信息失败
		result_id_player_get_card_id_timeout	= 814,		//获得用户身份证超时
		result_id_player_gm_punish_timeout				= 820,  //更新封账号失败: 超时
		result_id_player_relay_system_message_timeout	= 821,  //中转系统信息失败: 超时
		result_id_player_submit_system_message_timeout	= 822,	//提交系统消息失败：超时
		result_id_player_cancel_system_message_timeout	= 823,	//取消息系统失败：超时

		result_id_player_gm_punish_error				= 825,  //更新封账号失败
		result_id_player_relay_system_message_error		= 826,  //中转系统信息失败
		result_id_player_submit_system_message_error	= 827,	//提交系统消息失败
		result_id_player_cancel_system_message_error	= 828,	//取消息系统失败

		result_id_player_gm_get_detail_info_error		= 829,  //获得用户详细信息失败
		result_id_player_gm_get_detail_info_timeout		= 830,  //获得用户详细信息失败: 超时

		result_id_gm_query_platform_lock_money_error	= 831,  //查询平台游戏币状态出错
		result_id_gm_query_platform_lock_money_timeout  = 832,  //查询平台游戏币状态出错: 超时

		result_id_gm_update_platform_lock_money_error	= 833,  //更新平台游戏币状态出错
		result_id_gm_update_platform_lock_money_timeout = 834,  //更新平台游戏币状态出错: 超时

		result_id_change_money_error					= 835,	//修改游戏币出错
		result_id_change_money_money_timeout			= 836,	//修改游戏币超时

		result_id_player_unlock_money_error				= 837,	//修改玩家游戏币状态出错
		result_id_player_unlock_money_timeout			= 838,	//修改玩家游戏币状态超时

		result_id_gm_update_game_prop_error				= 839,
		result_id_gm_update_game_point_error			= 840,
		result_id_gm_update_witchery_value_error		= 841,
		result_id_gm_update_achievement_value_error		= 842,

		result_id_gm_update_game_prop_timeout			= 843,
		result_id_gm_update_game_point_timeout			= 844,
		result_id_gm_update_witchery_value_timeout		= 845,
		result_id_gm_update_achievement_value_timeout	= 846,

		result_id_gm_delete_item_error					= 847,
		result_id_gm_clean_expired_item_error			= 848,

		result_id_gm_delete_item_timeout				= 849, 
		result_id_gm_clean_expired_item_timeout			= 850,

		result_id_gm_change_happy_bean_error		    = 851, //修改玩家欢乐豆出错
		result_id_gm_change_happy_bean_timeout			= 852, //修改玩家欢乐豆出错

		result_id_gm_lock_happy_bean_error		        = 853, //解锁&加锁玩家欢乐豆出错
		result_id_gm_lock_happy_bean_timeout			= 854, //解锁&加锁玩家欢乐豆超时

		result_id_gm_get_player_all_game_data_error     = 855, //解锁&加锁玩家欢乐豆出错
		result_id_gm_get_player_all_game_data_timeout	= 856, //解锁&加锁玩家欢乐豆超时

		result_id_gm_get_player_item_data_error         = 857, //获取玩家道具出错
		result_id_gm_get_player_item_data_timeout	    = 858, //获取玩家道具超时

		result_id_gm_get_player_state_error             = 859, //获取玩家道具出错
		result_id_gm_get_player_state_timeout	        = 860, //获取玩家道具超时

		result_id_gm_get_all_player_state_error         = 861, //获取玩家道具出错
		result_id_gm_get_all_player_state_timeout	    = 862, //获取玩家道具超时

		result_id_player_cancel_message_error	        = 863, //删除息系统失败：出错
		result_id_player_cancel_message_timeout	        = 864, //删除息系统失败：超时

		result_id_gm_send_system_message_to_player_error	        = 865, //发送系统消息失败：出错
		result_id_gm_send_system_message_to_player_timeout	        = 866, //发送系统消息失败：超时

		result_id_gm_register_game_service_error	        = 867, //注册游戏服务失败：出错
		result_id_gm_register_game_service_timeout	        = 868, //注册游戏服务失败：超时

		result_id_gm_update_vip_data_error              = 869, //gm修改用户vip信息失败：出错
		result_id_gm_update_vip_data_timeout            = 870, //gm修改用户vip信息失败：超时

		//大家别抢哦！
		/*************范围[900-1000]的result id为client和Logicser之间通讯使用 Start*************/
		result_id_enter_charm_room_with_charm_passport  = 900,	//在魅力房间使用魅力通行证
		result_id_enter_charm_room_fail  = 901,					//进入魅力房间失败
		result_id_kick_player_low_vip_level = 902,				//对方也是VIP用户，您的VIP等级太低

		result_id_enter_compete_room_not_competitor		= 903,		//进入比赛房间失败：不是参赛选手！
		result_id_enter_compete_room_not_right_time		= 904,		//进入比赛房间失败：不是开通时间

		result_id_use_zero_point_success_by_update_itemdb	= 905,	//通过更新道具，使用负分清零成功
		result_id_use_zero_point_success_by_change_money	= 906,	//通过扣除51点，使用负分清零成功

		result_id_use_small_speaker_success_by_update_itemdb = 907,	//通过更新道具，使用小喇叭成功
		result_id_use_small_speaker_success_by_change_money = 908,	//通过扣除51点，使用小喇叭成功

		result_id_cant_kickplayer_by_object_not_on_table	= 909,	//被踢的玩家已经离开游戏 

		result_id_be_grabed_in_room_by_vip_player			= 910,	//被VIP玩家挤出房间
		result_id_vip_enter_room_is_full					= 911,	//VIP用户进人满房间失败

		//踢人协议中的踢人原因扩展段
		result_id_kick_player_reason_start_ext = 980,	
		result_id_kick_player_for_vip_player = result_id_kick_player_reason_start_ext,	//VIP玩家踢人
		result_id_kick_player_for_high_level_vip_player	= 981,		//高VIP等级玩家踢人
		result_id_kick_player_for_master_vip_player	= 982,			//同等级VIP玩家桌主踢人


		/*************范围[900-1000]的result id为client和Logicser之间通讯使用 End*************/
		//add by liujj@2009-09-22===>begin
		result_id_compete_not_in_progress			= 1001,		//比赛未开始
		result_id_getcompetedata_timeout			= 1002,		//获取游戏数据超时！
		result_id_open_so_many_compete				= 1003,		//不能同时参加太多比赛，您玩这么多比赛，您行吗?
		result_id_update_competedata_timeout		= 1004,	
		result_id_enter_mutil_rooms_in_one_compete	= 1005,		//同一个比赛，只能进入一个房间
		result_id_only_gm_can_kick_in_compete_room	= 1006,		//只有GM才能在比赛房间踢人
		result_id_not_in_white_list					= 1007,		//您不在比赛选手的白名单中，不能参加比赛
		result_id_in_black_list						= 1008,		//您在比赛选手的黑名单中，不能参加比赛
		result_id_player_and_condition_not_satisfy_one		= 1009,		//您的条件（欢乐豆&&51点&&蓝钻登记）有一种，没有达到比赛要求
		result_id_player_and_condition_not_satisfy_two		= 1010,		//您的条件（欢乐豆&&51点&&蓝钻登记）有两种，没有达到比赛要求
		result_id_player_and_condition_not_satisfy_three	= 1011,		//您的条件（欢乐豆&&51点&&蓝钻登记）有三种，没有达到比赛要求

		result_id_player_or_condition_not_satisfy_one		= 1012,		//您的条件（欢乐豆||51点||蓝钻登记）有一种，没有达到比赛要求
		result_id_player_or_condition_not_satisfy_two		= 1013,		//您的条件（欢乐豆||51点||蓝钻登记）有两种，没有达到比赛要求
		result_id_player_or_condition_not_satisfy_three		= 1014,		//您的条件（欢乐豆||51点||蓝钻登记）有三种，没有达到比赛要求

		result_id_player_not_satisfy_happybean		= 1015,		//必须拥有欢乐豆
		result_id_player_not_satisfy_51point		= 1016,		//必须拥有51点
		result_id_player_not_satisfy_vip			= 1017,		//必须成为蓝钻
		result_id_player_not_satisfy_middle_string	= 1018,		//玩家进房条件不满足对话框的中间格式控制字符串

		result_id_player_get_happybean				= 1019,		//获得欢乐豆按钮以及链接
		result_id_player_get_51point				= 1020,		//获得51点按钮以及链接
		result_id_player_become_vip					= 1021,		//成为蓝钻按钮以及链接
		result_id_player_not_satisfy_end_string		= 1022,		//玩家进房条件不满足对话框的末尾格式控制字符串

		result_id_player_canot_talk					= 1023,		//比赛房间内，禁止任何形式的聊天！
		result_id_compete_end_canot_sitdown			= 1024,		//比赛已经结束，无法坐下
		result_id_compete_room_item_not_allowed		= 1025,		//比赛已经结束，无法坐下		

		//预留1030-1049，给踢人出房协议,大家别抢哦！
		result_id_kick_player_from_room_reason_start	= 1030, //普通的踢人协议是指踢人出桌，这里增加踢人出房的协议
		result_id_kick_player_from_room_compete_clear	= 1031, //普通的踢人协议是指踢人出桌，这里增加踢人出房的协议
		reason_id_kick_player_from_room_not_enough_happybean	= 1032,//当欢乐豆不足，踢人出房
		reason_id_kick_player_from_room_not_enough_51point		= 1033,//当51点不足，踢人出房
		reason_id_kick_player_from_room_not_enough_both			= 1034,//当欢乐豆和51点不足，踢人出房
		//add by liujj@2009-09-22===>end


		result_id_close_chat							= 1040,      //关闭聊天
		result_id_mutil_login_player					= 1041,

		result_id_chat_room_full						= 1042,		//聊天室满
		result_id_invalid_chat_room_id					= 1043,		//非法聊天室
		result_id_add_chat_room_error					= 1044,		//加入聊天室失败
		max_result_id,					//最大result id. 保证最大result id不能超过2048！！
	};
}//namespace KingNet

namespace KingNet { namespace Server {
	enum
	{
		connector_head_base_size = 7,//sizeof(int32_t)+sizeof(int8_t)+sizeof(int16_t)

		max_client_pkg_size = 5120,		//客户端发送过来的包的最大大小

		max_relay_pkg_size = 4096,		//客户端给RelayServer发送过来的最大数据包大小
	};

	//CS协议数据包定义
	enum
	{
		max_cs_package_size = 0x3ffff,			/**< cs包的最大长度 256K */

		//最大协议体长度
		max_cs_package_body_size = (max_cs_package_size - MIN_CS_HEAD_LENGHT - MAX_CS_HEAD_OPTION_LENGTH), 
		max_cs_package_total_size = (max_cs_package_size + connector_head_base_size),
	};

	enum
	{
		max_cs_channel_size = 0x2000000,		//cs channel size 32MB
		max_udp_channel_size = 0x10000000,		//udp channel size 256MB
	};


	//web 用的定义
	enum
	{
		web_connector_head_base_size = 11,//sizeof(int32_t)+sizeof(int8_t)+sizeof(int16_t)

		max_web_client_pkg_size = 5120,		//客户端发送过来的包的最大大小
	};

	//web协议数据包定义
	enum
	{
		max_web_package_size = 0x3ffff,			/**< cs包的最大长度 256K */

		//最大协议体长度
		max_web_package_body_size = (max_web_package_size - MIN_CS_HEAD_LENGHT - MAX_CS_HEAD_OPTION_LENGTH), 
		max_web_package_total_size = (max_web_package_size + web_connector_head_base_size),
	};

	enum
	{
		max_web_channel_size = 0x2000000,		//cs channel size 32MB
	};

	enum RELAY_CMD
	{
		RELAY_KEEP_ALIVE = 0x1,
		RELAY_ACK = 0x2,
		RELAY_CREATE = 0x3,
		RELAY_DESTROY = 0x4,
		RELAY_SUCCESS = 0x5,
		RELAY_FAIL = 0x6,
		RELAY_BREAK = 0x7,
		RELAY_DATA = 0x8,
		RELAY_GETIPPORT = 0x9,
		RELAY_REDIRECT = 0xA,
		//....

		RELAY_LOGOUT_SELF = 0xFF,
	};


}}//namespace KingNet { namespace Server {

#endif //__GAMEPLATFORM_MESSAGE_H__

