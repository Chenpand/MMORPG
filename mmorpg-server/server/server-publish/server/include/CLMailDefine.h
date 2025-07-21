#ifndef _CL_MAIL_DEFINE_H_
#define _CL_MAIL_DEFINE_H_

#include "CLDefine.h"
#include "CLErrorCode.h"
#include "CLUtf8Define.h"

//modified by huchenhui
//��󸽼���
const static UInt32	MAX_MAILITEM_NUM = 4;
//�ʼ�����
const static UInt32 MAIL_COST = 50;
//���⺺����
const static Int32	MAX_TITLE_SHOW_LEN = 30;
//���ݺ�����
const static Int32	MAX_CONTENT_SHOW_LEN = 800; //400 * 2
//�����˺�����
const static UInt32 MAX_SENDER_NAME_LEN = 30; //15 * 2

//�ʼ������Ч����
const static UInt32 MAIL_VALID_DAY = 30;

//�ʼ������Ч��
const static UInt32 MAIL_VALID_TIME = (30 * DAY_TO_SEC);

//һ����ȡһ�������ȡ�ʼ���
const static UInt32 MAX_ONE_KEY_NUM = 100;

/**
 *@brief �ʼ�����
 */
enum MailType
{
	MAIL_TYPE_SYSTEM = 0,	//ϵͳ�ʼ�
	MAIL_TYPE_NORMAL = 1,	//��ͨ�ʼ�
	MAIL_TYPE_GM = 2,		//GM�ʼ�
	MAIL_TYPE_GUILD = 3,	//�����ʼ�
};


/**
*@brief ����ַ��� by huchenhui
*/
inline UInt32 CheckMailStr(const std::string& str, UInt32 length, UInt32 width)
{
	if (str.empty() || str.length() >= length)
	{
		InfoLogStream << "CheckMailStr : str = " << str.c_str() << " str len = " << (UInt32)str.length() << " len = " << length << ", width:" << width << LogStream::eos;
		return ErrorCode::MAIL_TITLE_LENGTH_ERROR;
	}
	UInt32 strWidth = (UInt32)GetUtf8ShowWidth(str.c_str());
	if (strWidth <= 0 || strWidth > width)
	{
		InfoLogStream << "CheckMailStr : str = " << str.c_str() << " str strWidth = " << strWidth << " len = " << length << ", width:" << width << LogStream::eos;
		return ErrorCode::MAIL_TITLE_LENGTH_ERROR;
	}
	return ErrorCode::SUCCESS;
};

/**
*@brief ����ʼ����������(�ַ���������UTF8����,Ӧ����ȷ��, ���Ի�����ʱʹ�ÿ�����Ӣ��) by huchenhui
*/
inline UInt32 CheckMail(ObjID_t tReceiverId, const std::string& title, const std::string& content, UInt32 itemSize, const std::string& senderName)
{
	if (itemSize > MAX_MAILITEM_NUM)
	{
		GameErrorLogStream("MailBox") << "ReceiverId(" << tReceiverId << ") SendSysMail:Rewards size = " << itemSize << " too many" << LogStream::eos;
		return ErrorCode::MAIL_REWARD_ERROR;
	}

	UInt32 res = CheckMailStr(title, 100, MAX_TITLE_SHOW_LEN);
	if (res != ErrorCode::SUCCESS)
	{
		GameErrorLogStream("MailBox") << "ReceiverId(" << tReceiverId << ") SendSysMail: title(" << title << ") size error" << LogStream::eos;
		return res;
	}

	if (!content.empty())
	{
		res = CheckMailStr(content, 2000, MAX_CONTENT_SHOW_LEN);
		if (res != ErrorCode::SUCCESS)
		{
			GameErrorLogStream("MailBox") << "ReceiverId(" << tReceiverId << ") SendSysMail: content(" << content << ") size error" << LogStream::eos;
			return res;
		}
	}

	res = CheckMailStr(senderName, 100, MAX_SENDER_NAME_LEN);
	if (res != ErrorCode::SUCCESS)
	{
		GameErrorLogStream("MailBox") << "ReceiverId(" << tReceiverId << ") SendSysMail: sender name(" << senderName << ") size error" << LogStream::eos;
		return res;
	}
	return ErrorCode::SUCCESS;
};


/**
*@brief ����ʼ���������� by huchenhui
*/
inline UInt32 CheckMail(ObjID_t tReceiverId, const char* title, const char* content, UInt32 itemSize, const char* senderName)
{
	if (title == NULL)
	{
		GameErrorLogStream("MailBox") << "ReceiverId(" << tReceiverId << ") SendSysMail: title is NULL" << LogStream::eos;
		return ErrorCode::MAIL_TITLE_ERROR;
	}
	if (senderName == NULL)
	{
		GameErrorLogStream("MailBox") << "ReceiverId(" << tReceiverId << ") SendSysMail: sender name NULL" << LogStream::eos;
		return ErrorCode::MAIL_SENDER_NAME_ERROR;
	}

	std::string titleStr = std::string(title);
	std::string senderNameStr = std::string(senderName);
	std::string contentStr;
	if (content != NULL)
	{
		contentStr = std::string(content);
	}
	return CheckMail(tReceiverId, titleStr, contentStr, itemSize, senderNameStr);
};

/**
 *@brief �ʼ�������Ϣ
 */
struct MailTitleInfo
{
	MailTitleInfo() :id(0), date(0), deadline(0), type(0), replyTimes(0), status(0), hasitem(0), itemId(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		//stream & id & sender & date & type & replyTimes & status & hasitem & title & itemId;
		stream & id & sender & date & deadline & type & status & hasitem & title & itemId;
	}

	//�ʼ�id
	ObjID_t		id;
	//������
	std::string	sender;
	//��������
	UInt32		date;
	//��������
	UInt32		deadline;
	//����	MailType
	UInt8		type;
	//�ظ�����
	UInt8		replyTimes;
	//״̬	0δ�� 1�Ѷ�
	UInt8		status;
	//�Ƿ��и��� 0û�� 1��
	UInt8		hasitem;
	//����
	std::string	title;
	//��Ʒ����ID,���ڿͻ�����ʾͼ��
	UInt32 itemId;
};

#endif
