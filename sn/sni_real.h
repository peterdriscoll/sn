#if !defined(SNI_REAL_H_INCLUDED)
#define SNI_REAL_H_INCLUDED

#pragma once

#include <string>
#include <limits>
using namespace std;

#include "sn_valueset.h"
#include "sn_bool.h"

#include "sni_number.h"
#include "sni_instance.h"

#define MIN_ERROR 0.000000000001

#define SN_OPERATOR_TYPE(OP, B, K, N)                                  \
    if (dynamic_cast<SNI_Real<K> *>(p_Other))                          \
    {                                                                  \
        SNI_Real<K> * l_Real = dynamic_cast<SNI_Real<K> *>(p_Other);   \
        return MakeType(m_Number OP l_Real->m_Number);                 \
    }

#define SN_OPERATOR_TYPE_CASES(OP)                                     \
	SN_APPLY_TYPES(OP, DUMMY, SN_OPERATOR_TYPE)                        \
    return SN::SN_Error(GetTypeName() + " " #OP " " + p_Other->GetTypeName() + " operator not implemented on ");

#define SN_DIVIDE_TYPE(C, B, K, N)                                     \
    if (dynamic_cast<SNI_Real<K> *>(p_Other))                          \
    {                                                                  \
        SNI_Real<K> * l_Real = dynamic_cast<SNI_Real<K> *>(p_Other);   \
        if (l_Real->m_Number == 0)                                     \
            return SN::SN_Error(GetTypeName() + " divide by zero.");   \
        return MakeType(m_Number / l_Real->m_Number);                  \
    }

#define SN_DIVIDE_TYPE_CASES()                                         \
	SN_APPLY_TYPES(DUMMY, DUMMY, SN_DIVIDE_TYPE)                       \
    return SN::SN_Error(GetTypeName() + " / " + p_Other->GetTypeName() + " operator not implemented on ");

#define SN_EQUIVALENT_TYPE(C, B, K, N)                                 \
    if (dynamic_cast<SNI_Real<K> *>(p_Other))                          \
    {                                                                  \
        SNI_Real<K> * l_Real = dynamic_cast<SNI_Real<K> *>(p_Other);   \
        return fabs(m_Number - l_Real->m_Number) < MIN_ERROR;          \
    }

#define SN_EQUIVALENT_TYPE_CASES()                                     \
	SN_APPLY_TYPES(DUMMY, DUMMY, SN_EQUIVALENT_TYPE)                   \
    return false;

#define SN_OPERATOR_TYPE_COMPARE(OP, B, K, N)                          \
    if (dynamic_cast<SNI_Real<K> *>(p_Other))                          \
    {                                                                  \
        SNI_Real<K> * l_Real = dynamic_cast<SNI_Real<K> *>(p_Other);   \
        return SN::SN_Bool(m_Number OP l_Real->m_Number);              \
    }

#define SN_OPERATOR_TYPE_EQUAL(OP)                                     \
	SN_APPLY_TYPES(OP, DUMMY, SN_OPERATOR_TYPE_COMPARE)                \
    return skynet::False;

#define SN_NAME_FROM_TYPE(C, B, K, N)                                  \
		template <>                                                    \
		string NameFromType<K>(K) const                                \
		{                                                              \
			return #N;                                                 \
		};

namespace SNI
{
    template <typename T>
    class SNI_Real : public SNI_Number
    {
        PGC_CLASS(SNI_Real);
		typedef T ThisType;

		static SNI_Instance *m_Instance;
	public:
        T m_Number;

    public:
		static SNI_Instance * Instance()
		{
			if (!m_Instance)
			{
				m_Instance = new SNI_Instance();
			}
			return m_Instance;
		}
		
		SNI_Real()
            : m_Number(0)
        {
        };

        SNI_Real(const SNI_Real &p_Other)
            : m_Number(p_Other.m_Number)
        {
        }

        SNI_Real(const T &p_Number)
            : m_Number(p_Number)
        {
        };

        virtual ~SNI_Real()
        {
        };

        T GetNumber() const
        {
            return m_Number;
        }

		template <typename S>
		string NameFromType(S p_Number) const
		{
			return "";
		};
		SN_APPLY_TYPES(DUMMY, DUMMY, SN_NAME_FROM_TYPE)

		virtual string GetTypeName() const
        {
            return NameFromType(m_Number);
        }

        virtual string DisplayCpp() const
        {
            return to_string(m_Number);
        }

        virtual string DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
        {
            return to_string(m_Number);
        }

        virtual long GetPriority() const
        {
            return 100;
        }

        string GetString()
        {
			return to_string(m_Number);
        }

        virtual bool Equivalent(SNI_Object * p_Other) const
        {
			SN_EQUIVALENT_TYPE_CASES()
		}

		// Inheritance
		SN::SN_Error  AssertIsAValue(const SNI_Value * p_Parent, SN::SN_Expression p_Result)
		{
			return p_Result.AssertValue(m_Instance->DoIsA(p_Parent));
		}

		SN::SN_Value DoIsA(SNI_Value * p_Parent) const
		{
			return m_Instance->DoIsA(p_Parent);
		}

		template <typename S>
		SN::SN_Value MakeType(S p_Value) const
		{
			return new SNI_Real<S>(p_Value);
		}
        // Arithmatic
        virtual SN::SN_Value DoAdd(SNI_Value *p_Other) const
        {
            SN_OPERATOR_TYPE_CASES(+);
        }

        virtual SN::SN_Value DoSubtract(SNI_Value *p_Other) const
        {
            SN_OPERATOR_TYPE_CASES(-);
        }

        virtual SN::SN_Value DoMultiply(SNI_Value * p_Other) const
        {
            SN_OPERATOR_TYPE_CASES(*);
        }

        virtual SN::SN_Value DoDivide(SNI_Value * p_Other) const
        {
            SN_DIVIDE_TYPE_CASES();
        }

        // Comparison
        SN::SN_Value DoEquals(SNI_Value * p_Other) const
        {
			return SN::SN_Bool(Equivalent(p_Other));
        }

        SN::SN_Value DoGreaterThan(SNI_Value * p_Other) const
        {
			SN_OPERATOR_TYPE_EQUAL(>)
        }

        SN::SN_Value DoLessThan(SNI_Value * p_Other) const
        {
			SN_OPERATOR_TYPE_EQUAL(<)
        }

        SN::SN_Value DoGreaterEquals(SNI_Value * p_Other) const
        {
			SN_OPERATOR_TYPE_EQUAL(>=)
        }

        SN::SN_Value DoLessEquals(SNI_Value * p_Other) const
        {
			SN_OPERATOR_TYPE_EQUAL(<=)
        }

        // Square
        virtual SN::SN_Value DoSquare() const
        {
            return MakeType(m_Number*m_Number);
        }

		virtual SN::SN_Value DoSquareRoot() const
		{
			if (m_Number < 0)
			{
				return SN::SN_Error(GetTypeName() + " Imaginary numbers not supported. Square root of " + to_string(m_Number));
			}
			else if (0 == m_Number)
			{
				return SN::SN_Value(new SNI_Real<T>(m_Number));
			}
			if (numeric_limits<T>::is_integer)
			{
				if (m_Number < 0)
				{
					return SN::SN_Error(GetTypeName() + " Imaginary numbers not supported. Square root of " + to_string(m_Number));
				}
				T n, p, low, high;
				if (m_Number == 0)
				{
					return new SNI_Real<T>(m_Number);
				}
				else if (m_Number == 1)
				{
					return(SN::SN_ValueSet(new SNI_Real<T>(m_Number), new SNI_Real<T>(-m_Number)));
				}
				low = 0;
				high = m_Number;
				while (high > low + 1)
				{
					n = (high + low) / 2;
					p = n * n;
					if (m_Number < p)
						high = n;
					else if (m_Number > p)
						low = n;
					else
						break;
				}
				if (m_Number == p)
				{
					return SN::SN_ValueSet(MakeType(n), MakeType(-n));
				}
			}
			auto result = sqrt(m_Number);
			return SN::SN_ValueSet(MakeType(result), MakeType(-result));
		}
    };

	SN_APPLY_TYPES(DUMMY, DUMMY, SNI_FORWARD)
	
	template <typename T> SNI_Instance *SNI_Real<T>::m_Instance = NULL;
}

#endif // !defined(SNI_REAL_H_INCLUDED)
