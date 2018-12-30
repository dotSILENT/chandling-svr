#pragma once
#include "HandlingStruct.h"

/*
* Default handling values for each (possible) model
*/

namespace HandlingDefault
{
	bool copyDefaultModelHandling(uint16_t modelid, struct tHandlingData* dest);
	struct tHandlingData* getDefaultModelHandling(uint16_t modelid);

	void Initialize();
}
