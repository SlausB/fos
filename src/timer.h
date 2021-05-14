#ifndef TIMING_H
#define TIMING_H

#if defined(LINUX)
	unsigned long timeGetTime();
#elif defined(WIN32) || defined(_WIN64)

#ifndef NOMINMAX
	#define NOMINMAX
#endif

//определяем это чтобы файл windows.h не подключал winsock.h, так как после вторичного подключенрия winsock.h в файлах где он требуется возникает множество ошибок:
#define _WINSOCKAPI_
#include <windows.h>

#endif

#if defined(LINUX)

#include <stdint.h>
#include <time.h>

typedef uint64_t LARGE_INTEGER;

#endif // defined(LINUX)

class Timer
{
public:
	Timer();

	/** Возвращает количество секунд, прошедшее с момента предыдущего вызова этой функции.*/
	double getElapsedSeconds();
	/** Сбросить отсчёт времени. getElapsedSeconds() вернёт время, прошедшее с вызова этой функции.*/
	void drop();

private:
#ifdef LINUX
	timeval previousTime;
#elif defined WIN32
	LARGE_INTEGER previousTime;
	LARGE_INTEGER freq;
#endif//#ifdef LINUX
};

#endif//#ifndef TIMING_H
