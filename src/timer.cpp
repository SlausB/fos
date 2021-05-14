#include <stdio.h>
#include "timer.h"

#ifdef WIN32 
	#ifndef NOMINMAX
	#define NOMINMAX
	#endif//#ifndef NOMINMAX
	#include <windows.h>
#else
#	ifndef LINUX
#		define LINUX
#	endif
#endif

#ifdef LINUX
#	include <sys/time.h>
#endif

Timer::Timer()
{
#ifdef LINUX
	gettimeofday(&previousTime, NULL);
#elif defined WIN32
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&previousTime);
#endif//#ifdef LINUX
}

double Timer::getElapsedSeconds()
{
#ifdef LINUX

	timeval currentTime;
	gettimeofday(&currentTime, NULL);
	double elapsedTime	= (currentTime.tv_sec + currentTime.tv_usec / 1000000.0) -
							(previousTime.tv_sec + previousTime.tv_usec / 1000000.0);
	previousTime	= currentTime;
	return (double)elapsedTime;

#elif defined WIN32

	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	unsigned long long elapsedTime	= currentTime.QuadPart - previousTime.QuadPart;
	previousTime	= currentTime;
	// результат возвращается в секундах
	return (double)(elapsedTime) / (freq.QuadPart)/* * 1000000*/;

#endif//#ifdef LINUX
}

void Timer::drop()
{
#ifdef LINUX
	gettimeofday(&previousTime, NULL);
#elif defined WIN32
	QueryPerformanceCounter(&previousTime);
#endif//#ifdef LINUX
}

