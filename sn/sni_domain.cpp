#include "sni_domain.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sn_value.h"

#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ SNI_Class* SNI_Domain::Class()
	{
		return SNI_User::GetCurrentUser()->GetOrCreatePointer<SNI_Domain, SNI_Class>("Domain");
	}

	SNI_Class *SNI_Domain::VClass()
	{
		return Class();
	}

	SNI_Domain::SNI_Domain(const std::string &p_Name)
		: m_Name(p_Name)
	{
	}

	SNI_Domain::~SNI_Domain()
	{
	}

	void SNI_Domain::Clear()
	{
		m_Map.clear();
	}

	void SNI_Domain::PromoteMembers()
	{
		for (auto &pair : m_Map)
		{
			REQUESTPROMOTION(pair.first);
			REQUESTPROMOTION(pair.second);
		}
	}

	std::string SNI_Domain::GetTypeName() const
	{
		return "Domain";
	}

	std::string SNI_Domain::DisplayCpp() const
	{
		return m_Name;
	}

	std::string SNI_Domain::DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const
	{
		return m_Name;
	}

	long SNI_Domain::GetPriority() const
	{
		return 3;
	}

	bool SNI_Domain::Equivalent(SNI_Object * p_Other) const
	{
		return false;
	}

	size_t SNI_Domain::Hash() const
	{
		return 0; // _Hash_representation(m_Map);
	}

	SN::SN_Error SNI_Domain::AssertSubscriptValue(const SNI_Value * p_Index, SN::SN_Expression p_Result)
	{
		return skynet::OK;
	}

	SN::SN_Expression SNI_Domain::operator[](const std::string & p_Index)
	{
		auto it = m_Map.find(p_Index);
		if (it == m_Map.end())
		{
			SN::SN_Variable v;
			v.SetName(p_Index);
			v.SetDomainName(m_Name);
			const_cast<SNI_Domain *>(this)->m_Map.emplace(p_Index, v);
			return v;
		}
		return m_Map.at(p_Index);
	}

	// Functions
	SN::SN_Value SNI_Domain::DoSubscriptCall(const SN::SN_Value & p_Index) const
	{
		auto it = m_Map.find(p_Index.GetString());
		if (it == m_Map.end())
		{
			SN::SN_Variable v(p_Index.GetString(), m_Name);
			const_cast<SNI_Domain *>(this)->m_Map.emplace(p_Index.GetString(), v);
			return SN::SN_Meta(1, v);
		}
		return SN::SN_Meta(1, m_Map.at(p_Index.GetString()));
	}

	SN::SN_Value SNI_Domain::DoReverseSubscript(const SN::SN_Value & p_Result) const
	{
		return SN::SN_Error(false, false, GetTypeName() + "Name not given.");
	}
}
