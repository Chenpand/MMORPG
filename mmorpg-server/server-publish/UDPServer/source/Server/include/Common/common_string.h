#ifndef __COMMON_STRING_H__
#define __COMMON_STRING_H__

#include <string.h>
#include <ctype.h>
#include "common_namespace_definition.h"

SERVER_BEGIN

#define STRNCPY(dst, src, length)					      \
	do								      \
	{								      \
		strncpy(dst, src, (length-1));			      \
		dst[(length-1)] = '\0';					      \
	}								      \
	while(0)


SERVER_END
#endif /* __COMMON_STRING_H__ */

