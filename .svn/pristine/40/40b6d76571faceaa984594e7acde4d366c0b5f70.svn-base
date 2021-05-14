

#ifndef HANDLERS_H
#define HANDLERS_H

#include "fos/src/fos.h"


void StartingHandler(fos::StartingTime* startingTime, void* clientData);

void TickHandler(const double elapsedSeconds, fos::TickTime* tickTime);

void MessageHandler(const uint64_t sessionId, const char* messageData, const size_t messageLength, fos::Batch* luggage, fos::ClientTime* clientTime);

void ConnectionHandler(const uint64_t sessionId, fos::Batch* luggage, const std::string& ipAddress, fos::ClientTime* clientTime);

void DisconnectionHandler(const uint64_t sessionId, fos::Batch* luggage, fos::ClientTime* clientTime);

void ExitingHandler(fos::ExitingTime* exitingTime);


#endif//#ifndef HANDLERS_H

