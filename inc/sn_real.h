#if !defined(SN_REAL_H_INCLUDED)
#define SN_REAL_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_number.h"
#include "sn_operators.h"

#include <string>

namespace SNI
{
    class SNI_Value;

    template <typename T>
    class SNI_Real;
}

namespace SN
{
    class SN_Expression;
    class SN_Value;
    class SN_Class;
    class SN_Number;

    template <typename T>
    class SN_EXPORT SN_Real
    {
    private:
        SNI::SNI_Real<T> *m_Real;

    public:
        static SN_Class Class();

        SN_Real();
        SN_Real(const SN_Real<T> &p_Real);
        SN_Real(T p_Number);
        SN_Real(const SN_Value &p_Value);
        SN_Real(const SN_Expression &p_Expression);

        virtual ~SN_Real();

        SNI::SNI_Real<T> *GetSNI_Real() const;

        std::string GetString() const;
        T GetNumber() const;
        std::string DisplayValueSN() const;
        bool IsNull() const;
        bool IsNullValue() const;

        //-------------------------------------------------------------
        // Declaration
        //-------------------------------------------------------------

        // Value sets
        SN_Expression operator ||(const SN_Expression &p_Other) const;

        // Inheritance
        SN_Expression IsA(const SN_Expression &p_Parent) const;

        // Sets
        SN_Expression BuildSet() const;

        // Arithmetic
        SN_Expression operator +(const SN_Expression &p_Other) const;
        SN_Expression operator -(const SN_Expression &p_Other) const;
        SN_Expression operator *(const SN_Expression &p_Other) const;
        SN_Expression operator /(const SN_Expression &p_Other) const;

        // Comparison
        SN_Expression operator ==(const SN_Expression &p_Other) const;
        SN_Expression operator !=(const SN_Expression &p_Other) const;
        SN_Expression operator <(const SN_Expression &p_Other) const;
        SN_Expression operator >(const SN_Expression &p_Other) const;
        SN_Expression operator <=(const SN_Expression &p_Other) const;
        SN_Expression operator >=(const SN_Expression &p_Other) const;

        // Math
        SN_Expression SquareRoot() const;
        SN_Expression Square() const;

        //-------------------------------------------------------------
        // Implementation facade calls
        //-------------------------------------------------------------

        // Value sets
        SN_Value DoOr(const SN_Value &p_Other) const;

        // Sets
        SN_Value DoBuildSet() const;

        // Arithmetic
        SN_Number DoAdd(const SN_Value &p_Other) const;
        SN_Number DoSubtract(const SN_Value &p_Other) const;
        SN_Number DoMultiply(const SN_Value &p_Other) const;
        SN_Number DoDivide(const SN_Value &p_Other) const;
        SN_Number DoNegative(const SN_Value &p_Other) const;

        // Comparison
        bool Equivalent(const SN_Expression &p_Other) const;
        SN_Value DoEquals(const SN_Value &p_Other) const;
        SN_Value DoGreaterThan(const SN_Value &p_Other) const;
        SN_Value DoLessThan(const SN_Value &p_Other) const;
        SN_Value DoGreaterEquals(const SN_Value &p_Other) const;
        SN_Value DoLessEquals(const SN_Value &p_Other) const;

        // Square
        SN_Number DoSquare() const;
        SN_Number DoSquareRoot() const;
    };

    SN_APPLY_TYPES(DUMMY, DUMMY, SN_FORWARD)
}

#endif // !defined(SN_REAL_H_INCLUDED)
