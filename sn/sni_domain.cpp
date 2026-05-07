#include "sni_domain.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sn_value.h"

#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ SNI_Class* SNI_Domain::PeekClass()
    {
        return SNI_User::GetCurrentUser()->GetPointer<SNI_Domain, SNI_Class>();
    }

    /*static*/ SNI_Class* SNI_Domain::Class()
	{
		return SNI_User::GetCurrentUser()->GetOrCreatePointer<SNI_Domain, SNI_Class>("Domain");
	}

	SN::SN_Expression SNI_Domain::Type() const
    {
        return PeekClass();
    }

    SN::SN_Expression SNI_Domain::Type()
	{
		return Class();
	}

	/*static*/ SNI_Domain* SNI_Domain::GetCurrent()
	{
		return SNI_Thread::GetThread()->GetDomain();
	}

	SNI_Domain::SNI_Domain(const std::string &p_Name)
		: m_Name(p_Name)
	{
		m_LastDomain = SNI_Thread::GetThread()->GetDomain();
		SNI_Thread::GetThread()->SetDomain(this);
    }

    SNI_Domain::SNI_Domain(SNI_Domain *p_ParentDomain, const std::string &p_Name)
		: m_Name(p_Name)
        , m_ParentDomain(p_ParentDomain)
    {
    }

	SNI_Domain::~SNI_Domain()
	{
		SNI_Thread::GetThread()->SetDomain(m_LastDomain);
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

	void SNI_Domain::WriteJSON(std::ostream &p_Stream, size_t p_DebugFieldWidth, SNI::SNI_DisplayOptions &p_DisplayOptions)
    {
		for (auto &pair : m_Map)
		{
            pair.second.GetSNI_Variable()->WriteJSON(p_Stream, "\t", p_DebugFieldWidth, p_DisplayOptions);
		}
    }

	void SNI_Domain::to_json(
		nlohmann::json &j,
		size_t p_DebugFieldWidth,
        SNI::SNI_DisplayOptions &p_DisplayOptions) const
	{
		for (auto const& [name, var] : m_Map) // Using modern structured bindings
		{
			var.GetSNI_Variable()->to_json(j[name], p_DebugFieldWidth, p_DisplayOptions);
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
		return 0; // std::_Hash_representation(m_Map);
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
			SN::SN_Variable v(SN::SN_Domain(this), p_Index);
			const_cast<SNI_Domain *>(this)->m_Map.emplace(p_Index, v);
			return v;
		}
		return m_Map.at(p_Index);
	}

	SN::SN_Variable SNI_Domain::LookupVariable(const std::string & p_Name)
	{
		auto it = m_Map.find(p_Name);
		if (it == m_Map.end())
		{
			SN::SN_Variable v(SN::SN_Domain(this), p_Name);
			m_Map.emplace(p_Name, v);
			return v;
		}
		return SN::SN_Variable(m_Map.at(p_Name).GetSNI_Variable());
	}

	SN::SN_Domain SNI_Domain::LookupDomain(const std::string & p_Name)
	{
		auto it = m_Map.find(p_Name);
		if (it == m_Map.end())
		{
			SN::SN_Domain d(this, p_Name);
			m_Map.emplace(p_Name, d);
			return d;
		}
		return SN::SN_Domain(m_Map.at(p_Name).GetSNI_Domain());
	}

	SN::SN_Domain SNI_Domain::DeclareDomain(const std::string & p_Name)
    {
		auto it = m_Map.find(p_Name);
		bool alreadyDeclared  = it != m_Map.end();
		if (alreadyDeclared )
        {
			HandleAction(
				SN::SN_Error(false, false, "Sub-domain '" + p_Name + "' is already declared in domain '" + m_Name + "'. Use SN_LOCAL_SUBDOMAIN for a domain not added to the current domain in this scope."),
				SNI_Thread::TopManager()->ErrorHandler());

			ASSERTM(false, "Shouldn't be here.");
		}

		SN::SN_Domain d(this, p_Name);
		m_Map.emplace(p_Name, d);
		return d;
	}

	SN::SN_Domain SNI_Domain::LocalDomain(const std::string & p_Name)
    {
		return SN::SN_Domain(this, p_Name);
	}


	SN::SN_Variable SNI_Domain::DeclareVariable(const std::string & p_Name)
    {
		auto it = m_Map.find(p_Name);
		bool alreadyDeclared  = it != m_Map.end();
		if (alreadyDeclared)
        {
			HandleAction(
				SN::SN_Error(false, false, "Variable '" + p_Name + "' is already declared in domain '" + m_Name + "'. Use SN_LOCAL for a variable not added to the current domain in this scope."),
				SNI_Thread::TopManager()->ErrorHandler());

			ASSERTM(false, "Shouldn't be here.");
		}

		SN::SN_Variable v(this, p_Name);
		m_Map.emplace(p_Name, v);
		return v;
	}

	SN::SN_Variable SNI_Domain::LocalVariable(const std::string & p_Name)
    {
		return SN::SN_Variable(this, p_Name);
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
