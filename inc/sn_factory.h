#if !defined(SN_FACTORY_H_INCLUDED)
#define SN_FACTORY_H_INCLUDED

#pragma once

// List DLLs you want to load here.
#define SN_APPLY_LIBRARIES(L, A)                          \
    L(A, sn, "sn.dll")                                    \
	L(A, http_server_lib, "http_server_lib.dll")          \
	L(A, http_server, "")

// List dlls above.
//  L(name, path) \

// List classes you want to create from those libraries here.
// Each class has a pure  virtual interface from which they inherit.
#define SN_APPLY_ENTRYPOINTS_sn(M)                                     \
    M(IHTTP_Handler, SNI::SNI_HTTP_Handler, sn, def)

#define SN_APPLY_ENTRYPOINTS_http_server_lib(M)                        \
	M(IHTTP_Server, HTTP::server::server, http_server_lib, def)

#define SN_APPLY_ENTRYPOINTS_http_server(M)

#define APPLY_ENTRY(A, N, L) \
	SN_APPLY_ENTRYPOINTS_##N(A)


// List interface implementations above.
//  M(interface, implementation. library path) \

// Declare the id for an interface. Place inside interface;
//
//	class IMyInterface
//	{
//		DEFINE_INTERFACE(IMyInterface)
//	}
#define DEFINE_INTERFACE(I) \
	public: \
		static const long Id = SN::I##_def_entry; \
	private:

namespace SN
{
	#define DEFINE_LIBRARY_ENUM(A, N, L) \
		N##_library,

	enum LibraryValues
	{
		SN_APPLY_LIBRARIES(DEFINE_LIBRARY_ENUM, "")
		LastLibrary
	};

	#define DEFINE_ENTRY_ENUM(I, C, L, D) \
		I##_##D##_entry,
	
	enum EntryValues
	{
		SN_APPLY_LIBRARIES(APPLY_ENTRY, DEFINE_ENTRY_ENUM)
		LastEntry
	};

	long CallEntryPoint(long id, long interfaceId);

	template <class I>
	class SN_Factory
	{
	public:
		SN_Factory()
		{
		}

		static I *CreateObject(long id = I::Id )
		{
			return (I *)CallEntryPoint(id, I::Id);
		}
	};
}

#endif //SN_FACTORY_H_INCLUDED
