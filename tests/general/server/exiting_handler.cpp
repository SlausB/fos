

#include "handlers.h"

using namespace fos;

void ExitingHandler(ExitingTime* exitingTime)
{
	Globals::messenger->write(boost::format("I: ExitingHandler(): called.\n"));
}

