#include "inc/sn_factory.h"

#include "skynet_http_server_sync_lib_pch.h"

#define SN_APPLY_CURRENT_LIBRARY(L, A) \
	L(A, skynet_http_server_sync_lib, "skynet_http_server_sync_lib.dll")

#include "inc/sn_factory.cpp"
