#include "sni_vector.h"

#include "logcontext.h"
#include "sn_expression.h"
#include "sn_parameter.h"
#include "sn_value.h"
#include "sn_error.h"
#include "sn_real.h"

#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ SNI_Class *SNI_Vector::m_Class = NULL;
	/*static*/ SNI_Class *SNI_Vector::Class()
	{
		if (!m_Class)
		{
			m_Class = new SNI_Class();
		}
		return m_Class;
	}

	SNI_Vector::SNI_Vector()
	: m_Fixed(false)
	{
	}

	SNI_Vector::~SNI_Vector()
	{
	}

	void SNI_Vector::PromoteMembers()
	{
		for (auto &entry : m_Vector)
		{
			REQUESTPROMOTION(entry);
		}
	}

	string SNI_Vector::GetTypeName() const
	{
		return "Vector";
	}

	string SNI_Vector::DisplayCpp() const
	{
		return GetTypeName() + "()";
	}

	string SNI_Vector::DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const
	{
		return GetTypeName() + "()";
	}

	long SNI_Vector::GetPriority() const
	{
		return 3;
	}

	bool SNI_Vector::Equivalent(SNI_Object * p_Other) const
	{
		if (m_Fixed)
		{
			if (dynamic_cast<SNI_Vector *>(p_Other))
			{
				SNI_Vector * l_vector = dynamic_cast<SNI_Vector *>(p_Other);
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

	size_t SNI_Vector::Hash() const
	{
		return _Hash_representation(m_Vector);
	}

	bool SNI_Vector::IsFixed() const
	{
		return m_Fixed;
	}

	// Inheritance
	SN::SN_Error SNI_Vector::AssertIsAValue(const SNI_Value * p_Parent, SN::SN_Expression p_Result)
	{
		return p_Result.AssertValue(Class()->DoIsA(p_Parent));
	}

	SN::SN_Value SNI_Vector::DoIsA(const SNI_Value * p_Parent) const
	{
		return Class()->DoIsA(p_Parent);
	}

	SN::SN_Error SNI_Vector::AssertSubscriptValue(const SNI_Value * p_Index, SN::SN_Expression p_Result)
	{
		SN::SN_Value indexValue(p_Index);
		SN::SN_Real<long> indexNumber(indexValue);
		if (indexNumber.IsNull())
		{
			return SN::SN_Error(GetTypeName() + " Subscript index is not a Long.");
		}
		size_t index = indexNumber.GetNumber();

		if (index >= m_Vector.size())
		{
			if (m_Fixed)
			{
				return SN::SN_Error(GetTypeName() + " Subscript index is out of bounds.");
			}
			else
			{
				m_Vector.resize(index + 1);
			}
		}
		if (m_Vector[index].IsNull())
		{
			m_Vector[index] = p_Result;
			return skynet::OK;
		}
		return p_Result.AssertValue(m_Vector[index]);
	}

	// Functions
	SN::SN_Value SNI_Vector::DoSubscriptCall(const SN::SN_Value & p_Index) const
	{
		SN::SN_Value indexValue(p_Index);
		SN::SN_Real<long> indexNumber(indexValue);
		if (indexNumber.IsNull())
		{
			return SN::SN_Error(GetTypeName() + " Subscript index is not a Long.");
		}
		size_t index = (size_t)indexNumber.GetNumber();
		if (index >= m_Vector.size())
		{
			if (m_Fixed)
			{
				return SN::SN_Error(GetTypeName() + " Subscript index is out of bounds.");
			}
			else
			{
				const_cast<SNI_Vector *>(this)->m_Vector.resize(index + 1);
			}
		}
		return m_Vector[index];
	}

	SN::SN_Value SNI_Vector::DoReverseSubscript(const SN::SN_Value & p_Result) const
	{
		if (!m_Vector.size())
		{
			return SN::SN_Error(GetTypeName() + " No values in map.");
		}
		if (!m_Fixed)
		{
			return SN::SN_Error(GetTypeName() + " Vector length not fixed.");
		}
		SN::SN_ValueSet vs;
		SNI_WorldSet *worldSet = vs.GetWorldSet();
		for (size_t j = 0; j < m_Vector.size(); j++)
		{
			if (p_Result.Equivalent(m_Vector[j]))
			{
				SN::SN_Error err = vs.AddValue(skynet::Long(j), 0, NULL, worldSet);
				if (err.IsError())
				{
					return err;
				}
			}
		}
		if (vs.Cardinality())
		{
			return vs;
		}
		return SN::SN_Error(GetTypeName() + " No matching values in map.");
	}

	SN::SN_Value SNI_Vector::DoCountIf(SN::SN_Expression p_Value) const
	{
		if (!m_Fixed)
		{
			return SN::SN_Error(GetTypeName() + " CountIf - Number of values in map may change. Fix the map before counting.");
		}
		SN_DECLARE_VALUE(countCondition, p_Value);
		long count = 0;
		for (auto &item : m_Vector)
		{
			if (p_Value.IsNull() || countCondition(item).Evaluate().GetBool())
			{
				++count;
			}
		}
		return SN::SN_Long(count);
	}

	SN::SN_Value SNI_Vector::DoCountAll() const
	{
		if (!m_Fixed)
		{
			return SN::SN_Error(GetTypeName() + " CountAll - Number of values in vector may change. Fix the vector before counting.");
		}
		size_t size = m_Vector.size();
		return SN::SN_Long(size);
	}

	SN::SN_Value SNI_Vector::DoSum() const
	{
		if (!m_Fixed)
		{
			return SN::SN_Error(GetTypeName() + " Sum - Number of values in map may change. Fix the map before summing.");
		}
		SN::SN_Expression sum;
		for (auto &item : m_Vector)
		{
			if (item.IsNullValue())
			{
				return SN::SN_Error(GetTypeName() + " Sum - Null value found.");
			}
			if (sum.IsNull())
			{
				sum = item;
			}
			else
			{
				sum = sum.GetSNI_Value()->DoAdd(item.GetSNI_Value());
			}
		}
		return sum;
	}

	void SNI_Vector::Fix()
	{
		m_Fixed = true;
	}
}
