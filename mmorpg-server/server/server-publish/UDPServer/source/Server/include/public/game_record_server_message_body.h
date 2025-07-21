#ifndef __GAME_RECORD_SERVER_MESSAGE_BODY_H__
#define __GAME_RECORD_SERVER_MESSAGE_BODY_H__

#include "server_message.h"
//
namespace KingNet { namespace Server {

	enum
	{
		max_game_record_data_size	= (int32_t)0xA000, // 40KByte
	};

class CRequestRecordGameRound : public CMessageBody
{
public:
	CRequestRecordGameRound();
	~CRequestRecordGameRound();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int16_t m_nGameID;
	char m_szGameTag[max_game_tag_length];
	int32_t m_iGameRecordSize;
	char m_szGameRecordData[max_game_record_data_size];
protected:
private:
};


}}//namespace KingNet { namespace Server {

#endif // __GAME_RECORD_SERVER_MESSAGE_BODY_H__

