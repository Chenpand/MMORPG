#ifndef __MD5_H__
#define __MD5_H__


//
namespace KingNet { namespace Server { 

#ifdef __cplusplus
extern "C"
{
#endif



#define MD5_DIGEST_LENGTH	16

void Md5HashBuffer( char *pbyOutBuffer, // 16Bytes
		const char *pbyInBuffer,
		int nInBufferLength);

#ifndef HASH_LEN
#define HASH_LEN 16
#endif





#ifdef __cplusplus
}
#endif

}}//namespace KingNet { namespace Server 

#endif /* md5.h */

