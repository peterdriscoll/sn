#if !defined(SN_DOMAIN_H_INCLUDED)
#define SN_DOMAIN_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_base.h"
#include "sn_class.h"

namespace SNI
{
	class SNI_Domain;
}

namespace SN
{
	class SN_Expression;
	class SN_Value;

	class SN_EXPORT SN_Domain : public SN_Base
	{
	public:
        static SN_Class ExprClass(); 
		static SN_Class Class();

        static SN_Domain GetCurrent();

        SN_Domain(SNI::SNI_Domain *p_Domain);
		SN_Domain(SNI::SNI_Domain *p_Domain, const std::string &p_Name);

		SN_Domain(const std::string &p_Name);
        virtual ~SN_Domain();

        SN_Domain DeclareDomain(const std::string &p_Name);
        SN_Domain LocalDomain(const std::string &p_Name);

	    SN_Variable DeclareVariable(const std::string &p_Name);
        SN_Variable LocalVariable(const std::string &p_Name);
		
		SN_Expression operator [](const std::string &p_Index);
		SN_Expression CreateMetaVariable(const SN_Expression &p_Index);
        SN_Variable LookupVariable(const std::string &p_Index);
        SN_Domain LookupDomain(const std::string &p_Index);
		SN_Expression IsA(const SN_Expression &p_Parent) const;

		SNI::SNI_Domain* GetSNI_Domain();

		SNI::SNI_Domain * GetSNI_Domain() const;
	};
}

#endif // !defined(SN_DOMAIN_H_INCLUDED)
