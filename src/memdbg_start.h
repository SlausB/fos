
/** \file Для определения файла и строки утечки памяти (Visual C++). Для нормального подключения загаловочных файлов библиотек после использования в конце файла нужно подключить memdbg_end.h */

#if (defined(WINDOWS) || defined(WIN32) || defined(WIN64)) && MEM_DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef MYDEBUG_NEW
	#define MYDEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#define new MYDEBUG_NEW

#endif//#if (defined(WINDOWS) || defined(WIN32) || defined(WIN64)) && MEM_DEBUG

