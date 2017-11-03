#include "sni_set.h"

#include "logcontext.h"
#include "sn_value.h"
#include "sn_set.h"

#include "sni_valueset.h"
#include "sni_error.h"
#include "sni_null.h"
#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Set::SNI_Set()
	{
	}

	SNI_Set::SNI_Set(const SNI_Set &)
	{
	}

	SNI_Set::SNI_Set(const SN::SN_Value &p_Value)
	{
		SN::LogContext context("SNI_Set::SNI_Set ( " + p_Value.DisplaySN() + " )");
		SN::SN_Value value = p_Value;
		SNI_Set *local_this = this;
		value.ForEach(
			[local_this](const SN::SN_Expression &p_Param, SNI::SNI_World *p_World) -> SN::SN_Error
		{
			if (!local_this->HasMember(p_Param))
			{
				local_this->m_SetList.push_back(p_Param);
			}
			return true;
		});
	}

	SNI_Set::~SNI_Set()
	{

	}

	void SNI_Set::PromoteMembers()
	{
	}

	string SNI_Set::GetTypeName() const
	{
		return "Set";
	}

	string SNI_Set::DisplayCpp() const
	{
		return "sn_Set( {" + DisplayPmValueList(m_SetList) + "} )";
	}

	string SNI_Set::DisplaySN(long /*priority*/, SNI_VariablePointerList & /*p_DisplayVariableList*/) const
	{
		return "{" + DisplayPmValueList(m_SetList) + "}";
	}

	long SNI_Set::GetPriority() const
	{
		return 100;
	}


	SN::SN_Expression SNI_Set::Evaluate(long /* p_MetaLevel /* = 0 */) const
	{
		return SN::SN_Expression(this);
	}

	SN::SN_Expression SNI_Set::PartialEvaluate(long /* p_MetaLevel = 0 */) const
	{
		return SN::SN_Expression(this);
	}

	SN::SN_Set SNI_Set::Remove(const SN::SN_Value &p_Other)
	{
		SN::LogContext context("SNI_Set::DoRemove ( " + DisplayPmValueList(m_SetList) + " )");

		SN::SN_Set Set;
		for (size_t i = 0; i<m_SetList.size(); i++)
		{
			if (!p_Other.Equivalent(m_SetList[i]))
			{
				Set.GetSNI_Set()->AddToSet(m_SetList[i]);
			}
		}
		return LOG_RETURN(context, Set);
	}

	bool SNI_Set::Equivalent(SNI_Object * p_Other) const
	{
		if (dynamic_cast<SNI_Set *>(p_Other))
		{
			SNI_Set * l_SetList = dynamic_cast<SNI_Set *>(p_Other);
			if (m_SetList.size() == l_SetList->Length())
			{
				for (size_t i = 0; i<m_SetList.size(); i++)
				{
					if (!l_SetList->HasMember(m_SetList[i]))
					{
						return false;
					}
				}
				return true;
			}
		}
		return false;
	}

	size_t SNI_Set::Length() const
	{
		return m_SetList.size();
	}

	bool SNI_Set::HasMember(const SN::SN_Expression &p_Value) const
	{
		SN::SN_Value value = p_Value;
		for (size_t j = 0; j<m_SetList.size(); j++)
		{
			if (!value.IsNull())
			{
				if (value.Equivalent(m_SetList[j]))
				{
					return true;
				}
			}
		}
		return false;
	}

	SN::SN_Value SNI_Set::DoHasMember(SNI_Value * p_Value) const
	{
		return SN::SN_Bool(HasMember(p_Value));
	}

	bool SNI_Set::DoIsEmpty() const
	{
		return m_SetList.size() == 0;
	}


	SN::SN_Value SNI_Set::operator[](long p_index)
	{
		return m_SetList[p_index];
	}

	SN::SN_Value SNI_Set::At(const long &p_Value)
	{
		return m_SetList.at(p_Value);
	}

	void SNI_Set::AddToSet(const SN::SN_Value &p_Value)
	{
		m_SetList.push_back(p_Value);
	}
}
