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
 *@brief ���ݿ�ͬ������
 */
#define	DB_SYNC_TYPE	(1 << 30)

/**
 *@brief ͬ������
 */
class CLSyncObject : public CLObject
{
public:
	CLSyncObject(){}
	virtual ~CLSyncObject(){}

public:
	/**
	*@brief ����������ԣ������Ƿ��࣬ �Ƿ���Ĭ��ֵ
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
	 *@brief �������ָ������
	 *@return true������ falseû������
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
	 *@brief �����ͬ������ʣ��ָ������
	 *@return true������ falseû������
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
	 *@brief �������ָ������                                                                     
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
	 *@brief ����
	 *@return true������ falseû������
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
	 *@brief ��ȡ���ݿ�����
	 */
	virtual void GetDBData(CLSaveCommand* cmd, bool bDirty = true){}
	virtual void SetDBData(CLRecordCallback* callback){}

	virtual void ClearDirty(){}
	

	/**
	 *@brief CLOwnerObjPropertyBase���ã���������ĳ�ֶ�Ϊ�࣬������CLOwnerObjPropertyBaseʹ��
	 */
	virtual void SetSelfDirty(UInt32 id, CLOwnerObjPropertyBase* ptr) {}

protected:
	virtual void EncodeProperty(Avalon::NetOutputStream& stream,UInt32 type,int encodeType) {}

	virtual bool DecodeProperty(Avalon::NetInputStream& stream,UInt8 dataid) { return false; }

	virtual void SetSyncType(UInt32 id, UInt32 allType) { m_ObjectSyncType[id] = allType; }

	virtual UInt32 GetSyncType(UInt32 id) { return m_ObjectSyncType[id]; }

	virtual void NewEncodeProperty(Avalon::NetOutputStream& stream, UInt32 type, int encodeType) {}

private:
	//�ֶ�id -> ͬ������
	HashMap<UInt32, UInt32> m_ObjectSyncType;
};

/**
 *@brief ����ͬ�����ԣ��ú���Ҫ���ڹ�����ʼ��������
 */
#define REGISTER_SYNC_PROPERTY(Data, Id, Type) \
	Data.Init(this, Id); \
	//SetSyncType(Id, Type);

/**
 *@brief �������Ա���
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
 *@brief �������Ա��뿪ʼ
 */
#define CL_ENCODE_PROPERTY_BEGIN(BaseT) \
	void EncodeProperty(Avalon::NetOutputStream& outstream,UInt32 type, int encodeType) { \
		BaseT::EncodeProperty(outstream,type,encodeType);

#define CL_ENCODE_PROPERTY_END() \
	}




/**
 *@brief �������Խ���
 */
#define CL_DECODE_PROPERTY(Data,Id) \
	if(Id == dataid) {\
		Data.Decode(instream); \
		return true; \
	}

/**
 *@brief �������Խ��뿪ʼ
 */
#define CL_DECODE_PROPERTY_BEGIN(BaseT) \
	bool DecodeProperty(Avalon::NetInputStream& instream,UInt8 dataid){ \
		if(BaseT::DecodeProperty(instream,dataid)) return true;

#define CL_DECODE_PROPERTY_END() \
		return false;\
	}



/**
 *@brief ����ͬ������
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
 *@brief ��������ͬ����ʼ
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
  *@brief �������������ͬ����ʼ
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
 *@brief �������ݿ�����
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
 *@brief �������ݿ����Կ�ʼ
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
