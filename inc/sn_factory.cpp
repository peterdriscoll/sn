#include "sn_factory.h"

#include <windows.h>

extern "C"
{
	__declspec(dllexport) long entrypoint(long id)
	{
		#define CREATE_OBJECT(I, C, L, D) \
			case SN::I##_##D: return (int) new C();

		switch (id)
		{
			SN_APPLY_ENTRYPOINTS(CREATE_OBJECT)
		}
		return 0;
	}
}

bool loaded = false;

char * libraryPathArray[SN::LastLibrary];
#define DEFINE_LIBRARY_PATH(N, L) \
			libraryPathArray[SN::N##_library] = L;
static void LoadLibraryPaths()
{
	SN_APPLY_LIBRARIES(DEFINE_LIBRARY_PATH)
}

typedef long(*f_entrypoint)(long id);

f_entrypoint libraryFunctionArray[SN::LastLibrary - 1];
void InitlibraryFunctionArray()
{
	for (long j = 0; j < SN::LastLibrary; j++)
	{
		libraryFunctionArray[j] = 0;
	}
	libraryFunctionArray[SN::LIBRARY] = &entrypoint;
}

enum SN::LibraryValues libraryForEntryPointArray[SN::LastEntry];
#define DEFINE_ENTRY_POINT_LIBRARY(I, C, L, D) \
			libraryForEntryPointArray[SN::I##_##D] = SN::L##_library;
void LoadLibraryForEntryPointArray()
{
	SN_APPLY_ENTRYPOINTS(DEFINE_ENTRY_POINT_LIBRARY)
}

enum SN::EntryValues defaultEntryArray[SN::LastEntry];
#define DEFINE_ENTRY_DEFAULT(I, C, L, D) \
			defaultEntryArray[SN::I##_##D] = SN::I##_def;
void LoadDefaultEntries()
{
	SN_APPLY_ENTRYPOINTS(DEFINE_ENTRY_DEFAULT)
}

void Initialize()
{
	if (!loaded)
	{
		loaded = true;
		LoadLibraryPaths();
		InitlibraryFunctionArray();
		LoadLibraryForEntryPointArray();
		LoadDefaultEntries();
	}
}

namespace SN
{
	long CallEntryPoint(long id, long interfaceId)
	{
		Initialize();

		// assert(defaultEntryArray[id] == interfaceId, "Request to create an object for the wrong interface.")
		enum SN::LibraryValues l = libraryForEntryPointArray[id];

		f_entrypoint f = libraryFunctionArray[l];
		if (!f)
		{
			HINSTANCE hGetProcIDDLL = LoadLibraryA(libraryPathArray[l]);

			if (!hGetProcIDDLL) {
				return NULL;
			}

			// resolve function address here
			f = (f_entrypoint)GetProcAddress(hGetProcIDDLL, "entrypoint");
			if (!f) {
				return NULL;
			}
			libraryFunctionArray[l] = f;
		}
		return f(id);
	}
}
