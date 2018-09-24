#include "sni_meta.h"

#include "logcontext.h"
#include "sn_expression.h"
#include "sn_meta.h"
#include "sn_error.h"

#include "sni_helpers.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ SNI_Class *SNI_Meta::m_Class = NULL;
	/*static*/ SNI_Class *SNI_Meta::Class()
	{
		if (!m_Class)
		{
			m_Class = new SNI_Class();
		}
		return m_Class;
	}

	SNI_Meta::SNI_Meta()
	{
	}

	SNI_Meta::SNI_Meta(long p_DeltaMetaLevel, SNI_Expression *p_Expression)
		: m_DeltaMetaLevel(p_DeltaMetaLevel)
		, m_Expression(p_Expression)
	{
	}

	SNI_Meta::~SNI_Meta()
	{
	}

	string SNI_Meta::GetTypeName() const
	{
		return "Meta";
	}

	string SNI_Meta::DisplayCpp() const
	{
		return GetTypeName() + "(" + m_Expression->DisplayCpp() + ")";
	}

	string SNI_Meta::DisplaySN(long /*priority*/, SNI_DisplayOptions &p_DisplayOptions) const
	{
		string value = m_Expression->DisplaySN(GetPriority(), p_DisplayOptions);
		if (m_DeltaMetaLevel < 0)
		{
			return string(-m_DeltaMetaLevel, '}') + value + string(-m_DeltaMetaLevel, '{');
		}
		else if (m_DeltaMetaLevel == 0)
		{
			return "{}" + value + "{}";
		}
		else if (m_DeltaMetaLevel > 0)
		{
			return string(m_DeltaMetaLevel, '{') + value + string(m_DeltaMetaLevel, '}');
		}
		FORCE_ASSERTM("Internal error: This path is impossible");
		return "";
	}

	long SNI_Meta::GetPriority() const
	{
		return 0;
	}

	SNI_Expression * SNI_Meta::GetExpression()
	{
		return m_Expression;
	}

	bool SNI_Meta::Equivalent(SNI_Object * p_Other) const
	{
		if (dynamic_cast<SNI_Meta *>(p_Other))
		{
			SNI_Meta * other = dynamic_cast<SNI_Meta *>(p_Other);

			return m_DeltaMetaLevel == other->m_DeltaMetaLevel && m_Expression->Equivalent(dynamic_cast<SNI_Object *>(other->m_Expression));
		}
		return false;
	}

	// Inheritance
	SN::SN_Error  SNI_Meta::AssertIsAValue(const SNI_Value * p_Parent, SN::SN_Expression p_Result)
	{
		return p_Result.AssertValue(Class()->DoIsA(p_Parent));
	}

	SN::SN_Value SNI_Meta::DoIsA(const SNI_Value * p_Parent) const
	{
		return Class()->DoIsA(p_Parent);
	}

	SN::SN_Expression SNI_Meta::Evaluate(long p_MetaLevel /* = 0 */) const
	{
		long l_DeltaMetaLevel = m_DeltaMetaLevel;
		long this_level = p_MetaLevel + l_DeltaMetaLevel;
		SN::SN_Expression result = m_Expression->Evaluate(this_level);
		if (l_DeltaMetaLevel != 0)
		{
			if ((p_MetaLevel < 0) && (l_DeltaMetaLevel > 0))
			{
				l_DeltaMetaLevel += p_MetaLevel;
				if (l_DeltaMetaLevel < 0)
				{
					l_DeltaMetaLevel = 0;
				}
			}
			if ((this_level <= 0) && (l_DeltaMetaLevel < 0))
			{
				l_DeltaMetaLevel = 0;
			}
			if (l_DeltaMetaLevel != 0)
			{
				return SN::SN_Meta(l_DeltaMetaLevel, result);
			}
		}
		return result;
	}

	SN::SN_Expression SNI_Meta::PartialEvaluate(long p_MetaLevel /* = 0 */) const
	{
		long l_DeltaMetaLevel = m_DeltaMetaLevel;
		long this_level = p_MetaLevel + l_DeltaMetaLevel;
		SN::SN_Expression result = m_Expression->PartialEvaluate(this_level);
		if (l_DeltaMetaLevel != 0)
		{
			if ((p_MetaLevel < 0) && (l_DeltaMetaLevel > 0))
			{
				l_DeltaMetaLevel += p_MetaLevel;
				if (l_DeltaMetaLevel < 0)
				{
					l_DeltaMetaLevel = 0;
				}
			}
			if ((this_level <= 0) && (l_DeltaMetaLevel < 0))
			{
				l_DeltaMetaLevel = 0;
			}
			if (l_DeltaMetaLevel != 0)
			{
				return SN::SN_Meta(l_DeltaMetaLevel, result);
			}
		}
		return result;
	}

	SN::SN_Expression SNI_Meta::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Meta::Call ( " + DisplayPmExpressionList(p_ParameterList) + " )");

		return m_Expression->Call(p_ParameterList, p_MetaLevel + m_DeltaMetaLevel);
	}

	SN::SN_Expression SNI_Meta::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		SN::LogContext context(DisplaySN0() + ".SNI_Meta::PartialCall ( " + DisplayPmExpressionList(p_ParameterList) + " )");

		return m_Expression->PartialCall(p_ParameterList, p_MetaLevel + m_DeltaMetaLevel);
	}

	SN::SN_Error SNI_Meta::PartialAssertValue(const SN::SN_Expression &p_Expression, bool /* p_Define = false */)
	{
		return m_Expression->PartialAssertValue(p_Expression, true);
	}

	SN::SN_Expression SNI_Meta::Unify(SN::SN_ExpressionList * p_ParameterList)
	{
		return m_Expression->Unify(p_ParameterList);
	}

	SN::SN_Error SNI_Meta::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result)
	{
		return m_Expression->PartialUnify(p_ParameterList, p_Result);
	}
}
