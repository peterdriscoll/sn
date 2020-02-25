#define LIBRARY http_server_library

// Create dummies for the classes that are not being implemented here.
namespace SNI
{
	class SNI_HTTP_Handler
	{
	public:
		SNI_HTTP_Handler() {};
	};
}

namespace HTTP
{
	namespace server
	{
		class server {};
	}
}

#include "../inc/sn_factory.cpp"
