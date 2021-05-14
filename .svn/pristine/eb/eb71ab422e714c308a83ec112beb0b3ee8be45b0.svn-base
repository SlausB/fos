#include <stdio.h>
#include "timer.h"

#if defined(WINDOWS) || defined(WIN32) || defined(WIN64)
#	define NOMINMAX
#	include <windows.h>
#else
#	ifndef LINUX
#		define LINUX
#	endif
#endif

#ifdef LINUX
#	include <sys/time.h>
#endif

#ifdef LINUX
	unsigned long timeGetTime()
	{
		timeval tim;
		gettimeofday(&tim, NULL);
		unsigned long ms = (tim.tv_sec * 1000u) + (long)(tim.tv_usec / 1000.0);
		return ms;
	}
//-----------------------------------------------------------------
	float getCurrentTime()
	{
		unsigned int currentTime = timeGetTime();
		return (float)(currentTime) * 0.001f;
	}
//-----------------------------------------------------------------
#endif//#ifdef LINUX

Timer::Timer()
{
#ifdef LINUX
	gettimeofday(&previousTime, NULL);
#elif defined(WINDOWS) || defined(WIN32) || defined(WIN64)
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&previousTime);
#endif//#ifdef LINUX
}

float Timer::getElapsedTime()
{
#ifdef LINUX

	timeval currentTime;
	gettimeofday(&currentTime, NULL);
	double elapsedTime	= (currentTime.tv_sec + currentTime.tv_usec / 1000000.0) -
							(previousTime.tv_sec + previousTime.tv_usec / 1000000.0);
	previousTime	= currentTime;
	return (float)elapsedTime;

#elif defined(WINDOWS) || defined(WIN32) || defined(WIN64)

	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	unsigned long long elapsedTime	= currentTime.QuadPart - previousTime.QuadPart;
	previousTime	= currentTime;
	// результат возвращается в секундах
	return (float)(elapsedTime) / (freq.QuadPart)/* * 1000000*/;

#endif//#ifdef LINUX
}

void Timer::drop()
{
#ifdef LINUX
	gettimeofday(&previousTime, NULL);
#elif defined(WINDOWS) || defined(WIN32) || defined(WIN64)
	QueryPerformanceCounter(&previousTime);
#endif//#ifdef LINUX
}

