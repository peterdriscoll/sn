#if !defined(SNI_OBJECT_H_INCLUDED)
#define SNI_OBJECT_H_INCLUDED

#pragma once

#include <string>
#include <vector>
#include "pgc.h"

namespace SNI
{
	class SNI_Object : public PGCX::Base
	{
		PGC_CLASS(SNI_Object);
	public:
		SNI_Object();
		SNI_Object(const SNI_Object &p_Other);
		virtual ~SNI_Object();

		virtual bool Equivalent(SNI_Object * p_Other) const;

	protected:
		virtual void PromoteMembers();
	};

	typedef vector<SNI_Object> SNI_ObjectList;
}

#endif // !defined(SNI_OBJECT_H_INCLUDED)
