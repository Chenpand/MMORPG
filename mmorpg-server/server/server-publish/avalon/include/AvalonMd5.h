/**
 *@author	 
 *@email	 
 *@date		2011-03-12	
 *@brief	md5�㷨
 */
#ifndef _AVALON_MD5_H_
#define _AVALON_MD5_H_

#include "AvalonDefine.h"

namespace Avalon
{
	/**
	 *@brief md5����
	 */
	void md5_encode(char [16], void*, unsigned int);

	/**
	 *@brief 32�ֽڱ�ʾmd5����
	 */
	void md5_encode32(char [32], void*, unsigned int);

	/**
	 *@brief ת��16����
	 */
	void md5_tohex(char src[16], char dest[32]);
}

#endif
