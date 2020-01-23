#pragma once

#include "Utilities/EventDispatcher/EventDispatcher.h"

namespace MomoEngine
{
	MAKE_EVENT_BASE(IEvent);

	using AppEventDispatcher = EventDispatcher<IEvent>;
}