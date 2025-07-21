#ifndef __PERFORMANCE_CHECK_H__
#define __PERFORMANCE_CHECK_H__

#include <PerformanceMgr.h>

class PerformanceElement
{
public:
    PerformanceElement(const char* elemName)
    {
        name_ = elemName;
        startTime_ = Avalon::Time::CurrentTime().MSec();
    }

    ~PerformanceElement()
    {
        UInt64 usedTime = Avalon::Time::CurrentTime().MSec() - startTime_;
        PerformanceMgr::Instance()->AddPerf(name_, usedTime);
    }

private:
    const char* name_;
    UInt64 startTime_;
};

#define PERF_SEG_BEGIN(name) \
            {  \
                PerformanceElement tmp_perf_seg_##name(#name)

#define PERF_SEG_END() \
            }

#define PERF_ALL(name) \
            PerformanceElement tmp_perf_all_##name(#name);

#define SHOW_PERF() \
            PerformanceMgr::Instance()->ShowPerf()

#define RESET_PERF() \
            PerformanceMgr::Instance()->Reset()

#endif