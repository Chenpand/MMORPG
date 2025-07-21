/**
 *@author	 
 *@email	 
 *@date		2011-02-26		
 *@brief	随机数
 */
#ifndef _AVALON_RANDOM_H_
#define _AVALON_RANDOM_H_

#include <cstdlib>
#include "AvalonDefine.h"

namespace Avalon
{
	/**
	 *@brief 随机数相关函数定义
	 */
	class Random
	{
	public:
		/**
		 *@brief 设置随机数种子
		 */
		static void SRand(UInt32 seed){ srand(seed); }

		/**
		 *@brief 生成一个随机数
		 */
		static Int32 Rand(){ return rand(); }

		/**
		 *@brief 在[min,max]范围内生成一个随机数
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
		 *@brief 百分比概率判断
		 */
		static bool SelectInHundred(Int32 value){ return RandBetween(1,100) <= value; }

		/**
		 *@brief 千分比概率判断
		 */
		static bool SelectInThousand(Int32 value){ return RandBetween(1,1000) <= value; }

		/**
		 *@brief 万分比概率判断
		 */
		static bool SelectInTenThousand(Int32 value){ return RandBetween(1,10000) <= value; }

		/**
		 *@brief 十万分比概率判断
		 **/
		static bool SelectInHundredThousand(Int32 value){ return RandBetween(1,100000) <= value; }
	};
}

#endif
