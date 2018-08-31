#if !defined(SNI_DISPLAYOPTIONS_H)
#define SNI_DISPLAYOPTIONS_H

#pragma once

namespace SNI
{
	class SNI_DisplayOptions
	{
	public:
		SNI_DisplayOptions();

		virtual ~SNI_DisplayOptions();

		bool FindVariable(SNI_Variable *p_Variable) const;

	private:
		SNI_VariableConstPointerList m_DisplayVariableList;

	};
}

#endif // !defined(SNI_DISPLAYOPTIONS_H)
