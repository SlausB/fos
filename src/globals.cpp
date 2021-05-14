
#include "output/messenger.h"

#include "output/outputs/console_output.h"

#include "memdbg_start.h"

namespace Globals
{
	Messenger* messenger = new Messenger(new ConsoleOutput);
}

#include "memdbg_end.h"

