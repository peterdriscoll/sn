#if !defined(SN_BASE_H_INCLUDED)
#define SN_BASE_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"

#include "sni_base.h"

#include <string>
using namespace std;

namespace SNI
{
	class SNI_Expression;
}

namespace SN
{
	class SN_Expression;
	class SN_Value;

	template <class T, class Exp, class Err>
	class SN_Base
	{
	protected:
		T *m_Expression;

		SNI::SNI_Base *GetSNI_Base() const
		{
			return dynamic_cast<SNI::SNI_Base *>(const_cast<T *>(m_Expression));
		}

	public:
		SN_Base()
			: m_Expression(NULL)
		{
		}

		SN_Base(T * p_Expression)
			: m_Expression(p_Expression)
		{
			if (m_Expression == NULL)
			{
				long dog = 10;
			}
		}

		~SN_Base()
		{
		}

		T *& GetSNI_ExpressionRef()
		{
			return m_Expression;
		}
		
		string GetTypeName() const
		{
			if (GetSNI_Base())
			{
				return GetSNI_Base()->GetTypeName();
			}
			return "";
		}

		string GetValueTypeName() const
		{
			if (GetSNI_Base())
			{
				return GetSNI_Base()->GetValueTypeName();
			}
			return "";
		}

		string DisplayCpp() const
		{
			return GetSNI_Base()->DisplayCpp();
		}

		string DisplaySN() const
		{
			if (GetSNI_Base())
			{
				SNI::SNI_DisplayOptions l_DisplayOptions(doTextOnly);
				return GetSNI_Base()->DisplaySN(0, l_DisplayOptions);
			}
			return "";
		}

		string DisplaySN(long p_Priority, SNI::SNI_DisplayOptions &p_DisplayOptions) const
		{
			if (GetSNI_Base())
			{
				return GetSNI_Base()->DisplaySN(p_Priority, p_DisplayOptions);
			}
			return "";
		}

		string DisplaySN(SNI::SNI_DisplayOptions &p_DisplayOptions) const
		{
			if (GetSNI_Base())
			{
				return GetSNI_Base()->DisplaySN(0, p_DisplayOptions);
			}
			return "";
		}

		string DisplayValueSN() const
		{
			if (GetSNI_Base())
			{
				SNI::SNI_DisplayOptions l_DisplayOptions(doTextOnly);
				return GetSNI_Base()->DisplayValueSN(0, l_DisplayOptions);
			}
			return "";
		}

		string DisplayValueSN(SNI::SNI_DisplayOptions &p_DisplayOptions) const
		{
			if (GetSNI_Base())
			{
				return GetSNI_Base()->DisplayValueSN(0, p_DisplayOptions);
			}
			return "";
		}

		string LogDisplaySN() const
		{
			if (GetSNI_Base())
			{
				LOGGING(SNI::SNI_DisplayOptions l_DisplayOptions(doTextOnly));
				LOGGING(SN::LogContext context(GetSNI_Base()->DisplaySN(0, l_DisplayOptions)));
			}
			return "";
		}

		bool IsNull() const
		{
			return !GetSNI_Base() || GetSNI_Base()->IsNull();
		}

		bool IsNullValue() const
		{
			return !GetSNI_Base() || GetSNI_Base()->IsNullValue();
		}

		bool IsKnownValue() const
		{
			return GetSNI_Base() && GetSNI_Base()->IsKnownValue();
		}

		bool IsLeftKnownValue() const
		{
			return GetSNI_Base() && GetSNI_Base()->IsLeftKnownValue();
		}

		bool IsRightKnownValue() const
		{
			return GetSNI_Base() && GetSNI_Base()->IsRightKnownValue();
		}

		bool IsKnownTypeValue() const
		{
			return GetSNI_Base() && GetSNI_Base()->IsKnownTypeValue();
		}

		bool IsReferableValue() const
		{
			return m_Expression && m_Expression->IsReferableValue();
		}

		bool IsLambdaValue() const
		{
			return GetSNI_Base() && GetSNI_Base()->IsLambdaValue();
		}

		bool IsInline() const
		{
			return GetSNI_Base() && GetSNI_Base()->IsInline();
		}

		bool IsVariable() const
		{
			return GetSNI_Base() && GetSNI_Base()->IsVariable();
		}

		bool IsValueHolder() const
		{
			return GetSNI_Base() && GetSNI_Base()->IsValueHolder();
		}

		bool IsValueSet() const
		{
			return GetSNI_Base() && GetSNI_Base()->IsValueSet();
		}

		bool IsString() const
		{
			return GetSNI_Base() && GetSNI_Base()->IsString();
		}

		bool IsStringValue() const
		{
			return GetSNI_Base() && GetSNI_Base()->IsStringValue();
		}

		bool IsComplete() const
		{
			return GetSNI_Base() && GetSNI_Base()->IsComplete();
		}

		bool IsError() const
		{
			return GetSNI_Base() && GetSNI_Base()->IsError();
		}

		bool IsSignificantError()
		{
			return GetSNI_Base() && GetSNI_Base()->IsSignificantError();
		}

		bool IsFixed() const
		{
			return !GetSNI_Base() || GetSNI_Base()->IsFixed();
		}

		bool AllValuesBoolean() const
		{
			return GetSNI_Base() && GetSNI_Base()->AllValuesBoolean();
		}

		bool Equivalent(const Exp &p_Other) const
		{
			return GetSNI_Base()->Equivalent(dynamic_cast<SNI::SNI_Object *>(p_Other.GetSNI_Expression()));
		}

		Exp In(const Exp &p_Set) const
		{
			return SN_Operators::HasMember(p_Set, m_Expression);
		}

		Exp BuildMeta(const Exp p_MetaLevel) const
		{
			return SN_Operators::BuildMeta(m_Expression, p_MetaLevel);
		}

		Exp Notes(const Exp p_Before, const Exp p_After) const
		{
			return SN_Operators::Notes(m_Expression, p_Before, p_After);
		}

		size_t Cardinality(size_t p_MaxCardinality = CARDINALITY_MAX) const
		{
			if (!GetSNI_Base())
			{
				return p_MaxCardinality;
			}
			return GetSNI_Base()->Cardinality(p_MaxCardinality);
		}

		size_t LeftCardinality(size_t p_MaxCardinality = CARDINALITY_MAX) const
		{
			if (!GetSNI_Base())
			{
				return p_MaxCardinality;
			}
			return GetSNI_Base()->LeftCardinality(p_MaxCardinality);
		}

		size_t RightCardinality(size_t p_MaxCardinality = CARDINALITY_MAX) const
		{
			if (!GetSNI_Base())
			{
				return p_MaxCardinality;
			}
			return GetSNI_Base()->RightCardinality(p_MaxCardinality);
		}

		Exp GetSafeValue() const
		{
			if (GetSNI_Base())
			{
				return GetSNI_Base()->GetSafeValue();
			}
			return skynet::Null;
		}

		Exp GetValue() const
		{
			if (GetSNI_Base())
			{
				return GetSNI_Base()->GetValue();
			}
			return skynet::Null;
		}

		Exp DoEvaluate(long p_MetaLevel = 0) const
		{
			return GetSNI_Base()->DoEvaluate(p_MetaLevel);
		}

		Exp DoPartialEvaluate(long p_MetaLevel = 0) const
		{
			return GetSNI_Base()->DoPartialEvaluate(p_MetaLevel);
		}

		Err DoAssert()
		{
			return GetSNI_Base()->DoAssert();
		}

		Err AssertValue(const SN_Expression &p_Expression)
		{
			return GetSNI_Base()->AssertValue(p_Expression);
		}

		Err DoPartialAssert()
		{
			return GetSNI_Base()->DoPartialAssert();
		}

		Exp DoBuildMeta(long p_MetaLevel = 0)
		{
			return GetSNI_Base()->DoBuildMeta(p_MetaLevel);
		}

		Err PartialAssertValue(const Exp &p_Expression, bool p_Define = false)
		{
			return GetSNI_Base()->PartialAssertValue(p_Expression, p_Define);
		}

		void Throw()
		{
			return GetSNI_Base()->Throw();
		}

		Err GetError() const
		{
			SNI_Error *err = dynamic_cast<SNI_Error *>(GetSNI_Base());
			if (err)
			{
				return Err(err);
			}
			SNI_Bool *result = dynamic_cast<SNI_Bool *>(GetSNI_Base());
			if (result)
			{
				return Err(result->GetBool(), false);
			}
			return Err(false, false, "Error object expected.");
		}

		void Do()
		{
			GetSNI_Base()->Do();
		}

		void Fix(Exp p_Value)
		{
			GetSNI_Base()->Fix(p_Value);
		}

		void Fix()
		{
			GetSNI_Base()->Fix();
		}
	};
}
#endif // !defined(SN_BASE_H_INCLUDED)
