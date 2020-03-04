#include "sn_factory.h"

#include <windows.h>

extern "C"
{
	__declspec(dllexport) long entrypoint(long id)
	{
		#define CREATE_OBJECT(I, C, L, D) \
			case SN::I##_##D##_entry: return (int) new C();

		switch (id)
		{
			SN_APPLY_CURRENT_LIBRARY(APPLY_ENTRY, CREATE_OBJECT)
		case -1:
		default:
			return 0;
		}
	}
}

namespace SN
{
	bool loaded = false;

	char * libraryPathArray[SN::LastLibrary];
#define DEFINE_LIBRARY_PATH(A, N, L) \
	libraryPathArray[N##_library] = L;

#define DUMMY_Library
	static void LoadLibraryPaths()
	{
		SN_APPLY_LIBRARIES(DEFINE_LIBRARY_PATH, "")
	}

	typedef long(*f_entrypoint)(long id);

	f_entrypoint libraryFunctionArray[LastLibrary];
	void InitlibraryFunctionArray()
	{
		for (long j = 0; j < LastLibrary; j++)
		{
			libraryFunctionArray[j] = 0;
		}

		#define DEFAULT_LIBRARY_ENTRY(A, N, L) \
			libraryFunctionArray[N##_library] = &entrypoint;

		SN_APPLY_CURRENT_LIBRARY(DEFAULT_LIBRARY_ENTRY, "")
	}

	enum SN::LibraryValues libraryForEntryPointArray[SN::LastEntry];
#define DEFINE_ENTRY_POINT_LIBRARY(I, C, L, D) \
			libraryForEntryPointArray[I##_##D##_entry] = L##_library;
	void LoadLibraryForEntryPointArray()
	{
		SN_APPLY_LIBRARIES(APPLY_ENTRY, DEFINE_ENTRY_POINT_LIBRARY)
	}

	enum SN::EntryValues defaultEntryArray[SN::LastEntry];
		#define DEFINE_ENTRY_DEFAULT(I, C, L, D) \
			defaultEntryArray[I##_##D##_entry] = I##_def_entry;

	void LoadDefaultEntries()
	{
		SN_APPLY_LIBRARIES(APPLY_ENTRY, DEFINE_ENTRY_DEFAULT)
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
