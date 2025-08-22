#if !defined(SNI_REAL_H_INCLUDED)
#define SNI_REAL_H_INCLUDED

#pragma once

#include <string>
#include <limits>

#include "sn_valueset.h"
#include "sn_bool.h"

#include "sni_number.h"
#include "sni_class.h"
#include "sni_user.h"

#define MIN_ERROR 0.000000000001

#define SN_OPERATOR_TYPE(OP, B, K, N)                                  \
    if (dynamic_cast<SNI_Real<K> *>(p_Other))                          \
    {                                                                  \
        SNI_Real<K> * l_Real = dynamic_cast<SNI_Real<K> *>(p_Other);   \
        return MakeType(m_Number OP l_Real->m_Number);                 \
    }

#define SN_OPERATOR_TYPE_CASES(OP)                                     \
	SN_APPLY_TYPES(OP, DUMMY, SN_OPERATOR_TYPE)                        \
    return SN::SN_Error(false, false, GetTypeName() + " " #OP " " + p_Other->GetTypeName() + " operator not implemented on ");

#define SN_DIVIDE_TYPE(C, B, K, N)                                     \
    if (dynamic_cast<SNI_Real<K> *>(p_Other))                          \
    {                                                                  \
        SNI_Real<K> * l_Real = dynamic_cast<SNI_Real<K> *>(p_Other);   \
        if (l_Real->m_Number == 0)                                     \
            return SN::SN_Error(false, false, GetTypeName() + " divide by zero.");   \
        return MakeType(m_Number / l_Real->m_Number);                  \
    }

#define SN_DIVIDE_TYPE_CASES()                                         \
	SN_APPLY_TYPES(DUMMY, DUMMY, SN_DIVIDE_TYPE)                       \
    return SN::SN_Error(false, false, GetTypeName() + " / " + p_Other->GetTypeName() + " operator not implemented on ");

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
    return SN::SN_Error(false, false, "Unknown equal type");

#define SN_NAME_FROM_TYPE(C, B, K, N)                                  \
		template <>                                                    \
		static std::string NameFromType<K>(K)                               \
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

		
	public:
        T m_Number;

    public:
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
		static std::string NameFromType(S p_Number)
		{
			return "";
		};
		SN_APPLY_TYPES(DUMMY, DUMMY, SN_NAME_FROM_TYPE)

		static std::string GetTypeNameStatic()
		{
			T var = 0;
			return NameFromType(var);
		}

		virtual std::string GetTypeName() const
		{
			return NameFromType(m_Number);
		}

		static SNI_Class* Class()
		{
			return SNI_User::GetCurrentUser()->GetOrCreatePointer<SNI_Real<T>, SNI_Class>(GetTypeNameStatic());
		}

		virtual SNI_Class *VClass()
		{
			return Class();
		}

		virtual std::string DisplayCpp() const
        {
            return std::to_string(m_Number);
        }

        virtual std::string DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
        {
            return std::to_string(m_Number);
        }

        virtual long GetPriority() const
        {
            return 100;
        }

		// True if this expression is a known integer type (short, int, long, long long)
		// Similar to std::is_integral<T> but using "integer" for clarity
		bool IsInteger() const override
		{
			// true if T is a built-in integral type (short, int, long, long long)
			return std::is_integral<T>::value;
		}

		std::string GetString()
        {
			return std::to_string(m_Number);
        }

        virtual bool Equivalent(SNI_Object * p_Other) const
        {
			if (!p_Other)
			{
				return false;
			}
			SN_EQUIVALENT_TYPE_CASES()
		}

		size_t Hash() const
		{
			return std::_Hash_representation(m_Number);
		}

		template <typename S>
		SN::SN_Value MakeType(S p_Value) const
		{
			return new SNI_Real<S>(p_Value);
		}
        // Arithmatic
        virtual SN::SN_Value DoAdd(SNI_Value *p_Other) const
        {
			ASSERTM(p_Other, "Null pointer.");
			SN_OPERATOR_TYPE_CASES(+);
        }

        virtual SN::SN_Value DoSubtract(SNI_Value *p_Other) const
        {
			ASSERTM(p_Other, "Null pointer.");
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
				return SN::SN_Error(false, false, GetTypeName() + " Imaginary numbers not supported. Square root of " + std::to_string(m_Number));
			}
			else if (0 == m_Number)
			{
				return SN::SN_Value(new SNI_Real<T>(m_Number));
			}
			if (std::numeric_limits<T>::is_integer)
			{
				if (m_Number < 0)
				{
					return SN::SN_Error(false, false, GetTypeName() + " Imaginary numbers not supported. Square root of " + std::to_string(m_Number));
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
}

#endif // !defined(SNI_REAL_H_INCLUDED)
