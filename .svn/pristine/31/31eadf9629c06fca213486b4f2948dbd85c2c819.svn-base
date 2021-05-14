g++ -g -o fos -std=c++0x \
	\
	-DLINUX \
	-DHAVE_CONFIG_H \
	-DFOS_HANDLER_TYPE=2 \
	\
	-I/usr/local/include \
	-I/usr/include/postgresql \
	-I/home/slav/programs/asio/asio/include \
	-L/usr/local/lib \
	\
	../../../../../src/fos.cpp \
	../../../../../tests/general/server/main.cpp \
	../../../../../tests/general/server/starting_handler.cpp \
	../../../../../tests/general/server/tick_handler.cpp \
	../../../../../tests/general/server/connection_handler.cpp \
	../../../../../tests/general/server/message_handler.cpp \
	../../../../../tests/general/server/disconnection_handler.cpp \
	../../../../../tests/general/server/exiting_handler.cpp \
	../../../../../tests/general/server/factory.cpp \
	../../../../../tests/general/server/general_batch.cpp \
	../../../../../tests/general/server/luggage_batch.cpp \
	../../../../../tests/general/server/heavy_batch.cpp \
	../../../../../tests/general/server/player_batch.cpp \
	../../../../../tests/general/server/mem_usage.cpp \
	../../../../../tests/general/server/mongoose.c \
	../../../../../tests/general/server/mongoose_batch.cpp \
	\
	-lboost_thread \
	-lboost_system \
	-lboost_date_time \
	-lboost_filesystem \
	-lpq \
	-lrt \
