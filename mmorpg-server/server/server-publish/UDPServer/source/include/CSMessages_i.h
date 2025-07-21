#ifndef __CS_MESSAGES_I_H__
#define __CS_MESSAGES_I_H__


#include "gameplatform_types.h"

#define MSG_TYPE_REQUEST	0x01
#define MSG_TYPE_RESPONSE	0x02
#define MSG_TYPE_NOTIFY		0x03
#define MSG_TYPE_OTHER		0x04

//////////////////////////////////////////////////////////////////////////
// CS Э��

// AuthenticateServer��Ϣ
// �ر�ע�⣬��֤�������ߵ��� AuthenticateBaseProtocol
#define MSG_AUTHENTICATE_REQUEST	        0x0001
#define MSG_AUTHENTICATE_RESPONSE	        0x0002
#define MSG_AUTHENTICATE_SUPPORT_NUM_REQUEST	0x0010

#define MSG_CHANGESIGNATURE_REQUEST			0x0003
#define MSG_CHANGESIGNATURE_RESPONSE		0x0004
#define MSG_RENEWSIGNATURE_REQUEST			0x0007
#define MSG_RENEWSIGNATURE_RESPONSE			0x0008

// CatalogServer��Ϣ

#define MSG_CATALOG_LOGIN					0x1001
#define MSG_CATALOG_GETCATALOGBLOCK			0x1002
#define MSG_CATALOG_GETCATALOGNODELOCALITY	0x1003


//Magic Server
#define MSG_MAGIC_HEARTBEAT					0x1101		//client��Magic֮�������
#define MSG_MAGIC_ADVERTISE					0x1102		//Magic Server�·����


// LogicServer��Ϣ
#define MSG_LOGIC_LOGIN				        0x2001
#define MSG_LOGIC_LOGOUT			        0x2002
#define MSG_LOGIC_UPDATE_PROFILE			0x2003

// ������Ϣ
#define MSG_LOGIC_ENTERROOM					0x2101
#define MSG_LOGIC_LEAVEROOM					0x2102
#define MSG_LOGIC_GETROOMSNAPSHOT			0x2103
#define MSG_LOGIC_UPDATEVISUALABLEAREA		0x2104
#define MSG_LOGIC_HEARTBEAT					0x2105
#define MSG_LOGIC_ROOMEVENT					0x2106
#define MSG_LOGIC_NOTIFY_TABLEINFO			0x2107		//֪ͨ��Ϸ�������Ϣ
#define MSG_LOGIC_PLAYER_GAMEDATA_SNAPSHOT  0x2108		//s --> c ͬ����������ҵ���Ϸ������Ϣ  response
#define MSG_LOGIC_PLAYER_OTHER_INFO			0x2109		//s --> c ͬ����������ҵĵ���,ͷ����Ϣ�� response
#define MSG_LOGIC_PLAYER_QUN_INFO			0x210A		//s --> c ͬ����������ҵ�Ⱥ����Ϣ response


// ��Ϸ��Ϣ
#define MSG_SITDOWN						    0x2201
#define MSG_STANDUP						    0x2202
#define MSG_OBSERVE						    0x2203
#define MSG_CANCELOBSERVE				    0x2204
#define MSG_PLAYGAME					    0x2205		// ˫��,GameData
#define MSG_STARTGAME					    0x2206		// C->S ��ҵ�"׼��"��ť
#define MSG_PUSHGAMEDATA				    0x2207		// S->C ��������ͻ���������Ϸ����
#define MSG_LOCKTABLE					    0x2208		// ˫�� ����
#define MSG_REPLAY						    0x2209		//��������
#define MSG_NOTIFY_BEKICKED					0x220A		//notify ��Ϣ��֪ͨ��ұ��� s --> c 
#define MSG_HEALTH_STATUS					0x220B		//C-->S client��logic server������ҵĽ���״̬
#define MSG_TALK_ON_TABLE					0x220C		//C<-->S ��Ϸ��������
#define MSG_NOTIFY_MESSAGE_ONTABLE			0x220D		//S-->C ֪ͨ��Ϸ������Ϣ
#define MSG_ZERO_NEGATIVE_POINT				0x220F		//C <-->S ʹ�ø����������
#define MSG_ADMINISTRATE_OPERATE			0x2210		//C <-->S ����Э��
#define MSG_KICK_PLAYER						0x2211		//C <-->S VIP/������������
#define MSG_TALK_IN_ROOM					0x2212		//c <--> s����������
#define MSG_NOTIFY_ROOM_MESSAGE				0x2213		//s-->c notify
#define MSG_NOTIFY_BEKICKED_FROM_ROOM		0x2214		//s-->c notify

#define MSG_ENTER_TABLE						0x2215		//c->s ������Ϸ�� added by aprilliu, 2010-01-06 for painting game
#define MSG_LEAVE_TABLE						0x2216		//c->s �뿪��Ϸ�� added by aprilliu, 2010-01-06 for painting game

#define MSG_NOTIFY_CALLPLAYER			    0x2300		//֪ͨ����Ϣ
#define MSG_REQUEST_SMALL_SPEAKER		    0x2301		//����ʹ��С����
#define MSG_NOTIFY_SMALL_SPEAKER		    0x2302		//С����֪ͨ��Ϣ

//�����
#define MSG_NOTIFY_ANTIBOT_DATA				0x2310		//͸�����������
#define MSG_GET_ANTIBOT_DATA				0x2311		//��ȡ�������ص�����

//
#define MSG_NOTIFY_SYSTEM_MESSAGE			0x2400		//֪ͨclientϵͳ��Ϣ(logic server&hall server�����Է��͸���Ϣ)

// P2P��Ϣ
#define MSG_DIRECTDATA						0x3000		//p2p��ͨ����
#define MSG_NOTIFY_DIRECTDATA				0x3001		//p2p��ͨת��


//web server client VS Market Server
#define MSG_REQUEST_BUY_GOODS			    0x3100		//web�����̳���Market Server֮��Ĺ���/����Э�� .˫��


// ProfileServer
#define MSG_PROFILE_FETCHALL			    0x3700		//                  (delete)
#define MSG_PROFILE_FETCHALL_OTHER		    0x3701		//��ȡ���˵���Ϣ... (delete)
#define MSG_PROFILE_PLAYER_GAMEDATA			0x3702 		//��ȡ��ҵ���Ϸ�б�. 
#define MSG_PROFILE_GET_PLAYER_QUN 			0x3703		//��ȡ��ҵ�51Ⱥ����Ϣ
#define MSG_PROFILE_WEB_BASE_INFO           0x3704      //�õ�������վ��������
#define MSG_PROFILE_WEB_SCORE               0x3705      //�õ�������վ�ȼ�
#define MSG_PROFILE_GAME_BASE_INFO          0x3706      //�õ�������Ϸ��������
#define MSG_PROFILE_ITEM_ALL_ITEM_DATA      0x3707      //�û��ĵ�����Ϣ��������Ʒ
#define MSG_PROFILE_GET_EXT_ACCOUNT    0x3708  //��ȡ�û����ⲿ�ʺ�


//Client VS HallServer
#define MSG_HALL_LOGIN					    0x3900		//client��½����
#define MSG_HALL_LOGOUT				        0x3901		//client�ǳ�����
#define MSG_HALL_HEARTBEAT					0x3902		//client��hall֮�������
#define MSG_HALL_GET_POSITIVE_FRIEND_LIST   0x3903      //��ȡ�Լ�����������б�hallͬʱ����ȡ���״̬
#define MSG_HALL_GET_STATISTICAL_TAG        0x3904      //��ȡĳ�˵�ͳ�Ʊ�ǩ��Ŀǰ���10��
#define MSG_HALL_ADD_TAG_TO_FRIEND          0x3905      //��������ǩ�����Զ����Ϊ���
#define MSG_HALL_DELETE_FRIEND              0x3906      //ɾ�����
#define MSG_HALL_NOTIFY_FRIEND_INFO		    0x3907      //֪ͨ��ҵ����������Ϣ
#define MSG_HALL_GET_RECOMMENDED_TAG        0x3908      //��ȡĳ����ҵ��Ƽ���ǩ��Ŀǰ���10�6��ͳ�Ʊ�ǩ+4��ϵͳ�Ƽ���ǩ
#define MSG_HALL_NOTIFY_GET_CHEST			0x3909		//֪ͨ��ñ���
#define MSG_HALL_NOTIFY_KICK_PLAYER 		0x390A		//֪ͨ�ѿͻ���������
#define MSG_HALL_NOTIFY_FRIEND_STATUS		0x390B		//֪ͨclient��ĳ������״̬
#define MSG_HALL_REQEUST_TALK_IN_PRIVATE	0x390C		//˽����Ϣת��
#define MSG_HALL_NOTIFY_PRIVATE_TALK		0x390D		//֪ͨ˽����Ϣ����
#define MSG_HALL_GET_PLAYER_STATE			0x390E		//client��ѯ���״̬
#define MSG_HALL_FETCH_FRIEND_LIST_ONLY		0x390F		//ֻ��ȡ�Լ�����������б�
#define MSG_HALL_NOTIFY_TIPS_MESSAGE		0x3910		//֪ͨTips��Ϣ
#define MSG_HALL_NOTIFY_SYSTEM_MESSAGE		0x3911		//֪ͨϵͳ��Ϣ
#define MSG_HALL_NOTIFY_HEALTHY_STATE		0x3912		//֪ͨ�����Ϸ����״̬
#define MSG_HALL_UPDATE_PLAYER_PROFILE      0x3913      //�ͻ���֪ͨhall�����Լ��ĸ������ϣ�Ŀǰֻ���Ա����� client -> hall notify
#define MSG_HALL_REPORT_GAME_STATE			0x3914		//�ͻ�����hall�ϱ���ǰ��Ϸ״̬ client -> hall notify
#define MSG_HALL_NOTIFY_CLEAR_RESOURCE		0x3915		//hall server�ڲ�������Դ
#define MSG_HALL_REQUEST_CHARM_GOODS		0x3916		//��������������Ʒ
#define MSG_HALL_REQUEST_USE_BOMB			0x3917		//����ʹ��ը��
#define MSG_HALL_REQUEST_USE_BIG_SPEAKER	0x3918		//����ʹ�ô�����
#define MSG_HALL_NOTIFY_HAVE_CHARM_GOODS	0x3919		//֪ͨ��������������Ʒ
#define MSG_HALL_NOTIFY_BOMB_BLAST			0x391A		//֪ͨclient����ը��
#define MSG_HALL_NOTIFY_HAVE_BIG_SPEAKER	0x391B		//֪ͨ�д�������Ϣ
#define MSG_HALL_NOTIFY_ITEM_INFO_UPDATE	0x391C		//֪ͨ�е��߸�����Ϣ
#define MSG_HALL_NOTIFY_HAVE_COMMON_GOODS	0x391D		//֪ͨ�����Ʒ����������Ʒ��
#define MSG_HALL_REQUEST_PLAY_WITH			0x391E		//����һ��������  client <-->hall
#define MSG_HALL_REQUEST_REPLY_PLAY			0x391F		//��������һظ�֪ͨ������  client -> hall
#define MSG_HALL_NOTIFY_PLAY_WITH			0x3920		//����һ��������ת�� hall -->client notify
#define MSG_HALL_NOTIFY_REPLY_PLAY			0x3921		//��������һظ�֪ͨ������ת�� hall -->client notify
#define MSG_HALL_NOTIFY_UPDATE_PLAYER_DATA  0x3922      //֪ͨ������Ըı�
#define MSG_HALL_REQUEST_TRANSFER_CLIENT_COMMAND 0x3923	//��Hall����ת���������ݵ�������� client->hall
#define MSG_HALL_NOTIFY_TRANSFER_CLIENT_COMMAND 0x3924	//Hall�����ת�����������������   hall->client notify
#define MSG_HALL_EXCHANGE_CHARM_ITEM		0x3925		//�һ�������Ʒ
#define MSG_HALL_ADD_FRIEND_GROUP			0x3926		//����б�������Զ������
#define MSG_HALL_DEL_FRIEND_GROUP			0x3927		//ɾ������б��е��Զ������
#define MSG_HALL_MOVE_FRIEND_TO_GROUP		0x3928		//������Ƶ�ָ���ķ�����
#define MSG_HALL_RENAME_FRIEND_GROUP		0x3929		//�Զ���������������
#define MSG_HALL_GET_FRIEND_GROUPS			0x392A		//��ȥ��ҵ���������Ϣ
#define MSG_HALL_NOTIFY_REFRESH_SERVICE		0x392B		//֪ͨ��Ҹ��·�����Ϣ(��˫�����ַ��񡢻���������)
#define MSG_HALL_RENAME_FRIEND_REMARKS		0x392C		//�޸����ı�ע
#define MSG_HALL_GET_PLAYED_WITH_INFO		0x392D		//��ȥ��˭һ���������Ϣ
#define MSG_HALL_ADD_PLAYED_WITH_INFO		0x392E		//��Ӻ�˭һ���������Ϣ
#define MSG_HALL_NOTIFY_DEL_PLAYED_WITH_INFO 0x392F		//����֪ͨ�ͻ���ɾ��ĳ����˭һ���������Ϣ
#define MSG_HALL_ADD_FRIEND					0x3930		//����飨����ǩ+����+��ע��
#define MSG_HALL_USE_GAME_SPEAKER			0x3931		//ʹ����Ϸ�����ȣ�ʹ����ϷID���ַ�����һ����Ϸ
#define MSG_HALL_NOTIFY_GAME_SPEAKER		0x3932		//֪ͨ��Ϸ��������Ϣ
#define MSG_HALL_NOTIFY_FETCH_51_FRIEND		0x3933		//֪ͨ�ͻ���ȥ��վ�Ǳ���ȥ����
#define MSG_HALL_IMPORT_51_FRIEND			0x3934		//����51����
#define MSG_HALL_NOTIFY_BE_ADDED_FRIEND		0x3935		//֪ͨ�������
#define MSG_HALL_NOTIFY_FORBIDDEN_CHAT		0x3936		//֪ͨ�ͻ��˱�����
#define MSG_HALL_NOTIFY_ACIONT_MSG_BOX		0x3937		//֪ͨ���Ϣ
#define MSG_HALL_NOTIFY_CANCEL_SYS_MSG		0x3938		//֪ͨ�ͻ���ȡ��ĳ��ϵͳ��Ϣ
#define MSG_HALL_NOTIFY_ITEM_INFO			0x3939		//֪ͨ����ĳ���û��ĵ�����Ϣ
#define MSG_HALL_MAKE_FRIEND_WITH_OUTHER	0x3940		//����Է�ͬ���Ϊ����
#define MSG_HALL_DEAL_WITH_FRIEND_REQUEST	0x3941		//�Է��Ƿ�ͬ����ļӺ�������
#define MSG_HALL_GET_BLACK_LIST_INFO		0x3942		//��ȡ�Լ��ĺ�����
#define MSG_HALL_ADD_BLACK_LIST_INFO		0x3943		//��ĳ�������ӵ��Լ��ĺ�������
#define MSG_HALL_DEL_BLACK_LIST_INFO		0x3944		//��ĳ����Ҵ��Լ��ĺ��������Ƴ�
#define MSG_HALL_UPDATE_GLOBAL_SETTINGS		0x3945		//����ȫ������
#define MSG_HALL_NOTIFY_GLOBAL_SETTINGS		0x3946		//֪ͨȫ������
#define MSG_HALL_NOTIFY_VIP_INFO			0x3947		//֪ͨVIP��Ϣ


//��ѽ����0x4�Ժ���Ƿ�����Э���ˣ��Ժ�����ôд������������

//Flash vs ChatServer
#define CS_MSG_LOGIN_CHAT_ROOM			0x0001		//��¼������
#define CS_MSG_START_CHAT				0x0002		//��������
#define CS_MSG_NOTIFY_START_CHAT		0x0003		//֪ͨ��ʼ����
#define CS_MSG_TRANSPORT_CHAT_INFO		0x0004		//��ת�����¼
#define CS_MSG_PUBLISH_PRIVATE_INFO		0x0005		//����������Ϣ
#define CS_MSG_CHAT_HEART				0x0006		//����������
#define CS_MSG_STOP_CHAT				0x0007		//�˳�����,TCP���Ͽ�
#define CS_MSG_LOGOUT_CHAT_ROOM			0x0008		//�˳������ң��Ͽ�TCP
#define CS_MSG_KICK_PLAYER				0x0009		//����
#define CS_MSG_NOTIFY_CLOSE_CHAT		0x000A		//֪ͨ�������
#define CS_MSG_ADMIN_GET_CHAT_GROUPS	0x000B      //����Ա��ȡ������
#define CS_MSG_ADMIN_ADD_CHAT_GROUP		0x000C		//����Ա����������
//php vs LogServer
#define CS_MSG_NOTIFY_WRITE_LOG         0x0010		//д��־
//GroupChatServer
#define CS_MSG_GET_CHAT_ROOM_ONLINE_COUNT 0x0020      //��ȡ��������
#define CS_MSG_NOTIFY_LOGIN_LOGOUT_ROOM   0x0021		//֪ͨĳ�˽������һ����˳�������
//0x4000�Ժ���Ƿ�����Э���ˣ��Ժ�����ôд������������



enum
{
	MSG_USERFACE_LOGIN				=	0x5001,		//��½ͷ�����ط�����
	MSG_USERFACE_HEARTBEAT			=	0x5002,		//ͷ�������������
	MSG_USERFACE_FETECH_USER_FACE	=	0x5003,		//�ͻ�����ȡͷ��
	MSG_USERFACE_NOTIFY_USER_FACE	=	0x5004,		//�������·�ͷ��
	MSG_USERFACE_LOGOUT				=	0x5005,		//ͷ�������logout

};


#define  MAX_CS_HEAD_OPTION_LENGTH 128									//CSHead��option����󳤶�
#define  MIN_CS_HEAD_LENGHT	(7*sizeof(int16_t) + 3*sizeof(int32_t))		//cs head(������option��ռ�õ�buff���ȣ�Ҳ������sequence)
#define MAX_ANTIBOT_KEYFILE_LENGTH	0x1000		// KEY�ļ���󳤶�
#define MAX_ANTIBOT_RECORDFILE_LENGTH 0xA000	// �����������ļ���󳤶�

#include "gameplatform_public.h"

namespace KingNet
{
	enum
	{
		MIN_CSPackage_Header_Length = (7*sizeof(int16_t) + 3*sizeof(int32_t))		//cs head(������option��ռ�õ�buff���ȣ�Ҳ������sequence)
	};


	//�����㷨
	enum enmCryptAlgorithm
	{
		enmCryptAlgorithm_None = 0x0, /**< �޼���*/
		enmCryptAlgorithm_XTEA = 0x4,
		enmCryptAlgorithm_XTEA_ROUND8 = 0x8,
		//...

	};

	//ѹ���㷨
	enum enmCompressAlgorithm
	{	
		enmCompressAlgorithm_None = 0x0,	/**< ��ѹ��*/	 
		enmCompressAlgorithm_7Zip = 0x1,    /**< 7zip*/
	};

	enum enmCSPackageFlag //
	{
		enmCSPackageFlag_Signature	    = 0x0010,			/**< ��ʾ��ǩ��*/
		enmCSPackageFlag_Crypt_Mask		= 0x000C,		    /**< �����㷨��Mask*/
		enmCSPackageFlag_Compress_Mask  = 0x0003			/**< ѹ���㷨��mask*/		
	};


	enum enmCSConstDefine
	{
		max_table_key_length = 32,			/**< ��Ϸ����Կ���� */

		max_string_message_length = 4096,   /**< �ַ���*/
		max_string_key_length	 = 1024	,   /* agent �ַ��� key ����*/
		max_gameevent_count = 4,			/**< һ����෢�͵���Ϸ�¼�(����) */
		max_gamedate_length = 4096,			/**< ��Ϸ���ݳ���*/

		max_p2pdata_length = 4096,           /**< p2p���ݳ��� (direct data size )*/ 

		max_push_room_event_count = 90,		/**< һ�����push�ķ����¼���Ŀ*/ 
		max_smallspeaker_message_length = 1024,		/*����С������Ϣ���ݳ���*/

		max_room_path_length = 256,			/*��󷿼�·������*/

		//max_player_name_length = 32,			/**< ����û����ֳ���*/
		//max_player_id_length = 32,				/**<  �û��ַ������ʺŵĳ���*/

		//max_face_version_length = 32,		/* �û�ͷ��汾�ų���*/
		//max_face_url_length  = 64,			/* �û�ͷ���URL����*/
		//max_user_zone_info_length = 40,		/* �û�������Ϣ����*/
		//max_user_nick_name_length = 32		/* �û��ǳƵĳ���*/
	};



	//MSG_STANDUP request
	enum enmEscapeMode
	{
		escape_mode_normal = 0,		/**< ��ͨ�˳�*/
		escape_mode_brutal = 1     /**< ǿ���˳�*/
	};

	//
	enum enmAdministrateOperate
	{
		enmAdministrateOperate_none		= 0,
		enmAdministrateOperate_dismiss_game = 1,
		enmAdministrateOperate_silence_user = 2,
		enmAdministrateOperate_freeze_user	= 3
	};

	//hall���Ѻ���ʾ����
	enum enmFriendTipType
	{
		enmSendIntegral		= 0,	//�ͻ���
		enmAntiIndulgence	= 1,	//������
		enmPresentBean		= 2,	//�ͻ��ֶ�
	};


	//51Ⱥ�������������
	enum enm51QunDefine
	{
		//Ⱥ���ɫ
		qun_role_ordinary_user	= 0,	//��ͨ�û�
		qun_role_webmaster_begin= 1,	//������ʼ
		qun_role_webmaster_end	= 98,	//��������
		qun_role_vice_manager	= 99,	//����
		qun_role_manager		= 100,	//����

		//Ⱥ���������
		max_qun_count			= 32,	//Ⱥ�����32��
		max_qun_uid_length		= 32,	//Ⱥ���˺�����󳤶�Ϊ32��Сд���ַ���
		max_qun_name_length		= 32,	//Ⱥ�����ֳ���
	};

	//�ͻ���״̬������
	enum enmClientStateMask
	{
		client_state_hall_mask		= 0x00000001,	//�����Ƿ�����
		client_state_camera_mask	= 0x00000002,	//����ͷ�Ƿ���
	};

	enum enmAntiFlag
	{
		antibot_room	= 0x00000001,		// ����ҷ���
	};



	enum enmClientGlobalSettings
	{
		//global setting type
		global_setting_private_talk		= 1,	//˽�����Ƶ�ȫ������
		global_setting_play_with		= 2,	//��Ϸ�����ȫ������
		global_setting_make_friends		= 3,	//�����֤��ȫ������
		global_setting_video_invitation	= 4,	//��Ƶ�����ȫ������

		//global setting value
		private_talk_setting_default_value		= 0,	//˽������δֵ֪���ͻ���δ���ã�
		not_to_recv_any_private_talk			= 1,	//�������κ�˽����Ϣ
		not_to_recv_stranger_private_talk		= 2,	//������İ���˵�˽����Ϣ
		recv_any_private_talk_from_other		= 3,	//���������˵�˽����Ϣ

		play_with_setting_default_value			= 0,	//��Ϸ����δֵ֪���ͻ���δ���ã�
		not_to_recv_any_play_with				= 1,	//�������κ���Ϸ����
		not_to_recv_stranger_play_with			= 2,	//������İ���˵���Ϸ����
		recv_any_play_with_from_other			= 3,	//���������˵���Ϸ����

		make_friends_setting_default_value		= 0,	//���������δֵ֪���ͻ���δ���ã�
		need_my_agree_to_make_friends			= 1,	//��Ҫ��ͬ����ܼ���Ϊ���
		auto_refuse_to_make_friends				= 2,	//�Զ��ܾ������˰��Ҽ�Ϊ���
		auto_agree_with_make_friends			= 3,	//����Ҫͬ��ͼ����

		video_invitation_setting_default_value	= 0,	//��Ƶ����δֵ֪���ͻ���δ���ã�
		not_to_recv_any_video_invitation		= 1,	//�������κ���Ƶ����
		not_to_recv_stranger_video_invitation	= 2,	//������İ���˵���Ƶ����
		recv_any_video_invitation_from_other	= 3,	//���������˵���Ƶ����
	};

	//�����ͻ���͸�����ݵ�CommandID
	enum enmLobbyDirectionSettings
	{		
		UnknownData_CommandID_P2P		= 425,
		DIRECT_DATA_SEND_SMILE			= 0x0100,	//Цһ��
		DIRECT_DATA_SUPERCILIOUS_LOOK	= 0x0200,	//����
		DIRECT_DATA_INVITE_TO_GAME		= 0x0010,	//������Ϸ
		DIRECT_DATA_ACCEPT_INVITE_TO_GAME  = 0x0020,//������Ϸ����		
		DIRECT_DATA_REFUSE_INVITE_TO_GAME  = 0x0040,//�ܾ���Ϸ����
		DIRECT_DATA_INVITE_TO_VIDEOCHAT = 0x0001,	//������Ƶ
		DIRECT_DATA_REPLY_VIDEOCHAT     = 0x0002,	//�ظ���Ƶ����(���ܻ�ܾ�)
		DIRECT_DATA_STOP_VIDEOCHAT		= 0x0003,	//��ֹ��Ƶ
	};



	enum enmClientType
	{
		client_type_not_client  = 0,    //
		client_type_flash_client = 1,   //flash �ͻ���
		client_type_iphone_client = 2,  //iphone �ͻ���
	};

}

#endif

