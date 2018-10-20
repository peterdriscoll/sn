#if !defined(SNI_VIRTUAL_H_INCLUDED)
#define SNI_VIRTUAL_H_INCLUDED

#pragma once

#include <vector>

#include "sni_value.h"

namespace SN
{
	class SN_Expression;
}

namespace SNI
{
	class SNI_Class;
	class SNI_Expression;

	enum MatchLevel
	{
		Incompatible,
		Unknown,
		Matched
	};

	class ParameterizedExpression
	{
	public:
		ParameterizedExpression(SNI_Expression *p_Expression)
		{
			m_Expression = p_Expression->LoadFormalParameters(m_FormalParameterList);
		};

		bool operator <(ParameterizedExpression &p_right)
		{
			size_t minLength = m_FormalParameterList.size();
			if (p_right.m_FormalParameterList.size() < minLength)
			{
				minLength = p_right.m_FormalParameterList.size();
			}
			for (size_t j = 0; j < minLength; j++)
			{
				if (m_FormalParameterList[j].IsA(p_right.m_FormalParameterList[j]).GetBool())
				{
					return true;
				}
				if (p_right.m_FormalParameterList[j].IsA(m_FormalParameterList[j]).GetBool())
				{
					return false;
				}
			}
			return m_FormalParameterList.size() < minLength;
		}

		enum MatchLevel MatchParameters(SN::SN_ExpressionList * p_ParameterList)
		{
			size_t paramSize = p_ParameterList->size();
			if (m_FormalParameterList.size() != paramSize)
			{
				return Incompatible;
			}
			for (size_t j = 0; j < paramSize; j++)
			{
				if (!(*p_ParameterList)[j].GetSNI_Expression()->DoIsA(m_FormalParameterList[j].GetSNI_Value()).GetBool())
				{
					if ((*p_ParameterList)[j].IsKnownValue())
					{
						return Incompatible;
					}
					else
					{
						return Unknown;
					}
				}
			}
			return Matched;
		}

		SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList)
		{
			return skynet::Null;
		}

		SN::SN_Expression BuildCondition(SN::SN_ExpressionList * p_ParameterList)
		{
			SN::SN_Expression condition;
			size_t paramSize = p_ParameterList->size();
			if (m_FormalParameterList.size() != paramSize)
			{
				return SN::SN_Error("Internal error, build condition.");
			}
			for (size_t j = 0; j < paramSize; j++)
			{
				SN::SN_Expression param((*p_ParameterList)[j]);
				SN::SN_Expression subCondition(param.IsA(m_FormalParameterList[j]));
				if (condition.IsNull())
				{
					condition = subCondition;
				}
				else
				{
					condition = condition && subCondition;
				}
			}
			return condition;
		}
		
		SN::SN_Expression BuildCall(SN::SN_ExpressionList * p_ParameterList)
		{
			return skynet::Null;
		}

	private:
		SN::SN_ExpressionList m_FormalParameterList;
		SN::SN_Expression m_Expression;
	};
	typedef vector<ParameterizedExpression> ParameterizedExpressionList;

	class SNI_Virtual : public SNI_Value
	{
		PGC_CLASS(SNI_Virtual);
	public:
		static SNI_Class * Class();

		SNI_Virtual();
		virtual ~SNI_Virtual();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual long GetPriority() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual size_t Hash() const;

		virtual bool IsFixed() const;

		virtual SN::SN_Value DoIsA(const SNI_Value * p_Parent) const;

		virtual void Fix();

		virtual SNI_Expression * Clone(SNI_Frame * p_Frame, bool & p_Changed);

		virtual SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel) const;

		virtual SN::SN_Expression Unify(SN::SN_ExpressionList * p_ParameterList);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Expression, bool p_Define = false);

	protected:
		virtual void PromoteMembers();

	private:
		void BuildCallList();

		static SNI_Class *m_Class;

		vector<SNI_Expression *> m_CallList;
		ParameterizedExpressionList m_OrderedCalls;
		long m_DefineId;
		bool m_Fixed;


	};
}

#endif // !defined(SNI_VIRTUAL_H_INCLUDED)
