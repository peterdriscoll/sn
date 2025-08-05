#include "sn_transaction.h"
#include "sni_transaction.h"

#include "sn_pch.h"

namespace SN
{
	size_t SN_Transaction::NetMemoryUsed()
	{
		return m_Transaction->NetMemoryUsed();
	}

	size_t SN_Transaction::GrossMemoryUsed()
	{
		return m_Transaction->GrossMemoryUsed();
	}

	/*static*/ void SN_Transaction::RegisterInWebServer()
	{
		PGC::PGC_Transaction::RegisterInWebServer();
	}

	/*static*/ bool SN_Transaction::InWebServer()
	{
		return PGC::PGC_Transaction::InWebServer();
	}

	SN_Transaction::SN_Transaction(bool p_Init)
		: m_Transaction(NULL)
	{
		if (p_Init)
		{
			Init();
		}
	}

	SN_Transaction::SN_Transaction()
		: m_Transaction(new SNI::SNI_Transaction())
	{
	}

	SN_Transaction::~SN_Transaction()
	{
		if (m_Transaction)
		{
			m_Transaction->EndTransaction();
			delete m_Transaction;
		}
	}

	void SN_Transaction::Init()
	{
		m_Transaction = new SNI_Transaction();
	}
}
