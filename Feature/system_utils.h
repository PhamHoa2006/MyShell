// ============================================================================
// system_utils.h — System Information Utilities
// ============================================================================
// Commands to implement:
//   time       — Show current system time
//   date       — Show current date
//   uptime     — Show how long the system has been running
//   cpuinfo    — Show CPU information
//   meminfo    — Show memory (RAM) usage
//   diskinfo   — Show disk space
//
// Libraries needed:
//   #include <windows.h>   — All system info APIs
//   #include <iostream>
//   #include <ctime>       — For time formatting
//
// Key Windows APIs:
//   GetLocalTime(&st)             — SYSTEMTIME struct with year/month/day/hour/min/sec
//   GetTickCount64()              — Returns uptime in milliseconds
//   GetSystemInfo(&si)            — SYSTEM_INFO struct with CPU count, type
//   GlobalMemoryStatusEx(&mem)    — MEMORYSTATUSEX struct with RAM info
//   GetDiskFreeSpaceExA(...)      — Free/total disk space
//
// Reference: ../Operating-System-Projects/Feature/system_utils.h
// ============================================================================

#ifndef SYSTEM_UTILS_H
#define SYSTEM_UTILS_H

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>

class SystemUtils
{
public:
    void showSystemTime(const std::vector<std::string>& args)
    {
        // TODO: Use GetLocalTime() or std::time/localtime
    }

    void showSystemDate(const std::vector<std::string>& args)
    {
        // TODO: Use GetLocalTime()
    }

    void showSystemUptime(const std::vector<std::string>& args)
    {
        // TODO: Use GetTickCount64()
        // Convert milliseconds → days/hours/minutes/seconds
    }

    void showCPUInfo(const std::vector<std::string>& args)
    {
        // TODO: Use GetSystemInfo()
        // Print: number of processors, processor type
    }

    void showMemoryInfo(const std::vector<std::string>& args)
    {
        // TODO: Use GlobalMemoryStatusEx()
        // Print: total RAM, available RAM, memory load %
    }

    void showDiskInfo(const std::vector<std::string>& args)
    {
        // TODO: Use GetDiskFreeSpaceExA()
        // Print: total space, free space for the given drive
    }
};

#endif // SYSTEM_UTILS_H
