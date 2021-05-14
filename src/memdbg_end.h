
/** \file Отмена переопределения оператора.*/

#if defined(WINDOWS) || defined(WIN32) || defined(WIN64)

#ifdef MYDEBUG_NEW
	#undef MYDEBUG_NEW
#endif

#ifdef new
	#undef new
#endif

#endif//#if defined(WINDOWS) || defined(WIN32) || defined(WIN64)

