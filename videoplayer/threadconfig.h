#ifndef THREADCONFIG_H
#define THREADCONFIG_H

#include <thread>

void SetThreadName(uint32_t dwThreadID, const char* threadName);
void SetThreadName( const char* threadName);

void SetThreadName(std::thread* thread, const char* threadName);



#endif // THREADCONFIG_H
