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

	private:
		string m_Name;
	};
}

#endif // !defined(SNI_NAMABLE_H_INCLUDED)
