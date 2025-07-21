/**
 *@author	 
 *@email	 
 *@date		2011-02-26		
 *@brief	�����
 */
#ifndef _AVALON_RANDOM_H_
#define _AVALON_RANDOM_H_

#include <cstdlib>
#include "AvalonDefine.h"

namespace Avalon
{
	/**
	 *@brief �������غ�������
	 */
	class Random
	{
	public:
		/**
		 *@brief �������������
		 */
		static void SRand(UInt32 seed){ srand(seed); }

		/**
		 *@brief ����һ�������
		 */
		static Int32 Rand(){ return rand(); }

		/**
		 *@brief ��[min,max]��Χ������һ�������
		 */
		static Int32 RandBetween(Int32 min,Int32 max)
		{
			if(min > max) std::swap(min,max);

			Int32 val = min + Int32( float(Rand()) / (RAND_MAX + 1.0f) * (max - min + 1));
			if(val < min) return min;
			else if(val > max) return max;
			return val;
		}

		/**
		 *@brief �ٷֱȸ����ж�
		 */
		static bool SelectInHundred(Int32 value){ return RandBetween(1,100) <= value; }

		/**
		 *@brief ǧ�ֱȸ����ж�
		 */
		static bool SelectInThousand(Int32 value){ return RandBetween(1,1000) <= value; }

		/**
		 *@brief ��ֱȸ����ж�
		 */
		static bool SelectInTenThousand(Int32 value){ return RandBetween(1,10000) <= value; }

		/**
		 *@brief ʮ��ֱȸ����ж�
		 **/
		static bool SelectInHundredThousand(Int32 value){ return RandBetween(1,100000) <= value; }
	};
}

#endif
