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
		{
		}

		SN_Base(T * p_Expression)
			: m_Expression(p_Expression)
		{
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

		bool IsVariable() const
		{
			return GetSNI_Base() && GetSNI_Base()->IsVariable();
		}

		bool IsString() const
		{
			return GetSNI_Base() && GetSNI_Base()->IsString();
		}

		bool IsStringValue() const
		{
			return GetSNI_Base() && GetSNI_Base()->IsStringValue();
		}

		bool IsError() const
		{
			return GetSNI_Base() && GetSNI_Base()->IsError();
		}

		bool IsFixed() const
		{
			return !GetSNI_Base() || GetSNI_Base()->IsFixed();
		}

		bool Equivalent(const Exp &p_Other) const
		{
			return GetSNI_Base()->Equivalent(dynamic_cast<SNI::SNI_Object *>(p_Other.GetSNI_Expression()));
		}

		Exp In(const Exp &p_Set) const
		{
			return SN_Operators::HasMember(p_Set, m_Expression);
		}

		Exp Meta(const Exp p_MetaLevel) const
		{
			return SN_Operators::Meta(m_Expression, p_MetaLevel);
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

		Err DoMeta(long p_MetaLevel = 0)
		{
			return GetSNI_Base()->DoMeta(p_MetaLevel);
		}

		Err PartialAssertValue(const Exp &p_Expression, bool p_Define = false)
		{
			return GetSNI_Base()->PartialAssertValue(p_Expression, p_Define);
		}

		void Throw()
		{
			return GetSNI_Base()->Throw();
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
