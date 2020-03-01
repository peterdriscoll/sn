#define LIBRARY http_server

#define SN_APPLY_CURRENT_LIBRARY(L, A) \
	L(A, http_server, "http_server.exe")

#include "../inc/sn_factory.cpp"
