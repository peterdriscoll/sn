#include "sni_metatype.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sn_expression.h"
#include "sn_meta.h"
#include "sn_error.h"

#include "sni_helpers.h"
#include "utility.h"
#include "sni_user.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ SNI_Class* SNI_MetaType::Class()
	{
		return SNI_User::GetCurrentUser()->GetOrCreatePointer<SNI_MetaType, SNI_Class>("MetaType");
	}

	SN::SN_Expression SNI_MetaType::Type() const
	{
		return Class();
	}

	SNI_MetaType::SNI_MetaType()
		: m_DeltaMetaLevel(0)
		, m_Expression(NULL)
	{
	}

	SNI_MetaType::SNI_MetaType(long p_DeltaMetaLevel, SNI_Expression *p_Expression)
		: m_DeltaMetaLevel(p_DeltaMetaLevel)
		, m_Expression(p_Expression)
	{
	}

	SNI_MetaType::~SNI_MetaType()
	{
	}

	std::string SNI_MetaType::GetTypeName() const
	{
		return "MetaType";
	}

	std::string SNI_MetaType::DisplayCpp() const
	{
		return GetTypeName() + "(" + m_Expression->DisplayCpp() + ")";
	}

	std::string SNI_MetaType::DisplaySN(long /*priority*/, SNI_DisplayOptions &p_DisplayOptions) const
	{
		std::string value = "null";
		if (m_Expression)
		{
			value = m_Expression->DisplaySN(GetPriority(), p_DisplayOptions);
        }

		if (m_DeltaMetaLevel < 0)
		{
			return std::string(-m_DeltaMetaLevel, '}') + value + std::string(-m_DeltaMetaLevel, '{');
		}
		else if (m_DeltaMetaLevel == 0)
		{
			return "{}" + value + "{}";
		}
		else if (m_DeltaMetaLevel > 0)
		{
			return std::string(m_DeltaMetaLevel, '{') + value + std::string(m_DeltaMetaLevel, '}');
		}
		FORCE_ASSERTM("Internal error: This path is impossible");
		return "";
	}

	void SNI_MetaType::AddVariables(long p_MetaLevel, SNI_VariablePointerMap& p_Map)
	{
		SNI_Expression* expression = m_Expression;
		if (expression)
		{
			expression->AddVariables(p_MetaLevel+m_DeltaMetaLevel,p_Map);
		}
	}

	long SNI_MetaType::GetPriority() const
	{
		return 0;
	}

	bool SNI_MetaType::IsMetaType() const
	{
		return true;
	}

	bool SNI_MetaType::IsKnownValue() const
	{
		return 0 < m_DeltaMetaLevel;
	}

	SN::SN_Value SNI_MetaType::DoIsA(const SNI_Value* p_Parent) const
	{
		if (Class()->DoIsA(p_Parent).GetBool())
		{
			return skynet::True;
		}
		if (m_Expression->DoIsA(p_Parent->DoEvaluate(-m_DeltaMetaLevel).GetSNI_Value()).GetBool())
		{
			return skynet::True;
		}
		return skynet::False;
	}

	size_t SNI_MetaType::Cardinality(size_t p_MaxCardinality) const
	{
		if (0 < m_DeltaMetaLevel)
		{
			return 1;
		}
		return p_MaxCardinality;
	}

	SNI_Expression * SNI_MetaType::GetExpression()
	{
		return m_Expression;
	}

	bool SNI_MetaType::Equivalent(SNI_Object * p_Other) const
	{
		if (dynamic_cast<SNI_MetaType *>(p_Other))
		{
			SNI_MetaType * other = dynamic_cast<SNI_MetaType *>(p_Other);

			return m_DeltaMetaLevel == other->m_DeltaMetaLevel && m_Expression->Equivalent(dynamic_cast<SNI_Object *>(other->m_Expression));
		}
		return false;
	}

	size_t SNI_MetaType::Hash() const
	{
		std::string data = DisplaySN0();
		return _Hash_array_representation(data.c_str(), data.size());
	}

	SNI_Expression *SNI_MetaType::Clone(long p_MetaLevel, SNI_Frame *p_Frame, bool &p_Changed)
	{
		bool changed = false;
		SNI_Expression * l_Meta = m_Expression->Clone(p_MetaLevel+ m_DeltaMetaLevel, p_Frame, changed);
		if (changed)
		{
			p_Changed = true;
			return new SNI_MetaType(m_DeltaMetaLevel, l_Meta);
		}
		return this;
	}


	SN::SN_Expression SNI_MetaType::DoEvaluate(long p_MetaLevel /* = 0 */) const
	{
		long thisLevel = p_MetaLevel + m_DeltaMetaLevel;
		SN::SN_Expression result = m_Expression->DoEvaluate(thisLevel);
		if (m_DeltaMetaLevel)
		{
			if (result.IsMetaType())
			{ // If the result is a meta, then combine this meta with it, simplifying one Meta within a Meta to a single one. 
				SNI_MetaType* meta_result = result.GetSNI_MetaType();
				return meta_result->CombineMetaValues(m_DeltaMetaLevel);
			}
			if (0 < p_MetaLevel || 0 < thisLevel)
			{
				return SN::SN_MetaType(m_DeltaMetaLevel, result);
			}
		}
		return result;
	}

	SN::SN_Expression SNI_MetaType::DoPartialEvaluate(long p_MetaLevel /* = 0 */) const
	{
		long thisLevel = p_MetaLevel + m_DeltaMetaLevel;
		SN::SN_Expression result = m_Expression->DoPartialEvaluate(thisLevel);
		if (m_DeltaMetaLevel)
		{
			if (result.IsMeta())
			{ // If the result is a meta, then combine this meta with it, simplifying one Meta within a Meta to a single one. 
				SNI_MetaType *meta_result = result.GetSNI_MetaType();
				return meta_result->CombineMetaValues(m_DeltaMetaLevel);
			}
			if (0 < p_MetaLevel || 0 < thisLevel)
			{
				return SN::SN_MetaType(m_DeltaMetaLevel, result);
			}
		}
		return result;
	}

	SN::SN_Expression SNI_MetaType::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_MetaType::Call ( " + DisplaySnExpressionList(p_ParameterList) + " )"));

		return LOG_RETURN(context, m_Expression->Call(p_ParameterList, p_MetaLevel + m_DeltaMetaLevel));
	}

	SN::SN_Expression SNI_MetaType::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		LOGGING(SN::LogContext context(DisplaySN0() + ".SNI_MetaType::PartialCall ( " + DisplaySnExpressionList(p_ParameterList) + " )"));

		return LOG_RETURN(context, m_Expression->PartialCall(p_ParameterList, p_MetaLevel + m_DeltaMetaLevel));
	}

	SN::SN_Error SNI_MetaType::PartialAssertValue(const SN::SN_Expression &p_Expression, bool /* p_Define = false */)
	{
		return m_Expression->PartialAssertValue(p_Expression, true);
	}

	SN::SN_Expression SNI_MetaType::Unify(SN::SN_ExpressionList * p_ParameterList)
	{
		return m_Expression->Unify(p_ParameterList);
	}

	SN::SN_Error SNI_MetaType::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		return m_Expression->PartialUnify(p_ParameterList, p_Result);
	}

	SN::SN_Expression SNI_MetaType::CombineMetaValues(long p_DeltaMetaLevel)
	{
		m_DeltaMetaLevel += p_DeltaMetaLevel;
		if (m_DeltaMetaLevel)
		{
			return this;
		}
		return m_Expression;
	}
} // namespace SNI
