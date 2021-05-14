g++ -g -o my_imp -std=c++0x -O0 \
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
        ../../fos/src/fos.cpp \
        ../../main.cpp \
        ../../starting_handler.cpp \
        ../../tick_handler.cpp \
        ../../connection_handler.cpp \
        ../../message_handler.cpp \
        ../../disconnection_handler.cpp \
	../../exiting_handler.cpp \
        ../../luggage_batch.cpp \
        ../../common_batch.cpp \
        ../../my_batch_factory.cpp \
	../../my_meters_factory.cpp \
        \
        -lboost_thread \
        -lboost_system \
        -lboost_date_time \
        -lboost_filesystem \
        -lpq \

