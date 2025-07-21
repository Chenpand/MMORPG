/**
 *@author	 
 *@email	 
 *@date		2011-02-25	
 *@brief	������Nocopyable���࣬�̳д��಻�ܱ�����
 */
#ifndef _AVALON_NOCOPYABLE_H_
#define _AVALON_NOCOPYABLE_H_

namespace Avalon
{
	/**
	 *@brief �̳д��������ܱ�����
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
