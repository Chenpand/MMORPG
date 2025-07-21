#ifndef _CL_SYNC_OBJECT_H_
#define _CL_SYNC_OBJECT_H_

#include <AvalonNetStream.h>
#include "CLObject.h"
//#include "CLOwnerObjectProperty.h"
#include "CLRecordCommand.h"
#include "CLRecordCallback.h"

//class CLOwneredObjPropertyBase;
class CLOwnerObjPropertyBase;

/**
 *@brief 数据库同步类型
 */
#define	DB_SYNC_TYPE	(1 << 30)

/**
 *@brief 同步对象
 */
class CLSyncObject : public CLObject
{
public:
	CLSyncObject(){}
	virtual ~CLSyncObject(){}

public:
	/**
	*@brief 打包所有属性，不管是否脏， 是否是默认值
	*/
	bool EncodeAllField(Avalon::NetOutputStream& stream, UInt32 type)
	{
		size_t oldpos = stream.GetPos();
		EncodeProperty(stream, type, 3);
		bool ret = (stream.GetPos() != oldpos);
		stream & UInt8(0);
		return ret;
	}

	/**
	 *@brief 打包所有指定属性
	 *@return true有属性 false没有属性
	 */
	bool Encode(Avalon::NetOutputStream& stream,UInt32 type,bool bDirty)
	{
		size_t oldpos = stream.GetPos();
		EncodeProperty(stream,type,(bDirty ? 0 : 1));
		bool ret = (stream.GetPos() != oldpos);
		stream & UInt8(0);
		return ret;
	}

	/**
	 *@brief 打包新同步机制剩余指定属性
	 *@return true有属性 false没有属性
	 */
	bool NewEncode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
	{
		size_t oldpos = stream.GetPos();
		NewEncodeProperty(stream, type, (bDirty ? 0 : 1));
		bool ret = (stream.GetPos() != oldpos);
		stream & UInt8(0);
		return ret;
	}

	/**
	 *@brief 打包所有指定属性                                                                     
	 */
	bool Encode(Avalon::NetOutputStream& stream,UInt32 type)
	{
		size_t oldpos = stream.GetPos();
		EncodeProperty(stream,type,2);
		bool ret = (stream.GetPos() != oldpos);
		stream & UInt8(0);
		return ret;
	}

	/**
	 *@brief 解码
	 *@return true有属性 false没有属性
	 */
	bool Decode(Avalon::NetInputStream& stream)
	{
		UInt8 dataID = 0;
		bool ret = false;
		while(true)
		{
			stream & dataID;
			if(dataID == 0) break;
			if(DecodeProperty(stream,dataID)) ret = true;
		}
		return ret;
	}

	/**
	 *@brief 获取数据库数据
	 */
	virtual void GetDBData(CLSaveCommand* cmd, bool bDirty = true){}
	virtual void SetDBData(CLRecordCallback* callback){}

	virtual void ClearDirty(){}
	

	/**
	 *@brief CLOwnerObjPropertyBase调用，设置自身某字段为脏，需配套CLOwnerObjPropertyBase使用
	 */
	virtual void SetSelfDirty(UInt32 id, CLOwnerObjPropertyBase* ptr) {}

protected:
	virtual void EncodeProperty(Avalon::NetOutputStream& stream,UInt32 type,int encodeType) {}

	virtual bool DecodeProperty(Avalon::NetInputStream& stream,UInt8 dataid) { return false; }

	virtual void SetSyncType(UInt32 id, UInt32 allType) { m_ObjectSyncType[id] = allType; }

	virtual UInt32 GetSyncType(UInt32 id) { return m_ObjectSyncType[id]; }

	virtual void NewEncodeProperty(Avalon::NetOutputStream& stream, UInt32 type, int encodeType) {}

private:
	//字段id -> 同步类型
	HashMap<UInt32, UInt32> m_ObjectSyncType;
};

/**
 *@brief 定义同步属性，该宏需要放在构造或初始化函数里
 */
#define REGISTER_SYNC_PROPERTY(Data, Id, Type) \
	Data.Init(this, Id); \
	//SetSyncType(Id, Type);

/**
 *@brief 定义属性编码
 */
#define CL_ENCODE_PROPERTY(Data, Id, Type) \
	if((Type) & type){ \
		if(encodeType == 0) { \
			if(Data.IsDirty(type)){\
				outstream & UInt8(Id);\
				Data.Encode(outstream,type,true); \
				Data.ClearDirty(type);\
			}\
		}else if(encodeType == 1){ \
			if(!Data.IsDefaultValue()){\
				outstream & UInt8(Id); \
				Data.Encode(outstream,type,false); \
			}\
		}else{\
			outstream & UInt8(Id); \
			Data.Encode(outstream,type,false); \
		}\
	}

/**
 *@brief 定义属性编码开始
 */
#define CL_ENCODE_PROPERTY_BEGIN(BaseT) \
	void EncodeProperty(Avalon::NetOutputStream& outstream,UInt32 type, int encodeType) { \
		BaseT::EncodeProperty(outstream,type,encodeType);

#define CL_ENCODE_PROPERTY_END() \
	}




/**
 *@brief 定义属性解码
 */
#define CL_DECODE_PROPERTY(Data,Id) \
	if(Id == dataid) {\
		Data.Decode(instream); \
		return true; \
	}

/**
 *@brief 定义属性解码开始
 */
#define CL_DECODE_PROPERTY_BEGIN(BaseT) \
	bool DecodeProperty(Avalon::NetInputStream& instream,UInt8 dataid){ \
		if(BaseT::DecodeProperty(instream,dataid)) return true;

#define CL_DECODE_PROPERTY_END() \
		return false;\
	}



/**
 *@brief 定义同步属性
 */
#define CL_SYNC_PROPERTY(Data,Id,Type) \
	if(Opt == 1){ \
		CL_ENCODE_PROPERTY(Data,Id,Type) \
	}else if(Opt == 2){ \
		CL_DECODE_PROPERTY(Data,Id) \
	}else { \
		Data.ClearDirty(0xFFFFFFFF); \
	}


/**
 *@brief 定义属性同步开始
 */
#define CL_SYNC_PROPERTY_BEGIN(BaseT) \
	void EncodeProperty(Avalon::NetOutputStream& stream,UInt32 type,int encodeType) {\
		BaseT::EncodeProperty(stream,type,encodeType); \
		static Avalon::NetInputStream instream(NULL,0); \
		SyncProperty<1>(stream,instream,0,type,encodeType); \
	} \
	bool DecodeProperty(Avalon::NetInputStream& stream,UInt8 dataid) { \
		if(BaseT::DecodeProperty(stream,dataid)) return true; \
		static Avalon::NetOutputStream outstream(NULL,0); \
		return SyncProperty<2>(outstream,stream,dataid,0,1); \
	} \
	void ClearDirty() { \
		BaseT::ClearDirty(); \
		static Avalon::NetInputStream instream(NULL,0); \
		static Avalon::NetOutputStream outstream(NULL,0); \
		SyncProperty<0>(outstream, instream, 0, 0, 1); \
	} \
	template<int Opt> \
	bool SyncProperty(Avalon::NetOutputStream& outstream,Avalon::NetInputStream& instream \
		,UInt8 dataid,UInt32 type,int encodeType){

#define CL_SYNC_PROPERTY_END() \
		return false; \
	}


 /**
  *@brief 定义配合新属性同步开始
  */
#define CL_NEW_SYNC_PROPERTY_BEGIN(BaseT) \
	void NewEncodeProperty(Avalon::NetOutputStream& stream,UInt32 type,int encodeType) {\
		BaseT::EncodeProperty(stream,type,encodeType); \
		static Avalon::NetInputStream instream(NULL,0); \
		SyncProperty<1>(stream,instream,0,type,encodeType); \
	} \
	bool DecodeProperty(Avalon::NetInputStream& stream,UInt8 dataid) { \
		if(BaseT::DecodeProperty(stream,dataid)) return true; \
		static Avalon::NetOutputStream outstream(NULL,0); \
		return SyncProperty<2>(outstream,stream,dataid,0,1); \
	} \
	void ClearDirty() { \
		BaseT::ClearDirty(); \
		static Avalon::NetInputStream instream(NULL,0); \
		static Avalon::NetOutputStream outstream(NULL,0); \
		SyncProperty<0>(outstream, instream, 0, 0, 1); \
	} \
	template<int Opt> \
	bool SyncProperty(Avalon::NetOutputStream& outstream,Avalon::NetInputStream& instream \
		,UInt8 dataid,UInt32 type,int encodeType){

#define CL_SYNC_PROPERTY_END() \
		return false; \
	}


/**
 *@brief 定义数据库属性
 */
#define CL_DBPROPERTY(Name,Data) \
	if(bGet){ \
		if(bDirty) { \
			if(Data.IsDirty(DB_SYNC_TYPE)){	\
				cmd->PutData(Name,Data.GetDBValue()); \
				Data.ClearDirty(DB_SYNC_TYPE); \
			} \
		}else{ \
			cmd->PutData(Name,Data.GetDBValue()); \
		}\
	}else{ \
		Data.SetDBValue(callback->GetData(Name)); \
	}

/**
 *@brief 定义数据库属性开始
 */
#define CL_DBPROPERTY_BEGIN(BaseT) \
	void GetDBData(CLSaveCommand* cmd, bool bDirty){ \
		BaseT::GetDBData(cmd,bDirty); \
		DoPersist<true>(cmd,NULL,bDirty); \
	} \
	void SetDBData(CLRecordCallback* callback){ \
		BaseT::SetDBData(callback); \
		DoPersist<false>(NULL,callback,false); \
	} \
	template<bool bGet> \
	void DoPersist(CLSaveCommand* cmd,CLRecordCallback* callback,bool bDirty){ 

		

#define CL_DBPROPERTY_END() \
	}

#endif
