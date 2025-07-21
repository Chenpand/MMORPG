#ifndef __GAMEPLATFORM_TYPES_H__
#define __GAMEPLATFORM_TYPES_H__

//!!不能放在namespace KingNet名字空间内
#ifdef __GNUC__ //gnu gcc/g++
#include <stdint.h>
#include <inttypes.h>
#endif // __GNUC__

namespace KingNet{

#if defined(_MSC_VER) //microsoft vc studio 
	#if defined(WIN32) || defined(_WIN32)
	
		typedef char				int8_t;
		typedef unsigned char		uint8_t;
		typedef short				int16_t;
		typedef unsigned short		uint16_t;
		typedef int					int32_t;
		typedef unsigned int		uint32_t;
		typedef long long			int64_t;
		typedef unsigned long long	uint64_t;
	#else if defined(_WIN64)
		typedef char				int8_t;
		typedef unsigned char		uint8_t;
		typedef short				int16_t;
		typedef unsigned short		uint16_t;
		typedef int					int32_t;
		typedef unsigned int		uint32_t;
		typedef long 				int64_t;
		typedef unsigned long		uint64_t;
	#endif
#endif	//_MSC_VER


	enum
	{
		UID_Length = 62,
		Nick_Length = 62,
		FaceURL_Length = 200,
		Uesr_Face_Version_Length= 200,
		Password_Length = 62,
		Invalid_UIN = -1,
		Invalid_ItemID = -1,
		Invalid_GoodsID = -1,
		ZoneInfo_Length = 40,
		GoodName_Length = 32,
		MD5HashCode_Size = 16,
		Invalid_GoodID = -1,	
	};

	typedef int32_t UIN;
	typedef int8_t Age;
	typedef int32_t GoodID;		//数字账号
	typedef uint32_t UID;

	typedef int32_t ServerID;
	typedef int16_t GameID;
	typedef int32_t RoomID;
	typedef int16_t SectionID;


	typedef int32_t TableID;
	typedef int8_t  SeatID;
	typedef int32_t PlayerID;
	typedef int8_t  PlayerStatus;

	typedef enum enmStandUpMode
	{
		enmStandUpMode_Normal = 0x00,
		enmStandUpMode_Force = 0x01
	}StandUpMode;

	typedef enum enmGender
	{
		enmGender_Unknown = 0,
		enmGender_Male    = 1,
		enmGender_Female  = 2,
	} Gender;

	typedef enum enmHealthStatus
	{
		enmHealthStatus_Normal	 = 0, //健康状态
		enmHealthStatus_Tired	 = 1, //疲劳状态
		enmHealthStatus_AbNormal = 2, //非健康状态
	}HealthStatus;

	typedef int32_t ItemID;
	typedef int32_t GoodsID;

	typedef int16_t ItemType;

#define  MESSAGE_CMD_GENDER_UNKNOW	0x00
#define  MESSAGE_CMD_GENDER_MALE	0x01
#define  MESSAGE_CMD_GENDER_FEMALE	0x02
}



#endif	// __PORTABLE_H__

