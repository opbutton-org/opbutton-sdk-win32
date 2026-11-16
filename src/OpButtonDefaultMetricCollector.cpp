#include "OpButton/OpButtonDefaultMetricCollector.h"
#include "OpButton/OpButtonLog.h"

#include "OpButtonWin32.h"
#include <psapi.h>

namespace OpButton
{

    //////////////////////////////////////////////////////////////////////////
    OpButtonDefaultMetricCollector::OpButtonDefaultMetricCollector()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonDefaultMetricCollector::collect( OpButtonVector<OpButtonMetricEntry> & _out )
    {
        this->collectMemory_( _out );
        this->collectCpu_( _out );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonDefaultMetricCollector::collectMemory_( OpButtonVector<OpButtonMetricEntry> & _out )
    {
        PROCESS_MEMORY_COUNTERS pmc;
        if( GetProcessMemoryInfo( GetCurrentProcess(), &pmc, sizeof( pmc ) ) != 0 )
        {
            _out.emplace_back( OpButtonMetricType::MEMORY, static_cast<double>( pmc.WorkingSetSize ) );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonDefaultMetricCollector::collectCpu_( OpButtonVector<OpButtonMetricEntry> & _out )
    {
        FILETIME creationTime, exitTime, kernelTime, userTime;
        if( GetProcessTimes( GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime ) == 0 )
        {
            return;
        }

        auto toInt64 = []( const FILETIME & _ft ) -> int64_t
        { return ( static_cast<int64_t>( _ft.dwHighDateTime ) << 32 ) | _ft.dwLowDateTime; };

        int64_t kernel = toInt64( kernelTime );
        int64_t user = toInt64( userTime );

        FILETIME nowFt;
        GetSystemTimeAsFileTime( &nowFt );
        int64_t now = toInt64( nowFt );

        if( m_prevCpuTime > 0 )
        {
            int64_t cpuDelta = ( kernel - m_prevCpuKernel ) + ( user - m_prevCpuUser );
            int64_t timeDelta = now - m_prevCpuTime;

            if( timeDelta > 0 )
            {
                double cpuPercent = ( static_cast<double>( cpuDelta ) / static_cast<double>( timeDelta ) ) * 100.0;
                _out.emplace_back( OpButtonMetricType::CPU_USAGE, cpuPercent );
            }
        }

        m_prevCpuKernel = kernel;
        m_prevCpuUser = user;
        m_prevCpuTime = now;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonDefaultMetricCollector::~OpButtonDefaultMetricCollector()
    {
    }
} // namespace OpButton
