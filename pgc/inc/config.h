#pragma once

// pgc_config.h — single source of truth for PGC build flags

// --- Decide PGC_DEBUG ---
#if !defined(PGC_DEBUG)
  // Manual override for special cases
#if defined(PGC_DEBUG_FORCE_ON)
#define PGC_DEBUG 1
#elif defined(PGC_DEBUG_FORCE_OFF)
  // leave undefined
// MSVC's debug define
#elif defined(_MSC_VER) && defined(_DEBUG) && !defined(NDEBUG)
#define PGC_DEBUG 1
// Portable default: Debug if NDEBUG is NOT defined
#elif !defined(NDEBUG)
#define PGC_DEBUG 1
#endif
#endif

// Optional convenience
#if defined(PGC_DEBUG)
#define PGC_IF_DEBUG(x) x
#else
#define PGC_IF_DEBUG(x)
#endif
