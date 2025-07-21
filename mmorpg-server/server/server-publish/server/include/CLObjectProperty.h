#ifndef _CL_OBJECT_PROPERTY_H_
#define _CL_OBJECT_PROPERTY_H_

#include <string>
#include <sstream>
#include <AvalonNetStream.h>
#include "CLDatabaseDefine.h"
#include "CLDefine.h"
#include "CLGameDefine.h"
#include "CLSyncObject.h"

/**
 *@brief �������Խӿ�
 */
class CLComplexProperty
{
public:
	CLComplexProperty():m_DirtyFlag(0){}
	virtual ~CLComplexProperty(){}

private:
	CLComplexProperty(const CLComplexProperty&);
	const CLComplexProperty& operator=(const CLComplexProperty&);

public:
	/**
	 *@brief ����ͬ����������
	 */
	virtual void Encode(Avalon::NetOutputStream& stream,UInt32 type,bool bDirty){};
	virtual void Decode(Avalon::NetInputStream& stream){};

	/**
	 *@brief ���ݿ��������
	 */
	void SetDBValue(const CLDatabaseData& data){
		std::istringstream is(data.GetString());
		DecodeString(is);
	}
	const std::string GetDBValue(){
		std::ostringstream os;
		EncodeString(os);
		return os.str(); 
	} 
	/**
	 *@brief ���Ӷ�����ַ���֮���ת��
	 */
	virtual void DecodeString(std::istringstream& is){}
	virtual void EncodeString(std::ostringstream& os){}

	/**
	 *@brief �ж��Ƿ�������
	 */
	bool IsDirty(UInt32 flag) const { return (m_DirtyFlag & flag) != 0; }

	/**
	 *@brief �������
	 */
	void ClearDirty(UInt32 flag) { m_DirtyFlag &= (~flag); }

	/**
	 *@brief �������ʶ
	 */
	void SetDirty() { m_DirtyFlag = 0xFFFFFFFF; }
	void SetDirty(UInt32 flag) { m_DirtyFlag |= flag; }

	/**
	 *@brief �Ƿ�Ĭ��ֵ
	 */
	virtual bool IsDefaultValue() const{ return false; }

private:
	//���ʶ
	UInt32	m_DirtyFlag;
};

/**
 *@brief ���԰�װ��
 */
template<typename T>
class CLObjProperty
{

public:
	CLObjProperty():m_Value(0),m_DirtyFlag(0){}
	~CLObjProperty(){}
	explicit CLObjProperty(const CLObjProperty<T>& val):m_Value(val.m_Value),m_DirtyFlag(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_Value;
	}

	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
	{
		stream & m_Value;
	}

	void Decode(Avalon::NetInputStream& stream)
	{
		stream & m_Value;
	}

	/**
	 *@brief תΪΪ��������
	 */
	operator T() const{ return m_Value; }

	/**
	 *@brief ��ֵ���������ʶ
	 */
	T operator=(T val)
	{
		if(m_Value != val)
		{
			m_Value = val;
			m_DirtyFlag = 0xFFFFFFFF;
		}
		return m_Value;
	}
	T operator=(const CLObjProperty<T>& val)
	{
		if(m_Value != val.m_Value)
		{
			m_Value = val.m_Value;
			m_DirtyFlag = 0xFFFFFFFF;
		}
		return m_Value;
	}

	T& GetValue() { return m_Value;  }

	/**
	 *@brief ���û�ȡ���ݿ�ֵ
	 */
	void SetDBValue(T data){  m_Value = data; }
	T GetDBValue() const{ return m_Value; }

	/**
	 *@brief �ж��Ƿ�������
	 */
	bool IsDirty(UInt32 flag) const { return (m_DirtyFlag & flag) != 0; }

	/**
	 *@brief �������
	 */
	void ClearDirty(UInt32 flag) { m_DirtyFlag &= (~flag); }

	/**
	 *@brief �Ƿ�Ĭ��ֵ
	 */
	bool IsDefaultValue() const{ return m_Value == 0; }

private:
	//��ֵ
	T		m_Value;
	//���ʶ
	UInt32	m_DirtyFlag;
};

// ����������
template<typename T, UInt8 Num>
class CLObjArray
{
public:
	CLObjArray() { memset(m_Value, 0, sizeof(m_Value)); }
	~CLObjArray() {}

	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty){
		stream & Num;
		for (UInt32 i = 0; i < Num; ++i)
		{
			m_Value[i].Encode(stream, type, bDirty);
		}
	}

	void Decode(Avalon::NetInputStream& stream){
		UInt8 num = 0;
		stream & num;
		for (UInt32 i = 0; i < num; ++i)
		{
			m_Value[i].Decode(stream);
		}
	}

	void SetDBValue(const CLDatabaseData& data) {
		std::istringstream is(data.GetString());
		T value = 0;
		char split;

		for (UInt32 i = 0; i < Num; ++i)
		{
			is >> value;
			m_Value[i].SetDBValue(value);
			is >> split;
		}
	}
	const std::string GetDBValue() {
		std::ostringstream os;
		for (UInt32 i = 0; i < Num; ++i)
		{
			os << m_Value[i].GetDBValue() << ';';
		}
		return os.str();
	}

	bool IsDirty(UInt32 flag) const {
		for (UInt32 i = 0; i < Num; ++i)
		{
			if (m_Value[i].IsDirty(flag)) { return true;}
		}
		return false; 
	}

	void ClearDirty(UInt32 flag) { 
		for (UInt32 i = 0; i < Num; ++i)
		{
			m_Value[i].ClearDirty(flag);
		}
	}

	bool IsDefaultValue() const {
		for (UInt32 i = 0; i < Num; ++i)
		{
			if (!m_Value[i].IsDefaultValue()) { return false; }
		}
		return true; 
	}

private:
	CLObjProperty<T>	m_Value[Num];
};

template<>
class CLObjProperty<CLPosition>
{
public:
	CLObjProperty():m_DirtyFlag(0){}
	~CLObjProperty(){}
	explicit CLObjProperty(const CLObjProperty<CLPosition>& val):m_Value(val.m_Value),m_DirtyFlag(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_Value;
	}

	void Encode(Avalon::NetOutputStream& stream,UInt32 type,bool bDirty){ stream & m_Value; }
	void Decode(Avalon::NetInputStream& stream){ stream & m_Value; }

	/**
	 *@brief תΪΪ��������
	 */
	operator const CLPosition&() const { return m_Value; }

	/**
	 *@brief ��ֵ
	 */
	void operator=(const CLPosition& val)
	{
		if(m_Value != val)
		{
			m_Value = val;
			m_DirtyFlag = 0xFFFFFFFF;
		}
	}
	void operator=(const CLObjProperty<CLPosition>& val)
	{
		if(m_Value != val.m_Value)
		{
			m_Value = val.m_Value;
			m_DirtyFlag = 0xFFFFFFFF;
		}
	}

	/**
	 *@brief ֻ��ֵ
	 */
	void SetDBValue(const CLDatabaseData& data){ m_Value.FromString(data.GetString()); }
	const std::string GetDBValue() { return m_Value.ToString(); }

	/**
	 *@brief �ж��Ƿ�������
	 */
	bool IsDirty(UInt32 flag) const { return (m_DirtyFlag & flag) != 0; }

	/**
	 *@brief �������
	 */
	void ClearDirty(UInt32 flag) { m_DirtyFlag &= (~flag); }

	/**
	 *@brief �Ƿ�Ĭ��ֵ
	 */
	bool IsDefaultValue() const{ return false; }

private:
	//��ֵ
	CLPosition		m_Value;
	//���ʶ
	UInt32	m_DirtyFlag;
};

template<>
class CLObjProperty<std::string>
{
public:
	CLObjProperty():m_DirtyFlag(0){}
	~CLObjProperty(){}
	explicit CLObjProperty(const CLObjProperty<std::string>& str):m_Value(str.m_Value),m_DirtyFlag(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_Value;
	}

	void Encode(Avalon::NetOutputStream& stream,UInt32 type,bool bDirty)
	{
		stream & m_Value;
	}

	void Decode(Avalon::NetInputStream& stream)
	{
		stream & m_Value;
	}

	/**
	 *@brief תΪΪ��������
	 */
	operator const std::string&() const{ return m_Value; }
	const char* c_str() const{ return m_Value.c_str(); }

	/**
	 *@brief ��ֵ
	 */
	void operator=(const std::string& val)
	{
		m_Value = val;
		m_DirtyFlag = 0xFFFFFFFF;
	}
	void operator=(const CLObjProperty<std::string>& val)
	{
		m_Value = val.m_Value;
		m_DirtyFlag = 0xFFFFFFFF;
	}

	/**
	 *@brief ֻ��ֵ
	 */
	void SetDBValue(const CLDatabaseData& data){ m_Value = data.GetString(); }
	std::string GetDBValue() const { return m_Value; }

	/**
	 *@brief �ж��Ƿ�������
	 */
	bool IsDirty(UInt32 flag) const { return (m_DirtyFlag & flag) != 0; }

	/**
	 *@brief �������
	 */
	void ClearDirty(UInt32 flag) { m_DirtyFlag &= (~flag); }

	/**
	 *@brief �Ƿ�Ĭ��ֵ
	 */
	bool IsDefaultValue() const{ return m_Value.empty(); }

private:
	//��ֵ
	std::string m_Value;
	//���ʶ
	UInt32	m_DirtyFlag;
};

template<>
class CLObjProperty<ScenePos>
{
public:
    CLObjProperty() :m_DirtyFlag(0) {}
    ~CLObjProperty() {}
    explicit CLObjProperty(const CLObjProperty<ScenePos>& val) :m_Value(val.m_Value), m_DirtyFlag(0) {}

    AVALON_DEFINE_NETSERIALIZATION()
    {
        stream & m_Value;
    }

    void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty) { stream & m_Value; }
    void Decode(Avalon::NetInputStream& stream) { stream & m_Value; }

    /**
    *@brief תΪΪ��������
    */
    operator const ScenePos&() const { return m_Value; }

    void SetPos(UInt32 x_, UInt32 y_)
    {
        m_Value.SetX(x_);
        m_Value.SetY(y_);
        m_DirtyFlag = 0xFFFFFFFF;
    }
    /**
    *@brief ��ֵ
    */
    void operator=(const ScenePos& val)
    {
        if (m_Value != val)
        {
            m_Value = val;
            m_DirtyFlag = 0xFFFFFFFF;
        }
    }
    void operator=(const CLObjProperty<ScenePos>& val)
    {
        if (m_Value != val.m_Value)
        {
            m_Value = val.m_Value;
            m_DirtyFlag = 0xFFFFFFFF;
        }
    }

    /**
    *@brief ֻ��ֵ
    ToDo...
    */
    void SetDBValue(const CLDatabaseData& data) {  }
    const std::string GetDBValue() { return ""; }

    /**
    *@brief �ж��Ƿ�������
    */
    bool IsDirty(UInt32 flag) const { return (m_DirtyFlag & flag) != 0; }

    /**
    *@brief �������
    */
    void ClearDirty(UInt32 flag) { m_DirtyFlag &= (~flag); }

    /**
    *@brief �Ƿ�Ĭ��ֵ
    */
    bool IsDefaultValue() const { return false; }

private:
    //��ֵ
    ScenePos m_Value;
    //���ʶ
    UInt32	m_DirtyFlag;
};

template<>
class CLObjProperty<SceneDir>
{
public:
    CLObjProperty() :m_DirtyFlag(0) {}
    ~CLObjProperty() {}
    explicit CLObjProperty(const CLObjProperty<SceneDir>& val) :m_Value(val.m_Value), m_DirtyFlag(0) {}

    AVALON_DEFINE_NETSERIALIZATION()
    {
        stream & m_Value;
    }

    void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty) { stream & m_Value; }
    void Decode(Avalon::NetInputStream& stream) { stream & m_Value; }

    /**
    *@brief תΪΪ��������
    */
    operator const SceneDir&() const { return m_Value; }

    /**
    *@brief ��ֵ
    */
    void operator=(const SceneDir& val)
    {
        if (m_Value != val)
        {
            m_Value = val;
            m_DirtyFlag = 0xFFFFFFFF;
        }
    }
    void operator=(const CLObjProperty<SceneDir>& val)
    {
        if (m_Value != val.m_Value)
        {
            m_Value = val.m_Value;
            m_DirtyFlag = 0xFFFFFFFF;
        }
    }

    /**
    *@brief ֻ��ֵ
    ToDo...
    */
    void SetDBValue(const CLDatabaseData& data) {  }
    const std::string GetDBValue() { return ""; }

    /**
    *@brief �ж��Ƿ�������
    */
    bool IsDirty(UInt32 flag) const { return (m_DirtyFlag & flag) != 0; }

    /**
    *@brief �������
    */
    void ClearDirty(UInt32 flag) { m_DirtyFlag &= (~flag); }

    /**
    *@brief �Ƿ�Ĭ��ֵ
    */
    bool IsDefaultValue() const { return false; }

private:
    //��ֵ
    SceneDir m_Value;
    //���ʶ
    UInt32	m_DirtyFlag;
};

template<>
class CLObjProperty<PlayerAvatar>
{
public:
	CLObjProperty() :m_DirtyFlag(0) {}
	~CLObjProperty() {}
	explicit CLObjProperty(const CLObjProperty<PlayerAvatar>& val) :m_Value(val), m_DirtyFlag(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_Value;
	}

	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty) { stream & m_Value; }
	void Decode(Avalon::NetInputStream& stream) { stream & m_Value; }

	/**
	*@brief תΪΪ��������
	*/
	operator const PlayerAvatar&() const { return m_Value; }

	/**
	*@brief ��ֵ
	*/
	void operator=(const PlayerAvatar& val)
	{
		if (val != m_Value)
		{
			m_Value = val;
			m_DirtyFlag = 0xFFFFFFFF;
		}
		
	}
	void operator=(const CLObjProperty<PlayerAvatar>& val)
	{
		if (m_Value != val)
		{
			m_Value = (const PlayerAvatar&)val;
			m_DirtyFlag = 0xFFFFFFFF;
		}
	}

	void SetDBValue(const CLDatabaseData& data) 
	{
		std::string avatarStr = data.GetString();
		std::vector<std::string> avatarInfoes;
		Avalon::StringUtil::Split(avatarStr, avatarInfoes, ",");
		if (avatarInfoes.size() >= 1)
		{
			std::vector<std::string> avatarEquips;
			Avalon::StringUtil::Split(avatarInfoes[0], avatarEquips, "|");
			for (auto& str : avatarEquips)
			{
				m_Value.equipItemIDs.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(str));
			}
		}

		if (avatarInfoes.size() >= 2)
		{
			m_Value.weaponStrengthen = Avalon::StringUtil::ConvertToValue<UInt8>(avatarInfoes[1]);
		}

		if (avatarInfoes.size() >= 3)
		{
			m_Value.isShoWeapon = Avalon::StringUtil::ConvertToValue<UInt8>(avatarInfoes[2]);
		}
	}

	const std::string GetDBValue() 
	{
		std::stringstream ss;
		bool first = true;
		for (auto id : m_Value.equipItemIDs)
		{
			if (first)
			{
				ss << id;
				first = false;
			}
			else
			{
				ss << "|" << id;
			}
		}

		ss << "," << (UInt32)m_Value.weaponStrengthen;
		ss << "," << (UInt32)m_Value.isShoWeapon;
		return ss.str();
	}

	/**
	*@brief �ж��Ƿ�������
	*/
	bool IsDirty(UInt32 flag) const { return (m_DirtyFlag & flag) != 0; }

	/**
	*@brief �������
	*/
	void ClearDirty(UInt32 flag) { m_DirtyFlag &= (~flag); }

	/**
	*@brief �Ƿ�Ĭ��ֵ
	*/
	bool IsDefaultValue() const { return false; }

private:
	//��ֵ
	PlayerAvatar m_Value;
	//���ʶ
	UInt32	m_DirtyFlag;
};


template<>
class CLObjProperty<SceneRetinue>
{
public:
	CLObjProperty() :m_DirtyFlag(0) {}
	~CLObjProperty() {}
	explicit CLObjProperty(const CLObjProperty<SceneRetinue>& val) :m_Value(val), m_DirtyFlag(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_Value;
	}

	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty) { stream & m_Value; }
	void Decode(Avalon::NetInputStream& stream) { stream & m_Value; }

	/**
	*@brief תΪΪ��������
	*/
	operator const SceneRetinue&() const { return m_Value; }

	/**
	*@brief ��ֵ
	*/
	void operator=(const SceneRetinue& val)
	{
		if (val != m_Value)
		{
			m_Value = val;
			m_DirtyFlag = 0xFFFFFFFF;
		}

	}
	void operator=(const CLObjProperty<SceneRetinue>& val)
	{
		if (m_Value != val)
		{
			m_Value = (const SceneRetinue&)val;
			m_DirtyFlag = 0xFFFFFFFF;
		}
	}

	void SetRetinue(UInt32 dataId, UInt8 level)
	{
		m_Value.m_DataId = dataId;
		m_Value.m_Level = level;
		m_DirtyFlag = 0xFFFFFFFF;
	}

	void SetDBValue(const CLDatabaseData& data) {  }
	const std::string GetDBValue() { return ""; }

	/**
	*@brief �ж��Ƿ�������
	*/
	bool IsDirty(UInt32 flag) const { return (m_DirtyFlag & flag) != 0; }

	/**
	*@brief �������
	*/
	void ClearDirty(UInt32 flag) { m_DirtyFlag &= (~flag); }

	/**
	*@brief �Ƿ�Ĭ��ֵ
	*/
	bool IsDefaultValue() const { return false; }

private:
	//��ֵ
	SceneRetinue m_Value;
	//���ʶ
	UInt32	m_DirtyFlag;
};


template<>
class CLObjProperty<ScenePet>
{
public:
	CLObjProperty() :m_DirtyFlag(0) {}
	~CLObjProperty() {}
	explicit CLObjProperty(const CLObjProperty<ScenePet>& val) :m_Value(val), m_DirtyFlag(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_Value;
	}

	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty) { stream & m_Value; }
	void Decode(Avalon::NetInputStream& stream) { stream & m_Value; }

	/**
	*@brief תΪΪ��������
	*/
	operator const ScenePet&() const { return m_Value; }

	/**
	*@brief ��ֵ
	*/
	void operator=(const ScenePet& val)
	{
		if (val != m_Value)
		{
			m_Value = val;
			m_DirtyFlag = 0xFFFFFFFF;
		}

	}
	void operator=(const CLObjProperty<ScenePet>& val)
	{
		if (m_Value != val)
		{
			m_Value = (const ScenePet&)val;
			m_DirtyFlag = 0xFFFFFFFF;
		}
	}

	void SetPet(UInt64 id, UInt32 dataId, UInt16 level)
	{
		if (m_Value.m_Id != id || m_Value.m_DataId != dataId || m_Value.m_Level != level)
		{
			m_Value.m_Id = id;
			m_Value.m_DataId = dataId;
			m_Value.m_Level = level;
			m_DirtyFlag = 0xFFFFFFFF;
		}
	}

	void SetDBValue(const CLDatabaseData& data) {  }
	const std::string GetDBValue() { return ""; }

	/**
	*@brief �ж��Ƿ�������
	*/
	bool IsDirty(UInt32 flag) const { return (m_DirtyFlag & flag) != 0; }

	/**
	*@brief �������
	*/
	void ClearDirty(UInt32 flag) { m_DirtyFlag &= (~flag); }

	/**
	*@brief �Ƿ�Ĭ��ֵ
	*/
	bool IsDefaultValue() const { return false; }

private:
	//��ֵ
	ScenePet m_Value;
	//���ʶ
	UInt32	m_DirtyFlag;
};


typedef CLObjProperty<Int8>		ObjInt8;
typedef CLObjProperty<UInt8>	ObjUInt8;
typedef CLObjProperty<Int16>	ObjInt16;
typedef CLObjProperty<UInt16>	ObjUInt16;
typedef CLObjProperty<Int32>	ObjInt32;
typedef CLObjProperty<UInt32>	ObjUInt32;
typedef CLObjProperty<Int64>	ObjInt64;
typedef CLObjProperty<UInt64>	ObjUInt64;
typedef CLObjProperty<Real32>	ObjReal32;
typedef CLObjProperty<Real64>	ObjReal64;
typedef CLObjProperty<CLPosition>	ObjPosition;
typedef CLObjProperty<std::string>	ObjString;
typedef CLObjProperty<ScenePos>	ObjScenePos;
typedef CLObjProperty<SceneDir> ObjSceneDir;
typedef CLObjProperty<PlayerAvatar> ObjPlayerAvatar;
typedef CLObjProperty<SceneRetinue> ObjSceneRetinue;
typedef CLObjProperty<ScenePet> ObjScenePet;



class CLOwneredObjPropertyBase
{
public:
	void Init(CLSyncObject *obj, UInt32 id) { m_SyncObject = obj; m_Id = id; }
	virtual void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty) {};
	virtual void Decode(Avalon::NetInputStream& stream) {};
protected:
	UInt32 m_Id;
	CLSyncObject* m_SyncObject;
};



template<typename T>
class CLOwneredObjProperty : public CLOwneredObjPropertyBase
{
public:
	CLOwneredObjProperty() {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_Data.GetValue();
	}

	virtual void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
	{
		stream & m_Data.GetValue();
	}

	virtual void Decode(Avalon::NetInputStream& stream)
	{
		stream & m_Data.GetValue();
	}

	/**
	 *@brief תΪΪ��������
	 */
	operator T() const { return m_Data.operator T(); }

	/**
	 *@brief ��ֵ���������ʶ
	 */
	T operator=(T val)
	{
		if (m_Data.GetValue() != val)
		{
			//m_Data.GetValue() = val;
			m_Data.operator =(val);
			m_SyncObject->SetSelfDirty(m_Id, this);
		}
		return m_Data.GetValue();
	}
	T operator=(const CLOwneredObjProperty<T>& val)
	{
		if (m_Data.GetValue() != val.m_Data.GetValue())
		{
			//m_Data.GetValue() = val.m_Data.GetValue();
			m_Data.operator=(val);
			m_SyncObject->SetSelfDirty(m_Id, this);
		}
		return m_Data.GetValue();
	}

	/**
	 *@brief ���û�ȡ���ݿ�ֵ
	 */
	void SetDBValue(T data) { m_Data.SetDBValue(data); }
	T GetDBValue() const { return m_Data.GetDBValue(); }

	/**
	 *@brief �ж��Ƿ�������
	 */
	bool IsDirty(UInt32 flag) const { return m_Data.IsDirty(flag); }

	/**
	 *@brief �������
	 */
	void ClearDirty(UInt32 flag) { m_Data.ClearDirty(flag); }

	/**
	 *@brief �Ƿ�Ĭ��ֵ
	 */
	bool IsDefaultValue() const { return m_Data.IsDefaultValue(); }
private:
	CLObjProperty<T> m_Data;
};


typedef CLOwneredObjProperty<Int8>		OwneredObjInt8;
typedef CLOwneredObjProperty<UInt8>		OwneredObjUInt8;
typedef CLOwneredObjProperty<Int16>		OwneredObjInt16;
typedef CLOwneredObjProperty<UInt16>	OwneredObjUInt16;
typedef CLOwneredObjProperty<Int32>		OwneredObjInt32;
typedef CLOwneredObjProperty<UInt32>	OwneredObjUInt32;
typedef CLOwneredObjProperty<Int64>		OwneredObjInt64;
typedef CLOwneredObjProperty<UInt64>	OwneredObjUInt64;
typedef CLOwneredObjProperty<Real32>	OwneredObjReal32;
typedef CLOwneredObjProperty<Real64>	OwneredObjReal64;


#endif
