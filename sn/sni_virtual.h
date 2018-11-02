#if !defined(SNI_VIRTUAL_H_INCLUDED)
#define SNI_VIRTUAL_H_INCLUDED

#pragma once

#include <vector>
#include <list>

#include "sni_value.h"

namespace SN
{
	class SN_Expression;
}

namespace SNI
{
	class SNI_Class;
	class SNI_Expression;

	class ConstructionTree
	{
	public:
		ConstructionTree(const string &p_ParameterName);
		ConstructionTree(const string &p_ParameterName, SN::SN_Expression p_Parameter);
		ConstructionTree(const string &p_ParameterName, SN::SN_Expression p_Parameter, SN::SN_Expression p_CallExpression);
		~ConstructionTree();

		void AddImplementation(SN::SN_Expression p_Implementation);
		SN::SN_Expression CreateExpression();

	private:
		SN::SN_Expression GetParameter();
		void AddParameterList(size_t p_Base, SN::SN_ExpressionList &p_FormalParameterList, SN::SN_Expression p_ImplementationCall);
		SN::SN_Expression BuildExpression(size_t p_Depth);

		SN::SN_Expression BuildCondition(size_t p_Depth, SN::SN_Expression p_Condition, SN::SN_Variable p_ParameterVariable);

		list<ConstructionTree> m_List;
		SN::SN_Expression m_Parameter;
		SN::SN_Variable m_ParameterVariable;
		SN::SN_Expression m_ImplementationCall;
	};

	enum MatchLevel
	{
		Incompatible,
		Unknown,
		Matched
	};

	class ParameterizedExpression
	{
	private:
		SN::SN_ExpressionList m_FormalParameterList;
		SN::SN_Expression m_Expression;
		SN::SN_Expression m_OriginalExpression;

	public:
		ParameterizedExpression(SN::SN_Expression p_Expression)
		{
			m_OriginalExpression = p_Expression;
			m_Expression = p_Expression.GetSNI_Expression()->LoadFormalParameters(m_FormalParameterList);
		};

		size_t size()
		{
			return m_FormalParameterList.size();
		}
		SN::SN_Expression &GetExpression()
		{
			return m_Expression;
		}

		SN::SN_Expression &GetOriginalExpression()
		{
			return m_OriginalExpression;
		}

		size_t Common(ParameterizedExpression &p_right)
		{
			size_t minLength = m_FormalParameterList.size();
			if (p_right.m_FormalParameterList.size() < minLength)
			{
				minLength = p_right.m_FormalParameterList.size();
			}
			size_t common = 0;
			for (size_t j = 0; j < minLength; j++)
			{
				if (m_FormalParameterList[j].Equivalent(p_right.m_FormalParameterList[j]))
				{
					common++;
				}
				else
				{
					return common;
				}
			}
			return common;
		}

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

		SN::SN_Expression BuildCondition(size_t j, SN::SN_Expression param)
		{
			return param.IsA(m_FormalParameterList[j].GetVariableValue());
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
	};
	typedef vector<ParameterizedExpression> ParameterizedExpressionList;
	typedef vector<ParameterizedExpression &> ParameterizedExpressionRefList;

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
		ConstructionTree *BuildTree();
		void BuildCallList(ParameterizedExpressionList &p_OrderedCalls);
		SN::SN_Expression CreateImplementation2();
		SN::SN_Expression CreateImplementation();
		void BuildImplementation();

		static SNI_Class *m_Class;

		vector<SN::SN_Expression> m_CallList;
		long m_DefineId;

		bool m_Fixed;
		SN::SN_Expression m_CallExpression;
	};
}

#endif // !defined(SNI_VIRTUAL_H_INCLUDED)
