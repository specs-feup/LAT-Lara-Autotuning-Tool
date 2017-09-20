#include "Timer.h"

#if _WIN32||_WIN64
	#include <windows.h>
#else // Linux and Unix
	#include <sys/time.h>
#endif 

void Timer::reset() {
	startStamp=0;
	endStamp=0;
}

void Timer::start() {
	// continue from last stop
	startStamp=getTimeStamp()-(endStamp-startStamp);
}

void Timer::stop() {
	endStamp=getTimeStamp();
}

long Timer::getTimeInMS() {
	#if _WIN32||_WIN64
		LARGE_INTEGER qpfreq;
		QueryPerformanceFrequency(&qpfreq);
		double freq = (double)qpfreq.QuadPart;	
		return static_cast<long>(1000.0*(endStamp/freq-startStamp/freq));
	#else // Linux and Unix
		return (endStamp - startStamp)/1000;
	#endif
}

long long Timer::getTimeStamp() {
	#if _WIN32||_WIN64
		LARGE_INTEGER qpcnt;
		QueryPerformanceCounter(&qpcnt);
		return qpcnt.QuadPart;
	#else // Linux and Unix
		struct timeval tv;
        gettimeofday(&tv, 0);
		return static_cast<long long>(1000000)
			   *static_cast<long long>(tv.tv_sec) 
			   + static_cast<long long>(tv.tv_usec);
	#endif
}

unsigned long long Timer::readRDTSC() {
#ifdef _WIN32
    unsigned long long t;
    __asm {
        rdtsc
        mov dword ptr t, eax
        mov dword ptr t + 4, edx
    }
    return t;
#else
    unsigned int a, d;
    asm( "rdtsc" : "=a" (a), "=d" (d) ); 
    return ( ( (unsigned long long)d ) << 32 ) | a;
#endif
}
