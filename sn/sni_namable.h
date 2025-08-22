#if !defined(SNI_NAMABLE_H_INCLUDED)
#define SNI_NAMABLE_H_INCLUDED

#pragma once

#include "ref.h"

#include <string>

namespace SNI
{
	class SNI_Namable
	{
	public:
		SNI_Namable();
		SNI_Namable(const std::string & p_Name);
		virtual ~SNI_Namable();

		std::string GetName() const;
		void SetName(const std::string &p_Name);

		std::string GetDomainName() const;
		void SetDomainName(const std::string &p_DomainName);

	private:
		std::string m_Name;
		std::string m_DomainName;
	};
}

#endif // !defined(SNI_NAMABLE_H_INCLUDED)
