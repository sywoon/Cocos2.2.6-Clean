#pragma once
#include "ccMacros.h"


NS_CC_BEGIN


/** returns the Next Power of Two value.
Examples:
- If "value" is 15, it will return 16.
- If "value" is 16, it will return 16.
- If "value" is 17, it will return 32.
*/
unsigned long ccNextPOT(unsigned long value);







NS_CC_END