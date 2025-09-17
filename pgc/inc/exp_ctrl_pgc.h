#pragma once

#if defined(PGC_STATIC)
    #define PGC_EXPORT
    #define PGC_LOCAL
#elif defined(_WIN32)
    #if defined(PGC_BUILD)
        #define PGC_EXPORT __declspec(dllexport)
    #else
        #define PGC_EXPORT __declspec(dllimport)
    #endif
    #define PGC_LOCAL
#else
    // GCC/Clang: hide non-API symbols in shared builds
    #if defined(PGC_BUILD)
        #define PGC_EXPORT __attribute__((visibility("default")))
        #define PGC_LOCAL __attribute__((visibility("hidden")))
    #else
        #define PGC_EXPORT
        #define PGC_LOCAL
    #endif
#endif
