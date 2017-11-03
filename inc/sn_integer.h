#if !defined(SN_INTEGER_H_INCLUDED)
#define SN_INTEGER_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"

#include "sn_real.h"

namespace SNI
{
	template <typename T>
	class SNI_Integer;
	class SNI_Value;
}

namespace SN
{
	class SN_Expression;
	class SN_Number;
	class SN_Value;
	class SN_Expression;
	class SN_Number;
	class SN_Long;

	template <typename T>
	class SN_EXPORT SN_Integer : public SN_Real<T>
	{
	public:
		SN_Integer()
			: SN_Real<T>(new SNI_Real<T>())
		{
		};

		SN_Integer(const SN_Integer &p_other)
			: SN_Real<T>(p_Other.m_Number())
		{
		};

		SN_Integer(const SN_Value &p_Value)
			: SN_Real<T>(dynamic_cast<SNI::SNI_Integer<T> *>(p_Value.GetSNI_Value()))
		{
		}

		SN_Integer(const SN_Long &p_Value)
			: SN_Real<T>(dynamic_cast<SNI::SNI_Integer<T> *>(p_Value.GetSNI_Long()))
		{
		}

		SN_Integer(const SN_Expression &p_Expression)
			: SN_Real<T>(dynamic_cast<SNI::SNI_Integer<T> *>(p_Expression.GetSNI_Expression()))
		{
		}

		SN_Integer(const SN_Number &p_Number)
			: SN_Real<T>(dynamic_cast<SNI::SNI_Integer<T> *>(p_Number.GetSNI_Number()))
		{
		}

		SN_Integer(SNI::SNI_Value *p_Value)
			: SN_Real<T>(dynamic_cast<SNI::SNI_Integer<T> *>(p_Value))
		{
		}

		SN_Integer(double p_Double)
			: SN_Real<T>(new SNI::SNI_Integer<T>(p_Double))
		{
		}

		virtual ~SN_Integer()
		{
		}

		SNI::SNI_Integer<T> * GetSNI_Integer() const
		{
			return dynamic_cast<SNI::SNI_Integer<T> *>(m_Real);
		}
	};
}

#endif // !defined(SN_INTEGER_H_INCLUDED)
