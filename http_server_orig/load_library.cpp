#include "../inc/sn_factory.h"
#include "../inc/ihttp_handler.h"

IHTTP_Handler * LoadLibrary()
{
	return SN::SN_Factory<IHTTP_Handler>::CreateObject();
}
