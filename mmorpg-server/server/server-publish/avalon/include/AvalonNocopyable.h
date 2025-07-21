/**
 *@author	 
 *@email	 
 *@date		2011-02-25	
 *@brief	定义了Nocopyable基类，继承此类不能被拷贝
 */
#ifndef _AVALON_NOCOPYABLE_H_
#define _AVALON_NOCOPYABLE_H_

namespace Avalon
{
	/**
	 *@brief 继承此类后对象不能被拷贝
	 */
	class Nocopyable
	{
	protected:
		Nocopyable(){}
		~Nocopyable(){}

	private:
		Nocopyable(const Nocopyable&);
		const Nocopyable& operator=(const Nocopyable&);
	};
}

#endif
