#include "VIX-timeutil.hpp"

#ifdef _WIN32
#include "Windows.h"
#include "Winbase.h"
#else
#include <sys/time.h>
#include <unistd.h>
#endif

TimeUtil::TimeUtil()
{

}

TimeUtil::~TimeUtil()
{

}

// WARNING:  boost get time may be way to slow for our usage.
// better to using OS specific calls.
#ifdef _WIN32

	unsigned long long TimeUtil::GetTickCount(){
		return ::GetTickCount64();
	}

	void TimeUtil::sleep(unsigned long ms){
		::Sleep(ms);
	}

#else

	unsigned long long TimeUtil::GetTickCount(){
	      
	  struct timeval tv;
	  if(gettimeofday(&tv, 0) != 0)
		return 0;

	  unsigned long long  value = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	  
	  return value;
	}

	void sleep(unsigned long ms){
		usleep(ms*1000);
	}
#endif
