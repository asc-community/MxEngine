#pragma once

#include "Utilities/EventDispatcher/EventDispatcher.h"

namespace MxEngine
{
	MAKE_EVENT_BASE(IEvent);

	using AppEventDispatcher = EventDispatcher<IEvent>;
}