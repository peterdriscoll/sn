#include "sni_meta.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

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
			m_Class = new SNI_Class("Meta");
		}
		return m_Class;
	}

	SNI_Class *SNI_Meta::VClass()
	{
		return Class();
	}

	SNI_Meta::SNI_Meta()
		: m_DeltaMetaLevel(0)
		, m_Expression(NULL)
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
		if (!m_Expression)
		{
			return "{null}";
		}
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

	void SNI_Meta::AddVariables(long p_MetaLevel, SNI_VariablePointerMap& p_Map)
	{
		SNI_Expression* expression = m_Expression;
		if (expression)
		{
			expression->AddVariables(p_MetaLevel+m_DeltaMetaLevel,p_Map);
		}
	}

	long SNI_Meta::GetPriority() const
	{
		return 0;
	}

	bool SNI_Meta::IsMeta() const
	{
		return true;
	}

	bool SNI_Meta::IsKnownValue() const
	{
		return 0 < m_DeltaMetaLevel;
	}

	size_t SNI_Meta::Cardinality(size_t p_MaxCardinality) const
	{
		if (0 < m_DeltaMetaLevel)
		{
			return 1;
		}
		return p_MaxCardinality;
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

	size_t SNI_Meta::Hash() const
	{
		string data = DisplaySN0();
		return _Hash_array_representation(data.c_str(), data.size());
	}

	SNI_Expression *SNI_Meta::Clone(long p_MetaLevel, SNI_Frame *p_Frame, bool &p_Changed)
	{
		bool changed = false;
		SNI_Expression * l_Meta = m_Expression->Clone(p_MetaLevel+ m_DeltaMetaLevel, p_Frame, changed);
		if (changed)
		{
			p_Changed = true;
			return new SNI_Meta(m_DeltaMetaLevel, l_Meta);
		}
		return this;
	}


	SN::SN_Expression SNI_Meta::DoEvaluate(long p_MetaLevel /* = 0 */) const
	{
		long thisLevel = p_MetaLevel + m_DeltaMetaLevel;
		SN::SN_Expression result = m_Expression->DoEvaluate(thisLevel);
		if (m_DeltaMetaLevel)
		{
			if (result.IsMeta())
			{ // If the result is a meta, then combine this meta with it, simplifying one Meta within a Meta to a single one. 
				SNI_Meta* meta_result = result.GetSNI_Meta();
				return meta_result->CombineMetaValues(m_DeltaMetaLevel);
			}
			if (!result.IsVariable() && result.IsKnownValue())
			{ // Meta wraps an expression so that it can be treated as a value. But if the result is already a value, no need for the Meta.
				return result;
			}
			if (0 < p_MetaLevel || 0 < thisLevel)
			{
				return SN::SN_Meta(m_DeltaMetaLevel, result);
			}
		}
		return result;
	}

	SN::SN_Expression SNI_Meta::DoPartialEvaluate(long p_MetaLevel /* = 0 */) const
	{
		long thisLevel = p_MetaLevel + m_DeltaMetaLevel;
		SN::SN_Expression result = m_Expression->DoPartialEvaluate(thisLevel);
		if (m_DeltaMetaLevel)
		{
			if (result.IsMeta())
			{ // If the result is a meta, then combine this meta with it, simplifying one Meta within a Meta to a single one. 
				SNI_Meta *meta_result = result.GetSNI_Meta();
				return meta_result->CombineMetaValues(m_DeltaMetaLevel);
			}
			if (!result.IsVariable() && result.IsKnownValue())
			{ // Meta wraps an expression so that it can be treated as a value. But if the result is already a value, no need for the Meta.
				return result;
			}
			if (0 < p_MetaLevel || 0 < thisLevel)
			{
				return SN::SN_Meta(m_DeltaMetaLevel, result);
			}
		}
		return result;
	}

	SN::SN_Expression SNI_Meta::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Meta::Call ( " + DisplaySnExpressionList(p_ParameterList) + " )"));

		return LOG_RETURN(context, m_Expression->Call(p_ParameterList, p_MetaLevel + m_DeltaMetaLevel));
	}

	SN::SN_Expression SNI_Meta::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_Meta::PartialCall ( " + DisplaySnExpressionList(p_ParameterList) + " )"));

		return LOG_RETURN(context, m_Expression->PartialCall(p_ParameterList, p_MetaLevel + m_DeltaMetaLevel));
	}

	SN::SN_Error SNI_Meta::PartialAssertValue(const SN::SN_Expression &p_Expression, bool /* p_Define = false */)
	{
		return m_Expression->PartialAssertValue(p_Expression, true);
	}

	SN::SN_Expression SNI_Meta::Unify(SN::SN_ExpressionList * p_ParameterList)
	{
		return m_Expression->Unify(p_ParameterList);
	}

	SN::SN_Error SNI_Meta::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		return m_Expression->PartialUnify(p_ParameterList, p_Result);
	}

	SN::SN_Expression SNI_Meta::CombineMetaValues(long p_DeltaMetaLevel)
	{
		m_DeltaMetaLevel += p_DeltaMetaLevel;
		if (m_DeltaMetaLevel)
		{
			return this;
		}
		return m_Expression;
	}
}
