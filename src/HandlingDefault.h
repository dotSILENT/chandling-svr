#pragma once
#include "HandlingStruct.h"

/*
* Default handling values for each (possible) model
*/

namespace HandlingDefault
{
	bool copyDefaultModelHandling(int modelid, struct tHandlingData* dest);

	void Initialize();
}
