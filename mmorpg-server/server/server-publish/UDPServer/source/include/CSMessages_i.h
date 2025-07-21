#ifndef __CS_MESSAGES_I_H__
#define __CS_MESSAGES_I_H__


#include "gameplatform_types.h"

#define MSG_TYPE_REQUEST	0x01
#define MSG_TYPE_RESPONSE	0x02
#define MSG_TYPE_NOTIFY		0x03
#define MSG_TYPE_OTHER		0x04

//////////////////////////////////////////////////////////////////////////
// CS 协议

// AuthenticateServer消息
// 特别注意，认证服务器走的是 AuthenticateBaseProtocol
#define MSG_AUTHENTICATE_REQUEST	        0x0001
#define MSG_AUTHENTICATE_RESPONSE	        0x0002
#define MSG_AUTHENTICATE_SUPPORT_NUM_REQUEST	0x0010

#define MSG_CHANGESIGNATURE_REQUEST			0x0003
#define MSG_CHANGESIGNATURE_RESPONSE		0x0004
#define MSG_RENEWSIGNATURE_REQUEST			0x0007
#define MSG_RENEWSIGNATURE_RESPONSE			0x0008

// CatalogServer消息

#define MSG_CATALOG_LOGIN					0x1001
#define MSG_CATALOG_GETCATALOGBLOCK			0x1002
#define MSG_CATALOG_GETCATALOGNODELOCALITY	0x1003


//Magic Server
#define MSG_MAGIC_HEARTBEAT					0x1101		//client和Magic之间的心跳
#define MSG_MAGIC_ADVERTISE					0x1102		//Magic Server下发广告


// LogicServer消息
#define MSG_LOGIC_LOGIN				        0x2001
#define MSG_LOGIC_LOGOUT			        0x2002
#define MSG_LOGIC_UPDATE_PROFILE			0x2003

// 房间消息
#define MSG_LOGIC_ENTERROOM					0x2101
#define MSG_LOGIC_LEAVEROOM					0x2102
#define MSG_LOGIC_GETROOMSNAPSHOT			0x2103
#define MSG_LOGIC_UPDATEVISUALABLEAREA		0x2104
#define MSG_LOGIC_HEARTBEAT					0x2105
#define MSG_LOGIC_ROOMEVENT					0x2106
#define MSG_LOGIC_NOTIFY_TABLEINFO			0x2107		//通知游戏桌玩家信息
#define MSG_LOGIC_PLAYER_GAMEDATA_SNAPSHOT  0x2108		//s --> c 同步房间内玩家的游戏数据信息  response
#define MSG_LOGIC_PLAYER_OTHER_INFO			0x2109		//s --> c 同步房间内玩家的地区,头像信息等 response
#define MSG_LOGIC_PLAYER_QUN_INFO			0x210A		//s --> c 同步房间内玩家的群组信息 response


// 游戏消息
#define MSG_SITDOWN						    0x2201
#define MSG_STANDUP						    0x2202
#define MSG_OBSERVE						    0x2203
#define MSG_CANCELOBSERVE				    0x2204
#define MSG_PLAYGAME					    0x2205		// 双向,GameData
#define MSG_STARTGAME					    0x2206		// C->S 玩家点"准备"按钮
#define MSG_PUSHGAMEDATA				    0x2207		// S->C 服务器向客户端推送游戏数据
#define MSG_LOCKTABLE					    0x2208		// 双向 锁桌
#define MSG_REPLAY						    0x2209		//断线重玩
#define MSG_NOTIFY_BEKICKED					0x220A		//notify 消息，通知玩家被踢 s --> c 
#define MSG_HEALTH_STATUS					0x220B		//C-->S client向logic server报告玩家的健康状态
#define MSG_TALK_ON_TABLE					0x220C		//C<-->S 游戏桌上聊天
#define MSG_NOTIFY_MESSAGE_ONTABLE			0x220D		//S-->C 通知游戏桌上信息
#define MSG_ZERO_NEGATIVE_POINT				0x220F		//C <-->S 使用负分清零道具
#define MSG_ADMINISTRATE_OPERATE			0x2210		//C <-->S 网管协议
#define MSG_KICK_PLAYER						0x2211		//C <-->S VIP/网管踢人离桌
#define MSG_TALK_IN_ROOM					0x2212		//c <--> s房间内聊天
#define MSG_NOTIFY_ROOM_MESSAGE				0x2213		//s-->c notify
#define MSG_NOTIFY_BEKICKED_FROM_ROOM		0x2214		//s-->c notify

#define MSG_ENTER_TABLE						0x2215		//c->s 进入游戏桌 added by aprilliu, 2010-01-06 for painting game
#define MSG_LEAVE_TABLE						0x2216		//c->s 离开游戏桌 added by aprilliu, 2010-01-06 for painting game

#define MSG_NOTIFY_CALLPLAYER			    0x2300		//通知类消息
#define MSG_REQUEST_SMALL_SPEAKER		    0x2301		//请求使用小喇叭
#define MSG_NOTIFY_SMALL_SPEAKER		    0x2302		//小喇叭通知消息

//反外挂
#define MSG_NOTIFY_ANTIBOT_DATA				0x2310		//透传反外挂数据
#define MSG_GET_ANTIBOT_DATA				0x2311		//获取反外挂相关的数据

//
#define MSG_NOTIFY_SYSTEM_MESSAGE			0x2400		//通知client系统消息(logic server&hall server都可以发送该消息)

// P2P消息
#define MSG_DIRECTDATA						0x3000		//p2p打通请求
#define MSG_NOTIFY_DIRECTDATA				0x3001		//p2p打通转发


//web server client VS Market Server
#define MSG_REQUEST_BUY_GOODS			    0x3100		//web道具商城与Market Server之间的购买/赠送协议 .双向


// ProfileServer
#define MSG_PROFILE_FETCHALL			    0x3700		//                  (delete)
#define MSG_PROFILE_FETCHALL_OTHER		    0x3701		//拉取别人的信息... (delete)
#define MSG_PROFILE_PLAYER_GAMEDATA			0x3702 		//拉取玩家的游戏列表. 
#define MSG_PROFILE_GET_PLAYER_QUN 			0x3703		//拉取玩家的51群组信息
#define MSG_PROFILE_WEB_BASE_INFO           0x3704      //得到个人网站基本资料
#define MSG_PROFILE_WEB_SCORE               0x3705      //得到个人网站等级
#define MSG_PROFILE_GAME_BASE_INFO          0x3706      //得到个人游戏基本资料
#define MSG_PROFILE_ITEM_ALL_ITEM_DATA      0x3707      //用户的道具信息及魅力物品
#define MSG_PROFILE_GET_EXT_ACCOUNT    0x3708  //拉取用户的外部帐号


//Client VS HallServer
#define MSG_HALL_LOGIN					    0x3900		//client登陆大厅
#define MSG_HALL_LOGOUT				        0x3901		//client登出大厅
#define MSG_HALL_HEARTBEAT					0x3902		//client和hall之间的心跳
#define MSG_HALL_GET_POSITIVE_FRIEND_LIST   0x3903      //获取自己的正向玩伴列表，hall同时会拉取玩伴状态
#define MSG_HALL_GET_STATISTICAL_TAG        0x3904      //获取某人的统计标签，目前最多10项
#define MSG_HALL_ADD_TAG_TO_FRIEND          0x3905      //给人贴标签，并自动添加为玩伴
#define MSG_HALL_DELETE_FRIEND              0x3906      //删除玩伴
#define MSG_HALL_NOTIFY_FRIEND_INFO		    0x3907      //通知玩家的玩伴最新信息
#define MSG_HALL_GET_RECOMMENDED_TAG        0x3908      //获取某个玩家的推荐标签，目前最多10项，6个统计标签+4个系统推荐标签
#define MSG_HALL_NOTIFY_GET_CHEST			0x3909		//通知获得宝箱
#define MSG_HALL_NOTIFY_KICK_PLAYER 		0x390A		//通知把客户端踢下线
#define MSG_HALL_NOTIFY_FRIEND_STATUS		0x390B		//通知client的某个好友状态
#define MSG_HALL_REQEUST_TALK_IN_PRIVATE	0x390C		//私聊消息转发
#define MSG_HALL_NOTIFY_PRIVATE_TALK		0x390D		//通知私聊消息到达
#define MSG_HALL_GET_PLAYER_STATE			0x390E		//client查询玩家状态
#define MSG_HALL_FETCH_FRIEND_LIST_ONLY		0x390F		//只获取自己的正向玩伴列表
#define MSG_HALL_NOTIFY_TIPS_MESSAGE		0x3910		//通知Tips消息
#define MSG_HALL_NOTIFY_SYSTEM_MESSAGE		0x3911		//通知系统消息
#define MSG_HALL_NOTIFY_HEALTHY_STATE		0x3912		//通知玩家游戏健康状态
#define MSG_HALL_UPDATE_PLAYER_PROFILE      0x3913      //客户端通知hall更新自己的个人资料，目前只有性别属性 client -> hall notify
#define MSG_HALL_REPORT_GAME_STATE			0x3914		//客户端向hall上报当前游戏状态 client -> hall notify
#define MSG_HALL_NOTIFY_CLEAR_RESOURCE		0x3915		//hall server内部清理资源
#define MSG_HALL_REQUEST_CHARM_GOODS		0x3916		//请求赠送魅力物品
#define MSG_HALL_REQUEST_USE_BOMB			0x3917		//请求使用炸弹
#define MSG_HALL_REQUEST_USE_BIG_SPEAKER	0x3918		//请求使用大喇叭
#define MSG_HALL_NOTIFY_HAVE_CHARM_GOODS	0x3919		//通知有人赠送魅力物品
#define MSG_HALL_NOTIFY_BOMB_BLAST			0x391A		//通知client被扔炸弹
#define MSG_HALL_NOTIFY_HAVE_BIG_SPEAKER	0x391B		//通知有大喇叭消息
#define MSG_HALL_NOTIFY_ITEM_INFO_UPDATE	0x391C		//通知有道具更新消息
#define MSG_HALL_NOTIFY_HAVE_COMMON_GOODS	0x391D		//通知获得物品（被赠送物品）
#define MSG_HALL_REQUEST_PLAY_WITH			0x391E		//邀请一起玩请求  client <-->hall
#define MSG_HALL_REQUEST_REPLY_PLAY			0x391F		//被邀请玩家回复通知邀请者  client -> hall
#define MSG_HALL_NOTIFY_PLAY_WITH			0x3920		//邀请一起玩请求转发 hall -->client notify
#define MSG_HALL_NOTIFY_REPLY_PLAY			0x3921		//被邀请玩家回复通知邀请者转发 hall -->client notify
#define MSG_HALL_NOTIFY_UPDATE_PLAYER_DATA  0x3922      //通知玩家属性改变
#define MSG_HALL_REQUEST_TRANSFER_CLIENT_COMMAND 0x3923	//向Hall请求转发命令数据到其它玩家 client->hall
#define MSG_HALL_NOTIFY_TRANSFER_CLIENT_COMMAND 0x3924	//Hall向玩家转发其他玩家命令数据   hall->client notify
#define MSG_HALL_EXCHANGE_CHARM_ITEM		0x3925		//兑换魅力物品
#define MSG_HALL_ADD_FRIEND_GROUP			0x3926		//玩伴列表中添加自定义分组
#define MSG_HALL_DEL_FRIEND_GROUP			0x3927		//删除玩伴列表中的自定义分组
#define MSG_HALL_MOVE_FRIEND_TO_GROUP		0x3928		//把玩伴移到指定的分组中
#define MSG_HALL_RENAME_FRIEND_GROUP		0x3929		//自定义玩伴分组重命名
#define MSG_HALL_GET_FRIEND_GROUPS			0x392A		//拉去玩家的玩伴分组信息
#define MSG_HALL_NOTIFY_REFRESH_SERVICE		0x392B		//通知玩家更新服务信息(如双倍积分服务、护身符服务等)
#define MSG_HALL_RENAME_FRIEND_REMARKS		0x392C		//修改玩伴的备注
#define MSG_HALL_GET_PLAYED_WITH_INFO		0x392D		//拉去和谁一起玩过的信息
#define MSG_HALL_ADD_PLAYED_WITH_INFO		0x392E		//添加和谁一起玩过的信息
#define MSG_HALL_NOTIFY_DEL_PLAYED_WITH_INFO 0x392F		//大厅通知客户端删除某个和谁一起玩过的信息
#define MSG_HALL_ADD_FRIEND					0x3930		//加玩伴（贴标签+分组+备注）
#define MSG_HALL_USE_GAME_SPEAKER			0x3931		//使用游戏内喇叭，使用游戏ID区分发往哪一款游戏
#define MSG_HALL_NOTIFY_GAME_SPEAKER		0x3932		//通知游戏内喇叭消息
#define MSG_HALL_NOTIFY_FETCH_51_FRIEND		0x3933		//通知客户端去网站那边拉去好友
#define MSG_HALL_IMPORT_51_FRIEND			0x3934		//导入51好友
#define MSG_HALL_NOTIFY_BE_ADDED_FRIEND		0x3935		//通知被加玩伴
#define MSG_HALL_NOTIFY_FORBIDDEN_CHAT		0x3936		//通知客户端被禁言
#define MSG_HALL_NOTIFY_ACIONT_MSG_BOX		0x3937		//通知活动信息
#define MSG_HALL_NOTIFY_CANCEL_SYS_MSG		0x3938		//通知客户端取消某个系统消息
#define MSG_HALL_NOTIFY_ITEM_INFO			0x3939		//通知更新某个用户的道具信息
#define MSG_HALL_MAKE_FRIEND_WITH_OUTHER	0x3940		//请求对方同意加为好友
#define MSG_HALL_DEAL_WITH_FRIEND_REQUEST	0x3941		//对方是否同意你的加好友请求
#define MSG_HALL_GET_BLACK_LIST_INFO		0x3942		//获取自己的黑名单
#define MSG_HALL_ADD_BLACK_LIST_INFO		0x3943		//把某个玩家添加到自己的黑名单中
#define MSG_HALL_DEL_BLACK_LIST_INFO		0x3944		//把某个玩家从自己的黑名单中移除
#define MSG_HALL_UPDATE_GLOBAL_SETTINGS		0x3945		//更新全局设置
#define MSG_HALL_NOTIFY_GLOBAL_SETTINGS		0x3946		//通知全局设置
#define MSG_HALL_NOTIFY_VIP_INFO			0x3947		//通知VIP信息


//哎呀！！0x4以后就是服务器协议了，以后不能这么写啦！！！！！

//Flash vs ChatServer
#define CS_MSG_LOGIN_CHAT_ROOM			0x0001		//登录聊天室
#define CS_MSG_START_CHAT				0x0002		//发起聊天
#define CS_MSG_NOTIFY_START_CHAT		0x0003		//通知开始聊天
#define CS_MSG_TRANSPORT_CHAT_INFO		0x0004		//中转聊天记录
#define CS_MSG_PUBLISH_PRIVATE_INFO		0x0005		//公开个人信息
#define CS_MSG_CHAT_HEART				0x0006		//发送心跳包
#define CS_MSG_STOP_CHAT				0x0007		//退出聊天,TCP不断开
#define CS_MSG_LOGOUT_CHAT_ROOM			0x0008		//退出聊天室，断开TCP
#define CS_MSG_KICK_PLAYER				0x0009		//踢人
#define CS_MSG_NOTIFY_CLOSE_CHAT		0x000A		//通知聊天结束
#define CS_MSG_ADMIN_GET_CHAT_GROUPS	0x000B      //管理员拉取聊天组
#define CS_MSG_ADMIN_ADD_CHAT_GROUP		0x000C		//管理员加入聊天组
//php vs LogServer
#define CS_MSG_NOTIFY_WRITE_LOG         0x0010		//写日志
//GroupChatServer
#define CS_MSG_GET_CHAT_ROOM_ONLINE_COUNT 0x0020      //获取在线人数
#define CS_MSG_NOTIFY_LOGIN_LOGOUT_ROOM   0x0021		//通知某人进聊天室或者退出聊天室
//0x4000以后就是服务器协议了，以后不能这么写啦！！！！！



enum
{
	MSG_USERFACE_LOGIN				=	0x5001,		//登陆头像下载服务器
	MSG_USERFACE_HEARTBEAT			=	0x5002,		//头像服务器心跳包
	MSG_USERFACE_FETECH_USER_FACE	=	0x5003,		//客户端拉取头像
	MSG_USERFACE_NOTIFY_USER_FACE	=	0x5004,		//服务器下发头像
	MSG_USERFACE_LOGOUT				=	0x5005,		//头像服务器logout

};


#define  MAX_CS_HEAD_OPTION_LENGTH 128									//CSHead中option的最大长度
#define  MIN_CS_HEAD_LENGHT	(7*sizeof(int16_t) + 3*sizeof(int32_t))		//cs head(不包括option所占用的buff长度，也不包括sequence)
#define MAX_ANTIBOT_KEYFILE_LENGTH	0x1000		// KEY文件最大长度
#define MAX_ANTIBOT_RECORDFILE_LENGTH 0xA000	// 病毒特征库文件最大长度

#include "gameplatform_public.h"

namespace KingNet
{
	enum
	{
		MIN_CSPackage_Header_Length = (7*sizeof(int16_t) + 3*sizeof(int32_t))		//cs head(不包括option所占用的buff长度，也不包括sequence)
	};


	//加密算法
	enum enmCryptAlgorithm
	{
		enmCryptAlgorithm_None = 0x0, /**< 无加密*/
		enmCryptAlgorithm_XTEA = 0x4,
		enmCryptAlgorithm_XTEA_ROUND8 = 0x8,
		//...

	};

	//压缩算法
	enum enmCompressAlgorithm
	{	
		enmCompressAlgorithm_None = 0x0,	/**< 无压缩*/	 
		enmCompressAlgorithm_7Zip = 0x1,    /**< 7zip*/
	};

	enum enmCSPackageFlag //
	{
		enmCSPackageFlag_Signature	    = 0x0010,			/**< 表示有签名*/
		enmCSPackageFlag_Crypt_Mask		= 0x000C,		    /**< 加密算法的Mask*/
		enmCSPackageFlag_Compress_Mask  = 0x0003			/**< 压缩算法的mask*/		
	};


	enum enmCSConstDefine
	{
		max_table_key_length = 32,			/**< 游戏桌密钥长度 */

		max_string_message_length = 4096,   /**< 字符串*/
		max_string_key_length	 = 1024	,   /* agent 字符串 key 长度*/
		max_gameevent_count = 4,			/**< 一次最多发送的游戏事件(数据) */
		max_gamedate_length = 4096,			/**< 游戏数据长度*/

		max_p2pdata_length = 4096,           /**< p2p数据长度 (direct data size )*/ 

		max_push_room_event_count = 90,		/**< 一次最多push的房间事件数目*/ 
		max_smallspeaker_message_length = 1024,		/*最大的小喇叭消息数据长度*/

		max_room_path_length = 256,			/*最大房间路径长度*/

		//max_player_name_length = 32,			/**< 最大用户名字长度*/
		//max_player_id_length = 32,				/**<  用户字符串型帐号的长度*/

		//max_face_version_length = 32,		/* 用户头像版本号长度*/
		//max_face_url_length  = 64,			/* 用户头像的URL长度*/
		//max_user_zone_info_length = 40,		/* 用户地区信息长度*/
		//max_user_nick_name_length = 32		/* 用户昵称的长度*/
	};



	//MSG_STANDUP request
	enum enmEscapeMode
	{
		escape_mode_normal = 0,		/**< 普通退出*/
		escape_mode_brutal = 1     /**< 强制退出*/
	};

	//
	enum enmAdministrateOperate
	{
		enmAdministrateOperate_none		= 0,
		enmAdministrateOperate_dismiss_game = 1,
		enmAdministrateOperate_silence_user = 2,
		enmAdministrateOperate_freeze_user	= 3
	};

	//hall的友好提示类型
	enum enmFriendTipType
	{
		enmSendIntegral		= 0,	//送积分
		enmAntiIndulgence	= 1,	//反沉迷
		enmPresentBean		= 2,	//送欢乐豆
	};


	//51群组基本常量定义
	enum enm51QunDefine
	{
		//群组角色
		qun_role_ordinary_user	= 0,	//普通用户
		qun_role_webmaster_begin= 1,	//版主开始
		qun_role_webmaster_end	= 98,	//版主结束
		qun_role_vice_manager	= 99,	//副管
		qun_role_manager		= 100,	//主管

		//群组基本常量
		max_qun_count			= 32,	//群组最多32个
		max_qun_uid_length		= 32,	//群组账号是最大长度为32个小写的字符串
		max_qun_name_length		= 32,	//群组名字长度
	};

	//客户端状态的掩码
	enum enmClientStateMask
	{
		client_state_hall_mask		= 0x00000001,	//大厅是否在线
		client_state_camera_mask	= 0x00000002,	//摄像头是否工作
	};

	enum enmAntiFlag
	{
		antibot_room	= 0x00000001,		// 反外挂房间
	};



	enum enmClientGlobalSettings
	{
		//global setting type
		global_setting_private_talk		= 1,	//私聊限制的全局设置
		global_setting_play_with		= 2,	//游戏邀请的全局设置
		global_setting_make_friends		= 3,	//身份验证的全局设置
		global_setting_video_invitation	= 4,	//视频邀请的全局设置

		//global setting value
		private_talk_setting_default_value		= 0,	//私聊限制未知值（客户端未设置）
		not_to_recv_any_private_talk			= 1,	//不接收任何私聊消息
		not_to_recv_stranger_private_talk		= 2,	//不接收陌生人的私聊消息
		recv_any_private_talk_from_other		= 3,	//接收所有人的私聊消息

		play_with_setting_default_value			= 0,	//游戏邀请未知值（客户端未设置）
		not_to_recv_any_play_with				= 1,	//不接收任何游戏邀请
		not_to_recv_stranger_play_with			= 2,	//不接收陌生人的游戏邀请
		recv_any_play_with_from_other			= 3,	//接收所有人的游戏邀请

		make_friends_setting_default_value		= 0,	//加玩伴限制未知值（客户端未设置）
		need_my_agree_to_make_friends			= 1,	//需要我同意才能加我为玩伴
		auto_refuse_to_make_friends				= 2,	//自动拒绝所有人把我加为玩伴
		auto_agree_with_make_friends			= 3,	//不需要同意就加玩伴

		video_invitation_setting_default_value	= 0,	//视频邀请未知值（客户端未设置）
		not_to_recv_any_video_invitation		= 1,	//不接收任何视频邀请
		not_to_recv_stranger_video_invitation	= 2,	//不接收陌生人的视频邀请
		recv_any_video_invitation_from_other	= 3,	//接收所有人的视频邀请
	};

	//大厅客户端透传数据的CommandID
	enum enmLobbyDirectionSettings
	{		
		UnknownData_CommandID_P2P		= 425,
		DIRECT_DATA_SEND_SMILE			= 0x0100,	//笑一个
		DIRECT_DATA_SUPERCILIOUS_LOOK	= 0x0200,	//白眼
		DIRECT_DATA_INVITE_TO_GAME		= 0x0010,	//邀请游戏
		DIRECT_DATA_ACCEPT_INVITE_TO_GAME  = 0x0020,//接受游戏邀请		
		DIRECT_DATA_REFUSE_INVITE_TO_GAME  = 0x0040,//拒绝游戏邀请
		DIRECT_DATA_INVITE_TO_VIDEOCHAT = 0x0001,	//邀请视频
		DIRECT_DATA_REPLY_VIDEOCHAT     = 0x0002,	//回复视频邀请(接受或拒绝)
		DIRECT_DATA_STOP_VIDEOCHAT		= 0x0003,	//中止视频
	};



	enum enmClientType
	{
		client_type_not_client  = 0,    //
		client_type_flash_client = 1,   //flash 客户端
		client_type_iphone_client = 2,  //iphone 客户端
	};

}

#endif

