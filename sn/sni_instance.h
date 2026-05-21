#if !defined(SNI_INSTANCE_H_INCLUDED)
#define SNI_INSTANCE_H_INCLUDED

#pragma once

#include <string>

#include "sni_value.h"
#include "sni_namable.h"

namespace SNI
{
	class SNI_Instance : public SNI_Value, public SNI_Namable
	{
		PGC_CLASS(SNI_Instance);

	public:
		virtual SN::SN_Expression Type();
        virtual SN::SN_Expression Type() const;
        virtual SN::SN_Error SetType(SNI_Class *p_Class);

		SNI_Instance();
		SNI_Instance(const SNI_Instance &p_Other);
		SNI_Instance(const std::string &p_Name, const std::string &p_DomainName);
        SNI_Instance(SNI_Domain *p_Domain, const std::string &p_Name);

		void InitClass(SNI_Expression *p_Class);
		virtual ~SNI_Instance();

		SNI_Expression * Copy() const;

		virtual std::string GetTypeName() const;
		virtual std::string DisplayCpp() const;
		virtual std::string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

		virtual bool IsKnownTypeValue() const;
        virtual bool HasSyntheticType() const;
		virtual bool SupportsTypeNarrowing() const;

		// Inheritance
		virtual SN::SN_Error AssertIsAValue(const SNI_Value * p_Parent, SN::SN_Expression p_Result);
		virtual SN::SN_Value DoIsA(const SNI_Value * p_Parent) const;
		virtual SN::SN_Value DoHasA(SNI_Value * p_Member, SNI_Value * p_Name) const;

		void Fix();

	private:
		virtual void PromoteMembers();

		SNI_Domain *m_Domain = nullptr;
		SN::SN_Class m_Class;
		bool m_HasSyntheticType = false;
		bool m_Fixed = false;
	};

	typedef std::vector<SNI_Instance *> SNI_InstanceList;
}

#endif // !defined(SNI_BOOL_H_INCLUDED)
