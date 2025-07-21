#ifndef __GAMEPLATFORM_PUBLIC_H__
#define __GAMEPLATFORM_PUBLIC_H__

#include "gameplatform_types.h"

namespace KingNet{

enum
{
	max_player_name_length = 128,			/**< 最大用户名字长度*/
	max_player_id_length = 32,				/**<  用户字符串型帐号的长度*/

	max_face_version_length = 32,		/* 用户头像版本号长度*/
	max_face_url_length  = 256,			/* 用户头像的URL长度*/
	max_user_zone_info_length = 40,		/* 用户地区信息长度*/
	max_user_nick_name_length = 32,		/* 用户昵称的长度*/

	max_url_length = 64,				//URL地址的长度
};

enum sex_type {
	sex_type_no_type = 0,          //无类型
	sex_type_male = 1,		 	   //男
	sex_type_female = 2,		   //女
	sex_type_shemale = 3,		   //人妖
};

} //namespace KingNet{


#endif
