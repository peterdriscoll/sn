#if !defined(SNI_STRING_H_INCLUDED)
#define SNI_STRING_H_INCLUDED

#pragma once

#include <string>
using namespace std;

#include "sni_value.h"

namespace SN
{
	class SN_Error;
}

namespace SNI
{
	class SNI_StringRef;

#undef PGC_ACTION_OVER_VALUES
#undef PGC_ACTION_OVER_MEMBERS
#undef PGC_ACTION_OVER_CONTAINERS

#define PGC_ACTION_OVER_VALUES(ACTION) \
	ACTION(String, std::string, std::string())

#define PGC_ACTION_OVER_MEMBERS(ACTION)

#define PGC_ACTION_OVER_CONTAINERS(ACTION)

	class SNI_String : public SNI_Value
	{
		PGC_CLASS(SNI_String);

		using Base = SNI_Value;

		//PGC_MEMBER_DEFINITIONS(SNI_String);
	private:
		// From: PGC_DEFINE_MEMBERS(SNI_String)
		// -> PGC_ACTION_OVER_VALUES(PGC_DEFINE_VALUE)
		std::string m_String = std::string();

	public:
		// From: PGC_MEMBER_ACCESSORS(SNI_String)
		// -> PGC_ACTION_OVER_MEMBERS(PGC_GETSET_MEMBER)
		// (no members => nothing emitted)

		// From: PGC_PROMOTION_LOGIC(SNI_String)
		void PromoteMembers()
		{
			// PGC_ACTION_OVER_MEMBERS(PGC_PROMOTE_MEMBER)
			// PGC_ACTION_OVER_CONTAINERS(PGC_PROMOTE_CONTAINER)
			// (both empty => no statements)
		}

		// From: PGC_COPY_CONSTRUCTOR(SNI_String)
		SNI_String(const SNI_String& other)
			: Base(other)
			, m_String(other.m_String)
		{
			// PGC_ACTION_OVER_MEMBERS(PGC_REQUEST_PROMOTION_MEMBER)
			// PGC_ACTION_OVER_CONTAINERS(PGC_REQUEST_PROMOTION_CONTAINER_COPY)
			// (both empty)
		}

		// From: PGC_MOVE_CONSTRUCTOR(SNI_String)  [present if your header includes it]
		SNI_String(SNI_String&& other) noexcept
			: Base(std::move(other))
			, m_String(std::move(other.m_String))
		{
		}

		// From: PGC_CLONE_TO(SNI_String)
		virtual PGC::PGC_Base* CloneTo(void* memory) const override
		{
			return new (memory) SNI_String(*this);
		}

		// From: PGC_MOVE_TO(SNI_String)  [present if your header includes it]
		virtual PGC::PGC_Base* MoveTo(void* memory) override
		{
			return new (memory) SNI_String(std::move(*this));
		}
		//End PGC_MEMBER_DEFINITIONS(SNI_String);
	public:
		
		static SNI_Class *Class();
		virtual SNI_Class *VClass();

		SNI_String();
		SNI_String(const SNI_StringRef &p_Other);
		SNI_String(const std::string &p_Other);
		virtual ~SNI_String();

		virtual SNI_Expression * Copy() const;

		// Logging
		virtual std::string GetTypeName() const;
		virtual std::string DisplayCpp() const;
		virtual std::string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

		// Status
		virtual bool IsString() const;


		// Comparisons
		SN::SN_Value DoEquals(SNI_Value * p_Other) const;
		SN::SN_Value DoNotEquals(SNI_Value * p_Other) const;
		SN::SN_Value DoLessThan(SNI_Value * p_Other) const;
		SN::SN_Value DoGreaterThan(SNI_Value * p_Other) const;
		SN::SN_Value DoLessEquals(SNI_Value * p_Other) const;
		SN::SN_Value DoGreaterEquals(SNI_Value * p_Other) const;

		// Members
		virtual std::string GetString() const;
		virtual size_t Length() const;
		virtual SN::SN_String GetSource() const;
		virtual	SN::SN_Expression GetStart() const;
		virtual	SN::SN_Expression GetEnd() const;
		virtual const std::string &GetSourceString() const;
		virtual long GetLeftMostPos() const;
		virtual long GetRightMostPos() const;

		// Base
		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual size_t Hash() const;
		virtual SN::SN_Error AssertValue(const SN::SN_Expression &p_Value);

		// Implementation
		virtual SN::SN_Value DoAdd(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoSubtract(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoConcat(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoSubtractLeft(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoSubtractRight(SNI_Value * p_Other) const;
		virtual SN::SN_Value DoSubtractLeftChar() const;
		virtual SN::SN_Value DoSubtractRightChar() const;
		virtual	SN::SN_Value DoSelectLeftChar() const;
		virtual	SN::SN_Value DoSelectRightChar() const;
		virtual	SN::SN_Value DoLookaheadLeft() const;
		virtual	SN::SN_Value DoLookaheadRight() const;
		virtual	SN::SN_Value DoLookStringLeft(SNI_Value * p_Other) const;
		virtual	SN::SN_Value DoLookStringRight(SNI_Value * p_Other) const;
		virtual	SN::SN_Value DoFile() const;

		virtual void DoWriteFile(const SN::SN_Value & p_Contents) const;

		virtual SN::SN_Value DoEscape(enum SN::EscapeType p_EscapeType) const;
		virtual SN::SN_Value DoUnescape(enum SN::EscapeType p_EscapeType) const;
		virtual SN::SN_Value DoStringToInt() const;
		virtual SN::SN_Value DoStringToDouble() const;
	};

	typedef std::vector<SNI_String *> SNI_StringList;
}

#endif // !defined(SNI_STRING_H_INCLUDED)
