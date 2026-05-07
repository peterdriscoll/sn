#if !defined(SNI_DOMAIN_H_INCLUDED)
#define SNI_DOMAIN_H_INCLUDED

#pragma once

#include <unordered_map>

#include "sni_value.h"
#include "sni_variable.h"

namespace SN
{
	class SN_Expression;
	class SN_Variable;
}

namespace SNI
{
	class SNI_Class;
	class SNI_Expression;

	class SNI_Domain : public SNI_Value
	{
		PGC_CLASS(SNI_Domain);

	public:
		static SNI_Class* PeekClass();
		static SNI_Class* Class();
        virtual SN::SN_Expression Type();
        virtual SN::SN_Expression Type() const;

		static SNI_Domain* GetCurrent();

		SNI_Domain(const std::string &p_Name);
		SNI_Domain(SNI_Domain* p_ParentDomain, const std::string &p_Name);
		virtual ~SNI_Domain();

		void Clear();

		virtual std::string GetTypeName() const;
		virtual std::string DisplayCpp() const;
		virtual long GetPriority() const;
		virtual std::string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual size_t Hash() const;
        void WriteJSON(std::ostream &p_Stream, size_t p_DebugFieldWidth, SNI::SNI_DisplayOptions &p_DisplayOptions);

		void to_json(
			nlohmann::json &j,
			size_t p_DebugFieldWidth,
			SNI::SNI_DisplayOptions &p_DisplayOptions) const;

		virtual SN::SN_Error AssertSubscriptValue(const SNI_Value *p_Index, SN::SN_Expression p_Result);
		virtual SN::SN_Expression operator[](const std::string & p_Index);
		virtual SN::SN_Value DoSubscriptCall(const SN::SN_Value &p_Index) const;
		virtual SN::SN_Value DoReverseSubscript(const SN::SN_Value & p_Result) const;

		virtual SN::SN_Variable LookupVariable(const std::string & p_Name);
		virtual SN::SN_Domain LookupDomain(const std::string & p_Name);

		SN::SN_Domain DeclareDomain(const std::string &p_Index);
        SN::SN_Domain LocalDomain(const std::string &p_Name);

		SN::SN_Variable DeclareVariable(const std::string & p_Index);
		SN::SN_Variable LocalVariable(const std::string & p_Index);

	protected:
        virtual void PromoteMembers();

	private:
		SNI_Domain *m_ParentDomain = nullptr;
		SNI_Domain *m_LastDomain = nullptr;
		std::unordered_map<std::string, SN::SN_Expression> m_Map;
		std::string m_Name;
	};
}

#endif // !defined(SNI_DOMAIN_H_INCLUDED)
