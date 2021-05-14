
//batch:
#include "batch.cpp"
#include "batch_id_imp.cpp"
#include "batch_ref.cpp"

//times:
#include "times/batch_time_imp.cpp"
#include "times/starting_time_imp.cpp"
#include "times/tick_time_imp.cpp"
#include "times/client_time_imp.cpp"
#include "times/exiting_time_imp.cpp"
#include "times/common_time_imp.cpp"

//db:
#ifndef FOS_NO_DB
#include "db/db_iface.cpp"
#include "db/pql.cpp"
#endif

#define FOS_HANDLER_REALTIME 1
#define FOS_HANDLER_ASIO 2
#define FOS_HANDLER_AIO 3

#include "dispatcher.cpp"

//handlers:
#ifdef FOS_HANDLER_TYPE
	#if (FOS_HANDLER_TYPE == FOS_HANDLER_REALTIME)
		#if defined(WINDOWS) || defined(WIN32) || defined(WIN64)
			#error realtime handler cannot be used under windows.
		#include "handlers/clients_handler_realtime.cpp"
	#endif
	#elif (FOS_HANDLER_TYPE == FOS_HANDLER_AIO)
		#include "handlers/aio/clients_handler_aio.cpp"
		#include "handlers/meter/leb128_collector.cpp"
	#else
		#define FOS_DEFAULT_HANDLER
	#endif
#else//#ifdef FOS_HANDLER_TYPE
#define FOS_DEFAULT_HANDLER
#endif//#ifdef FOS_HANDLER_TYPE
#ifdef FOS_DEFAULT_HANDLER
	#include "handlers/asio/clients_handler_asio.cpp"
	#undef FOS_DEFAULT_HANDLER
#endif//#ifdef FOS_DEFAULT_HANDLER

#include "run.cpp"
#include "client.cpp"
#include "globals.cpp"
#include "outcoming.cpp"
#include "stream/stream.cpp"
#include "batch_usage_map.cpp"
#include "clients_conn_iface.cpp"

//output:
#include "output/messenger.cpp"
#include "output/outputs/console_output.cpp"
#include "output/outputs/file_output.cpp"
#include "output/outputs/string_output.cpp"

//sync:
#include "sync/async_queue.cpp"
#include "sync/context_lock.cpp"
#include "sync/sync_queue.cpp"

#include "timer.cpp"

//colonel:
#include "colonel/colonel.cpp"
#include "colonel_imp/requesters/con_req.cpp"
#include "colonel_imp/handlers/exit_h.cpp"
#ifndef FOS_NO_DB
	#include "colonel_imp/handlers/dbstats_h.cpp"
#endif//#ifndef FOS_NO_DB
#include "colonel_imp/handlers/conns_h.cpp"
#include "colonel_imp/handlers/clients_h.cpp"
#include "colonel_imp/handlers/seg_fault_h.cpp"

//пусть будут подключены все чтобы не подключать их по-отдельности в каждом специфичном проекте:
#include "handlers/meter/simple_collector.cpp"
#include "handlers/meter/simple_transmitter.cpp"
#include "handlers/meter/leb128_collector.cpp"
#include "handlers/meter/leb128_transmitter.cpp"
#include "handlers/meter/http_base64_collector.cpp"
#include "handlers/meter/http_base64_transmitter.cpp"
#include "handlers/meter/http_base64_utils.cpp"
#include "handlers/meter/base64.cpp"



