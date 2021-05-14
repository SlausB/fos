gcc -o client -std=c++0x \
	\
	-DLINUX \
	-DHAVE_CONFIG_H \
	\
	-I/usr/local/include \
	-L/usr/local/lib \
	\
	../../../../../tests/general/client/main.cpp \
	../../../../../tests/general/client/comm.cpp \
	../../../../../tests/general/client/timer.cpp \
	../../../../../tests/general/client/utils.cpp \
	../../../../../tests/general/client/stream/stream.cpp \
	../../../../../src/output/messenger.cpp \
	../../../../../src/output/outputs/console_output.cpp \
	../../../../../src/sync/context_lock.cpp \
	\
	-lboost_thread \
	-lboost_system \
	-lboost_date_time \
	-lboost_filesystem \
