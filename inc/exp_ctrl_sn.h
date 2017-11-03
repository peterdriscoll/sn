// ===================================================================
// -------------------------------------------------------------------
#ifndef EXPORT_CONTROL_SN_DLL_H
   #define EXPORT_CONTROL_SN_DLL_H

#ifdef _WIN32
    #pragma once
#endif

   #ifdef _WIN32
       #ifdef SN_BUILD
           #define SN_EXPORT __declspec(dllexport)
           #define SN_EXTERN
       #else
           #define SN_EXPORT __declspec(dllimport)
           #define SN_EXTERN extern
       #endif
   #else
       #define SN::SN_EXPORT
   #endif
#endif

