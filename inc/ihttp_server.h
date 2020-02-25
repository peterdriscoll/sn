#if !defined(IHTTP_SERVER_H)
#define IHTTP_SERVER_H

#pragma once

#include "sn_factory.h"
#include <string>

class IHTTP_Server
{
	DEFINE_INTERFACE(IHTTP_Server);

public:
	virtual ~IHTTP_Server() {};

	// Setup the server.
	virtual void setup(
		const std::string& address,
		const std::string& port,
		const std::string& doc_root) = 0;
	
	// Run the server until stopped.
	virtual void run() = 0;
	virtual void stop() = 0;

};

#endif // IHTTP_SERVER_H