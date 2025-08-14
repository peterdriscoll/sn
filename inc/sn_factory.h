#pragma once

// ***********************************************************************************
//   WARNING: CROCODILES IN THE WATER
//
// This file uses macro-based template expansion across libraries.
// It *looks* innocent, but if you touch the include order or split it up,
// it will bite your arm and your leg clean off.
//
// - `sn_factory.h` contains the macros, enums, and declarations.
// - `sn_factory.cpp` is a template that must be included *once per library*,
//   with SN_APPLY_CURRENT_LIBRARY defined to select the correct entrypoints.
//
// DO NOT:
//   • Try to make it header-only (you will summon the crocodiles).
//   • Change the include order without thinking (they lurk in the weeds).
//   • Duplicate the logic across libraries (they will circle and strike).
//
// Treat this as a fragile ecosystem: respect the pattern, and it will work.
// Ignore the warning, and you will be eaten alive.
//
// ***********************************************************************************

// --- Library list ---
#define SN_APPLY_LIBRARIES(L, A) \
    L(A, sn, "sn.dll") \
    L(A, http_server_lib, "http_server_lib.dll") \
    L(A, skynet_http_server_sync_lib, "skynet_http_server_sync_lib.dll") \
    L(A, http_server, "")

// --- Entrypoints per library ---
#define SN_APPLY_ENTRYPOINTS_sn(M) \
    M(IHTTP_Handler, SNI::SNI_HTTP_Handler, sn, def)

#define SN_APPLY_ENTRYPOINTS_http_server_lib(M) \
    M(IHTTP_Server, HTTP::server::Server, http_server_lib, alt)

#define SN_APPLY_ENTRYPOINTS_skynet_http_server_sync_lib(M) \
    M(IHTTP_Server, skynet::http::server::sync::Server, skynet_http_server_sync_lib, def)

#define SN_APPLY_ENTRYPOINTS_http_server(M)

#define APPLY_ENTRY(A, N, L) \
    SN_APPLY_ENTRYPOINTS_##N(A)

#define DEFINE_INTERFACE(I) \
public: \
    static const long Id = SN::I##_def_entry; \
private:

// Boilerplate macros
#define DEFINE_LIBRARY_ENUM(A, N, L) N##_library,
#define DEFINE_ENTRY_ENUM(I, C, L, D) I##_##D##_entry,
#define DEFINE_ENTRY_DEFAULT(I, C, L, D) defaultEntryArray[I##_##D##_entry] = I##_def_entry;
#define DEFINE_LIBRARY_PATH(A, N, L) libraryPathArray[N##_library] = L;
#define DEFINE_ENTRY_POINT_LIBRARY(I, C, L, D) libraryForEntryPointArray[I##_##D##_entry] = L##_library;
#define DEFAULT_LIBRARY_ENTRY(A, N, L) libraryFunctionArray[N##_library] = &entrypoint;
#define CREATE_OBJECT(I, C, L, D) case SN::I##_##D##_entry: return reinterpret_cast<void*>(new C());

#include <windows.h>
#include <string>
#include <mutex>
#include <cassert>

namespace SN {

    enum LibraryValues {
        SN_APPLY_LIBRARIES(DEFINE_LIBRARY_ENUM, "")
        LastLibrary
    };

    enum EntryValues {
        SN_APPLY_LIBRARIES(APPLY_ENTRY, DEFINE_ENTRY_ENUM)
        LastEntry
    };

    // Shared arrays
    extern const char* libraryPathArray[LastLibrary];
    extern void* (*libraryFunctionArray[LastLibrary])(long id);
    extern EntryValues defaultEntryArray[LastEntry];
    extern LibraryValues libraryForEntryPointArray[LastEntry];
    extern std::once_flag initFlag;

    void* CallEntryPoint(long id, long interfaceId);

    template <class I>
    class SN_Factory {
    public:
        static I* CreateObject(long id = I::Id) {
            return reinterpret_cast<I*>(CallEntryPoint(id, I::Id));
        }
    };

} // namespace SN
