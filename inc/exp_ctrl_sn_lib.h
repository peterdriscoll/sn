// ===================================================================
// -------------------------------------------------------------------
#ifndef EXPORT_CONTROL_SN::SN_LIB_DLL_H
   #define EXPORT_CONTROL_SN::SN_LIB_DLL_H

#ifdef _WIN32
    #pragma once
#endif

   #ifdef _WIN32
       #ifdef SN::SN_LIB_BUILD
           #define SN::SN_LIB_EXPORT __declspec(dllexport)
       #else
           #define SN::SN_LIB_EXPORT __declspec(dllimport)
       #endif
   #else
       #define SN::SN_LIB_EXPORT
   #endif
#endif

