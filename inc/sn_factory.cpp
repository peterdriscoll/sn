#include "sn_factory.h"

#include <windows.h>
#include <string>
#include <mutex>
#include <cassert>

extern "C"
{
	__declspec(dllexport) void* entrypoint(long id)
	{
		switch (id)
		{
			SN_APPLY_CURRENT_LIBRARY(APPLY_ENTRY, CREATE_OBJECT)
		case -1:
		default:
			return nullptr;
		}
	}
}

namespace SN
{
	const char * libraryPathArray[SN::LastLibrary];
#define DEFINE_LIBRARY_PATH(A, N, L) \
	libraryPathArray[N##_library] = L;

#define DUMMY_Library
	static void LoadLibraryPaths()
	{
		SN_APPLY_LIBRARIES(DEFINE_LIBRARY_PATH, "")
	}

	typedef void * (*f_entrypoint)(long id);

	f_entrypoint libraryFunctionArray[LastLibrary];
	void InitlibraryFunctionArray()
	{
		for (long j = 0; j < LastLibrary; j++)
		{
			libraryFunctionArray[j] = 0;
		}

		#define DEFAULT_LIBRARY_ENTRY(A, N, L) \
			libraryFunctionArray[N##_library] = &entrypoint;

		SN_APPLY_CURRENT_LIBRARY(DEFAULT_LIBRARY_ENTRY, "");
	}

	enum SN::LibraryValues libraryForEntryPointArray[SN::LastEntry];
#define DEFINE_ENTRY_POINT_LIBRARY(I, C, L, D) \
			libraryForEntryPointArray[I##_##D##_entry] = L##_library;
	void LoadLibraryForEntryPointArray()
	{
		SN_APPLY_LIBRARIES(APPLY_ENTRY, DEFINE_ENTRY_POINT_LIBRARY)
	}

	enum SN::EntryValues defaultEntryArray[SN::LastEntry];

	void LoadDefaultEntries()
	{
		SN_APPLY_LIBRARIES(APPLY_ENTRY, DEFINE_ENTRY_DEFAULT)
	}

	std::once_flag initFlag;

	void Initialize()
	{
		std::call_once(initFlag, []() {
			LoadLibraryPaths();
			InitlibraryFunctionArray();
			LoadLibraryForEntryPointArray();
			LoadDefaultEntries();
		});
	}

	void * CallEntryPoint(long id, long interfaceId)
	{
		Initialize();

		if (id < 0 || id >= LastEntry) {
			OutputDebugStringA("Invalid entrypoint ID\n");
			return nullptr;
		}

		assert(defaultEntryArray[id] == interfaceId &&
			"Request to create an object for the wrong interface.");
		enum SN::LibraryValues l = libraryForEntryPointArray[id];

		f_entrypoint f = libraryFunctionArray[l];
		if (!f)
		{
			HINSTANCE hGetProcIDDLL = LoadLibraryA(libraryPathArray[l]);

			if (!hGetProcIDDLL) {
				std::string msg = "Failed to load library: " + std::string(libraryPathArray[l]) + "\n";
				OutputDebugStringA(msg.c_str());
				return nullptr;
			}

			// resolve function address here
			f = reinterpret_cast<f_entrypoint>(GetProcAddress(hGetProcIDDLL, "entrypoint"));
			
			if (!f) {
				std::string msg = "Failed to find entrypoint [" + std::to_string(id) +
					"] in library: " + std::string(libraryPathArray[l]) + "\n";
				OutputDebugStringA(msg.c_str());
				return nullptr;
			}
			libraryFunctionArray[l] = f;
		}
		return f(id);
	}
}
