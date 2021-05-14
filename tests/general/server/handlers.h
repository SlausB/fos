

#ifndef HANDLERS_H
#define HANDLERS_H

#include "../../../src/fos.h"

void StartingHandler(fos::StartingTime* startingTime, void*);
void TickHandler(const double elapsedSeconds, fos::TickTime* tickTime);
void ExitingHandler(fos::ExitingTime* exitingTime);

void ConnectionHandler(const uint64_t sessionId, fos::Batch* luggage, const std::string& ipAddress, fos::ClientTime* clientTime);
void MessageHandler(const uint64_t sessionId, const char* messageData, const size_t messageLength, fos::Batch* luggage, fos::ClientTime* clientTime);
void DisconnectionHandler(const uint64_t sessionId, fos::Batch* luggage, fos::ClientTime* clientTime);

#endif//#ifndef HANDLERS_H

