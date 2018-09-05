#if !defined(SNI_NUMBER_H_INCLUDED)
#define SNI_NUMBER_H_INCLUDED

#pragma once
#include "ref.h"

#include "sni_value.h"

namespace SNI
{
	class SNI_Number : public SNI_Value
	{
		PGC_CLASS(SNI_Number);
	public:
		SNI_Number();
		SNI_Number(const SNI_Number &p_other);

		virtual ~SNI_Number();
	};

	typedef vector<SNI_Number> SNI_NumberList;
}

#endif // !defined(SNI_NUMBER_H_INCLUDED)
