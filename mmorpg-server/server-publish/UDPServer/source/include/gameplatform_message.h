#ifndef __GAMEPLATFORM_MESSAGE_H__
#define __GAMEPLATFORM_MESSAGE_H__

// #include "gameplatform_types.h"
#include "CSMessages_i.h"

//////////////////////////////////////////////////////////////////////////
// SSЭ��  �ر�ע��: ID��Χ: [0x4000 -- 0x7FFD]
//////////////////////////////////////////////////////////////////////////
//Logic server  VS Game DB server
#define SS_MSG_GETGAMEDATA		                    0x4000			//(must)
#define SS_MSG_LOCKMONEY		                    0x4001
#define SS_MSG_UPDATEPOINT		                    0x4002          //must
#define SS_MSG_REFRESH_GAMEPOINT	                0x4003			//notify game db -> logic server��Point��Money�ı仯��ͨ����ֵ������ʵ��.(must)
#define SS_MSG_NOTIFY_LOCKMONEY		                0x4004			//game db -> logic server notify ֪ͨ������������Ϸ�Ҽ���/����
#define SS_MSG_UPDATE_GAME_CONFIG	                0x4005			//
#define SS_MSG_LOGICSERVER_INIT		                0x4006			//notify logic server -> game db ֪ͨgamedb logic server�����˳�ʼ������(must)
#define SS_MSG_NOTIFY_DELETE_PLAYER_REFRENCE	    0x4007			//notify logic server -> game db ֪ͨgame db ɾ����ҵ���Ϸ��������
#define SS_MSG_REFRESH_GAMECONFIG	                0x4008          //game db -> logic server notify ֪ͨ������������Ϸ����Ϸ�������Ѿ����� add by lz 2008.04.08
#define SS_MSG_CHANGEMONEY			                0x4009			//������Ϸ��(must)
#define SS_MSG_UPDATE_PLAY_COMMON_DATA		  	    0x4010			//����player�Ļ�������(����coin֮���) add by lz 2008.08.16
#define SS_MSG_REFRESH_PLAY_COMMON_DATA			    0x4011			//֪ͨplayer�Ļ������ݱ仯 add by lz 2008.08.16
#define SS_MSG_RECORD_SHARED_MONEY				    0x4012			//logic server --> game db. ����Ҫresponse
#define SS_MSG_GET_PLAYER_ALL_GAME_DATA		        0x4013			//profile server -->game db,��ȡָ����ҵ���Ϸ����.
#define SS_MSG_UPDATE_EXT_GAME_INFO					0x4014			//service server -->game db,������ҵ�ĳ����Ϸ����������
#define SS_MSG_REFRESH_EXT_GAME_INFO				0x4015			//game db--->service server,֪ͨǰ����Ϸ������ĳ����ҵ���Ϸ���������ݵ�ˢ��
#define SS_MSG_UPDATE_PLAYER_CARDID                 0x4016          //�����û����֤��
#define SS_MSG_PLAYER_CHARGE                        0x4017          //�û���ֵ
#define SS_MSG_PLYAER_EXCHANGE                      0x4018          //�û��ض�
#define SS_MSG_GET_PLAYER_ENTHRALMENTDATA           0x4019          //��ȡ��ҵķ���������.
#define SS_MSG_NOTIFY_FORBIDDEN_TALK				0x401A			//��ֹ����	
#define SS_MSG_PRESENT_HAPPY_BEAN					0x401B			//���ͻ��ֶ�
#define SS_MSG_NOTIFY_PRESENT_HAPPY_BEAN          0x401C			//���ͻ��ֶ���֪ͨ��Ϣ
#define SS_MSG_LOCK_HAPPY_BEAN                      0x401D          //�����ֶ�
#define SS_MSG_NOTIFY_LOCK_HAPPY_BEAN			    0x401E          //ˢ�»��ֶ�����
#define SS_MSG_CHANGE_HAPPY_BEAN                    0x401F          //������ֶ�
#define SS_MSG_NOTIFY_REFRESH_HAPPY_BEAN            0x4020          //������ֶ���֪ͨ.

//#define SS_MSG_SET_PLAYER_ACCOUNT                 0x4020          //����ĳ���ʻ�
#define SS_MSG_UPDATE_PLAYER_ENTHRALMENTDATA        0X4021          //������ҵķ���������
#define SS_MSG_NOTIFY_PLAYER_CONSUME_DATA           0x4022          //֪ͨ��ҵ��������(GameDB-->��ط�����)
#define SS_MSG_QUERY_PLATFORM_MONEY_LOCK	    0x4023          //��ѯƽ̨�Ľ����...
#define SS_MSG_UPDATE_PLATFORM_MONEY_LOCK           0x4024			//����ƽ̨�Ľ����...
#define SS_MSG_REFRESH_PLATFORM_MONEY_LOCK          0x4025          //����֪ͨ��Ϸ�߼�������,ƽ̨������ı仯....
#define SS_MSG_REGISTER_SERVICE                     0x4026          //ע�����
#define SS_MSG_REFRESH_SERVICE                      0x4027          //�������ˢ��֪ͨ....
#define SS_MSG_RENEW_SERVICE                        0x4030          //��������
#define SS_MSG_GET_USER_DETAIL_INFO                 0x4028          //��ȡ�û�����ϸ��Ϣ...(Ŀǰ������̨����ƽ̨ʹ��,��gm)
#define SS_MSG_CHECK_USER_IS_EXIST                  0x4029          //����û�����(Ŀǰ����hall serverʹ��)
#define SS_MSG_NOTIFY_GAME_RESULT                   0x4030          //֪ͨ��Ϸ���(Ŀǰ����Action Serverʹ��)
#define SS_MSG_NOTIFY_QUN_PLAYER_DATA               0x4031          //Ⱥ�û������ݱ��֪ͨ
#define SS_MSG_NOTIFY_GAMEDATA_HEARTBEAT	    0x4032			//֪ͨGameDB��Ϸ����������
#define	SS_MSG_NOTIFY_PLAYER_ACTIONDATA             0x4033          //֪ͨ��һ��Ϣ��.
#define SS_MSG_GET_PLAY_COMMON_DATA                 0x4034          //��ȡ��һ�������
#define SS_MSG_NOTIFY_PLAYER_COMMON_DATA            0x4035          //�û�����֪ͨhall server���û���common data����
#define SS_MSG_GET_USER_PROFILE                     0x4036          //��ȡ�û��ĸ�������(profileר��)
#define SS_MSG_KICK_PLAYER			    0x4100			//����
#define SS_MSG_BROADCAST_SYSTEM_MESSAGE		    0x4101			//�㲥ϵͳ��Ϣ
#define SS_MSG_NOTIFY_BROADCAST_NOTICE_INFO	    0x4102			//����������Ϣ

#define SS_MSG_REFRESH_VIP_DATA                         0x4201          //ˢ���û���VIP���� vipserver-->gamedb server
#define SS_MSG_VIP_NOTIFY_MESSAGE                       0x4202          //֪ͨ�û���VIP��ص���Ϣ
#define SS_MSG_UPDATE_VIP_DATA                          0x4203          //(GM)�ֶ������û���VIP����.


//logic server vs antibot server

#define SS_MSG_ANTIBOT_DATA						    0x4151          //���������
#define SS_MSG_ANTIBOT_PUNISH						0x4152          //����ҳͷ�

//logic server vs compete db
#define SS_MSG_GET_COMPETE_GAME_DATA				0x4300			//��ȡ��������
#define SS_MSG_UPDATE_COMPETE_GAME_POINT			0x4301			//���±�������
#define SS_MSG_REFRESH_COMPETE_GAME_POINT			0x4302			//֪ͨ���±�������
#define SS_MSG_NOTIFY_PLAYER_LEAVE_COMPETE			0x4303			//֪ͨ����뿪��Ϸ

//������������hotnews server֮���Э��
#define SS_MSG_NOTIFY_HOTNEWS			            0x4E00          //���Ѷ�̬

//������������router server֮���Э��
#define SS_MSG_ROUTER_HEARTBEAT			            0x4F00			//��router server֮���heart beat
#define SS_MSG_ROUTER_REGISTER			            0x4F01			//������������router server�Լ�


//vip server


//Logic server VS Catalog server
#define SS_MSG_HELLO			                    0x5000
#define SS_MSG_REGISTER			                    0x5001			// s<->s ע�����������ȡ������Ϣ
#define SS_MSG_REPORTSTATICS	                    0x5002			//�ϱ�ͳ����Ϣ notify



//logic server VS State Server ��should be deleted��
#define SS_MSG_NOTIFY_LOGICSERVER_INIT		        0x5100			//logic server ֪ͨstate server �����ʼ��
//#define SS_MSG_NOTIFY_PLAYER_STATUS			        0x5101			//logic server�ϱ����״̬��state server
//#define SS_MSG_NOTIFY_HEARTBEAT_LOGIC_STATE	        0x5102			//logic server��state server֮���������


//Profile server VS Game DB server
#define SS_MSG_GETUSERINFO                          0x6000          //profile server -> game db server add by lz 2008.04.08
#define SS_MSG_GETOTHERUSERINFO                     0x6001          //profile server -> game db server add by lz 2008.04.08


#define SS_MSG_AD_URL								0x6200			//magic server <--> ad server����ȡ���URL

#define SS_MSG_PUBLISH_AD							0x6201    // gm server <--> ad server �������
#define SS_MSG_DELETE_AD							0x6202    //  gm server <--> ad server ɾ�����
#define SS_MSG_GET_AD_LIST							0x6203    //  gm server <--> ad server ��ѯ���
#define SS_MSG_SEND_MAC_LIST						0x6204     //  gm server <--> ad server ����mac��ַ�б�
//hall server vs score server
#define SS_MSG_NOTIFY_51_SCORE_DELTA				0x7300			//���ַ�����֪ͨ�û����ڵ�hall��ta��51��������
#define SS_MSG_NOTIFY_MESSAGEBOX				    0x7301
#define SS_MSG_NOTIFY_LOGIN     				    0x7302
//HallServer��LogicServer��StateServer�� VS Friend DB server 
#define SS_MSG_GET_POSITIVE_FRIEND_LIST             0x7400          //��ȡ�Լ�����������б�
#define SS_MSG_GET_STATISTICAL_TAG                  0x7401          //��ȡĳ�˵�ͳ�Ʊ�ǩ��Ŀǰ���10��
#define SS_MSG_ADD_TAG_TO_FRIEND                    0x7402          //��������ǩ�����Զ����Ϊ���
#define SS_MSG_DELETE_FRIEND                        0x7403          //ɾ�����
#define SS_MSG_GET_REVERSE_FRIEND_LIST              0x7404          //��ȡ�Լ�����������б�
#define SS_MSG_ADD_REVERSE_TAG                      0x7405          //�������ǩ
#define SS_MSG_NOTIFY_FRIEND_INFO                   0x7406          //ȫ��֪ͨĳ�����ı�ǩ��Ϣ
#define SS_MSG_GET_RECOMMEND_TAG                    0x7407          //��ȡ��ҵ��Ƽ���ǩ��Ŀǰ��ͳ�Ʊ�ǩ��ȡ6��������6��ȡȫ��
#define SS_MSG_DELETE_REVERSE_FRIEND				0x7408			//ɾ���������
#define SS_MSG_GET_SIMPLE_FRIEND_LIST				0x7409			//��ȡ�򵥵�����б�ֻ������UIN��
#define SS_MSG_ADD_FRIEND_GROUP						0x740A			//����б�������Զ������
#define SS_MSG_DEL_FRIEND_GROUP						0x740B			//ɾ������б��е��Զ������
#define SS_MSG_MOVE_FRIEND_TO_GROUP					0x740C			//������Ƶ�ָ���ķ�����
#define SS_MSG_RENAME_FRIEND_GROUP					0x740D			//�Զ���������������
#define SS_MSG_GET_FRIEND_GROUPS					0x740E			//��ȥ��ҵ���������Ϣ
#define SS_MSG_RENAME_FRIEND_REMARKS				0x740F			//�������ı�ע
#define SS_MSG_GET_PLAYED_WITH_INFO					0x7410			//��ȥ��˭һ���������Ϣ
#define SS_MSG_ADD_PLAYED_WITH_INFO					0x7411			//��Ӻ�˭һ���������Ϣ
#define SS_MSG_NOTIFY_DEL_PLAYED_WITH_INFO			0x7412			//֪ͨɾ��ĳ����˭һ���������Ϣ
#define SS_MSG_ADD_FRIEND							0x7413			//�����(����ǩ+����+��ע)
#define SS_MSG_GET_FRIEND_BASE_INFO					0x7414			//��ȡ���Ļ�����Ϣ��UIN+�˺ţ�
#define SS_MSG_GET_BLACK_LIST_INFO					0x7415			//��ȡ������
#define SS_MSG_ADD_BLACK_LIST_INFO					0x7416			//��Ӻ�����
#define SS_MSG_DEL_BLACK_LIST_INFO					0x7417			//ɾ��������
#define SS_MSG_ADD_NOT_VERIFIED_FRIEND_INFO			0x7418			//���δ��֤�ĺ�����Ϣ
#define SS_MSG_CHECK_NOT_VERIFIED_FRIEND_INFO		0x7419			//��ʵδ��֤�ĺ�����Ϣ
#define SS_MSG_NOTIFY_MAKE_FRIEND_COUNT_INFO		0x741A			//֪ͨ�Ӻ��ѵ�������Ϣ
#define SS_MSG_NOTIFY_GET_CHAT_WITH_INFO			0x741B			//֪ͨ���غ�˭�Ĺ�����Ϣ
#define SS_MSG_NOTIFY_ADD_CHAT_WITH_INFO			0x741C			//֪ͨ��Ӻ�˭�Ĺ�����Ϣ
#define SS_MSG_NOTIFY_DEL_CHAT_WITH_INFO			0x741D			//֪ͨɾ����˭�Ĺ�����Ϣ



//hall server��������˷�����������Э�飬����˵��ʼ�����ϱ����״̬��
//hall server vs state��game db��item db��friend db server
#define SS_HALL_NOTIFY_INIT						    0x7500		    //hall����ʱ����state��friend db��game db��item db serverע��
#define SS_HALL_NOTIFY_PLAYER_STATE 			    0x7501		    //hall��state��friend db��game db��item db server�ϱ����״̬�����ߡ����ߵ�
#define SS_MSG_BROADCAST_TRANSFRE_MESSAGE			0x7502			//������ָ�����͵�����Ŀ�ķ�����ת��͸������
#define SS_MSG_NOTIFY_BROADCAST_TRANSFRE_MESSAGE	0x7503			//��ָ�����͵�����Ŀ�ķ�����ת��͸������

//gm server��state server
#define SS_MSG_BROADCAST_GM_SYSTEM_MESSAGE          0x7600          //GM����ϵͳ��Ϣ

//GM Server VS(Logic&Hall Server)
#define SS_MSG_RELAY_SYSTEM_MESSAGE					0x7601			//GMͨ��logic server ��(��)hall server����ϵͳ��Ϣ
#define	SS_MSG_SUBMIT_SYSTEM_MESSAGE				0x7602			//GM �ύ�����ã�ϵͳ��Ϣ��logic & hall server
#define SS_MSG_CANCEL_SYSTEM_MESSAGE				0x7603			//GM ȡ��(���)ԭ���ύ�����ã���logic & hall Server��ϵͳ��Ϣ

//GM Server VS State Server
#define SS_GM_GET_PLAYER_STATE						0x7604			//GM�����ȡ���״̬
#define SS_GM_GET_ALL_PLAYER_STATE					0x7605			//GM����鿴State Server�е������ڴ�����

#define SS_MSG_CANCEL_MESSAGE				        0x7606			//GM HallServerɾ��ԭ���趨��ϵͳ��Ϣ������֪ͨ�ͻ���ɾ��ָ�������յ�����Ϣ 
#define SS_MSG_SEND_SYSTEM_MESSAGE_TO_PLAYER        0x7607          //GM <-> HallServer ��ָ���û�����ϵͳ��Ϣ

//State Server
#define SS_MSG_REGISTER_TO_STATESERVER				0x7610			//��StateServerע�����
#define SS_MSG_STATESERVER_NOTIFY_PLAYER_STATE		0x7611			//StateServer֪ͨ���״̬�仯���Ѿ�ע�����ķ�����
#define SS_MSG_STATE_NOTIFY_BROADCAST_MSG_INFO		0x7612			//��logic server�ķ����hall server�·��㲥��Ϣ

//hall server vs state server
#define SS_HALL_STATE_REPORT_GAME_STATE				0x7700		    //�ͻ�����hall�ϱ���ǰ��Ϸ״̬��hall֪ͨ������state
#define SS_HALL_STATE_UPDATE_PLAYER_PROFILE			0x7701			//�ͻ��˸��»������ϣ�hall֪ͨ������state
#define SS_HALL_STATE_REQUEST_PLAYER_STATE			0x7703		    //cliet��hall�����ȡ���״̬��hallת����state
#define SS_HALL_STATE_REQUEST_PLAYERCOUNT_IN_HALL	0x7704		    //hall��state���������������
#define SS_HALL_STATE_HEARTBEAT						0x7705		    //���������ܸ���������������һ����
#define SS_HALL_STATE_NOTIFY_FRIEND_ONLINE			0x7706		    //֪ͨ��������                 state -> hall
#define SS_HALL_STATE_NOTIFY_KICKPLAYER				0x7707		    //���������ط���½��֪ͨ��ҹر���Ϸ
#define SS_HALL_STATE_REQUEST_TRANSFER_MESSAGE		0x7708		    //����state��תһ����Ϣ
#define SS_HALL_STATE_NOTIFY_TRANSFER_MESSAGE		0x7709		    //֪ͨhall����ϢҪת��client   state -> hall
#define SS_STATE_HALL_GET_REVERSE_FRIEND_UINS		0x770A			//state server��ȡ��������UIN
#define SS_HALL_STATE_NOTIFY_BROADCAST_MESSAGE		0x770B			//֪ͨstate server�㲥��Ϣ
#define SS_STATE_HALL_NOTIFY_STATUS_TOFRIEND		0x7710		    //State Server��Hall����push�û���״̬��Ϣ���û��������

//hall server vs item db
//#define SS_HALL_ITEM_NOTIFY_PLAYER_STATUS         0x7800          //hall��item db�ϱ����״̬��login��logout del by lz 2008.08.21 SS_HALL_NOTIFY_PLAYER_STATE
#define SS_ITEMDB_UPDATE_PLAYER_ITEMINFO			0x7801	        //������������Item DB֮����������Ʒ��Ϣ��Э��	
#define SS_ITEM_HALL_NOTIFY_PLAYER_INFO             0x7802          //item db�·������Ʒ������Ϣ��hall
#define SS_MSG_ITEMDB_GET_USER_ITEM_INFO		    0x7803	        //ҵ�������(��hall server,...)��ѯĳ���û��ĵ��߻�������Ʒ��Ϣ...
#define SS_MSG_ITEMDB_TRANSFER_MESSAGE				0x7804          //ҵ���������ItemDB֮�䴫����Ϣ(����ϢΪ֪ͨ����Ϣ)
#define SS_MSG_DELETE_ITEM                          0x7805          //ɾ��ָ���ĵ��ߣ����������û�
#define SS_MSG_CLEAN_EXPIRE_ITEM                    0x7806          //������ڵ���,���������û�
#define SS_MSG_QUERY_PLAYER_ITEMDATA                0x7807          //��ѯ�û���ָ�����͵ĵ���
#define SS_MSG_UPDATE_PLAYER_EQUIPMENT_INFO         0x7808          //�����û���װ����Ϣ
#define SS_MSG_REFRESH_PLAYER_EQUIPMENT_INFO        0x7809          //ˢ���û���װ����Ϣ

//state server VS Catalog server
#define SS_MSG_STATE_SERVER_HELLO					0x7900
#define SS_MSG_STATE_SERVER_REGISTER				0x7901						// s<->s ע�����������ȡ������Ϣ
#define SS_MSG_STATE_SERVER_REPORTSTATICS			0x7902						//�ϱ�ͳ����Ϣ notify
#define WS_MSG_WEB_REPORTSTATICS					0x7903						//web�ڵ��ϱ�ͳ����Ϣ notify


//Misc server VS Group server
#define SS_MSG_GET_GROUP_RANK						0x7A01         //��ȡָ��Ⱥ����
#define SS_MSG_GET_GROUP_TOP_RANK					0x7A02         //��ȡȺtop����


//offline server vs hall server
#define SS_MSG_PUSH_OFFLINE_MSG					0x7A10


//pay.51.com  VS proxy server
#define CS_MSG_PAY_TO_GAME							0x7B01
#define CS_MSG_PAY_FROM_GAME						0x7B02
#define CS_MSG_SEARCH_GAME_MONEY					0x7B03
#define CS_MSG_PAY_TO_VIP					        0x7B04        //PayServerֱ��ע���û�VIP��Ϣ
#define CS_MSG_SEARCH_VIP_INFO                      0x7B05        //PayServer��ѯ�û�VIP��Ϣ
#define CS_MSG_PAY_MOBILE_TO_VIP					0x7B06        //�ֻ�����VIP
#define CS_MSG_PAY_UNSUBSCRIBE_VIP					0x7B07        //�ֻ��˶�����VIP

//game.51.com  VS proxy server
#define CS_MSG_UPDATE_CARD_ID						0x7C01
#define CS_MSG_GET_USER_INFO						0x7C02
#define CS_MSG_ITEM_EXCHANGE						0x7C03
#define CS_MSG_GET_ITEM_INFO						0x7C04
#define CS_MSG_BUY_GOODS							0x7C05		//web�����̳���Market Server֮��Ĺ���/����Э�� .˫��
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

#define CS_MSG_GM_GET_USER_DETAIL_INFO				0x7D09 //�鿴�û�״̬�����ԣ���ţ�����&������Ϸ��
#define CS_MSG_CHANGE_MONEY				            0x7D0A //�޸������Ϸ��
#define CS_MSG_LOCKMONEY							0x7D0B //����&���������Ϸ��
#define CS_MSG_QUERY_PLATFORM_MONEYLOCK_TO_GAMEDB   0x7D0C //�鿴gamedbƽ̨��Ϸ��״̬
#define CS_MSG_UPDATE_PLATFORM_MONEYLOCK_TO_GAMEDB  0x7D0D //����gamedbƽ̨��Ϸ��״̬
#define CS_MSG_GM_UPDATE_GAME_PROP                  0x7D0E //�����û���Ϸ����
#define CS_MSG_GM_UPDATE_GAME_POINT                 0x7D0F //�����û���Ϸ����
#define CS_MSG_GM_UPDATE_Withery_Value              0x7D10 //�����û�����ֵ
#define CS_MSG_GM_UPDATE_Achievement_Value          0x7D11 //�����û��ɾ�ֵ
#define CS_MSG_GM_DELETE_ITEM                       0x7D12 //ɾ��ָ������
#define CS_MSG_GM_CLEAN_EXPIRED_ITEM                0x7D13 //������ڵ���
#define CS_MSG_QUERY_PLATFORM_MONEYLOCK_TO_ITEMDB   0x7D14 //�鿴itemdbƽ̨��Ϸ��״̬
#define CS_MSG_UPDATE_PLATFORM_MONEYLOCK_TO_ITEMDB  0x7D15 //����itemdbƽ̨��Ϸ��״̬
#define CS_MSG_GM_CHANGE_HAPPY_BEAN                 0x7D16 //�޸���ҿ��ֶ�
#define CS_MSG_GM_LOCK_HAPPY_BEAN                   0x7D17 //����&������ҿ��ֶ�
#define CS_MSG_GET_PLAYER_ALL_GAME_DATA             0x7D18 //��ȡ���������Ϸ��Ϣ
#define CS_MSG_GET_PLAYER_ITEM_DATA                 0x7D19 //��ȡ������е�����Ϣ
#define CS_MSG_GET_PLAYER_STATE_FROM_STATE_SERVER   0x7D20 //��StateServer��ȡ�������״̬��Ϣ
#define CS_MSG_GET_ALL_PLAYER_STATE                 0x7D21 //��ȡ�������״̬��Ϣ
#define CS_MSG_GM_CANCEL_MESSAGE                    0x7D22 //ɾ��ϵͳ��Ϣ
#define CS_MSG_GM_SEND_SYSTEM_MESSAGE_TO_PLAYER     0x7D23 //����ϵͳ��Ϣ��ָ���û�
#define CS_MSG_GM_REGISTER_GAME_SERVICE             0x7D24 //Register��Ϸ����
#define CS_MSG_GM_UPDATE_VIP_DATA                   0x7D25 //�����û�VIP�����Ϣ
#define CS_MSG_GM_REGISTER_SERVICE                  0x7D26 //���û��ӷ��񣬰���VIP����
#define CS_MSG_GM_PUBLISH_AD                        0x7D27 //GM���������Ϣ
#define CS_MSG_GM_DELETE_AD                         0x7D28 //GMȡ�����
#define CS_MSG_GM_GET_AD_LIST                       0x7D29 //GM��ȡ����б�
#define CS_MSG_GM_SEND_MAC_LIST                     0x7D30 //GM����MAC��ַ�б�

//client.51.com VS misc Server
#define CS_MSG_MISC_GET_PROFILE_FRIEND_LIST			0x7E01 //��ȡ������б�
#define CS_MSG_GET_PLAYER_CARD_ID					0x7E02 //��ѯ�û����֤��Ϣ
#define CS_MSG_GET_GROUP_RANK						0x7E03 //��ȡָ��Ⱥ����
#define CS_MSG_GET_GROUP_TOP_RANK					0x7E04 //��ȡȺtop����
#define CS_MSG_GET_FRIEND_GROUP_INFO				0x7E05 //��ȡ������Ϣ
#define CS_MSG_GET_PLAYER_GROUP_DATA				0x7E06 //��ȡ�û�ȺȨ��
#define WS_MSG_GET_SIMPLE_FRIEND_LIST				0x7E08 //�������б�
#define WS_MSG_GET_SIMPLE_GAME_ID_LIST				0x7E09 //��ȡ����������ϷID


//client.51.com VS Recommend Server

#define CS_MSG_GET_RECOMMEND_USER					0x7E07  //��ȡ�Ƽ��û�
#define WS_MSG_GET_RECOMMEND_USER_LIST				0x7E0A  //��ȡ�Ƽ��û��б�
//�ر�ע��:SSЭ���ID��Χ: [0x4000 -- 0x7FFD]


//chat server 

#define MSG_CHAT_NOTIFY_CLEAR_PLAYER				0x5000			//����player 
namespace KingNet 
{
	enum enmCSResultID
	{
		result_id_success						= 0,
		result_id_fail							= 1,		//δ֪���������ԣ�
		result_id_invalid_param					= 2,		//������ƥ�䣬�����ԣ�
		result_id_acion_deny					= 3,		//��ϵͳ�ܾ���������ϵͳҪ�����ǲ�����ɻ���?
		result_id_action_timeout				= 4,		//ϵͳ��ʱ������ô���?
		result_id_action_enterroom_deny			= 5,		//����Ȩ�޲���,�����������
		result_id_sequence_invalid				= 6,		//�ͻ��˵�sequence����

		result_id_action_invalid				= 10,		//��Ч�Ĳ�������������ԣ����ǲ�����ɻ���?С�ľ���������
		result_id_sitdown_noseat				= 11,		//Ŷ������λ���Ѿ������ˣ�	
		result_id_sitdown_deney					= 12,		//ϵͳ�ܾ����ˣ���������ʲô?
		result_id_lock_fail						= 13,		//������Ϸ��ʧ�ܣ�
		result_id_unlock_fail					= 14,		//������Ϸ��ʧ�ܣ�
		result_id_lack_resource					= 15,		//��Ϸ�Ҳ��㣡
		result_id_room_deny_observe				= 16,		//�÷����ڲ������Թۣ�
		result_id_not_in_room					= 17,		//���ڷ����ڲ����Թۣ�
		result_id_on_other_table				= 18,		//������������Ϸ�У�����վ��
		result_id_table_not_gaming				= 19,		//��Ϸ��û���֣������Թۣ�

		result_id_seat_no_player				= 20,		//�����ڿ���λ���Թۣ�
		result_id_game_deny_observer			= 21,		//�ܾ��Թ۸���Ϸ��
		result_id_table_full					= 22,		//�����˱����������ټ�����
		result_id_invalid_table_key				= 23,		//��Ϸ������������������ȷ�����룡
		result_id_deny_kick						= 24,		//û���㹻��Ȩ�����ˣ�
		result_id_sitdown_deny_lack_money		= 25,		//��Ϸ�Ҳ��㣡
		result_id_enter_so_many_room			= 26,		//����ķ���̫���ˣ�
		result_id_enter_room_is_full			= 27,		//�÷�������������������ѡ����΢�����ķ���ɣ�
		result_id_replay_not_in_room			= 28,		//�ز�ȥ�ˣ���������������ڣ�
		result_id_replay_invalid_status			= 29,		//�޷������ˣ�����������Ϸ����?

		result_id_open_so_many_game				= 30,		//����ͬʱ��̫�����Ϸ��������ô����Ϸ��������?
		result_id_not_be_observing				= 31,		//�����Թ�״̬��
		result_id_not_on_seat					= 32,		//����ָ����λ���ϣ�
		result_id_table_alread_in_gameing		= 33,		//��Ϸ���Ѿ����֣�
		result_id_player_not_on_table			= 34,		//������Ϸ���ϣ�
		result_id_cant_locktable_in_compete		= 35,		//�������䲻��������
		result_id_cant_locktable_someelse		= 36,       //���ϻ���������,����������
		result_id_cant_obsv_alread_sitted		= 37,		//�����ڸ���,�����Թ�����λ�ã�
		result_id_cant_kickplayer_in_gaming		= 38,		//������Ϸ��,�������ˣ� 
		result_id_target_not_in_room			= 39,		//Ŀ�겻�ڷ����ڣ�

		result_id_buy_no_goods					= 40,		//û��ָ������Ʒ�������Ŀ�������Щ��Ʒ��?
		result_id_buy_not_enough_money			= 41,		//Ǯ���������Ͻ�ȥ��㣡
		result_id_buy_chang_money_timeout		= 42,		//�޷��۷ѣ������ԣ�
		result_id_must_on_table_to_kick			= 43,		//��������Ϸ���ϲ������ˣ�
		result_id_cant_standup_in_gameing		= 44,		//������Ϸ���޷�վ��
		result_id_enter_room_less_charm			= 45,		//����ʧ�ܣ���������,������Ҫ%d��������
		result_id_enter_room_less_yxb			= 46,		//����ʧ�ܣ���Ϸ�Ҳ���,������Ҫ%d��Ϸ�ң�
		result_id_cant_talk_not_on_table		= 47,		//������Ϸ���ϲ������죡
		result_id_observor_cant_talk			= 48,		//�������Թ��߽�ֹ���죡
		result_id_cant_talk_not_in_room			= 49,		//���ڷ����ڲ������죡

		result_id_cant_talk_prohibit			= 50,		//ϵͳ�����������죡	
		result_id_cant_talk_illegal_words		= 51,		//51��Ϸף��������Ϸ���������֣�
		result_id_anti_addict_tired_notify		= 52,		//�������Ѿ�����ƣ����Ϸʱ��,���ֻ�����ԭ����(%d)����Ϊ(%d),51��Ϸ��ܰ������:����51,������Ϸ��
		result_id_anti_addict_abnormal_notify	= 53,	    //�������Ѿ����벻������Ϸʱ��,���ֻ�����ԭ����(%d)����Ϊ(%d),51��Ϸ��ܰ������:����51,������Ϸ��
		result_id_client_version_so_old			= 54,		//�����汾̫�ͣ��������������������µ�½��
		result_id_zero_fail_no_gamedata			= 55,		//����û���������Ϸ���޷�ʹ�ø���������ߣ�
		result_id_zero_fail_isnot_negative		= 56,		//������Ϸ�Ļ��ַǸ����벻Ҫʹ�ø���������ߣ�
		result_id_kick_player_not_exist			= 57,		//����ʧ��:Ŀ����Ҳ����ڣ�
		result_id_kick_player_not_inroom		= 58,		//����ʧ�ܣ�Ŀ����Ҳ��ڷ����ڣ�
		result_id_replay_game_deny				= 59,		//�����ػ�ʧ�ܣ���Ϸ�ܾ��ػأ�
		result_id_locktable_not_master			= 60,		//����ʧ��,ֻ��������������!
		result_id_kick_player_target_is_vip		= 61,		//�Է���VIP��ң���������!���|��Ҳ��ΪVIP���!|
		result_id_kick_player_not_table_master  = 62,		//��������,ֻ��VIP��һ�����Ϸ��������������,���|��Ҫ��ΪVIP���!|
		result_id_kick_player_not_ahead_vip_target = 63,	//�Է�Ҳ��VIP���,��ֻ�����ڶԷ����²����߶Է�,�����������ta!
		result_id_enter_room_less_happybean		= 64,		//
		result_id_enter_room_somany_happybean	= 65,		//
		result_id_lock_table_invalid_key		= 66,		//����ʧ�ܣ���Ч������
		result_id_lock_enter_room_fail_default  = 67,		//���ź�����û�дﵽ����÷��������,��ѡ���������䣡
		result_id_action_deny_service_halting	= 68,		//ϵͳ����ͣ��
		result_id_not_selectable_seat			= 69,		//�Ҳ������ʵ���Ϸ��,���Ժ�����!(Ӧ��������һ�εİ�ť)
		result_id_chat_51score_not_enough		= 70,		//51���ֵȼ�����������˽��
		result_id_chat_illegal_words			= 71,		//˵�˷Ƿ��Ĵʣ�����˽��
		result_id_chat_refused_not_friend		= 72,		//������飬�ܾ�˽��
		result_id_assign_table_failed			= 73,		//
		result_id_wait_to_assign_table			= 74,		//�ȴ�ϵͳΪ��������Ϸ��
		result_id_dogfight_no_idle_table		= 75,		//ϵͳ����ʱû�п�����Ϸ��
		result_id_talk_inroom_so_fast			= 76,		//����Ƶ�ʹ���
		result_id_talk_too_many_words			= 77,		//һ�仰˵��̫�����
		result_id_cant_talk_to_many_players		= 78,		//����ͬʱ��N��������

		//added by aprilliu, 2010-01-07
		result_id_painting_enter_room_fail		= 80,		//


		string_code_be_silenced					= 100,		//���Ѿ�������,��%sǰ�����޷����ԣ�
		string_code_be_forbidden				= 101,		//�����ʺ��ѱ�����, ����%s���⣡
		string_code_first_enter_game			= 102,		//����51��Ϸƽ̨��[%s]��Ϸ��
		string_code_first_login_platform		= 103,		//����һ�ε�½��51��Ϸƽ̨��
		string_code_gameplayer_name				= 104,		//��Ϸ���
		string_code_gameobserver_name			= 105,		//�Թ����
		string_code_gamemaster_name				= 106,		//��Ϸ����
		string_code_talk_inroom_be_delayed		= 107,		//���е�һ���������ڷ��������죡
		string_code_talk_inroom_less_gameround  = 108,		//Ҫ����Ϸ���������ڷ��������죡
		string_code_talk_inroom_less_day		= 109,		//�յ�¼51��Ϸ���������ڷ��������죡

		string_code_first_login_platform_score0 = 200,		//�״ε�½��Ϸƽ̨����51���֣����ͺ��Ѷ�̬����վ
		string_code_first_login_platform_score1 = 201,		//�״ε�½��Ϸƽ̨����51���֣����ͺ��Ѷ�̬����վ
		string_code_first_login_platform_score2 = 202,		//�״ε�½��Ϸƽ̨����51���֣����ͺ��Ѷ�̬����վ
		string_code_first_login_platform_score3 = 203,		//�״ε�½��Ϸƽ̨����51���֣����ͺ��Ѷ�̬����վ
		string_code_first_login_platform_score4 = 204,		//�״ε�½��Ϸƽ̨����51���֣����ͺ��Ѷ�̬����վ
		string_code_recv_many_flowers0			= 210,		//�յ��ö��ʻ������ͺ��Ѷ�̬����վ
		string_code_recv_many_flowers1			= 211,		//�յ��ö��ʻ������ͺ��Ѷ�̬����վ
		string_code_recv_many_flowers2			= 212,		//�յ��ö��ʻ������ͺ��Ѷ�̬����վ
		string_code_recv_many_flowers3			= 213,		//�յ��ö��ʻ������ͺ��Ѷ�̬����վ
		string_code_recv_many_flowers4			= 214,		//�յ��ö��ʻ������ͺ��Ѷ�̬����վ


		//��ұ���Ŷ���Ѿ�ռ�õĲ��ᱻɾ�������ģ�
		//��Χ[300-400)��result idΪclient and hall��
		result_id_friend_list_full				= 300,      //�����������,����б�������Ѿ��������ˣ�
		result_id_add_friend_fail				= 301,		//������ʧ�ܣ�
		result_id_message_not_send				= 302,	    //˽��ʧ�ܣ�
		result_id_friend_list_null				= 304,		//û��ȡ������б�
		result_id_add_tag_fail					= 305,		//����ǩʧ�ܣ�
		result_id_delete_friend_fail			= 306,		//ɾ�����ʧ�ܣ�
		result_id_get_stat_tags_fail			= 307,		//û��ȡ�ñ������е�Ta��
		result_id_get_recomm_tags_fail			= 308,		//û��ȡ���Ƽ���ǩ��
		result_id_use_bomb_less_money			= 309,		//ʹ��ը��ʧ�ܣ���Ϸ�Ҳ��������Ͻ�ȥ���ɣ�
		result_id_present_charm_less_money		= 310,		//�ͻ�ʧ�ܣ���Ϸ�Ҳ��������Ͻ�ȥ���ɣ�
		result_id_use_speaker_less_money		= 311,		//ʹ������ʧ�ܣ���Ϸ�Ҳ��������Ͻ�ȥ���ɣ�
		result_id_cumulative_time_one_hour		= 312,		//���ۼ�����ʱ������1Сʱ��
		result_id_cumulative_time_two_hour		= 313,		//���ۼ�����ʱ������2Сʱ��
		result_id_cumulative_time_three_hour	= 314,		//���ۼ�����ʱ������3Сʱ������������Ϣ�����ʵ�������
		result_id_cumulative_time_four_hour		= 315,		//���Ѿ�����ƣ����Ϸʱ�䣬������Ϸ���潫��Ϊ����ֵ��50%��Ϊ�����Ľ������뾡��������Ϣ�����ʵ�������������ѧϰ���
		result_id_cumulative_time_five_hour		= 316,		//���ѽ��벻������Ϸʱ�䣬Ϊ�����Ľ�������������������Ϣ���粻���ߣ��������彫�ܵ��𺦣����������ѽ�Ϊ�㣬ֱ�������ۼ�����ʱ����5Сʱ�󣬲��ָܻ�������
		result_id_add_51_score					= 317,		//��ͨ��51Game���ۻ�����ʱ�䣬���%d��51���֣�
		result_id_custom_group_full				= 318,		//������������·�����,�������Ѿ��������ˣ�
		result_id_make_friend_another_day		= 319,		//��ʾ�û������Ѿ����˹���ĺ����ˣ�������ܼ����Ӻ���
		result_id_add_group_fail				= 320,		//��ӷ���ʧ�ܣ�
		result_id_del_group_fail				= 321,		//ɾ������ʧ�ܣ�
		result_id_move_friend_fail				= 322,		//������Ƶ�ĳ��ָ��������ʧ�ܣ�
		result_id_rename_group_fail				= 323,		//����������ʧ�ܣ�
		result_id_get_groups_fail				= 324,		//��ȡ����ʧ�ܣ�
		result_id_rename_remarks_fail			= 325,		//������鱸עʧ�ܣ�
		result_id_first_login_hall_present_bean = 326,		//��ӭ��51��Ϸ��ϵͳ������%d���ֶ�
		result_id_use_bomb_lock_money			= 327,		//ʹ��ը��ʧ�ܣ���Ϸ�ұ������ˣ�
		result_id_present_charm_lock_money		= 328,		//�ͻ�ʧ�ܣ���Ϸ�ұ������ˣ�
		result_id_use_speaker_lock_money		= 329,		//ʹ������ʧ�ܣ���Ϸ�ұ������ˣ�
		result_id_use_zero_point_less_money		= 330,		//ʹ�ø������㣬��Ϸ�Ҳ���
		result_id_use_zero_point_lock_money		= 331,		//ʹ�ø������㣬��Ϸ������
		result_id_use_small_speaker_success		= 332,		//ʹ��С���ȳɹ�
		result_id_use_zero_point_success		= 333,		//ʹ�ø�������ɹ�
		result_id_refuse_make_friends			= 334,		//�Է��ܾ������
		result_id_refuse_private_chat			= 335,		//�Է��ܾ�����
		result_id_refuse_play_with				= 336,		//�Է��ܾ�һ����
		result_id_flower_present_info_0			= 337,		//�����ʻ��Ĺ㲥��Ϣ
		result_id_flower_present_info_1			= 338,		//�����ʻ��Ĺ㲥��Ϣ
		result_id_flower_present_info_2			= 339,		//�����ʻ��Ĺ㲥��Ϣ
		result_id_flower_present_info_3			= 340,		//�����ʻ��Ĺ㲥��Ϣ
		result_id_flower_present_info_4			= 341,		//�����ʻ��Ĺ㲥��Ϣ
		result_id_anonymous_flower_present_info_0	= 342,		//���������ʻ��Ĺ㲥��Ϣ
		result_id_anonymous_flower_present_info_1	= 343,		//���������ʻ��Ĺ㲥��Ϣ
		result_id_anonymous_flower_present_info_2	= 344,		//���������ʻ��Ĺ㲥��Ϣ
		result_id_anonymous_flower_present_info_3	= 345,		//���������ʻ��Ĺ㲥��Ϣ
		result_id_anonymous_flower_present_info_4	= 346,		//���������ʻ��Ĺ㲥��Ϣ
		result_id_ff_message_box				= 347,		//��������ʾ��
		//��Χ[300-400)��result idΪclient and hall��


		result_id_has_no_compete_game_data		= 400,		//û�б������ֻ��ʸ�
		result_id_compete_already_started		= 401,		//�����Ѿ���ʼ�����ظ�����Ϸ���ڣ�һ������ֻ����һ����Ϸ���ڣ�
		result_id_has_no_such_compete_id		= 402,		//û���������

		//Server�˵ķ��ؽ��ID
		result_id_system						= 501,		//ϵͳ����
		result_id_toomany_openroom				= 502,		//����ķ���̫�࣡
		result_id_invalid_uin					= 503,		//��Ч���û�UIN��
		//��Ϸ�Ҽ���&����
		result_id_already_lock					= 504,      //��Ϸ���Ѿ�������
		result_id_lock_lack_money				= 505,		//��Ϸ�Ҳ��㣡
		result_id_lock_permission_deny			= 506,		//�ܾ���������Ϸ�ʻ����ڱ���״̬��
		result_id_lock_failed					= 507,		//��Ϸ�Ҽ���ʧ�ܣ�
		result_id_unlock_failed					= 508,		//��Ϸ�ҽ���ʧ�ܣ�
		result_id_lock_timeout					= 509,		//����ʧ��:����Ϸ�ҳ�ʱ��

		result_id_consume_many_money			= 510,		//���������Ѷ��Ѿ�������������ޣ�
		result_id_getgamedata_timeout			= 511,		//��ȡ��Ϸ���ݳ�ʱ��
		result_id_updategamedata_timeout		= 512,		//������Ϸ���ݳ�ʱ��
		result_id_updateitemdb_timeout			= 513,		//���µ��߳�ʱ��
		result_id_updategamedb_timeout			= 514,		//������Ϸ���ݳ�ʱ��
		result_id_changemoney_useitem_timeout	= 515,		//�۷ѳ�ʱ,�޷�ʹ�ã������ԣ�
		result_id_freeze_account				= 516,		//�ʺ��ѱ����ᣡ
		result_id_close_service_money_change	= 517,		//������Ϸ��ʧ��,���ѹرո�ҵ��Ľ�Ҹ��£�
		result_id_not_exist_service				= 518,		//�����ڸ���Ч�ķ���
		result_id_lockhappybean_timeout			= 519,		//����ʧ��:�����ֶ���ʱ!������

		result_id_more_than_present_count       = 520,      //��������(���ֶ�)�Ĵ���
		result_id_cost_happybean_round_fare		= 521,		//ÿ����ȡ���ֶ�̨��
		result_id_cost_yxb_round_fare			= 522,		//ÿ����ȡ��Ϸ��̨��
		result_id_lock_happybean_fail			= 523,		//�������ֶ�ʧ��
		result_id_unlock_happybean_fail			= 524,		//�������ֶ�ʧ��
		result_id_enter_room_required_camera	= 525,		//�÷���Ҫ����뿪������ͷ���ܽ���,��ȷ����������ͷ�Ѿ�������
		result_id_enter_room_required_videoauth = 526,		//�÷���ֻ��ͨ����Ƶ��֤�Ĳ��ܽ���,�������������Ƶ��֤
		result_id_enter_room_qun_member_only	= 527,		//
		result_id_happybean_already_lock        = 528,
		result_id_happybean_lack				= 529,
		result_id_not_player_data               = 530,      //ƽ̨�޴��û�����...
		result_id_qun_room_string				= 531,		//����Ⱥ�鷿��ʱ��֪ͨ��Ϣ

		result_id_present_happybean_timeout     = 532,		//���ͻ��ֶ���ʱ
		result_id_in_process_of_present_happybean = 533,	//��gamedb�������ͻ��ֶ��Ĺ�����
		result_id_game_change_51point_timeout	= 534,		//��ϷSO����51�㳬ʱ
		result_id_get_iteminfo_from_itemdb_timeout = 535,	//��item db��ȡ������Ϣ��ʱ
		result_id_game_purchase_goods_from_market_timeout = 536,	//��market���������Ϣ��ʱ


		//����Э���е�����ԭ��
		result_id_kick_player_reason_start		= 700,		//�����ʺ��������ط���¼�������������Ѿ���������Ͽ����ӣ���ע���Լ����ʺŰ�ȫ��
		result_id_kick_player_multilogin		= result_id_kick_player_reason_start,		//�ظ���½
		result_id_kick_player_system_halt		= 701,		//ϵͳͣ��ά��,������ʱ�뿪ƽ̨��
		result_id_kick_player_dull				= 702,		//̫��ʱ�䲻׼����ʼ��Ϸ,ϵͳ������ʱ�뿪��
		result_id_kick_player_gameover			= 703,		//������Ϸ����,ϵͳ������Ϸ��,��ʱ�����뿪��
		result_id_kick_player_by_other			= 704,		//���������뿪��
		result_id_kick_player_observed_leave	= 705,		//�����Թ۵�����Ѿ��뿪��
		result_id_kick_player_off				= 706,		//���������ң�
		result_id_kick_player_not_enough_money	= 707,		//��Ϸ�Ҳ���,ϵͳ������ʱ�뿪��
		result_id_kick_player_lock_money_fail	= 708,		//����51��ʧ�ܣ�ϵͳ������ʱ�뿪!
		result_id_kick_player_not_enough_happybean = 709,	//���ź���������û���㹻�Ļ��ֶ����޷�����������Ϸ��������ֵ�������Ϸ
		result_id_kick_player_lock_happybean_fail = 710,	//�޷��������ֶ�
		result_id_kick_player_for_by_other		= 711,		//���������뿪��
		result_id_kick_player_toomuch_score     = 712,		//���ֹ���
		result_id_kick_player_toomuch_happybean = 713,		//���ֶ�����
		result_id_kick_player_toomuch_51dian    = 714,		//51�����
		result_id_kick_player_unknown			= 715,		//ϵͳ������ʱ�뿪
		result_id_kick_player_not_enough_score  = 716,		//���ֲ���
		result_id_kick_player_system_clear		= 717,		//ϵͳ������Ϸ��

		//720��ǰ��id���ܱ�ʹ�ã���Ϊ����
		result_id_kick_player_be_killed			= 720,		//���ѱ�ϵͳ�߳�51��Ϸ,��δ��%dСʱ�������޷���¼��
		result_id_kick_player_be_freezed		= 721,      //�����ʺ��ѱ�����, ����%s���⣡
		result_id_kick_player_old_version		= 722,		//���İ汾����,����°汾�����µ�¼��
		result_id_present_happybean_first_play  = 723,		//��һ�����ͻ��ֶ�
		result_id_present_happybean_daily		= 724,		//ÿ�����ͻ��ֶ�

		//������ֵ�û����뷿��֪ͨ
		result_id_high_charming_notify			= 750,		//������ֵ��ҽ���֪ͨ��ʼID
		//...................����32��ID��������IDΪ782

		result_id_player_charge_money_timeout   = 800,		//��ֵʧ��: ��ʱ
		result_id_player_exchange_money_timeout = 801,		//�ض�ʧ��: ��ʱ
		result_id_player_search_money_timeout	= 802,		//��ѯ��Ϸ��ʧ��: ��ʱ
		result_id_player_charge_money_error		= 803,		//��ֵʧ��
		result_id_player_exchange_money_error	= 804,      //�ض�ʧ��
		result_id_player_search_money_error		= 805,      //��ѯʧ��

		result_id_player_update_card_id_timeout = 810,		//�������֤ʧ��: ��ʱ
		result_id_player_get_user_info_timeout	= 811,		//�õ��û���Ϣʧ��: ��ʱ
		result_id_player_update_card_id_error   = 812,		//�������֤ʧ��
		result_id_player_get_user_info_error	= 813,		//�õ��û���Ϣʧ��
		result_id_player_get_card_id_timeout	= 814,		//����û����֤��ʱ
		result_id_player_gm_punish_timeout				= 820,  //���·��˺�ʧ��: ��ʱ
		result_id_player_relay_system_message_timeout	= 821,  //��תϵͳ��Ϣʧ��: ��ʱ
		result_id_player_submit_system_message_timeout	= 822,	//�ύϵͳ��Ϣʧ�ܣ���ʱ
		result_id_player_cancel_system_message_timeout	= 823,	//ȡ��Ϣϵͳʧ�ܣ���ʱ

		result_id_player_gm_punish_error				= 825,  //���·��˺�ʧ��
		result_id_player_relay_system_message_error		= 826,  //��תϵͳ��Ϣʧ��
		result_id_player_submit_system_message_error	= 827,	//�ύϵͳ��Ϣʧ��
		result_id_player_cancel_system_message_error	= 828,	//ȡ��Ϣϵͳʧ��

		result_id_player_gm_get_detail_info_error		= 829,  //����û���ϸ��Ϣʧ��
		result_id_player_gm_get_detail_info_timeout		= 830,  //����û���ϸ��Ϣʧ��: ��ʱ

		result_id_gm_query_platform_lock_money_error	= 831,  //��ѯƽ̨��Ϸ��״̬����
		result_id_gm_query_platform_lock_money_timeout  = 832,  //��ѯƽ̨��Ϸ��״̬����: ��ʱ

		result_id_gm_update_platform_lock_money_error	= 833,  //����ƽ̨��Ϸ��״̬����
		result_id_gm_update_platform_lock_money_timeout = 834,  //����ƽ̨��Ϸ��״̬����: ��ʱ

		result_id_change_money_error					= 835,	//�޸���Ϸ�ҳ���
		result_id_change_money_money_timeout			= 836,	//�޸���Ϸ�ҳ�ʱ

		result_id_player_unlock_money_error				= 837,	//�޸������Ϸ��״̬����
		result_id_player_unlock_money_timeout			= 838,	//�޸������Ϸ��״̬��ʱ

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

		result_id_gm_change_happy_bean_error		    = 851, //�޸���һ��ֶ�����
		result_id_gm_change_happy_bean_timeout			= 852, //�޸���һ��ֶ�����

		result_id_gm_lock_happy_bean_error		        = 853, //����&������һ��ֶ�����
		result_id_gm_lock_happy_bean_timeout			= 854, //����&������һ��ֶ���ʱ

		result_id_gm_get_player_all_game_data_error     = 855, //����&������һ��ֶ�����
		result_id_gm_get_player_all_game_data_timeout	= 856, //����&������һ��ֶ���ʱ

		result_id_gm_get_player_item_data_error         = 857, //��ȡ��ҵ��߳���
		result_id_gm_get_player_item_data_timeout	    = 858, //��ȡ��ҵ��߳�ʱ

		result_id_gm_get_player_state_error             = 859, //��ȡ��ҵ��߳���
		result_id_gm_get_player_state_timeout	        = 860, //��ȡ��ҵ��߳�ʱ

		result_id_gm_get_all_player_state_error         = 861, //��ȡ��ҵ��߳���
		result_id_gm_get_all_player_state_timeout	    = 862, //��ȡ��ҵ��߳�ʱ

		result_id_player_cancel_message_error	        = 863, //ɾ��Ϣϵͳʧ�ܣ�����
		result_id_player_cancel_message_timeout	        = 864, //ɾ��Ϣϵͳʧ�ܣ���ʱ

		result_id_gm_send_system_message_to_player_error	        = 865, //����ϵͳ��Ϣʧ�ܣ�����
		result_id_gm_send_system_message_to_player_timeout	        = 866, //����ϵͳ��Ϣʧ�ܣ���ʱ

		result_id_gm_register_game_service_error	        = 867, //ע����Ϸ����ʧ�ܣ�����
		result_id_gm_register_game_service_timeout	        = 868, //ע����Ϸ����ʧ�ܣ���ʱ

		result_id_gm_update_vip_data_error              = 869, //gm�޸��û�vip��Ϣʧ�ܣ�����
		result_id_gm_update_vip_data_timeout            = 870, //gm�޸��û�vip��Ϣʧ�ܣ���ʱ

		//��ұ���Ŷ��
		/*************��Χ[900-1000]��result idΪclient��Logicser֮��ͨѶʹ�� Start*************/
		result_id_enter_charm_room_with_charm_passport  = 900,	//����������ʹ������ͨ��֤
		result_id_enter_charm_room_fail  = 901,					//������������ʧ��
		result_id_kick_player_low_vip_level = 902,				//�Է�Ҳ��VIP�û�������VIP�ȼ�̫��

		result_id_enter_compete_room_not_competitor		= 903,		//�����������ʧ�ܣ����ǲ���ѡ�֣�
		result_id_enter_compete_room_not_right_time		= 904,		//�����������ʧ�ܣ����ǿ�ͨʱ��

		result_id_use_zero_point_success_by_update_itemdb	= 905,	//ͨ�����µ��ߣ�ʹ�ø�������ɹ�
		result_id_use_zero_point_success_by_change_money	= 906,	//ͨ���۳�51�㣬ʹ�ø�������ɹ�

		result_id_use_small_speaker_success_by_update_itemdb = 907,	//ͨ�����µ��ߣ�ʹ��С���ȳɹ�
		result_id_use_small_speaker_success_by_change_money = 908,	//ͨ���۳�51�㣬ʹ��С���ȳɹ�

		result_id_cant_kickplayer_by_object_not_on_table	= 909,	//���ߵ�����Ѿ��뿪��Ϸ 

		result_id_be_grabed_in_room_by_vip_player			= 910,	//��VIP��Ҽ�������
		result_id_vip_enter_room_is_full					= 911,	//VIP�û�����������ʧ��

		//����Э���е�����ԭ����չ��
		result_id_kick_player_reason_start_ext = 980,	
		result_id_kick_player_for_vip_player = result_id_kick_player_reason_start_ext,	//VIP�������
		result_id_kick_player_for_high_level_vip_player	= 981,		//��VIP�ȼ��������
		result_id_kick_player_for_master_vip_player	= 982,			//ͬ�ȼ�VIP�����������


		/*************��Χ[900-1000]��result idΪclient��Logicser֮��ͨѶʹ�� End*************/
		//add by liujj@2009-09-22===>begin
		result_id_compete_not_in_progress			= 1001,		//����δ��ʼ
		result_id_getcompetedata_timeout			= 1002,		//��ȡ��Ϸ���ݳ�ʱ��
		result_id_open_so_many_compete				= 1003,		//����ͬʱ�μ�̫�������������ô�������������?
		result_id_update_competedata_timeout		= 1004,	
		result_id_enter_mutil_rooms_in_one_compete	= 1005,		//ͬһ��������ֻ�ܽ���һ������
		result_id_only_gm_can_kick_in_compete_room	= 1006,		//ֻ��GM�����ڱ�����������
		result_id_not_in_white_list					= 1007,		//�����ڱ���ѡ�ֵİ������У����ܲμӱ���
		result_id_in_black_list						= 1008,		//���ڱ���ѡ�ֵĺ������У����ܲμӱ���
		result_id_player_and_condition_not_satisfy_one		= 1009,		//�������������ֶ�&&51��&&����Ǽǣ���һ�֣�û�дﵽ����Ҫ��
		result_id_player_and_condition_not_satisfy_two		= 1010,		//�������������ֶ�&&51��&&����Ǽǣ������֣�û�дﵽ����Ҫ��
		result_id_player_and_condition_not_satisfy_three	= 1011,		//�������������ֶ�&&51��&&����Ǽǣ������֣�û�дﵽ����Ҫ��

		result_id_player_or_condition_not_satisfy_one		= 1012,		//�������������ֶ�||51��||����Ǽǣ���һ�֣�û�дﵽ����Ҫ��
		result_id_player_or_condition_not_satisfy_two		= 1013,		//�������������ֶ�||51��||����Ǽǣ������֣�û�дﵽ����Ҫ��
		result_id_player_or_condition_not_satisfy_three		= 1014,		//�������������ֶ�||51��||����Ǽǣ������֣�û�дﵽ����Ҫ��

		result_id_player_not_satisfy_happybean		= 1015,		//����ӵ�л��ֶ�
		result_id_player_not_satisfy_51point		= 1016,		//����ӵ��51��
		result_id_player_not_satisfy_vip			= 1017,		//�����Ϊ����
		result_id_player_not_satisfy_middle_string	= 1018,		//��ҽ�������������Ի�����м��ʽ�����ַ���

		result_id_player_get_happybean				= 1019,		//��û��ֶ���ť�Լ�����
		result_id_player_get_51point				= 1020,		//���51�㰴ť�Լ�����
		result_id_player_become_vip					= 1021,		//��Ϊ���갴ť�Լ�����
		result_id_player_not_satisfy_end_string		= 1022,		//��ҽ�������������Ի����ĩβ��ʽ�����ַ���

		result_id_player_canot_talk					= 1023,		//���������ڣ���ֹ�κ���ʽ�����죡
		result_id_compete_end_canot_sitdown			= 1024,		//�����Ѿ��������޷�����
		result_id_compete_room_item_not_allowed		= 1025,		//�����Ѿ��������޷�����		

		//Ԥ��1030-1049�������˳���Э��,��ұ���Ŷ��
		result_id_kick_player_from_room_reason_start	= 1030, //��ͨ������Э����ָ���˳����������������˳�����Э��
		result_id_kick_player_from_room_compete_clear	= 1031, //��ͨ������Э����ָ���˳����������������˳�����Э��
		reason_id_kick_player_from_room_not_enough_happybean	= 1032,//�����ֶ����㣬���˳���
		reason_id_kick_player_from_room_not_enough_51point		= 1033,//��51�㲻�㣬���˳���
		reason_id_kick_player_from_room_not_enough_both			= 1034,//�����ֶ���51�㲻�㣬���˳���
		//add by liujj@2009-09-22===>end


		result_id_close_chat							= 1040,      //�ر�����
		result_id_mutil_login_player					= 1041,

		result_id_chat_room_full						= 1042,		//��������
		result_id_invalid_chat_room_id					= 1043,		//�Ƿ�������
		result_id_add_chat_room_error					= 1044,		//����������ʧ��
		max_result_id,					//���result id. ��֤���result id���ܳ���2048����
	};
}//namespace KingNet

namespace KingNet { namespace Server {
	enum
	{
		connector_head_base_size = 7,//sizeof(int32_t)+sizeof(int8_t)+sizeof(int16_t)

		max_client_pkg_size = 5120,		//�ͻ��˷��͹����İ�������С

		max_relay_pkg_size = 4096,		//�ͻ��˸�RelayServer���͹�����������ݰ���С
	};

	//CSЭ�����ݰ�����
	enum
	{
		max_cs_package_size = 0x3ffff,			/**< cs������󳤶� 256K */

		//���Э���峤��
		max_cs_package_body_size = (max_cs_package_size - MIN_CS_HEAD_LENGHT - MAX_CS_HEAD_OPTION_LENGTH), 
		max_cs_package_total_size = (max_cs_package_size + connector_head_base_size),
	};

	enum
	{
		max_cs_channel_size = 0x2000000,		//cs channel size 32MB
		max_udp_channel_size = 0x10000000,		//udp channel size 256MB
	};


	//web �õĶ���
	enum
	{
		web_connector_head_base_size = 11,//sizeof(int32_t)+sizeof(int8_t)+sizeof(int16_t)

		max_web_client_pkg_size = 5120,		//�ͻ��˷��͹����İ�������С
	};

	//webЭ�����ݰ�����
	enum
	{
		max_web_package_size = 0x3ffff,			/**< cs������󳤶� 256K */

		//���Э���峤��
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

