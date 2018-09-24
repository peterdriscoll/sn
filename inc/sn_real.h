#if !defined(SN_REAL_H_INCLUDED)
#define SN_REAL_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_number.h"

#include "sn_operators.h"
#include "../sn/sni_real.h"

#include <string>
using namespace std;

namespace SNI
{
	class SNI_Value;
}

namespace SN
{
	class SN_Expression;
	class SN_Value;

	template <typename T>
	class SN_EXPORT SN_Real
	{
	private:
		SNI::SNI_Real<T> *m_Real;

	public:
		static SN_Class Class()
		{
			return SNI_Real<T>::Class();
		}

		SN_Real()
			: m_Real(NULL)
		{
		};

		SN_Real(const  SN_Real<T> &p_Real)
			: m_Real(p_Real.m_Real)
		{
		}

		SN_Real(T p_Number)
			: m_Real(new SNI::SNI_Real<T>(p_Number))
		{
		}

		SN_Real(const SN_Value &p_Value)
			: m_Real(dynamic_cast<SNI::SNI_Real<T> *>(const_cast<SN_Value &>(p_Value).SimplifyValue().GetSNI_Expression()))
		{
		}

		SN_Real(const SN_Expression &p_Expression)
			: m_Real(dynamic_cast<SNI::SNI_Real<T> *>(const_cast<SN_Expression &>(p_Expression).SimplifyValue().GetSNI_Expression()))
		{
		}

		/* Very dangerous constructor Pointer gets mixed up with number types.
		SN_Real(SNI::SNI_Value *p_Value)
			: m_Real(dynamic_cast<SNI::SNI_Real<T> *>(p_Value))
		{
		}
		*/

		virtual ~SN_Real()
		{
		}

		SNI::SNI_Real<T> * GetSNI_Real() const
		{
			return m_Real;
		}

		string GetString() const
		{
			return m_Real->GetString();
		}

		T GetNumber() const
		{
			return m_Real->GetNumber();
		}

		string DisplayValueSN() const
		{
			if (m_Real)
			{
				SNI::SNI_DisplayOptions l_DisplayOptions;
				return m_Real->DisplayValueSN(0, l_DisplayOptions);
			}
			return "NULL";
		}

		bool IsNull() const
		{
			return !m_Real || m_Real->IsNull();
		}

		bool IsNullValue() const
		{
			return !m_Real || m_Real->IsNull();
		}

		//-------------------------------------------------------------
		// Declaration
		//-------------------------------------------------------------

		// Value sets
		SN_Expression operator ||(const SN_Expression &p_Other) const
		{
			return SN_Operators::operator ||(*this, p_Other);
		}

		// Inheritance
		SN_Expression IsA(const SN_Expression &p_Parent) const
		{
			return SN_Operators::IsA(*this, p_Parent);
		}

		// Sets
		SN_Expression BuildSet() const
		{
			return SN_Operators::BuildSet(*this);
		}

		// Arithmatic
		SN_Expression operator +(const SN_Expression  &p_Other) const
		{
			return SN_Operators::operator +(*this, p_Other);
		}

		SN_Expression operator -(const SN_Expression  &p_Other) const
		{
			return SN_Operators::operator -(*this, p_Other);
		}

		SN_Expression operator *(const SN_Expression  &p_Other) const
		{
			return SN_Operators::operator *(*this, p_Other);
		}

		SN_Expression operator /(const SN_Expression  &p_Other) const
		{
			return SN_Operators::operator /(*this, p_Other);
		}

		// Comparisons
		SN_Expression operator ==(const SN_Expression &p_Other) const
		{
			return SN_Operators::operator ==(*this, p_Other);
		}

		// Math
		SN_Expression SquareRoot() const
		{
			return SN_Operators::SquareRoot(*this);
		}

		SN_Expression Square() const
		{
			return SN_Operators::Square(*this);
		}


        //-------------------------------------------------------------
		// Implementation
		//-------------------------------------------------------------

		// 	Value sets
		SN_Value DoOr(const SN_Value &p_Other) const
		{
			return m_Real->DoOr(p_Other.GetSNI_Value());
		}

		// Sets
		SN_Value DoBuildSet() const
		{
			return m_Real->DoBuildSet();
		}

		// Arithmatic
		SN_Number DoAdd(const SN_Value &p_Other) const
		{
			return m_Real->DoAdd(SN_Value(p_Other).GetSNI_Value());
		}

		SN_Number DoSubtract(const SN_Value &p_Other) const
		{
			return m_Real->DoSubtract(SN_Value(p_Other).GetSNI_Value());
		}

		SN_Number DoMultiply(const SN_Value &p_Other) const
		{
			return m_Real->DoMultiply(SN_Value(p_Other).GetSNI_Value());
		}

		SN_Number DoDivide(const SN_Value &p_Other) const
		{
			return m_Real->DoDivide(SN_Value(p_Other).GetSNI_Value());
		}

		SN_Number DoNegative(const SN_Value &p_Other) const
		{
			return m_Real->DoNegative();
		}

		// Comparison
		bool Equivalent(const SN_Expression &p_Other) const
		{
			return m_Real->Equivalent(dynamic_cast<SNI::SNI_Object *>(p_Other.GetSNI_Expression()));
		}

		SN_Value DoEquals(const SN_Value &p_Other) const
		{
			return m_Real->DoEquals(SN_Value(p_Other).GetSNI_Value());
		}

		SN_Value DoGreaterThan(const SN_Value &p_Other) const
		{
			return m_Real->DoGreaterThan(SN_Value(p_Other).GetSNI_Value());
		}

		SN_Value DoLessThan(const SN_Value &p_Other) const
		{
			return m_Real->DoLessThan(SN_Value(p_Other).GetSNI_Value());
		}

		SN_Value DoGreaterEquals(const SN_Value &p_Other) const
		{
			return m_Real->DoGreaterEquals(SN_Value(p_Other).GetSNI_Value());
		}

		SN_Value DoLessEquals(const SN_Value &p_Other) const
		{
			return m_Real->DoLessEquals(SN_Value(p_Other).GetSNI_Value());
		}

		// Square
		SN_Number DoSquare() const
		{
			return m_Real->DoSquare();
		}

		SN_Number DoSquareRoot() const
		{
			return m_Real->DoSquareRoot();
		}
	};

	SN_APPLY_TYPES(DUMMY, DUMMY, SN_FORWARD)
}

#endif // !defined(SN_REAL_H_INCLUDED)