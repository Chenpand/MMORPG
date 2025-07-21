#include "AvalonMemoryTracker.h"
#include <cassert>
#include <AvalonObjectPool.h>

namespace Avalon
{
    void MemoryTracker::RecordAlloc(MemPtr ptr, UInt32 size, const char* moduleName, const char* fileName, const char* functionName, UInt32 line)
    {
        if (!ptr)
        {
            return;
        }

		m_Mutex.Acquire();
        assert(m_MemoryRecordMap.find(ptr) == m_MemoryRecordMap.end() && "double allocate with same address.");

        m_MemoryRecordMap[ptr] = MemoryRecord(size, moduleName, fileName, functionName, line);

        m_TotalAllocSize += size;
        if (moduleName)
        { 
            m_ModuleMemoryRecordMap[moduleName].RecordAlloc(size, functionName);
        }
		m_Mutex.Release();
    }

    void MemoryTracker::RecordRelease(MemPtr ptr)
    {
        if (!ptr)
        {
            return;
        }

		m_Mutex.Acquire();
        MemoryRecordMap::iterator itr = m_MemoryRecordMap.find(ptr);
        //assert(m_MemoryRecordMap.find(ptr) != m_MemoryRecordMap.end() && "delete unknown memory.");

        m_TotalAllocSize -= itr->second.m_Size;

        if (itr->second.m_ModuleName)
        {
            m_ModuleMemoryRecordMap[itr->second.m_ModuleName].RecordRelease(itr->second.m_Size, itr->second.m_FunctionName);
        }

        m_MemoryRecordMap.erase(itr);
		m_Mutex.Release();
    }

    void MemoryTracker::Report(Logger* logger)
    {
        if (!logger)
        {
            return;
        }

        if (!m_TotalAllocSize)
        {
            return;
        }

		m_Mutex.Acquire();
        logger->Info("Total Alloc Size: %llu", m_TotalAllocSize);
        for (ModuleMemroyRecordMap::const_iterator itr = m_ModuleMemoryRecordMap.begin(); itr != m_ModuleMemoryRecordMap.end(); ++itr)
        {
            const char* moduleName = itr->first;
            const ModuleMemoryRecord& record = itr->second;

            if (!record.m_TotalSize)
            {
                continue;
            }

            logger->Info("Module[%s] Total Alloc: %llu", moduleName, record.m_TotalSize);

            /*for (const auto itr : record.m_FunctionUsedSize)
            {
                logger->Info("Function[%s] Alloc: %llu", itr.first, itr.second);
            }*/
        }
		m_Mutex.Release();
    }

	MemoryObjectRecordMgr::MemoryObjectRecordMgr()
	{
		m_IsRecord = false;
	}

	MemoryObjectRecordMgr::~MemoryObjectRecordMgr()
	{

	}

	void MemoryObjectRecordMgr::IsRecord(bool isRecord)
	{
		m_IsRecord = isRecord;
	}

	void MemoryObjectRecordMgr::RecordAlloc(MemPoolInterface* ptr)
	{
		if (!ptr)
		{
			return;
		}

		m_Mutex.Acquire();
		m_MemPoolVec.push_back(ptr);
		m_Mutex.Release();
	}

	void MemoryObjectRecordMgr::Report(Logger* logger)
	{
		if (!m_IsRecord)
		{
			return;
		}

		m_Mutex.Acquire();

		MemPoolVec::iterator iter = m_MemPoolVec.begin();
		for (;iter != m_MemPoolVec.end(); ++iter)
		{
			if (!(*iter) || (*iter)->GetNodeName().empty())
			{
				continue;
			}

			logger->Info("Object[%s] TotalObject: %u, FreeObject: %u, BlockSize: %u", (*iter)->GetNodeName().c_str(), 
				(*iter)->GetNodeCount(), (*iter)->GetFreeCount(), (*iter)->GetBlockSize());
		}

		m_Mutex.Release();
	}
}