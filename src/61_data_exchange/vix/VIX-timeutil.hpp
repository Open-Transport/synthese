#ifndef TIMEUTIL_H
#define TIMEUTIL_H

class TimeUtil
{
public:
  TimeUtil();
  ~TimeUtil();
  
  unsigned long long GetTickCount();
  void sleep(unsigned long ms);
};

#endif // TIMEUTIL_H
