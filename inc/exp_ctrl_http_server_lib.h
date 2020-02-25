// ===================================================================
// -------------------------------------------------------------------
#ifndef EXPORT_CONTROL_HTTP_SERVER_LIB_DLL_H
   #define EXPORT_CONTROL_HTTP_SERVER_LIB_DLL_H

#ifdef _WIN32
    #pragma once
#endif

   #ifdef _WIN32
       #ifdef HTTP_SERVER_LIB_BUILD
           #define HTTP_SERVER_LIB_EXPORT __declspec(dllexport)
       #else
           #define HTTP_SERVER_LIB_EXPORT __declspec(dllimport)
       #endif
   #else
       #define SN::HTTP_SERVER_LIB_EXPORT
   #endif
#endif
