#if !defined(SN_FACTORY_H_INCLUDED)
#define SN_FACTORY_H_INCLUDED

#pragma once

// List DLLs you want to load here.
#define SN_APPLY_LIBRARIES(L)                          \
    L(sn, "sn.dll")                                    \
	L(http_server_lib, "http_server_lib.dll")          \
	L(http_server, "")

// List dlls above.
//  L(name, path) \

// List classes you want to create from those libraries here.
// Each class has a pure  virtual interface from which they inherit.
#define SN_APPLY_ENTRYPOINTS(M)                        \
    M(IHTTP_Handler, SNI::SNI_HTTP_Handler, sn, def)\
	M(IHTTP_Server, HTTP::server::server, http_server_lib, def)

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
		static const long Id = SN::I##_def; \
	private:

namespace SN
{
	#define DEFINE_LIBRARY_ENUM(N, L) \
		N##_library,

	enum LibraryValues
	{
		SN_APPLY_LIBRARIES(DEFINE_LIBRARY_ENUM)
		LastLibrary
	};

	#define DEFINE_ENTRY_ENUM(I, C, L, D) \
		I##_##D,
	
	enum EntryValues
	{
		SN_APPLY_ENTRYPOINTS(DEFINE_ENTRY_ENUM)
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
