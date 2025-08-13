#if !defined(SN_ERROR_H_INCLUDED)
#define SN_ERROR_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"

#include <string>


namespace SNI
{
	class SNI_Error;
	class SNI_Expression;
}

namespace SN
{
	class LogContext;
	class SN_Expression;
	class SN_Value;

	class SN_EXPORT SN_Error
	{
	private:
		SNI::SNI_Error * m_Error;

	public:
		SN_Error();
		SN_Error(const SN_Error &p_Other);
		SN_Error(bool p_Success, bool p_Delay, const std::string &p_Description="");
		SN_Error(SNI::SNI_Error *p_Error);
		virtual ~SN_Error();

		SNI::SNI_Error * GetSNI_Error() const;

		bool IsError();
		bool IsSignificantError();
		bool IsNull();
		bool GetBool();
		bool GetDelay();
		bool RequestRerun();
		std::string GetDescription();

		void AddError(const SN_Error & p_Error);
		void Log();
	};
}

#endif // !defined(SN_ERROR_H_INCLUDED)
