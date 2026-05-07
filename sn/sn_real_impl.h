#if !defined(SN_REAL_IMPL_H_INCLUDED)
#define SN_REAL_IMPL_H_INCLUDED

#pragma once

#include "sn_real.h"

#include "sni_real.h"
#include "sni_displayoptions.h"

namespace SN
{
    template <typename T>
    SN_Class SN_Real<T>::Class()
    {
        return SNI::SNI_Real<T>::Class();
    }

    template <typename T>
    SN_Real<T>::SN_Real()
        : m_Real(NULL)
    {
    }

    template <typename T>
    SN_Real<T>::SN_Real(const SN_Real<T> &p_Real)
        : m_Real(p_Real.m_Real)
    {
    }

    template <typename T>
    SN_Real<T>::SN_Real(T p_Number)
        : m_Real(new SNI::SNI_Real<T>(p_Number))
    {
    }

    template <typename T>
    SN_Real<T>::SN_Real(const SN_Value &p_Value)
        : m_Real(dynamic_cast<SNI::SNI_Real<T> *>(const_cast<SN_Value &>(p_Value).SimplifyValue().GetSNI_Expression()))
    {
    }

    template <typename T>
    SN_Real<T>::SN_Real(const SN_Expression &p_Expression)
        : m_Real(dynamic_cast<SNI::SNI_Real<T> *>(const_cast<SN_Expression &>(p_Expression).SimplifyValue().GetSNI_Expression()))
    {
    }

    template <typename T>
    SN_Real<T>::~SN_Real()
    {
    }

    template <typename T>
    SNI::SNI_Real<T> *SN_Real<T>::GetSNI_Real() const
    {
        return m_Real;
    }

    template <typename T>
    std::string SN_Real<T>::GetString() const
    {
        return m_Real->GetString();
    }

    template <typename T>
    T SN_Real<T>::GetNumber() const
    {
        if (!m_Real)
        {
            return 0;
        }
        return m_Real->GetNumber();
    }

    template <typename T>
    std::string SN_Real<T>::DisplayValueSN() const
    {
        if (m_Real)
        {
            SNI::SNI_DisplayOptions l_DisplayOptions(SNI::DisplayOptionType::doTextOnly);
            return m_Real->DisplayValueSN(0, l_DisplayOptions);
        }
        return "NULL";
    }

    template <typename T>
    bool SN_Real<T>::IsNull() const
    {
        return !m_Real || m_Real->IsNull();
    }

    template <typename T>
    bool SN_Real<T>::IsNullValue() const
    {
        return !m_Real || m_Real->IsNull();
    }

    //-------------------------------------------------------------
    // Declaration
    //-------------------------------------------------------------

    template <typename T>
    SN_Expression SN_Real<T>::operator ||(const SN_Expression &p_Other) const
    {
        return SN_Operators::operator ||(*this, p_Other);
    }

    template <typename T>
    SN_Expression SN_Real<T>::IsA(const SN_Expression &p_Parent) const
    {
        return SN_Operators::IsA(*this, p_Parent);
    }

    template <typename T>
    SN_Expression SN_Real<T>::BuildSet() const
    {
        return SN_Operators::BuildSet(*this);
    }

    template <typename T>
    SN_Expression SN_Real<T>::operator +(const SN_Expression &p_Other) const
    {
        return SN_Operators::operator +(*this, p_Other);
    }

    template <typename T>
    SN_Expression SN_Real<T>::operator -(const SN_Expression &p_Other) const
    {
        return SN_Operators::operator -(*this, p_Other);
    }

    template <typename T>
    SN_Expression SN_Real<T>::operator *(const SN_Expression &p_Other) const
    {
        return SN_Operators::operator *(*this, p_Other);
    }

    template <typename T>
    SN_Expression SN_Real<T>::operator /(const SN_Expression &p_Other) const
    {
        return SN_Operators::operator /(*this, p_Other);
    }

    template <typename T>
    SN_Expression SN_Real<T>::operator ==(const SN_Expression &p_Other) const
    {
        return SN_Operators::operator ==(*this, p_Other);
    }

    template <typename T>
    SN_Expression SN_Real<T>::operator !=(const SN_Expression &p_Other) const
    {
        return SN_Operators::operator !=(*this, p_Other);
    }

    template <typename T>
    SN_Expression SN_Real<T>::operator <(const SN_Expression &p_Other) const
    {
        return SN_Operators::operator <(*this, p_Other);
    }

    template <typename T>
    SN_Expression SN_Real<T>::operator >(const SN_Expression &p_Other) const
    {
        return SN_Operators::operator >(*this, p_Other);
    }

    template <typename T>
    SN_Expression SN_Real<T>::operator <=(const SN_Expression &p_Other) const
    {
        return SN_Operators::operator <=(*this, p_Other);
    }

    template <typename T>
    SN_Expression SN_Real<T>::operator >=(const SN_Expression &p_Other) const
    {
        return SN_Operators::operator >=(*this, p_Other);
    }

    template <typename T>
    SN_Expression SN_Real<T>::SquareRoot() const
    {
        return SN_Operators::SquareRoot(*this);
    }

    template <typename T>
    SN_Expression SN_Real<T>::Square() const
    {
        return SN_Operators::Square(*this);
    }

    //-------------------------------------------------------------
    // Implementation facade calls
    //-------------------------------------------------------------

    template <typename T>
    SN_Value SN_Real<T>::DoOr(const SN_Value &p_Other) const
    {
        return m_Real->DoOr(p_Other.GetSNI_Value());
    }

    template <typename T>
    SN_Value SN_Real<T>::DoBuildSet() const
    {
        return m_Real->DoBuildSet();
    }

    template <typename T>
    SN_Number SN_Real<T>::DoAdd(const SN_Value &p_Other) const
    {
        return m_Real->DoAdd(SN_Value(p_Other).GetSNI_Value());
    }

    template <typename T>
    SN_Number SN_Real<T>::DoSubtract(const SN_Value &p_Other) const
    {
        return m_Real->DoSubtract(SN_Value(p_Other).GetSNI_Value());
    }

    template <typename T>
    SN_Number SN_Real<T>::DoMultiply(const SN_Value &p_Other) const
    {
        return m_Real->DoMultiply(SN_Value(p_Other).GetSNI_Value());
    }

    template <typename T>
    SN_Number SN_Real<T>::DoDivide(const SN_Value &p_Other) const
    {
        return m_Real->DoDivide(SN_Value(p_Other).GetSNI_Value());
    }

    template <typename T>
    SN_Number SN_Real<T>::DoNegative(const SN_Value &p_Other) const
    {
        return m_Real->DoNegative();
    }

    template <typename T>
    bool SN_Real<T>::Equivalent(const SN_Expression &p_Other) const
    {
        return m_Real->Equivalent(dynamic_cast<SNI::SNI_Object *>(p_Other.GetSNI_Expression()));
    }

    template <typename T>
    SN_Value SN_Real<T>::DoEquals(const SN_Value &p_Other) const
    {
        return m_Real->DoEquals(SN_Value(p_Other).GetSNI_Value());
    }

    template <typename T>
    SN_Value SN_Real<T>::DoGreaterThan(const SN_Value &p_Other) const
    {
        return m_Real->DoGreaterThan(SN_Value(p_Other).GetSNI_Value());
    }

    template <typename T>
    SN_Value SN_Real<T>::DoLessThan(const SN_Value &p_Other) const
    {
        return m_Real->DoLessThan(SN_Value(p_Other).GetSNI_Value());
    }

    template <typename T>
    SN_Value SN_Real<T>::DoGreaterEquals(const SN_Value &p_Other) const
    {
        return m_Real->DoGreaterEquals(SN_Value(p_Other).GetSNI_Value());
    }

    template <typename T>
    SN_Value SN_Real<T>::DoLessEquals(const SN_Value &p_Other) const
    {
        return m_Real->DoLessEquals(SN_Value(p_Other).GetSNI_Value());
    }

    template <typename T>
    SN_Number SN_Real<T>::DoSquare() const
    {
        return m_Real->DoSquare();
    }

    template <typename T>
    SN_Number SN_Real<T>::DoSquareRoot() const
    {
        return m_Real->DoSquareRoot();
    }
}

#endif // !defined(SN_REAL_IMPL_H_INCLUDED)
