/**
*@author    zhengjiyue
*@email
*@date		2016-06-14
*@brief	    ÄÚ´æ×·×ÙÆ÷
*/
#ifndef _AVALON_MEMORY_TRAKER_H_
#define _AVALON_MEMORY_TRAKER_H_

#include <vector>
#include "AvalonDefine.h"
#include "AvalonSingleton.h"
#include "AvalonLogger.h"

namespace Avalon
{
    struct MemoryRecord
    {
    public:
        MemoryRecord() : m_ModuleName(NULL), m_FileName(NULL), m_FunctionName(NULL), m_Line(0), m_Size(0)  {}
        MemoryRecord(UInt32 size, const char* moduleName = NULL, const char* fileName = NULL, const char* functionName = NULL, UInt32 line = 0)
            : m_ModuleName(moduleName), m_FileName(fileName), m_FunctionName(functionName), m_Line(line), m_Size(size){}

        const char*     m_ModuleName;
        const char*     m_FileName;
        const char*     m_FunctionName;
        UInt32          m_Line;
        UInt32          m_Size;
    };

    struct ModuleMemoryRecord
    {
        ModuleMemoryRecord() : m_TotalSize(0) {}

        void RecordAlloc(UInt32 size, const char* functionName)
        {
            m_TotalSize += size;
            if (functionName)
            {
                m_FunctionUsedSize[functionName] += size;
            }
        }

        void RecordRelease(UInt32 size, const char* functionName)
        {
            m_TotalSize -= size;
            if (functionName)
            {
                m_FunctionUsedSize[functionName] -= size;
            }
        }

        UInt64                          m_TotalSize;
        HashMap<const char*, UInt64>    m_FunctionUsedSize;
    };

    class MemoryTracker : public Singleton<MemoryTracker>
    {
    public:
        typedef void* MemPtr;
        typedef HashMap<MemPtr, MemoryRecord> MemoryRecordMap;
        typedef HashMap<const char*, ModuleMemoryRecord> ModuleMemroyRecordMap;

        MemoryTracker() : m_TotalAllocSize(0) {}

        void RecordAlloc(MemPtr ptr, UInt32 size, const char* moduleName = NULL, const char* fileName = NULL, const char* functionName = NULL, UInt32 line = 0);
        void RecordRelease(MemPtr ptr);

        void Report(Logger* logger);

    private:
        MemoryRecordMap         m_MemoryRecordMap;
        ModuleMemroyRecordMap   m_ModuleMemoryRecordMap;
        UInt64                  m_TotalAllocSize;
		Mutex					m_Mutex;
    };

#ifdef ENABLE_MEMORY_TRACKER
    #define RECORD_MEMORY_ALLOC(ptr, size, moduleName) Avalon::MemoryTracker::Instance()->RecordAlloc(ptr, size, moduleName, __FILE__, __FUNCTION__, __LINE__)
	#define RECORD_MEMORY_RELEASE(ptr) Avalon::MemoryTracker::Instance()->RecordRelease(ptr)
	#define REPORT_MEMORY(logger) Avalon::MemoryTracker::Instance()->Report(logger)
#else
    #define RECORD_MEMORY_ALLOC(ptr, size, moduleName) 
    #define RECORD_MEMORY_RELEASE(ptr) 
    #define REPORT_MEMORY(logger) 
#endif

	struct MemPoolInterface;

	class MemoryObjectRecordMgr : public Singleton<MemoryObjectRecordMgr>
	{
	public:
		typedef std::vector<MemPoolInterface*> MemPoolVec;

	public:
		MemoryObjectRecordMgr();
		~MemoryObjectRecordMgr();

		void IsRecord(bool isRecord);
		void RecordAlloc(MemPoolInterface* ptr);
		void Report(Logger* logger);

	private:
		bool					m_IsRecord;
		MemPoolVec				m_MemPoolVec;
		Mutex					m_Mutex;
	};

#define MEMORY_OBJECT_ALLOC(ptr) Avalon::MemoryObjectRecordMgr::Instance()->RecordAlloc(ptr)
#define REPORT_MEMORY_OBJECT(logger) Avalon::MemoryObjectRecordMgr::Instance()->Report(logger)
#define MEMORY_OBJECT_SWITCH(isOpen) Avalon::MemoryObjectRecordMgr::Instance()->IsRecord(isOpen)

}

#endif