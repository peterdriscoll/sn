#if !defined(SNI_SPLITTER_H_INCLUDED)
#define SNI_SPLITTER_H_INCLUDED

#pragma once
namespace SN
{
	class SN_Expression;
	class SN_ValueSet;
}

namespace SNI
{
	class SNI_World;

	class SNI_Splitter
	{
	public:
		SNI_Splitter();
		~SNI_Splitter();

		bool PositiveNotNull();
		bool NegativeNotNull();

		SN::SN_Expression Positive();
		SN::SN_Expression Negative();

		void ProcessValueSplit(const SN::SN_Expression & p_Param, SNI_World * p_World);

	private:
		SN::SN_ValueSet m_Positive;
		SN::SN_ValueSet m_Negative;
		SN::SN_Expression m_PositiveSimple;
		SN::SN_Expression m_NegativeSimple;
	};
}
	

#endif // !defined(SNI_SPLITTER_H_INCLUDED)
