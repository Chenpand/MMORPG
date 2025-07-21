#ifndef __GAMEPLATFORM_PUBLIC_H__
#define __GAMEPLATFORM_PUBLIC_H__

#include "gameplatform_types.h"

namespace KingNet{

enum
{
	max_player_name_length = 128,			/**< ����û����ֳ���*/
	max_player_id_length = 32,				/**<  �û��ַ������ʺŵĳ���*/

	max_face_version_length = 32,		/* �û�ͷ��汾�ų���*/
	max_face_url_length  = 256,			/* �û�ͷ���URL����*/
	max_user_zone_info_length = 40,		/* �û�������Ϣ����*/
	max_user_nick_name_length = 32,		/* �û��ǳƵĳ���*/

	max_url_length = 64,				//URL��ַ�ĳ���
};

enum sex_type {
	sex_type_no_type = 0,          //������
	sex_type_male = 1,		 	   //��
	sex_type_female = 2,		   //Ů
	sex_type_shemale = 3,		   //����
};

} //namespace KingNet{


#endif
