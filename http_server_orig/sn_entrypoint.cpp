#include "../inc/sn_entrypoint.h"

namespace SN
{
	char *libraryList[] = {
		SN_APPLY_ENTRYPOINTS(DEFINE_LIBRARY)
		"Dummy entry"
	};

	char* GetLibraryName(long id)
	{
		return libraryList[id];
	};
}
