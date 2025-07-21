#ifndef _CL_OBJECT_INDEX_H_
#define _CL_OBJECT_INDEX_H_

#include "CLObject.h"
#include <vector>

//T需要继承自CLObject
template<typename T>
class CLIDObjIndex
{
	//最大访问深度
	const static UInt32 MAX_VISIT_DEPTH = 3;

	typedef std::map<ObjID_t,T*> ObjectMap;
	typedef std::vector<typename ObjectMap::iterator> IterVec;

public:
	CLIDObjIndex(){ m_VisitDepth = 0; }
	~CLIDObjIndex(){}

public:
	bool AddObject(T* object)
	{
		return m_Objects.insert(std::make_pair(object->GetID(),object)).second;
	}

	T* FindObject(ObjID_t id)
	{
		typename ObjectMap::iterator iter = m_Objects.find(id);
		if(iter != m_Objects.end() && iter->second->GetID() == id)
		{
			return iter->second;
		}
		return NULL;
	}

	void RemoveObject(T* object)
	{
		typename ObjectMap::iterator iter = m_Objects.find(object->GetID());
		if(iter != m_Objects.end() && iter->second == object)
		{
			for(UInt32 i = 0; i < m_VisitDepth; ++i)
			{
				if(m_Iters[i] == iter) ++m_Iters[i];
			}
			m_Objects.erase(iter);
		}
	}

	void VisitObjects(CLVisitor<T>& visitor)
	{
		if(m_VisitDepth >= MAX_VISIT_DEPTH) ++(*(UInt32*)NULL);
		
		UInt32 depth = m_VisitDepth++;
		m_Iters[depth] = m_Objects.begin();
		while(m_Iters[depth] != m_Objects.end())
		{
			T* ptr = m_Iters[depth]->second;
			++m_Iters[depth];
			if(!visitor(ptr))
			{
				break;
			}
		}
		m_Iters[--m_VisitDepth] = m_Objects.end();
	}

	void Clear(){ m_Objects.clear(); m_VisitDepth = 0; }

	UInt32 GetNum() const{ return m_Objects.size(); }

private:
	//object map
	ObjectMap m_Objects;
	//迭代器
	typename ObjectMap::iterator m_Iters[MAX_VISIT_DEPTH];
	//访问深度
	UInt32	m_VisitDepth;
};

template<typename T>
class CLUIntObjIndex
{
	//最大访问深度
	const static UInt32 MAX_VISIT_DEPTH = 3;

	typedef std::map<UInt32,T*> ObjectMap;

public:
	CLUIntObjIndex(){ m_VisitDepth = 0; }
	~CLUIntObjIndex(){}

public:
	bool AddObject(T* object)
	{
		return m_Objects.insert(std::make_pair(object->GetID(),object)).second;
	}

	T* FindObject(UInt32 id)
	{
		typename ObjectMap::iterator iter = m_Objects.find(id);
		if(iter != m_Objects.end())
		{
			return iter->second;
		}
		return NULL;
	}

	void RemoveObject(T* object)
	{
		typename ObjectMap::iterator iter = m_Objects.find(object->GetID());
		if(iter != m_Objects.end() && iter->second == object)
		{
			for(UInt32 i = 0; i < m_VisitDepth; ++i)
			{
				if(m_Iters[i] == iter) ++m_Iters[i];
			}
			m_Objects.erase(iter);
		}
	}

	void VisitObjects(CLVisitor<T>& visitor)
	{
		if(m_VisitDepth >= MAX_VISIT_DEPTH) ++(*(UInt32*)NULL);
		
		UInt32 depth = m_VisitDepth++;
		m_Iters[depth] = m_Objects.begin();
		while(m_Iters[depth] != m_Objects.end())
		{
			T* ptr = m_Iters[depth]->second;
			++m_Iters[depth];
			if(!visitor(ptr))
			{
				break;
			}
		}
		m_Iters[--m_VisitDepth] = m_Objects.end();
	}

	void Clear()
	{
		m_Objects.clear();
		for(UInt32 i = 0; i < m_VisitDepth; ++i)
		{
			m_Iters[i] = m_Objects.end();
		}
	}

	UInt32 GetNum() const{ return m_Objects.size(); }

private:
	//object map
	ObjectMap m_Objects;
	//迭代器
	typename ObjectMap::iterator m_Iters[MAX_VISIT_DEPTH];
	//访问深度
	UInt32	m_VisitDepth;
};

template<typename T>
class CLNameObjectIndex
{
	//最大访问深度
	const static UInt32 MAX_VISIT_DEPTH = 3;

	typedef HashMap<std::string,T*> ObjectMap;

public:
	CLNameObjectIndex(){ m_VisitDepth = 0;}
	~CLNameObjectIndex(){}

public:
	bool AddObject(T* object)
	{
		return m_Objects.insert(std::make_pair(object->GetName(),object)).second;
	}
	
	T* FindObject(const std::string& name)
	{
		typename ObjectMap::iterator iter = m_Objects.find(name);
		if(iter != m_Objects.end())
		{
			return iter->second;
		}
		return NULL;
	}

	void RemoveObject(T* object)
	{
		typename ObjectMap::iterator iter = m_Objects.find(object->GetName());
		if(iter != m_Objects.end() && iter->second == object)
		{
			for(UInt32 i = 0; i < m_VisitDepth; ++i)
			{
				if(m_Iters[i] == iter) ++m_Iters[i];
			}
			m_Objects.erase(iter);
		}
	}

	void VisitObjects(CLVisitor<T>& visitor)
	{
		if(m_VisitDepth >= MAX_VISIT_DEPTH) ++(*(UInt32*)NULL);
		
		UInt32 depth = m_VisitDepth++;
		m_Iters[depth] = m_Objects.begin();
		while(m_Iters[depth] != m_Objects.end())
		{
			T* ptr = m_Iters[depth]->second;
			++m_Iters[depth];
			if(!visitor(ptr))
			{
				break;
			}
		}
		m_Iters[--m_VisitDepth] = m_Objects.end();
	}

	void Clear()
	{
		m_Objects.clear();
		for(UInt32 i = 0; i < m_VisitDepth; ++i)
		{
			m_Iters[i] = m_Objects.end();
		}
	}

private:
	//object map
	ObjectMap m_Objects;
	//迭代器
	typename ObjectMap::iterator m_Iters[MAX_VISIT_DEPTH];
	//访问深度
	UInt32	m_VisitDepth;
};

#endif
