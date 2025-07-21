#ifndef _CL_OWNER_OBJECT_PROPERTY_H_
#define _CL_OWNER_OBJECT_PROPERTY_H_

#include <string>
#include <sstream>
#include <AvalonNetStream.h>
#include "CLDatabaseDefine.h"
#include "CLDefine.h"
#include "CLGameDefine.h"
#include "CLSyncObject.h"

/**
 *@brief ��ӵ���ߵ����԰�װ��
 */
class CLOwnerObjPropertyBase
{
public:
	void Init(CLSyncObject *obj, UInt32 id) { m_SyncObject = obj; m_Id = id; }
	virtual void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty) {};
	virtual void Decode(Avalon::NetInputStream& stream) {};
	virtual void ClearDirty(UInt32 flag) { }
	virtual void OnSetDirty() { 
		if (m_SyncObject == nullptr)
		{
			return;
		}
		m_SyncObject->SetSelfDirty(m_Id, this); 
	}
protected:
	UInt32 m_Id;
	CLSyncObject* m_SyncObject = nullptr;
};

/**
 *@brief �������Խӿ�
 */
class CLOwnerComplexProperty : public CLOwnerObjPropertyBase
{
public:
	CLOwnerComplexProperty():m_DirtyFlag(0){}
	virtual ~CLOwnerComplexProperty(){}

private:
	CLOwnerComplexProperty(const CLOwnerComplexProperty&);
	const CLOwnerComplexProperty& operator=(const CLOwnerComplexProperty&);

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
	void SetDirty() { m_DirtyFlag = 0xFFFFFFFF; OnSetDirty(); }
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
class CLOwnerObjProperty : public CLOwnerObjPropertyBase
{

public:
	CLOwnerObjProperty():m_Value(0),m_DirtyFlag(0){}
	~CLOwnerObjProperty(){}
	explicit CLOwnerObjProperty(const CLOwnerObjProperty<T>& val):m_Value(val.m_Value),m_DirtyFlag(0){}

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
			OnSetDirty();
		}
		return m_Value;
	}
	T operator=(const CLOwnerObjProperty<T>& val)
	{
		if(m_Value != val.m_Value)
		{
			m_Value = val.m_Value;
			m_DirtyFlag = 0xFFFFFFFF;
			OnSetDirty();
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
class CLOwnerObjArray
{
public:
	CLOwnerObjArray() { memset(m_Value, 0, sizeof(m_Value)); }
	~CLOwnerObjArray() {}

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
	CLOwnerObjProperty<T>	m_Value[Num];
};

template<>
class CLOwnerObjProperty<CLPosition> : public CLOwnerObjPropertyBase
{
public:
	CLOwnerObjProperty():m_DirtyFlag(0){}
	~CLOwnerObjProperty(){}
	explicit CLOwnerObjProperty(const CLOwnerObjProperty<CLPosition>& val):m_Value(val.m_Value),m_DirtyFlag(0){}

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
			OnSetDirty();
		}
	}
	void operator=(const CLOwnerObjProperty<CLPosition>& val)
	{
		if(m_Value != val.m_Value)
		{
			m_Value = val.m_Value;
			m_DirtyFlag = 0xFFFFFFFF;
			OnSetDirty();
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
class CLOwnerObjProperty<std::string> : public CLOwnerObjPropertyBase
{
public:
	CLOwnerObjProperty():m_DirtyFlag(0){}
	~CLOwnerObjProperty(){}
	explicit CLOwnerObjProperty(const CLOwnerObjProperty<std::string>& str):m_Value(str.m_Value),m_DirtyFlag(0){}

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
		OnSetDirty();
	}
	void operator=(const CLOwnerObjProperty<std::string>& val)
	{
		m_Value = val.m_Value;
		m_DirtyFlag = 0xFFFFFFFF;
		OnSetDirty();
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
class CLOwnerObjProperty<ScenePos> : public CLOwnerObjPropertyBase
{
public:
    CLOwnerObjProperty() :m_DirtyFlag(0) {}
    ~CLOwnerObjProperty() {}
    explicit CLOwnerObjProperty(const CLOwnerObjProperty<ScenePos>& val) :m_Value(val.m_Value), m_DirtyFlag(0) {}

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
		OnSetDirty();
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
			OnSetDirty();
        }
    }
    void operator=(const CLOwnerObjProperty<ScenePos>& val)
    {
        if (m_Value != val.m_Value)
        {
            m_Value = val.m_Value;
            m_DirtyFlag = 0xFFFFFFFF;
			OnSetDirty();
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
class CLOwnerObjProperty<SceneDir> : public CLOwnerObjPropertyBase
{
public:
    CLOwnerObjProperty() :m_DirtyFlag(0) {}
    ~CLOwnerObjProperty() {}
    explicit CLOwnerObjProperty(const CLOwnerObjProperty<SceneDir>& val) :m_Value(val.m_Value), m_DirtyFlag(0) {}

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
			OnSetDirty();
        }
    }
    void operator=(const CLOwnerObjProperty<SceneDir>& val)
    {
        if (m_Value != val.m_Value)
        {
            m_Value = val.m_Value;
            m_DirtyFlag = 0xFFFFFFFF;
			OnSetDirty();
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
class CLOwnerObjProperty<PlayerAvatar> : public CLOwnerObjPropertyBase
{
public:
	CLOwnerObjProperty() :m_DirtyFlag(0) {}
	~CLOwnerObjProperty() {}
	explicit CLOwnerObjProperty(const CLOwnerObjProperty<PlayerAvatar>& val) :m_Value(val), m_DirtyFlag(0) {}

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
			OnSetDirty();
		}
		
	}
	void operator=(const CLOwnerObjProperty<PlayerAvatar>& val)
	{
		if (m_Value != val)
		{
			m_Value = (const PlayerAvatar&)val;
			m_DirtyFlag = 0xFFFFFFFF;
			OnSetDirty();
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
class CLOwnerObjProperty<SceneRetinue> : public CLOwnerObjPropertyBase
{
public:
	CLOwnerObjProperty() :m_DirtyFlag(0) {}
	~CLOwnerObjProperty() {}
	explicit CLOwnerObjProperty(const CLOwnerObjProperty<SceneRetinue>& val) :m_Value(val), m_DirtyFlag(0) {}

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
			OnSetDirty();
		}

	}
	void operator=(const CLOwnerObjProperty<SceneRetinue>& val)
	{
		if (m_Value != val)
		{
			m_Value = (const SceneRetinue&)val;
			m_DirtyFlag = 0xFFFFFFFF;
			OnSetDirty();
		}
	}

	void SetRetinue(UInt32 dataId, UInt8 level)
	{
		m_Value.m_DataId = dataId;
		m_Value.m_Level = level;
		m_DirtyFlag = 0xFFFFFFFF;
		OnSetDirty();
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
class CLOwnerObjProperty<ScenePet> : public CLOwnerObjPropertyBase
{
public:
	CLOwnerObjProperty() :m_DirtyFlag(0) {}
	~CLOwnerObjProperty() {}
	explicit CLOwnerObjProperty(const CLOwnerObjProperty<ScenePet>& val) :m_Value(val), m_DirtyFlag(0) {}

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
			OnSetDirty();
		}

	}
	void operator=(const CLOwnerObjProperty<ScenePet>& val)
	{
		if (m_Value != val)
		{
			m_Value = (const ScenePet&)val;
			m_DirtyFlag = 0xFFFFFFFF;
			OnSetDirty();
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
			OnSetDirty();
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


typedef CLOwnerObjProperty<Int8>		OwnerObjInt8;
typedef CLOwnerObjProperty<UInt8>		OwnerObjUInt8;
typedef CLOwnerObjProperty<Int16>		OwnerObjInt16;
typedef CLOwnerObjProperty<UInt16>		OwnerObjUInt16;
typedef CLOwnerObjProperty<Int32>		OwnerObjInt32;
typedef CLOwnerObjProperty<UInt32>		OwnerObjUInt32;
typedef CLOwnerObjProperty<Int64>		OwnerObjInt64;
typedef CLOwnerObjProperty<UInt64>		OwnerObjUInt64;
typedef CLOwnerObjProperty<Real32>		OwnerObjReal32;
typedef CLOwnerObjProperty<Real64>		OwnerObjReal64;
typedef CLOwnerObjProperty<CLPosition>	OwnerObjPosition;
typedef CLOwnerObjProperty<std::string>	OwnerObjString;
typedef CLOwnerObjProperty<ScenePos>	OwnerObjScenePos;
typedef CLOwnerObjProperty<SceneDir>	OwnerObjSceneDir;
typedef CLOwnerObjProperty<PlayerAvatar>OwnerObjPlayerAvatar;
typedef CLOwnerObjProperty<SceneRetinue>OwnerObjSceneRetinue;
typedef CLOwnerObjProperty<ScenePet>	OwnerObjScenePet;


#endif
