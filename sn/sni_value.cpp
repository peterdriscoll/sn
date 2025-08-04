#include "sni_value.h"

#include "sn_expression.h"
#include "sn_value.h"
#include "sn_valueset.h"
#include "sn_error.h"
#include "sn_set.h"
#include "sn_cartesian.h"

#include "sni_cartesian.h"
#include "sni_splitter.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ SNI_Class* SNI_Value::Class()
	{
		return SNI_User::GetCurrentUser()->GetOrCreatePointer<SNI_Value, SNI_Class>("Value");
	}

	SNI_Class* SNI_Value::VClass()
	{
		return Class();
	}

	SNI_Value::SNI_Value()
	{
	}

	SNI_Value::SNI_Value(unsigned long p_Id)
		: SNI_Expression(p_Id)
	{
	}

	SNI_Value::SNI_Value(const SNI_Value &)
	{
	}

	SNI_Value::~SNI_Value()
	{
	}

	string SNI_Value::GetTypeName() const
	{
		return "Value";
	}

	string SNI_Value::DisplayCpp() const
	{
		return "Value";
	}

	string SNI_Value::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		return "Value";
	}

	string SNI_Value::DisplayValueSN(long priority, SNI_DisplayOptions &p_DisplayOptions) const
	{
		if (p_DisplayOptions.GetDebugHTML() == doDebugPointsJS)
		{
			return DisplaySN(priority, p_DisplayOptions);
		}
		return GetTypeName() + "(" + DisplaySN(priority, p_DisplayOptions) + ")";
	}

	long SNI_Value::GetPriority() const
	{
		return 1;
	}

	bool SNI_Value::IsValue() const
	{
		return true;
	}

	bool SNI_Value::IsKnownValue() const
	{
		return true;
	}

	bool SNI_Value::IsReferableValue() const
	{
		return false;
	}

	SN::SN_Expression SNI_Value::GetVariableValue(bool p_IfComplete)
	{
		return this;
	}

	size_t SNI_Value::Cardinality(size_t p_MaxCardinality) const
	{
		return 1;
	}

	bool SNI_Value::operator<(const SNI_Value& p_Right) const
	{
		return false;
	}

	bool SNI_Value::operator==(const SNI_Value& p_Right) const
	{
		return false;
	}

	SN::SN_Error SNI_Value::ForEach(std::function<SN::SN_Error(const SN::SN_Expression &p_Param, SNI_World *p_World)> p_Action)
	{
		return p_Action(SN::SN_Expression(this), NULL);
	}

	SN::SN_Error SNI_Value::ForEachCart(long p_Depth, SNI_Cart * p_Cart)
	{
		return p_Cart->ProcessValue(p_Depth, this, NULL);
	}

	void SNI_Value::ForEachSplit(SNI_Splitter * p_Splitter)
	{
		p_Splitter->ProcessValueSplit(SN::SN_Expression(this), NULL);
	}

	bool SNI_Value::AllValuesEqual(const SN::SN_Expression & p_Value) const
	{
		return Equivalent(p_Value.GetSNI_Expression());
	}

	SN::SN_Error SNI_Value::AssertValue(const SN::SN_Expression &p_Value)
	{
		if (SN::Is<SNI_ValueSet *>(p_Value))
		{
			SN::SN_ValueSet valueSet(p_Value, 0);
			LOGGING(SN::LogContext context(DisplaySN0() + "SNI_Expression::AssertValue ( " + valueSet.DisplaySN() + " )"));
			bool result = false;
			for (size_t i = 0; i < valueSet.Length(); i++)
			{
				if (valueSet[i].GetValue().IsNull())
				{
					result = true;
				}
				else if (valueSet[i].GetValue().IsNullValue())
				{
					SN::SN_Error e1 = valueSet[i].GetValue().AssertValue(this);
					if (e1.IsError())
					{
						return e1;
					}
					result = true;
				}
				else if (valueSet[i].GetValue().Equivalent(this))
				{
					result = true;
				}
				else
				{
					SNI_World * world = valueSet[i].GetWorld();
					if (world)
					{
						SN::SN_Error err = world->Fail(IncompatibleValue);
						if (err.IsError())
						{
							return err;
						}
					}
				}
			}
			valueSet.Simplify();
			return SN::SN_Error(result, false);
		}
		else if (p_Value.IsKnownValue())
		{
			return SN::SN_Error(p_Value.Equivalent(this->GetVariableValue()), false);
		}
		SN::SN_Expression value = p_Value;
		return value.AssertValue(DoEvaluate());
	}

	SN::SN_Expression SNI_Value::DoEvaluate(long /* p_MetaLevel = 0 */) const
	{
		return this;
	}

	SN::SN_Expression SNI_Value::DoPartialEvaluate(long /* p_MetaLevel = 0 */) const
	{
		return this;
	}

	SN::SN_Value SNI_Value::DoOr(SNI_Expression * p_Other) const
	{
		return SNI_ValueSet::GeneralDoOr(this, p_Other);
	}

	SN::SN_Value SNI_Value::DoUnaryOr() const
	{
		return skynet::Null; // Let GeneralDoOr construct the valueset.
	}

	SN::SN_Value SNI_Value::DoBuildSet() const
	{
		return SN::SN_Set(SN::SN_Value(this));
	}

	// Inheritance
	SN::SN_Value SNI_Value::DoIsA(const SNI_Value* p_Parent) const
	{
		if (p_Parent->Equivalent(SNI_Value::Class()))
		{
			return skynet::True;
		}
		return SNI_Expression::DoIsA(p_Parent);
	}

	SN::SN_Value SNI_Value::DoAssign(SNI_Value* p_Right, SNI_Value* p_State)
	{
		return SN::SN_StateValue(p_Right, p_State->DoUpdateState(this, p_Right));
	}
}
