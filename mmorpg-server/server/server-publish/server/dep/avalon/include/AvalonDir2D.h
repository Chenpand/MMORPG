/**
 *@author	����
 *@email	huanglie1986@gmail.com
 *@date		2011-04-25		
 *@brief	2D�������
 */
#ifndef _AVALON_DIR2D_H_
#define _AVALON_DIR2D_H_

#include <AvalonRandom.h>

namespace Avalon
{
	/**
	 *@brief ���򣬺Ϳͻ��˼���
	 */
	enum Dir2D
	{
		DIR_UP = 8,			//��
		DIR_DOWN = 2,		//��
		DIR_LEFT = 4,		//��
		DIR_RIGHT = 6,		//��
		DIR_LEFTUP = 7,		//����
		DIR_LEFTDOWN = 1,	//����
		DIR_RIGHTUP = 9,	//����
		DIR_RIGHTDOWN = 3,	//����
	};

	/**
	 *@brief ��ȡָ�������ƫ����
	 */
	inline void GetOffsetInDir(Int32 dir, int& offsetX, int& offsetY)
	{
		switch(dir)
		{
		case DIR_UP:
			offsetX = 0;
			offsetY = -1;
			break;
		case DIR_DOWN:
			offsetX = 0;
			offsetY = 1;
			break;
		case DIR_LEFT:
			offsetX = -1;
			offsetY = 0;
			break;
		case DIR_RIGHT:
			offsetX = 1;
			offsetY = 0;
			break;
		case DIR_LEFTUP:
			offsetX = -1;
			offsetY = -1;
			break;
		case DIR_LEFTDOWN:
			offsetX = -1;
			offsetY = 1;
			break;
		case DIR_RIGHTUP:
			offsetX = 1;
			offsetY = -1;
			break;
		case DIR_RIGHTDOWN:
			offsetX = 1;
			offsetY = 1;
			break;
		}
	}

	/**
	 *@brief ��ȡָ������ķ�����
	 */
	inline Int32 GetReverseDir(Int32 dir)
	{
		switch(dir)
		{
		case DIR_UP:	return DIR_DOWN;
		case DIR_DOWN:	return DIR_UP;
		case DIR_LEFT:	return DIR_RIGHT;
		case DIR_RIGHT:	return DIR_LEFT;
		case DIR_LEFTUP:	return DIR_RIGHTDOWN;
		case DIR_RIGHTDOWN:	return DIR_LEFTUP;
		case DIR_LEFTDOWN:	return DIR_RIGHTUP;
		case DIR_RIGHTUP:
		default:		return DIR_LEFTDOWN;
		}
	}

	/**
	 *@brief ��ȡһ���������
	 */
	inline Int32 GetRandDir()
	{
		Int32 i = Avalon::Random::RandBetween(1,8);
		if(i >= 5) ++i;
		return i;
	}
}

#endif
