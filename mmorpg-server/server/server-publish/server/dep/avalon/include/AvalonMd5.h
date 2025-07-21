/**
 *@author	 
 *@email	 
 *@date		2011-03-12	
 *@brief	md5算法
 */
#ifndef _AVALON_MD5_H_
#define _AVALON_MD5_H_

#include "AvalonDefine.h"

namespace Avalon
{
	/**
	 *@brief md5编码
	 */
	void md5_encode(char [16], void*, unsigned int);

	/**
	 *@brief 32字节表示md5编码
	 */
	void md5_encode32(char [32], void*, unsigned int);

	/**
	 *@brief 转成16进制
	 */
	void md5_tohex(char src[16], char dest[32]);
}

#endif
