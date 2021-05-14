Multiplayer game C++ TCP server based on [Asio](https://think-async.com/Asio/) with synchronous thread-safe multi-threaded operations.

User provides request handlers which get executed by **fos** on incoming requests with provided interfaces to execute outgoing (server->client) requests and database ops *synchronously*. All possible data-races are handled by **fos**: no need to worry about thread-safety.

Some of used approaches are kinda outdated as of 2021, so this project must be considered mostly as [Asio](https://think-async.com/Asio/) usage example which is still absolutely relevant.