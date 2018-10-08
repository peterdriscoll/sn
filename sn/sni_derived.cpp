#include "sni_derived.h"

#include "logcontext.h"
#include "sn_expression.h"
#include "sn_parameter.h"
#include "sn_value.h"
#include "sn_error.h"

#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ SNI_Class *SNI_Derived::m_Class = NULL;
	/*static*/ SNI_Class *SNI_Derived::Class()
	{
		if (!m_Class)
		{
			m_Class = new SNI_Class();
		}
		return m_Class;
	}

	SNI_Derived::SNI_Derived()
	: m_Fixed(false)
	{
	}

	SNI_Derived::~SNI_Derived()
	{
	}

	void SNI_Derived::PromoteMembers()
	{
		for (auto &entry : m_Vector)
		{
			REQUESTPROMOTION(entry);
		}
	}

	string SNI_Derived::GetTypeName() const
	{
		return "Derived";
	}

	string SNI_Derived::DisplayCpp() const
	{
		return GetTypeName() + "()";
	}

	string SNI_Derived::DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const
	{
		return GetTypeName() + "()";
	}

	long SNI_Derived::GetPriority() const
	{
		return 3;
	}

	bool SNI_Derived::Equivalent(SNI_Object * p_Other) const
	{
		if (m_Fixed)
		{
			if (dynamic_cast<SNI_Derived *>(p_Other))
			{
				SNI_Derived * l_vector = dynamic_cast<SNI_Derived *>(p_Other);
				size_t size = m_Vector.size();
				if (size != l_vector->m_Vector.size())
				{
					return false;
				}
				
				for (size_t j =0; j < size; j++)
				{
					if (!m_Vector[j].Equivalent(l_vector->m_Vector[j]))
					{
						return false;
					}
				}
			}
		}
		return false;
	}

	size_t SNI_Derived::Hash() const
	{
		return _Hash_representation(m_Vector);
	}

	bool SNI_Derived::IsFixed() const
	{
		return m_Fixed;
	}

	// Inheritance
	SN::SN_Value SNI_Derived::DoIsA(const SNI_Value * p_Parent) const
	{
		return Class()->DoIsA(p_Parent);
	}

	void SNI_Derived::Fix()
	{
		m_Fixed = true;
	}
}
