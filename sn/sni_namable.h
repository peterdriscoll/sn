#if !defined(SNI_NAMABLE_H_INCLUDED)
#define SNI_NAMABLE_H_INCLUDED

#pragma once

#include "ref.h"

#include <string>
using namespace std;

namespace SNI
{
	class SNI_Namable
	{
	public:
		SNI_Namable();
		virtual ~SNI_Namable();

		string GetName() const;
		void SetName(const string &p_Name);

		string GetDomainName() const;
		void SetDomainName(const string &p_DomainName);

	private:
		string m_Name;
		string m_DomainName;
	};
}

#endif // !defined(SNI_NAMABLE_H_INCLUDED)
