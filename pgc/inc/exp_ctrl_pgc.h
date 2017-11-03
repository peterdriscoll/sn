// ===================================================================
// -------------------------------------------------------------------
#ifndef EXPORT_CONTROL_PGC_DLL_H
   #define EXPORT_CONTROL_PGC_DLL_H

#ifdef _WIN32
    #pragma once
#endif

   #ifdef _WIN32
       #ifdef PGC_BUILD
           #define PGC_EXPORT __declspec(dllexport)
       #else
           #define PGC_EXPORT __declspec(dllimport)
       #endif
   #else
       #define PGC_EXPORT
   #endif
#endif

