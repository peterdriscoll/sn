
#pragma once

// Define exactly one of these per build:
// - SKYNET_HTTP_SERVER_SYNCOO_LIB_BUILD  -> when building the DLL
// - SKYNET_HTTP_SERVER_SYNCOO_LIB_STATIC -> when building/using a static lib
//
// Consumers of the DLL define neither.

#if defined(SKYNET_HTTP_SERVER_SYNC_STATIC)

    #define DLL_EXPORT

#elif defined(_WIN32) || defined(__CYGWIN__)

    #if defined(SKYNET_HTTP_SERVER_SYNCOO_LIB_BUILD)
        #define SKYNET_HTTP_SERVER_SYNCOO_EXPORT __declspec(dllexport)
    #else
        #define SKYNET_HTTP_SERVER_SYNCOO_EXPORT __declspec(dllimport)
    #endif

#else  // non-Windows

    #if defined(__GNUC__) && __GNUC__ >= 4
        #define SKYNET_HTTP_SERVER_SYNCOO_EXPORT __attribute__((visibility("default")))
    #else
        #define SKYNET_HTTP_SERVER_SYNCOO_EXPORT
    #endif

#endif
