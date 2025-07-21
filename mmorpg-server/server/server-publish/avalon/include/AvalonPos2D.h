#ifndef _AVALON_POS2D_H_
#define _AVALON_POS2D_H_

/**
 *@author	 
 *@email	 
 *@date		2011-04-25		
 *@brief	2Dλ�����
 */
#include <AvalonNetStream.h>
#include <algorithm>
#include <limits>
#include "AvalonDir2D.h"

#undef max
#undef min

namespace Avalon
{
	/**
	 *@brief λ��
	 */
	class Pos2D
	{
	public:
		Pos2D():x(0),y(0){}
		Pos2D(UInt16 _x,UInt16 _y):x(_x),y(_y){}
		Pos2D(const Pos2D& pos):x(pos.x),y(pos.y){}
		~Pos2D(){}

		const Pos2D& operator=(const Pos2D& pos)
		{
			if(this == &pos) return *this;
			x = pos.x;
			y = pos.y;
			return *this;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & x & y;
		}

		bool operator==(const Pos2D& pos) const
		{
			return x == pos.x && y == pos.y;
		}

		bool operator!=(const Pos2D& pos) const
		{
			return x != pos.x || y != pos.y;
		}

		/**
		 *@brief ��������λ��֮��Ľ��ƾ���
		 */
		UInt16 Distance(const Pos2D& pos) const
		{
			Int32 distx = std::abs((Int32)pos.x - (Int32)x);
			Int32 disty = std::abs((Int32)pos.y - (Int32)y);
			return UInt16(std::max<Int32>(distx,disty));
		}

		/**
		 *@brief ����Ŀ��㣬���㵽Ŀ���ķ���
		 */
		Int32 GetDirTo(const Pos2D& pos) const;

		/**
		 *@brief ���ݷ��򣬼��㷽���ϵ���һ��
		 */
		bool GetNextPosInDir(Int32 dir, Pos2D& pos) const;

		/**
		 *@brief �ж�Ŀ����Ƿ���ָ��������
		 */
		bool CheckIfInDir(const Pos2D& pos, Int32 dir) const;

		/**
		 *@brief �ж�Ŀ����Ƿ����ƶ������90��������
		 */
		bool CheckIfInSector(const Pos2D& pos, Int32 dir) const;

		/**
		 *@brief ���ַ������ת��
		 */
		void FromString(const std::string& str)
		{
			int x = 0,y = 0;
			avalon_sscanf(str.c_str(),"%d,%d",&x,&y);
			this->x = UInt16(x);
			this->y = UInt16(y);
		}
		std::string ToString() const
		{
			char buf[32];
			int x = this->x;
			int y = this->y;
			avalon_snprintf(buf,31,"%d,%d",x,y);
			return buf;
		}

		/**
		 *@brief �����ӿ�
		 */
		void SetX(UInt16 _x){ x = _x; }
		UInt16 GetX() const{ return x; }

		void SetY(UInt16 _y){ y = _y; }
		UInt16 GetY() const{ return y; }

	public:
		//x
		UInt16 x;
		//y
		UInt16 y;
	};
}

#endif
