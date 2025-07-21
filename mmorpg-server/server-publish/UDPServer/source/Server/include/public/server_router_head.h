#ifndef __SERVER_ROUTER_HEAD_H__
#define __SERVER_ROUTER_HEAD_H__
#include "Common/common_base.h"
#include "Common/common_codeengine.h"
namespace KingNet { namespace Server {

#define max_routerhead_length 519 //��ͷ���ĳ���
#define max_option_size 128
#define max_dest_count	126

#define ROUTERSERVER_HEARTBEAT_TIME 30 //����ʱ��

enum
{
	router_head_basic_size = 14,			//router head�Ļ�������
											//sizeof(int32_t) + sizeof(int32_t) + 4* sizeof(int8_t) + sizeof(int16_t)
};
//typedef char int8_t;
//typedef unsigned char uint8_t;

/*���¶���ת������*/
enum enmTransfType
{
    trans_type_null      = -1,          //invalid
	trans_type_p2p       = 0,			//��Ե�
	trans_type_p2g       = 1,			//�鲥
	trans_type_broadcast = 2,   		//�㲥
	trans_type_by_key    = 3,			//���ݹؼ���ת��
};


enum enmRouterOptionFlag
{
	router_option_flag_reserved		= 0x01,				//�����ô˱�ʶ����Ҫ��֤����Ϣ���͵�db
	router_option_flag_command		= 0x02,				//�����ô˱�ʶ,���ʾ����Ϣ'������Ϣ'
};

enum enProxyCmds
{
	COMMAND_NULL	= 0,
	COMMAND_LOGIN	= 1,			//ÿ��server��¼�������ӵ�router,����Ҫ���ʹ���Ϣ�Ա�ʾ���(������Ϣ��)
};

class CRouterHead
{
public:
	CRouterHead();
	~CRouterHead();

 
	/*
	* @method:    size ���㸳ֵ���router head��ʵ�ʳ���
	* @fullname:  KingNet::Server::CRouterHead::size
	* @access:    public 
	* @return:   KingNet::int32_t ����ͷ������(�̶����� + option����)
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t size();

	/*
	* @method:    encode ����
	* @fullname:  CRouterHead::encode
	* @access:    public 
	* @param: char * pszCode ����������code
	* @param: int32_t & iCodeSize ����pszCode����󳤶ȣ�����ʵ��code�ĳ��� 
	* @return:   int 
	* - success: ����ɹ�
	* - ����: ʧ��
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code

	* @endcode

	* @see
	* @deprecated 
	*/
	int32_t encode(char* pszCode, int32_t& iCodeSize);

	/*
	* @method:    decode ����
	* @fullname:  CRouterHead::decode
	* @access:    public 
	* @param: const char * pszCode ����������code
	* @param: const int32_t iCodeSize ������code�ĳ���
	* @return:   int
	* - success : �ɹ�
	* - ����: ʧ��
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code

	* @endcode

	* @see
	* @deprecated 
	*/
	int32_t decode(const char *pszCode, const int32_t iCodeSize,int16_t *pHeadLen=NULL);

	/*
	* @method:    dump ��ͷ����Ϣdump���������ļ���stdout�ȣ�
	* @fullname:  CRouterHead::dump
	* @access:    public 
	* @return:   void
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code

	* @endcode

	* @see
	* @deprecated 
	*/
	void dump();

	//Э����ܳ���
	int32_t m_iTotalSize;
	//Դʵ���ID
	int32_t m_iSourceEntityID;
	//Դʵ������
	int8_t m_cSourceEntityType;
	//Ŀ��ʵ������
	int8_t m_cDestEntityType;
	//ת�����ͣ�ȡֵΪenum enmTransfType
	int8_t m_cTransferType;
	union _U
	{
		int32_t m_iUin;				//m_cTransferType = trans_type_by_key��Ч
		int32_t m_iDestEntityID;	//m_cTransferType = trans_type_p2p ��Ч
		struct _tagSvrs				
		{
			int16_t m_nDestCount;
			int32_t m_aiDestsID[max_dest_count];
		} stSvrs;					//m_cTransferType = trans_type_p2g ��Ч
	} m_stTransferInfo;
	uint8_t m_byOpFlag;				//enmRouterOptionFlag, ��λ��
	int16_t m_nOptLength;			//m_abyOpt�ֶ�ʵ�ʴ�С
	uint8_t m_abyOpt[max_option_size];	//option
	//short m_nBodyLength;
};

}}//namespace KingNet { namespace Server {

#endif //__SERVER_ROUTER_HEAD_H__
